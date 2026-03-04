#include "distributedMapping.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	class distributedMapping: handle message callback 
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * 函数名：globalDescriptorHandler
 * 作用：接收来自其他机器人的全局描述子消息。
 * 实现机制：将接收到的描述子及其对应的时间戳和索引存储到 robots[id] 结构和 store_descriptors 队列中，用于后续的回环检测。
 */
void distributedMapping::globalDescriptorHandler(
	const dcl_slam::msg::GlobalDescriptor::SharedPtr msg,
	const int& id)
{
	// save timestamp
	robots[id].time_cloud_input_stamp = msg->header.stamp;
	robots[id].time_cloud_input = robots[id].time_cloud_input_stamp.seconds();

	// save descriptors
	LOG(INFO) << "[globalDescriptorHandler(" << id << ")]" <<
		" saveDescriptorAndKey:" << msg->index <<  "." << endl;
	// keyframe_descriptor->saveDescriptorAndKey(msg->values.data(), id, msg->index);
	store_descriptors.emplace_back(make_pair(id,*msg));
}

/*
 * 函数名：optStateHandler
 * 作用：接收邻居机器人的优化状态消息。
 * 实现机制：更新邻居机器人的优化状态（是否开始优化、当前状态枚举值），并将其加入通信范围内的邻居集合。
 */
void distributedMapping::optStateHandler(
	const std_msgs::msg::Int8::SharedPtr msg,
	const int& id)
{
	neighbors_started_optimization[id] = (OptimizerState)msg->data <= OptimizerState::Start;
	neighbor_state[id] = (OptimizerState)msg->data;
	neighbors_lowest_id_included[id] = lowest_id_included;
	if(neighbors_within_communication_range.find(id) == neighbors_within_communication_range.end())
	{
		neighbors_within_communication_range.insert(id);
	}
}

/*
 * 函数名：rotationStateHandler
 * 作用：接收邻居机器人的旋转估计完成状态。
 * 实现机制：更新 neighbors_rotation_estimate_finished 标志位。
 */
void distributedMapping::rotationStateHandler(
	const std_msgs::msg::Int8::SharedPtr msg,
	const int& id)
{
	neighbors_rotation_estimate_finished[id] = msg->data;
}

/*
 * 函数名：poseStateHandler
 * 作用：接收邻居机器人的位姿估计完成状态。
 * 实现机制：更新 neighbors_pose_estimate_finished 和 neighbors_estimation_done 标志位。
 */
void distributedMapping::poseStateHandler(
	const std_msgs::msg::Int8::SharedPtr msg,
	const int& id)
{
	neighbors_pose_estimate_finished[id] = msg->data;
	neighbors_estimation_done[id] = msg->data;
}

/*
 * 函数名：neighborRotationHandler
 * 作用：接收邻居机器人的旋转估计值，并进行分布式旋转优化。
 * 实现机制：
 * 1. 检查消息来源和当前优化状态。
 * 2. 更新优化器中邻居的线性化旋转值。
 * 3. 检查所有邻居是否完成初始化。
 * 4. 如果所有邻居就绪且自身未完成，执行旋转估计 (estimateRotation) 和更新。
 * 5. 检查收敛条件或最大迭代次数，判断旋转估计是否完成。
 * 6. 向后序机器人发送旋转估计值，或者如果所有邻居都完成，发布完成状态。
 */
void distributedMapping::neighborRotationHandler(
	const dcl_slam::msg::NeighborEstimate::SharedPtr msg,
	const int& id)
{
	if(msg->receiver_id != id_ || optimizer_state != OptimizerState::RotationEstimation)
	{
		return;
	}

	/*** prepare for optimization ***/
	// update neighbor roatation estimates
	for(int i = 0; i < msg->pose_id.size(); i++)
	{
		Symbol symbol((id + 'a'), msg->pose_id[i]);
		if(!use_pcm_ || other_robot_keys_for_optimization.find(symbol.key()) != other_robot_keys_for_optimization.end())
		{
			Vector rotation_matrix(9);
			rotation_matrix << msg->estimate[0 + 9*i], msg->estimate[1 + 9*i], msg->estimate[2 + 9*i],
				msg->estimate[3 + 9*i], msg->estimate[4 + 9*i], msg->estimate[5 + 9*i],
				msg->estimate[6 + 9*i], msg->estimate[7 + 9*i], msg->estimate[8 + 9*i];
			optimizer->updateNeighborLinearizedRotations(symbol.key(), rotation_matrix);
		}
		else
		{
			RCLCPP_DEBUG(this->get_logger(), "Stop optimization<%d>. Key %d %d doesn't exist.", id_, symbol.chr(), symbol.index());
			abortOptimization(false);
		}
	}
	// update neighbor flags
	if(optimizer_state == OptimizerState::RotationEstimation)
	{
		// used only with flagged initialization
		optimizer->updateNeighboringRobotInitialized(char(id + 'a'), msg->initialized);
		neighbors_rotation_estimate_finished[id] = msg->estimation_done;
	}
	RCLCPP_DEBUG(this->get_logger(), "neighborRotationHandler<%d> from robot %d done? %d (%d/%d]", id_, id, msg->estimation_done,
		optimizer->getNeighboringRobotsInit().size(), optimization_order.size() - 1);

	/*** perform rotation optimization ***/
	// all other robot is finished rotation optimization
	if(optimizer->getNeighboringRobotsInit().size() == optimization_order.size() - 1)
	{
		if(!estimation_done)
		{
			// rotation estimation
			try
			{
				optimizer->estimateRotation();
				optimizer->updateRotation();
				optimizer->updateInitialized(true);
				current_rotation_estimate_iteration++;
			}
			catch(const std::exception& ex)
			{
				RCLCPP_WARN(this->get_logger(), "Stopping rotation optimization<%d> : %s.", id_, ex.what());
				abortOptimization(true);
			}

			// if change is small enough, end rotation optimization
			if((optimizer->latestChange() <= rotation_estimate_change_threshold_) ||
				(current_rotation_estimate_iteration >= optmization_maximum_iteration_))
			{
				rotation_estimate_finished = true;
				estimation_done = true;
			}
			RCLCPP_DEBUG(this->get_logger(), "--->Rotation estimation<%d> iter:[%d/%d] change:%.4f.", id_,
				current_rotation_estimate_iteration, optmization_maximum_iteration_, optimizer->latestChange());
		}

		// check neigbors rotation optimization state
		bool send_flag = estimation_done;
		for(int i = 0; i < optimization_order.size(); i++)
		{
			int other_robot = optimization_order[i];
			if(!neighbors_rotation_estimate_finished[other_robot] && other_robot != id_)
			{
				send_flag = false;
			}
		}
		// send rotation estimate to the aft-order robot
		if(!send_flag)
		{
			// clear buffer
			for(const auto& neighbor : neighbors_within_communication_range)
			{
				robots[neighbor].estimate_msg.pose_id.clear();
				robots[neighbor].estimate_msg.estimate.clear();
			}
			// extract rotation estimate for each loop closure
			for(const std::pair<Symbol, Symbol>& separator_symbols: optimizer->separatorsSymbols())
			{
				// robot id
				int other_robot = (int)(separator_symbols.first.chr() - 'a');
				// pose id
				robots[other_robot].estimate_msg.pose_id.push_back(separator_symbols.second.index());
				// rotation estimates
				Vector rotation_estimate = optimizer->linearizedRotationAt(separator_symbols.second.key());
				for(int it = 0; it < 9; it++)
				{
					robots[other_robot].estimate_msg.estimate.push_back(rotation_estimate[it]);
				}
			}
			// send rotation estimate
			int publish_flag = false;
			for(int i = 0; i < optimization_order.size(); i++)
			{
				int other_robot = optimization_order[i];
				if(other_robot == id_)
				{
					publish_flag = true;
					continue;
				}

				if(publish_flag)
				{
					robots[other_robot].estimate_msg.initialized = optimizer->isRobotInitialized();
					robots[other_robot].estimate_msg.receiver_id = other_robot;
					robots[other_robot].estimate_msg.estimation_done = estimation_done;
					robots[id_].pub_neighbor_rotation_estimates->publish(robots[other_robot].estimate_msg);
				}
			}

			rotation_estimate_start = false;
			optimizer->clearNeighboringRobotInit();
		}
		// send rotation optimization state
		state_msg.data = estimation_done? 1:0;
		robots[id_].pub_rotation_estimate_state->publish(state_msg);
	}
}

/*
 * 函数名：neighborPoseHandler
 * 作用：接收邻居机器人的位姿估计值，并进行分布式位姿优化。
 * 实现机制：
 * 1. 保存邻居的锚点偏移。
 * 2. 更新来自邻居的位姿估计值。
 * 3. 执行分布式位姿优化逻辑 (具体实现依赖后续代码块)。
 */
void distributedMapping::neighborPoseHandler(
	const dcl_slam::msg::NeighborEstimate::SharedPtr msg,
	const int& id)
{
	if(msg->receiver_id != id_ || optimizer_state != OptimizerState::PoseEstimation)
	{
		return;
	}

	/*** prepare for optimization ***/
	// save neighbor anchor offset
	if(msg->estimation_done)
	{
		Point3 offset(msg->anchor_offset[0], msg->anchor_offset[1], msg->anchor_offset[2]);
		neighbors_anchor_offset[id] = offset;
	}
	// update neighbor pose estimates
	for(int i = 0; i < msg->pose_id.size(); i++)
	{
		if(neighbors_within_communication_range.find(id) != neighbors_within_communication_range.end())
		{
			Symbol symbol((id + 'a'), msg->pose_id[i]);
			Vector pose_vector(6);
			pose_vector << msg->estimate[0 + 6*i], msg->estimate[1 + 6*i], msg->estimate[2 + 6*i],
				msg->estimate[3 + 6*i], msg->estimate[4 + 6*i], msg->estimate[5 + 6*i];
			optimizer->updateNeighborLinearizedPoses(symbol.key(), pose_vector);
		}
	}
	// update neighbor flags
	if(optimizer_state == OptimizerState::PoseEstimation)
	{
		// used only with flagged initialization
		optimizer->updateNeighboringRobotInitialized(char(id + 'a'), msg->initialized);
		neighbors_estimation_done[id] = msg->estimation_done;
		neighbors_pose_estimate_finished[id] = msg->estimation_done;
	}
	RCLCPP_DEBUG(this->get_logger(), "neighborPoseHandler<%d> from robot %d, done? %d [%d/%d]", id_, id, msg->estimation_done,
		optimizer->getNeighboringRobotsInit().size(), optimization_order.size() - 1);

	/*** perform pose estimation ***/
	if(optimizer->getNeighboringRobotsInit().size() == optimization_order.size() - 1)
	{
		if(!estimation_done)
		{
			// pose Estimation
			try
			{
				optimizer->estimatePoses();
				optimizer->updatePoses();
				optimizer->updateInitialized(true);
				current_pose_estimate_iteration++;
			}
			catch(const std::exception& ex)
			{
				RCLCPP_WARN(this->get_logger(), "Stopping pose optimization<%d> : %s.",id_,ex.what());
				abortOptimization(true);
			}

			// if change is small enough, end pose optimization
			if((current_pose_estimate_iteration >= optmization_maximum_iteration_) ||
				(optimizer->latestChange() <= pose_estimate_change_threshold_))
			{
				pose_estimate_finished = true;
				estimation_done = true;
			}
			RCLCPP_DEBUG(this->get_logger(), "--->Pose estimation<%d> iter:[%d/%d] change:%.4f.", id_,
				current_pose_estimate_iteration, optmization_maximum_iteration_, optimizer->latestChange());

			// extract anchor offset
			Key first_key = KeyVector(initial_values->keys()).at(0);
			anchor_point = initial_values->at<Pose3>(first_key).translation();
			anchor_offset = anchor_point - (optimizer->currentEstimate().at<Pose3>(first_key).translation() +
				Point3(optimizer->linearizedPoses().at(first_key).tail(3)));
		}
		
		// check neigbors rotation optimization state
		bool send_flag = estimation_done;
		for(int i = 0; i < optimization_order.size(); i++)
		{
			int other_robot = optimization_order[i];
			if(!neighbors_pose_estimate_finished[other_robot] && other_robot != id_)
			{
				send_flag = false;
			}
		}
		// send pose estimate to the aft-order robot
		if(!send_flag)
		{
			// clear buffer
			for(const auto& neighbor : neighbors_within_communication_range)
			{
				robots[neighbor].estimate_msg.pose_id.clear();
				robots[neighbor].estimate_msg.estimate.clear();
				robots[neighbor].estimate_msg.anchor_offset.clear();
			}
			// extract pose estimate from each loop closure
			for(const std::pair<Symbol, Symbol>& separator_symbols: optimizer->separatorsSymbols())
			{
				int other_robot = (int)(separator_symbols.first.chr() - 'a');

				robots[other_robot].estimate_msg.pose_id.push_back(separator_symbols.second.index());

				Vector pose_estimate = optimizer->linearizedPosesAt(separator_symbols.second.key());
				for(int it = 0; it < 6; it++)
				{
					robots[other_robot].estimate_msg.estimate.push_back(pose_estimate[it]);
				}
			}
			// send pose estimate
			bool publish_flag = false;
			for(int i = 0; i < optimization_order.size(); i++)
			{
				int other_robot = optimization_order[i];
				if(other_robot == id_)
				{
					publish_flag = true;
					continue;
				}

				if(publish_flag)
				{
					for(int i = 0; i < 3; i++)
					{
						robots[other_robot].estimate_msg.anchor_offset.push_back(anchor_offset[i]);
					}
					robots[other_robot].estimate_msg.initialized = optimizer->isRobotInitialized();
					robots[other_robot].estimate_msg.receiver_id = other_robot;
					robots[other_robot].estimate_msg.estimation_done = estimation_done;
					robots[id_].pub_neighbor_pose_estimates->publish(robots[other_robot].estimate_msg);
				}
			}
			// send pose optimization state
			pose_estimate_start = false;
			optimizer->clearNeighboringRobotInit();
		}
		// send optimization state
		state_msg.data = estimation_done? 1:0;
		robots[id_].pub_pose_estimate_state->publish(state_msg);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	class distributedMapping: saving keyframe API
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * 函数名：performDistributedMapping
 * 作用：执行分布式建图的核心逻辑，处理关键帧和位姿图更新。
 * 实现机制：
 * 1. 保存关键帧点云和时间戳。
 * 2. 如果是第一帧，添加先验因子 (PriorFactor)。
 * 3. 如果不是第一帧，计算与上一帧的相对位姿，添加里程计因子 (BetweenFactor)。
 * 4. 使用 iSAM2 进行局部优化。
 * 5. 保存优化后的关键帧位姿（3D和6D）。
 * 6. 更新局部路径和全局路径用于可视化。
 */
void distributedMapping::performDistributedMapping(
	const Pose3& pose_to,
	const pcl::PointCloud<PointPose3D>::Ptr frame_to,
	const rclcpp::Time& timestamp)
{
	// save keyframe cloud
	pcl::copyPointCloud(*frame_to, *robots[id_].keyframe_cloud);
	robots[id_].keyframe_cloud_array.push_back(*robots[id_].keyframe_cloud);
	// save timestamp
	robots[id_].time_cloud_input_stamp = timestamp;
	robots[id_].time_cloud_input = timestamp.seconds();

	// add piror factor
	Pose3 new_pose_to;
	int poses_num = initial_values->size();
	Symbol current_symbol = Symbol('a'+id_, poses_num);
	if(poses_num == 0)
	{
		// save piror value
		robots[id_].piror_odom = pose_to;

		// add piror factor to graph
		auto prior_factor = PriorFactor<Pose3>(current_symbol, pose_to, prior_noise);
		local_pose_graph_no_filtering->add(prior_factor);
		isam2_graph.add(prior_factor);

		// add piror value
		initial_values->insert(current_symbol, pose_to);
		isam2_initial_values.insert(current_symbol, pose_to);
		new_pose_to = pose_to;

		LOG(INFO) << "createPrior:[" << id_ << "]" << new_pose_to.translation().x() << " " << new_pose_to.translation().y()
			<< " " << new_pose_to.translation().z() << new_pose_to.rotation().roll() << " " << new_pose_to.rotation().pitch()
			<< " " << new_pose_to.rotation().yaw() << "." << endl;
	}
	// add odometry factor
	else
	{
		// incremental odom in local frame
		auto pose_from = pclPointTogtsamPose3(keyposes_cloud_6d->points[poses_num - 1]);
		auto pose_increment = pose_from.between(pose_to);
		Symbol previous_symbol = Symbol('a'+id_, poses_num - 1);
		Matrix covariance = odometry_noise->covariance();
		
		// add odometry factor to graph
		NonlinearFactor::shared_ptr factor(new BetweenFactor<Pose3>(
			previous_symbol, current_symbol, pose_increment, odometry_noise));
		local_pose_graph->add(factor);
		local_pose_graph_no_filtering->add(factor);
		isam2_graph.add(factor);

		// add odometry value
		isam2_initial_values.insert(current_symbol, pose_to);
		// incremental odom in global graph frame
		pose_from = initial_values->at<Pose3>(previous_symbol);
		new_pose_to = pose_from * pose_increment;
		initial_values->insert(current_symbol, new_pose_to);

		// save factor in local map (for PCM)
		auto new_factor = boost::dynamic_pointer_cast<BetweenFactor<Pose3>>(factor);
		robot_local_map.addTransform(*new_factor, covariance);

		LOG(INFO) << "createOdom:[" << id_ << "]" << "[" << poses_num - 1 << "-" << poses_num << "]--["
			<< pose_from.translation().x() << " " << pose_from.translation().y() << " " << pose_from.translation().z()
			<< pose_from.rotation().roll() << " " << pose_from.rotation().pitch() << " " << pose_from.rotation().yaw() << "],["
			<< new_pose_to.translation().x() << " " << new_pose_to.translation().y() << " " << new_pose_to.translation().z()
			<< new_pose_to.rotation().roll() << " " << new_pose_to.rotation().pitch() << " " << new_pose_to.rotation().yaw() << "]." << endl;
	}

	// optimizing
	// isam2_graph.print("GTSAM Graph:\n");
	isam2->update(isam2_graph, isam2_initial_values);
	isam2_graph.resize(0);
	isam2_initial_values.clear();
	isam2_current_estimates = isam2->calculateEstimate();
	isam2_keypose_estimate = isam2_current_estimates.at<Pose3>(current_symbol);

	// save pose in local frame
	static PointPose3D pose_3d;
	pose_3d.x = isam2_keypose_estimate.translation().x();
	pose_3d.y = isam2_keypose_estimate.translation().y();
	pose_3d.z = isam2_keypose_estimate.translation().z();
	pose_3d.intensity = poses_num; // keyframe index
	keyposes_cloud_3d->push_back(pose_3d);

	static PointPose6D pose_6d;
	pose_6d.x = pose_3d.x;
	pose_6d.y = pose_3d.y;
	pose_6d.z = pose_3d.z;
	pose_6d.intensity = pose_3d.intensity;
	pose_6d.roll = isam2_keypose_estimate.rotation().roll();
	pose_6d.pitch = isam2_keypose_estimate.rotation().pitch();
	pose_6d.yaw = isam2_keypose_estimate.rotation().yaw();
	pose_6d.time = robots[id_].time_cloud_input; // keyframe timestamp
	keyposes_cloud_6d->push_back(pose_6d);

	LOG(INFO) << "save:[" << id_ << "]--[" << poses_num << "]--" << isam2_keypose_estimate.translation().x()
		<< " " << isam2_keypose_estimate.translation().y() << " " << isam2_keypose_estimate.translation().z()
		<< " " << isam2_keypose_estimate.rotation().roll() << " " << isam2_keypose_estimate.rotation().pitch()
		<< " " << isam2_keypose_estimate.rotation().yaw() << "." << endl;

	// save path for visualization
	updateLocalPath(pose_6d);
	updateGlobalPath(new_pose_to);
}

/*
 * 函数名：saveFrame
 * 作用：判断是否需要保存当前帧为关键帧。
 * 实现机制：
 * 1. 如果是第一帧，直接保存。
 * 2. 计算当前帧与上一关键帧的相对位姿变化（距离和角度）。
 * 3. 如果变化量小于设定的阈值 (keyframe_distance_threshold_, keyframe_angle_threshold_)，则丢弃该帧。
 */
bool distributedMapping::saveFrame(
	const Pose3& pose_to)
{
	if(keyposes_cloud_3d->empty())
	{
		return true;
	}

	auto last_keypose = pclPointTogtsamPose3(keyposes_cloud_6d->back());
	auto pose_increment = last_keypose.inverse() * pose_to;

	float x = pose_increment.translation().x();
	float y = pose_increment.translation().y();
	float z = pose_increment.translation().z();
	float roll = pose_increment.rotation().roll();
	float pitch = pose_increment.rotation().pitch();
	float yaw = pose_increment.rotation().yaw();

	// select keyframe
	if(abs(roll) < keyframe_angle_threshold_ && abs(pitch) < keyframe_angle_threshold_ && 
		abs(yaw) < keyframe_angle_threshold_ && sqrt(x*x + y*y + z*z) < keyframe_distance_threshold_)
	{
		return false;
	}

	return true;
}

/*
 * 函数名：updateLocalPath
 * 作用：更新局部路径 (Local Path)。
 * 实现机制：将新的位姿转换为 geometry_msgs::PoseStamped 消息，添加到 local_path 消息中。
 */
void distributedMapping::updateLocalPath(
	const PointPose6D& pose)
{
	static geometry_msgs::msg::PoseStamped pose_stamped_msg;
	pose_stamped_msg.header.stamp = this->now();
	pose_stamped_msg.header.frame_id = world_frame_;
	pose_stamped_msg.pose.position.x = pose.x;
	pose_stamped_msg.pose.position.y = pose.y;
	pose_stamped_msg.pose.position.z = pose.z;
	tf2::Quaternion q;
	q.setRPY(pose.roll, pose.pitch, pose.yaw);
	pose_stamped_msg.pose.orientation.x = q.x();
	pose_stamped_msg.pose.orientation.y = q.y();
	pose_stamped_msg.pose.orientation.z = q.z();
	pose_stamped_msg.pose.orientation.w = q.w();

	local_path.poses.push_back(pose_stamped_msg);
}

/*
 * 函数名：updatePoses
 * 作用：更新所有关键帧的位姿。
 * 实现机制：
 * 1. 如果发生机器人内部回环 (intra_robot_loop_close_flag)，从 iSAM2 优化结果中提取所有关键帧的最新位姿，并更新点云和路径。
 * 2. 否则，仅更新最新一帧的估计值。
 * 3. 备份关键帧点云用于后续计算。
 */
bool distributedMapping::updatePoses()
{
	bool return_value = false;

	if(keyposes_cloud_3d->empty())
	{
		return return_value;
	}

	if(intra_robot_loop_close_flag)
	{
		// clear path
		local_path.poses.clear();

		// aggregate estimates
		for(const Values::ConstKeyValuePair &key_value: isam2_current_estimates)
		{
			// update key poses
			Symbol key = key_value.key;
			int index = key.index();
			Pose3 pose = isam2_current_estimates.at<Pose3>(key);

			keyposes_cloud_3d->points[index].x = pose.translation().x();
			keyposes_cloud_3d->points[index].y = pose.translation().y();
			keyposes_cloud_3d->points[index].z = pose.translation().z();

			keyposes_cloud_6d->points[index].x = keyposes_cloud_3d->points[index].x;
			keyposes_cloud_6d->points[index].y = keyposes_cloud_3d->points[index].y;
			keyposes_cloud_6d->points[index].z = keyposes_cloud_3d->points[index].z;
			keyposes_cloud_6d->points[index].roll = pose.rotation().roll();
			keyposes_cloud_6d->points[index].pitch = pose.rotation().pitch();
			keyposes_cloud_6d->points[index].yaw = pose.rotation().yaw();
			

			updateLocalPath(keyposes_cloud_6d->points[index]);
		}
		
		intra_robot_loop_close_flag = false;
		return_value = true;
		LOG(INFO) << "updatePoses:[" << id_ << "]." << endl;
	}

	// copy keyposes
	*copy_keyposes_cloud_3d = *keyposes_cloud_3d;
	*copy_keyposes_cloud_6d = *keyposes_cloud_6d;

	// save updated keypose
	isam2_keypose_estimate = pclPointTogtsamPose3(keyposes_cloud_6d->back());

	return return_value;
}

/*
 * 函数名：makeDescriptors
 * 作用：生成和发布当前关键帧的全局描述子。
 * 实现机制：
 * 1. 处理待处理的描述子队列。
 * 2. 对当前关键帧点云进行降采样。
 * 3. 调用 keyframe_descriptor 生成全局描述子。
 * 4. 构造 global_descriptor 消息并发布，供其他机器人订阅。
 */
void distributedMapping::makeDescriptors()
{
	while (!store_descriptors.empty())
	{
		auto msg_data = store_descriptors.front().second;
		auto msg_id = store_descriptors.front().first;
		store_descriptors.pop_front();
		keyframe_descriptor->saveDescriptorAndKey(msg_data.values.data(), msg_id, msg_data.index);
	}

	if(initial_values->empty() || (!intra_robot_loop_closure_enable_ && !inter_robot_loop_closure_enable_))
	{
		return;
	}

	// downsample keyframe
	cloud_for_decript_ds->clear();
	if(sensor_ == LiDARType::LIVOX)
	{
		// pcl::PointCloud<PointPose3D>::Ptr multiKeyFrameCloud(new pcl::PointCloud<PointPose3D>());
		// loopFindNearKeyframes(multiKeyFrameCloud, initial_values->size() - 1, history_keyframe_search_num_);
		// downsample_filter_for_descriptor.setInputCloud(multiKeyFrameCloud);
		downsample_filter_for_descriptor.setInputCloud(robots[id_].keyframe_cloud);
	}
	else
	{
		downsample_filter_for_descriptor.setInputCloud(robots[id_].keyframe_cloud);
	}
	downsample_filter_for_descriptor.filter(*cloud_for_decript_ds);

	// make and save global descriptors
	auto descriptor_vec = keyframe_descriptor->makeAndSaveDescriptorAndKey(*cloud_for_decript_ds, id_, initial_values->size()-1);
	LOG(INFO) << "makeDescriptors[" << id_ << "]." << endl;

	// extract descriptors values
	global_descriptor_msg.values.swap(descriptor_vec);
	// keyfame index
	global_descriptor_msg.index = initial_values->size()-1;
	global_descriptor_msg.header.stamp = robots[id_].time_cloud_input_stamp;
	// publish message
	robots[id_].pub_descriptors->publish(global_descriptor_msg);
}

/*
 * 函数名：publishPath
 * 作用：发布全局路径和局部路径用于可视化。
 * 实现机制：检查是否有订阅者，如果有，更新时间戳并发布 nav_msgs::Path 消息。
 */
void distributedMapping::publishPath()
{
	// publish path
	if(pub_global_path->get_subscription_count() != 0)
	{
		global_path.header.stamp = robots[id_].time_cloud_input_stamp;
		global_path.header.frame_id = world_frame_;
		pub_global_path->publish(global_path);
	}

	if(pub_local_path->get_subscription_count() != 0)
	{
		local_path.header.stamp = robots[id_].time_cloud_input_stamp;
		local_path.header.frame_id = robots[id_].odom_frame_;
		pub_local_path->publish(local_path);
	}
}

/*
 * 函数名：publishTransformation
 * 作用：发布世界坐标系到里程计坐标系的 TF 变换。
 * 实现机制：计算优化后的第一帧位姿与初始第一帧位姿的差值，作为 map 到 odom 的校正变换并发布。
 */
void distributedMapping::publishTransformation(
	const rclcpp::Time& timestamp)
{
	static tf2_ros::TransformBroadcaster world_to_odom_tf_broadcaster(this);
	static Symbol first_key((id_+'a'), 0);

	Pose3 first_pose = initial_values->at<Pose3>(first_key);
	Pose3 old_first_pose = robots[id_].piror_odom;
	Pose3 pose_between = first_pose * old_first_pose.inverse();

	geometry_msgs::msg::TransformStamped world_to_odom;
	world_to_odom.header.stamp = timestamp;
	world_to_odom.header.frame_id = world_frame_;
	world_to_odom.child_frame_id = robots[id_].odom_frame_;
	
	world_to_odom.transform.translation.x = pose_between.translation().x();
	world_to_odom.transform.translation.y = pose_between.translation().y();
	world_to_odom.transform.translation.z = pose_between.translation().z();
	world_to_odom.transform.rotation.x = pose_between.rotation().toQuaternion().x();
	world_to_odom.transform.rotation.y = pose_between.rotation().toQuaternion().y();
	world_to_odom.transform.rotation.z = pose_between.rotation().toQuaternion().z();
	world_to_odom.transform.rotation.w = pose_between.rotation().toQuaternion().w();

	world_to_odom_tf_broadcaster.sendTransform(world_to_odom);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	class distributedMapping: distributed mapping
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * 函数名：startOptimizationCondition
 * 作用：判断是否满足开始分布式优化的条件。
 * 实现机制：
 * 1. 检查是否有邻居开始优化 (neighbors_started_optimization)。
 * 2. 检查所有开始优化的邻居状态是否处于 Start 阶段。
 * 3. 检查自身是否已发送开始优化标志 (sent_start_optimization_flag)。
 */
bool distributedMapping::startOptimizationCondition()
{
	// check if all neighbors started
	bool all_neighbor_started = !neighbors_started_optimization.empty();
	for(const auto& neighbor_started : neighbors_started_optimization)
	{
		all_neighbor_started &= neighbor_started.second && (neighbor_state[neighbor_started.first] <= OptimizerState::Start);
	}

	return all_neighbor_started && sent_start_optimization_flag;
}

/*
 * 函数名：updateOptimizer
 * 作用：更新分布式优化器的图结构和状态。
 * 实现机制：
 * 1. 加载子图和初始值。
 * 2. 确定哪个机器人拥有先验因子 (prior factor)，通常是 ID 最小的机器人。
 * 3. 检查图的连通性。
 * 4. 检查与邻居的通信状态，如果失去连接，则重置优化器状态。
 */
void distributedMapping::updateOptimizer()
{
	// load subgraphs
	graph_values_vec = std::make_pair(local_pose_graph, initial_values);
	optimizer->loadSubgraphAndCreateSubgraphEdge(graph_values_vec);

	// add prior to the first robot
	std::pair<int, int> robot_pair = std::make_pair(id_, lowest_id_included);
	for(const auto& neighbor_lowest_id : neighbors_lowest_id_included)
	{
		if(neighbors_within_communication_range.find(neighbor_lowest_id.first) != neighbors_within_communication_range.end())
		{
			if(neighbor_lowest_id.second < robot_pair.second)
			{
				robot_pair = neighbor_lowest_id;
				if(lowest_id_to_included > neighbor_lowest_id.second)
				{
					lowest_id_to_included = neighbor_lowest_id.second;
				}
			}
			if(neighbor_lowest_id.second == robot_pair.second)
			{
				if(robot_pair.first > neighbor_lowest_id.first)
				{
					robot_pair = neighbor_lowest_id;
				}
			}
		}
	}
	// first robot always have prior
	prior_owner = robot_pair.first;
	RCLCPP_DEBUG(this->get_logger(), "priorOwner<%d> %d",id_, prior_owner);
	if(robot_pair.first == id_)
	{
		static Key prior_key = Symbol('a'+id_, 0);
		optimizer->addPrior(prior_key, robots[id_].piror_odom, prior_noise);
		prior_added = true;
	}

	// check for graph connectivity
	neighboring_robots = optimizer->getNeighboringChars();
	if(neighboring_robots.size() > 0)
	{
		graph_disconnected = false;
	}
	bool has_separator_with_neighbor = false;
	// check neighbor within communication range
	for(const auto& neighbor : neighbors_within_communication_range)
	{
		if(neighboring_robots.find((char)(neighbor + 'a')) != neighboring_robots.end())
		{
			has_separator_with_neighbor = true;
			neighbors_estimation_done[neighbor] = false;
			neighbors_pose_estimate_finished[neighbor] = false;
			neighbors_rotation_estimate_finished[neighbor] = false;
		}
	}
	if(!has_separator_with_neighbor)
	{
		changeOptimizerState(OptimizerState::Idle);
	}
}

/*
 * 函数名：outliersFiltering
 * 作用：使用分布式两两一致性最大化 (Distributed PCM) 算法过滤错误的回环约束（外点）。
 * 实现机制：
 * 1. 遍历每个邻居机器人。
 * 2. 获取邻居的局部地图和机器人间的测量值。
 * 3. 构建全局地图 (GlobalMap) 并运行 PCM 算法寻找最大团 (Max Clique)。
 * 4. 根据最大团结果，识别并拒绝错误的回环约束。
 * 5. 从优化器和局部位姿图中移除被拒绝的因子。
 * 6. 更新接受和拒绝的键值对集合，用于调试和分析。
 */
void distributedMapping::outliersFiltering()
{
	robot_local_map_backup = robot_local_map;

	if(use_pcm_)
	{
		measurements_accepted_num = 0;
		measurements_rejected_num = 0;
		// perform pairwise consistency maximization for each pair robot 
		for(const auto& neighbor : neighbors_within_communication_range)
		{
			if(neighbor == id_ || pose_estimates_from_neighbors.find(neighbor) == pose_estimates_from_neighbors.end())
			{
				continue;
			}

			// get other robot trajectory
			graph_utils::Transforms empty_transforms;
			auto neighbor_local_info = robot_measurements::RobotLocalMap(
				pose_estimates_from_neighbors.at(neighbor), empty_transforms, robot_local_map.getLoopClosures());

			// get inter-robot loop closure measurements
			graph_utils::Transforms loop_closures_transforms;
			for (const auto& transform : robot_local_map.getTransforms().transforms)
			{
				auto robot0 = (int) (Symbol(transform.second.i).chr()-97);
				auto robot1 = (int) (Symbol(transform.second.j).chr()-97);
				if(robot0 == neighbor || robot1 == neighbor)
				{
					loop_closures_transforms.transforms.insert(transform);
				}
			}
			auto inter_robot_measurements = robot_measurements::InterRobotMeasurements(
				loop_closures_transforms, optimizer->robotName(), (char)neighbor + 97);

			// local trajectory 
			auto local_info = robot_measurements::RobotLocalMap(
				pose_estimates_from_neighbors.at(id_), robot_local_map.getTransforms(), robot_local_map.getLoopClosures());

			// create global map
			auto globalMap = global_map::GlobalMap(local_info, neighbor_local_info,
				inter_robot_measurements, pcm_threshold_, use_heuristics_);

			// pairwise consistency maximization
			auto max_clique_info = globalMap.pairwiseConsistencyMaximization();
			std::vector<int> max_clique = max_clique_info.first;
			measurements_accepted_num += max_clique.size();
			measurements_rejected_num += max_clique_info.second;

			// retrieve indexes of rejected measurements
			auto loopclosures_ids = optimizer->loopclosureEdge();
			std::vector<int> rejected_loopclosure_ids;
			rejected_loopclosure_ids.reserve(1000);
			std::set<std::pair<Key, Key>> accepted_key_pairs, rejected_key_pairs;
			for(int i = 0; i < loopclosures_ids.size(); i++)
			{
				if(distributed_pcm::DistributedPCM::isloopclosureToBeRejected(max_clique, loopclosures_ids[i],
					loop_closures_transforms, inter_robot_measurements.getLoopClosures(), optimizer))
				{
					rejected_loopclosure_ids.emplace_back(i);

					// Update robot local map and store keys
					auto loopclosure_factor = boost::dynamic_pointer_cast<BetweenFactor<Pose3>>(
						optimizer->currentGraph().at(loopclosures_ids[i]));
					robot_local_map.removeTransform(std::make_pair(loopclosure_factor->keys().at(0), loopclosure_factor->keys().at(1)));
					optimizer->eraseseparatorsSymbols(std::make_pair(loopclosure_factor->keys().at(0), loopclosure_factor->keys().at(1)));
					optimizer->eraseseparatorsSymbols(std::make_pair(loopclosure_factor->keys().at(1), loopclosure_factor->keys().at(0)));
					rejected_key_pairs.insert(std::make_pair(loopclosure_factor->keys().at(0), loopclosure_factor->keys().at(1)));
				}
				else
				{
					auto loopclosure_factor = boost::dynamic_pointer_cast<BetweenFactor<Pose3>>(
						optimizer->currentGraph().at(loopclosures_ids[i]));
					accepted_key_pairs.insert(std::make_pair(loopclosure_factor->keys().at(0), loopclosure_factor->keys().at(1)));
				}
			}

			// remove measurements not in the max clique
			int number_loopclosure_ids_removed = 0;
			for(const auto& index : rejected_loopclosure_ids)
			{
				auto factor_id = loopclosures_ids[index] - number_loopclosure_ids_removed;
				number_loopclosure_ids_removed++;
				optimizer->eraseFactor(factor_id);
				local_pose_graph->erase(local_pose_graph->begin()+factor_id);
			}

			// Update loopclosure ids
			std::vector<size_t> new_loopclosure_ids;
			new_loopclosure_ids.reserve(10000);
			int number_of_edges = optimizer->currentGraph().size();
			for(int i = 0; i < number_of_edges; i++)
			{
				auto keys = optimizer->currentGraph().at(i)->keys();
				if(keys.size() == 2)
				{
					char robot0 = symbolChr(keys.at(0));
					char robot1 = symbolChr(keys.at(1));
					int index0 = symbolIndex(keys.at(0));
					int index1 = symbolIndex(keys.at(1));
					if(robot0 != robot1)
					{
						new_loopclosure_ids.push_back(i);
					}
				}
			}
			optimizer->setloopclosureIds(new_loopclosure_ids);

			// save accepted pair
			for(const auto& accepted_pair : accepted_key_pairs)
			{
				accepted_keys.insert(accepted_pair);
				if(Symbol(accepted_pair.first).chr() == char(id_ + 'a'))
				{
					other_robot_keys_for_optimization.insert(accepted_pair.second);
				}
				else
				{
					other_robot_keys_for_optimization.insert(accepted_pair.first);
				}
			}
			// save rejected pair
			for(const auto& rejected_pair : rejected_key_pairs)
			{
				rejected_keys.insert(rejected_pair);
			}

			RCLCPP_DEBUG(this->get_logger(), "outliersFiltering<%d>, other=%d, max clique size=%d, removed=%d", id_, neighbor,
				max_clique.size(), max_clique_info.second);
		}
	}
}

/*
 * 函数名：computeOptimizationOrder
 * 作用：计算分布式优化的机器人执行顺序。
 * 实现机制：
 * 1. 这是一个启发式算法。
 * 2. 从拥有先验因子的机器人开始。
 * 3. 迭代选择与当前序列中机器人连接边数最多的机器人加入序列。
 * 4. 最终确定当前机器人在优化序列中的位置 (in_order)。
 */
void distributedMapping::computeOptimizationOrder()
{
	optimization_order.clear();
	optimization_order.emplace_back(prior_owner);
	std::map<int,int> edges_num;
	auto robots_consider = neighbors_within_communication_range;
	robots_consider.insert(id_);
	while(true)
	{
		edges_num.clear();
		for(const auto& robot0 : robots_consider)
		{
			if(std::find(optimization_order.begin(),optimization_order.end(),robot0) == optimization_order.end())
			{
				// for each robot not in the ordering, compute the number of edges
				// towards robots inside the ordering and select the one with the largest number
				int robot_edges_num = 0;
				for(size_t robot1: optimization_order)
				{
					robot_edges_num += adjacency_matrix(robot0, robot1);
				}
				if(robot_edges_num != 0)
				{
					edges_num.insert(std::make_pair(robot0, robot_edges_num));
				}
			}
		}
		if(edges_num.empty())
		{
			break;
		}
		int maximum_edge_num = -1;
		int maximum_robot = -1;
		for(const auto& robot_info : edges_num)
		{
			if(robot_info.second > maximum_edge_num)
			{
				maximum_edge_num = robot_info.second;
				maximum_robot = robot_info.first;
			}
		}
		optimization_order.emplace_back(maximum_robot);
		edges_num.erase(maximum_robot);
	}

	in_order = false;
	for(int i = 0; i < optimization_order.size(); i++)
	{
		if(optimization_order[i] == id_)
		{
			in_order = true;
		}
	}
}

/*
 * 函数名：initializePoseGraphOptimization
 * 作用：初始化位姿图优化流程。
 * 实现机制：
 * 1. 调用 updateOptimizer 加载图数据。
 * 2. 调用 outliersFiltering 剔除误匹配回环。
 * 3. 调用 computeOptimizationOrder 计算优化顺序。
 * 4. 重置优化器初始化状态和相关标志位。
 */
void distributedMapping::initializePoseGraphOptimization()
{
	// load graph&value and check graph connectivity
	updateOptimizer();

	// fliter outlier with distributed PCM method
	outliersFiltering();
	
	// ordering
	computeOptimizationOrder();

	// initialize
	optimizer->updateInitialized(false);
	optimizer->clearNeighboringRobotInit();
	rotation_estimate_finished = false;
	pose_estimate_finished = false;
	estimation_done = false;
}

/*
 * 函数名：rotationEstimationStoppingBarrier
 * 作用：旋转估计阶段的同步和停止屏障。
 * 实现机制：
 * 1. 检查轮次内的邻居是否都处于旋转估计阶段。
 * 2. 向前序机器人发送旋转估计值。
 * 3. 检查所有邻居是否已进入位姿估计阶段。
 * 4. 检查是否所有机器人（包括自己）都已完成旋转估计收敛。
 * 5. 如果条件满足，转换状态并准备进入位姿估计阶段。
 */
bool distributedMapping::rotationEstimationStoppingBarrier()
{
	// check neighbor state
	bool in_turn = true;
	neighboring_robots = optimizer->getNeighboringChars();
	for(int i = 0; i < optimization_order.size(); i++)
	{
		if(optimization_order[i] != id_ &&
			neighboring_robots.find((char)(optimization_order[i] + 'a')) != neighboring_robots.end())
		{
			in_turn &= (neighbor_state[optimization_order[i]] == OptimizerState::RotationEstimation);
		}
	}
	// send rotation estimate to the pre-order robot
	if(in_turn && !rotation_estimate_start)
	{
		rotation_estimate_start = true;
		// clear buffer
		for(const auto& neighbor : neighbors_within_communication_range)
		{
			robots[neighbor].estimate_msg.pose_id.clear();
			robots[neighbor].estimate_msg.estimate.clear();
		}
		// extract rotation estimate for each loop closure
		for(const std::pair<Symbol, Symbol>& separator_symbols: optimizer->separatorsSymbols())
		{
			// robot id
			int other_robot = (int)(separator_symbols.first.chr() - 'a');
			// pose id
			robots[other_robot].estimate_msg.pose_id.push_back(separator_symbols.second.index());
			// rotation estimates
			Vector rotation_estimate = optimizer->linearizedRotationAt(separator_symbols.second.key());
			for(int it = 0; it < 9; it++)
			{
				robots[other_robot].estimate_msg.estimate.push_back(rotation_estimate[it]);
			}
		}
		// send rotation estimate
		for(int i = 0; i < optimization_order.size(); i++)
		{
			int other_robot = optimization_order[i];
			if(other_robot == id_)
			{
				break;
			}

			robots[other_robot].estimate_msg.initialized = optimizer->isRobotInitialized();
			robots[other_robot].estimate_msg.receiver_id = other_robot;
			robots[other_robot].estimate_msg.estimation_done = estimation_done;
			robots[id_].pub_neighbor_rotation_estimates->publish(robots[other_robot].estimate_msg);
		}
	}

	// check if neighbors have begun pose estimation
	bool all_finished_rotation_estimation = true;
	for(const auto& neighbor : neighbors_within_communication_range)
	{
		all_finished_rotation_estimation &= (neighbor_state[neighbor] == OptimizerState::PoseEstimation);
	}
	if(all_finished_rotation_estimation)
	{
		return true;
	}

	// neighbors have finished rotation estimation
	bool stop_rotation_estimation = rotation_estimate_finished;
	for(int i = 0; i < optimization_order.size(); i++)
	{
		int otherRobot = optimization_order[i];
		if(otherRobot != id_)
		{
			stop_rotation_estimation &= neighbors_rotation_estimate_finished[otherRobot] || 
				neighbor_state[otherRobot] > optimizer_state;
		}
	}
	return stop_rotation_estimation;
}

void distributedMapping::abortOptimization(
	const bool& log_info)
{
	for(const auto& transform : robot_local_map_backup.getTransforms().transforms)
	{
		if(robot_local_map.getTransforms().transforms.find(transform.first) == robot_local_map.getTransforms().transforms.end())
		{
			SharedNoiseModel model = noiseModel::Gaussian::Covariance(transform.second.pose.covariance_matrix);
			auto factor = BetweenFactor<Pose3>(
				transform.second.i, transform.second.j, transform.second.pose.pose, model);
			robot_local_map.addTransform(factor, transform.second.pose.covariance_matrix);
			local_pose_graph->push_back(factor);
		}
	}

	changeOptimizerState(OptimizerState::Idle);
}

void distributedMapping::removeInactiveNeighbors()
{
	std::vector<int> removed_neighbors;
	removed_neighbors.reserve(1000);
	for(const auto& neighbor : neighbors_within_communication_range)
	{
		if(neighbor_state[neighbor] == OptimizerState::Idle)
		{
			removed_neighbors.emplace_back(neighbor);
		}
	}
	for(const auto& neighbor : removed_neighbors)
	{
		neighbors_within_communication_range.erase(neighbor);;
		neighbors_rotation_estimate_finished.erase(neighbor);
		neighbors_pose_estimate_finished.erase(neighbor);
		neighbors_estimation_done.erase(neighbor);
		neighbors_lowest_id_included.erase(neighbor);
	}
	if(neighbors_within_communication_range.empty())
	{
		RCLCPP_DEBUG(this->get_logger(), "Stop optimization<%d>, there are inactive neighbors.",id_);
		abortOptimization(false);
	}
}

void distributedMapping::failSafeCheck()
{
	if(latest_change == optimizer->latestChange())
	{
		steps_without_change++;
	} 
	else
	{
		steps_without_change = 0;
		latest_change = optimizer->latestChange();
	}

	// wait enough time to receive data from neighbors
	if(steps_without_change > fail_safe_steps_ * neighbors_within_communication_range.size())
	{
		RCLCPP_DEBUG(this->get_logger(), "No progress<%d>, Stop optimization", id_);
		abortOptimization(false);
	}
}

void distributedMapping::initializePoseEstimation()
{
	optimizer->convertLinearizedRotationToPoses();
	Values neighbors = optimizer->neighbors();
	for(const Values::ConstKeyValuePair& key_value: neighbors)
	{
		Key key = key_value.key;
		// pick linear rotation estimate
		VectorValues neighbor_estimate_rot_lin;
		neighbor_estimate_rot_lin.insert(key,  optimizer->neighborsLinearizedRotationsAt(key));
		// make a pose out of it
		Values neighbor_rot_estimate = 
			InitializePose3::normalizeRelaxedRotations(neighbor_estimate_rot_lin);
		Values neighbor_pose_estimate = 
			distributed_mapper::evaluation_utils::pose3WithZeroTranslation(neighbor_rot_estimate);
		// store it
		optimizer->updateNeighbor(key, neighbor_pose_estimate.at<Pose3>(key));
	}

	// reset flags for flagged initialization.
	optimizer->updateInitialized(false);
	optimizer->clearNeighboringRobotInit();
	estimation_done = false;
	for(auto& neighbor_done : neighbors_estimation_done)
	{
		neighbor_done.second = false;
	}
	optimizer->resetLatestChange();
}

bool distributedMapping::poseEstimationStoppingBarrier()
{
	// check neighbor state
	bool in_turn = true;
	// neighboring_robots = optimizer->getNeighboringChars();
	for(int i = 0; i < optimization_order.size(); i++)
	{
		if(optimization_order[i] != id_ && 
			neighbors_within_communication_range.find(optimization_order[i]) != neighbors_within_communication_range.end())
		{
			in_turn &= (neighbor_state[optimization_order[i]] == OptimizerState::PoseEstimation);
		}
	}
	// send pose estimate to the pre-order robot
	if(in_turn && !pose_estimate_start)
	{
		pose_estimate_start = true;
		// clear buffer
		for(const auto& neighbor : neighbors_within_communication_range)
		{
			robots[neighbor].estimate_msg.pose_id.clear();
			robots[neighbor].estimate_msg.estimate.clear();
			robots[neighbor].estimate_msg.anchor_offset.clear();
		}
		// extract pose estimate from each loop closure
		for(const std::pair<Symbol, Symbol>& separator_symbols: optimizer->separatorsSymbols())
		{
			int other_robot = (int)(separator_symbols.first.chr() - 'a');

			robots[other_robot].estimate_msg.pose_id.push_back(separator_symbols.second.index());

			Vector pose_estimate = optimizer->linearizedPosesAt(separator_symbols.second.key());
			for(int it = 0; it < 6; it++)
			{
				robots[other_robot].estimate_msg.estimate.push_back(pose_estimate[it]);
			}
		}
		// send pose estimate
		for(int i = 0; i < optimization_order.size(); i++)
		{
			int other_robot = optimization_order[i];
			if(other_robot == id_)
			{
				break;
			}

			for(int i = 0; i < 3; i++)
			{
				robots[other_robot].estimate_msg.anchor_offset.push_back(anchor_offset[i]);
			}
			robots[other_robot].estimate_msg.initialized = optimizer->isRobotInitialized();
			robots[other_robot].estimate_msg.receiver_id = other_robot;
			robots[other_robot].estimate_msg.estimation_done = pose_estimate_finished;
			robots[id_].pub_neighbor_pose_estimates->publish(robots[other_robot].estimate_msg);
		}
	}

	// check if others have ended optimization
	bool all_finished_pose_estimation = true;
	for(const auto& neighbor : neighbors_within_communication_range)
	{
		bool other_robot_finished = (neighbor_state[neighbor] != OptimizerState::PoseEstimation) &&
			(neighbor_state[neighbor] != OptimizerState::PoseEstimationInitialization) &&
			(neighbor_state[neighbor] != OptimizerState::RotationEstimation);
		all_finished_pose_estimation &= other_robot_finished;
	} 
	if(all_finished_pose_estimation)
	{
		return true;
	}

	// neighbors have finished pose estimation
	bool stop_pose_estimation = pose_estimate_finished;
	for(int i = 0; i < optimization_order.size(); i++)
	{
		int other_robot = optimization_order[i];
		if(other_robot != id_)
		{
			stop_pose_estimation &= neighbors_pose_estimate_finished[other_robot] ||
				neighbor_state[other_robot] > optimizer_state;
		}
	}
	return stop_pose_estimation;
}

void distributedMapping::updateGlobalPath(
	const Pose3& pose)
{
	static geometry_msgs::msg::PoseStamped pose_stamped_msg;
	pose_stamped_msg.header.stamp = this->now();
	pose_stamped_msg.header.frame_id = world_frame_;
	pose_stamped_msg.pose.position.x = pose.translation().x();
	pose_stamped_msg.pose.position.y = pose.translation().y();
	pose_stamped_msg.pose.position.z = pose.translation().z();
	pose_stamped_msg.pose.orientation.x = pose.rotation().toQuaternion().x();
	pose_stamped_msg.pose.orientation.y = pose.rotation().toQuaternion().y();
	pose_stamped_msg.pose.orientation.z = pose.rotation().toQuaternion().z();
	pose_stamped_msg.pose.orientation.w = pose.rotation().toQuaternion().w();

	global_path.poses.push_back(pose_stamped_msg);
}

void distributedMapping::incrementalInitialGuessUpdate()
{
	// update poses values
	initial_values->update(optimizer->currentEstimate());

	// incremental update
	for(size_t k = 0; k < local_pose_graph->size(); k++)
	{
		KeyVector keys = local_pose_graph->at(k)->keys();
		if(keys.size() == 2)
		{
			auto between_factor = boost::dynamic_pointer_cast<BetweenFactor<Pose3>>(local_pose_graph->at(k));
			Symbol key0 = between_factor->keys().at(0);
			Symbol key1 = between_factor->keys().at(1);
			int index0 = key0.index();
			int robot0 = key0.chr();
			int index1 = key1.index();
			int robot1 = key1.chr();
			if(!optimizer->currentEstimate().exists(key1) && (robot0 == robot1) && (index1 = index0 + 1))
			{
				// get previous pose
				auto previous_pose = initial_values->at<Pose3>(key0);
				// compose previous pose and measurement
				auto current_pose = previous_pose * between_factor->measured();
				// update pose in initial guess
				initial_values->update(key1, current_pose);
			}
		}
	}

	
	// aggregate estimates
	global_path.poses.clear();
	pcl::PointCloud<PointPose3D>::Ptr poses_3d_cloud(new pcl::PointCloud<PointPose3D>());
	for(const Values::ConstKeyValuePair &key_value: *initial_values)
	{
		// update key poses
		Symbol key = key_value.key;
		int index = key.index();
		Pose3 pose = initial_values->at<Pose3>(key);

		PointPose3D pose_3d;
		pose_3d.x = pose.translation().x();
		pose_3d.y = pose.translation().y();
		pose_3d.z = pose.translation().z();
		pose_3d.intensity = key.index();
		poses_3d_cloud->push_back(pose_3d);

		updateGlobalPath(pose);
	}

	if(pub_keypose_cloud->get_subscription_count() != 0)
	{
		// publish global map
		sensor_msgs::msg::PointCloud2 keypose_cloud_msg;
		pcl::toROSMsg(*poses_3d_cloud, keypose_cloud_msg);
		keypose_cloud_msg.header.stamp = this->now();
		keypose_cloud_msg.header.frame_id = world_frame_;
		pub_keypose_cloud->publish(keypose_cloud_msg);
	}

	if(pub_global_path->get_subscription_count() != 0)
	{
		global_path.header.stamp = this->now();
		global_path.header.frame_id = world_frame_;
		pub_global_path->publish(global_path);
	}
}

void distributedMapping::endOptimization()
{
	// retract to global frame
	if(prior_owner == id_)
	{
		optimizer->retractPose3GlobalWithOffset(anchor_offset);
	}
	else
	{
		optimizer->retractPose3GlobalWithOffset(neighbors_anchor_offset[prior_owner]);
	}

	// update estimates
	incrementalInitialGuessUpdate();

	lowest_id_included = lowest_id_to_included;
}

void distributedMapping::changeOptimizerState(
	const OptimizerState& state)
{
	switch(state)
	{
		case OptimizerState::Idle:
			RCLCPP_DEBUG(this->get_logger(), "<%d>Idle", id_);
			break;
		case OptimizerState::Start:
			RCLCPP_DEBUG(this->get_logger(), "<%d>Start", id_);
			break;
		case OptimizerState::Initialization:
			RCLCPP_DEBUG(this->get_logger(), "<%d>Initialization", id_);
			break;
		case OptimizerState::RotationEstimation:
			RCLCPP_DEBUG(this->get_logger(), "<%d>RotationEstimation", id_);
			break;
		case OptimizerState::PoseEstimationInitialization:
			RCLCPP_DEBUG(this->get_logger(), "<%d>PoseEstimationInitialization", id_);
			break;
		case OptimizerState::PoseEstimation:
			RCLCPP_DEBUG(this->get_logger(), "<%d>PoseEstimation", id_);
			break;
		case OptimizerState::End:
			RCLCPP_DEBUG(this->get_logger(), "<%d>End", id_);
			break;
		case OptimizerState::PostEndingCommunicationDelay:
			RCLCPP_DEBUG(this->get_logger(), "<%d>PostEndingCommunicationDelay", id_);
			break;
	}

	optimizer_state = state;
	state_msg.data = (int)optimizer_state;
	robots[id_].pub_optimization_state->publish(state_msg);
}

void distributedMapping::run()
{
	// update optimizer state
	switch(optimizer_state)
	{
		case OptimizerState::Idle:
			if(startOptimizationCondition())
			{
				current_rotation_estimate_iteration = 0;
				current_pose_estimate_iteration = 0;
				// neighbors_within_communication_range.clear();
				neighbors_rotation_estimate_finished.clear();
				neighbors_pose_estimate_finished.clear();
				neighbors_anchor_offset.clear();
				neighbors_estimation_done.clear();
				latest_change = -1;
				steps_without_change = 0;
				lowest_id_to_included = lowest_id_included;
				neighbors_started_optimization.clear();
				if(prior_added)
				{
					optimizer->removePrior();
					prior_added = false;
				}
				optimization_steps = 0;
				changeOptimizerState(OptimizerState::Start);
			}
			else
			{
				changeOptimizerState(OptimizerState::Idle);
			}
			state_msg.data = (int)optimizer_state;
			robots[id_].pub_optimization_state->publish(state_msg);
			break;

		case OptimizerState::Start:
			initializePoseGraphOptimization();
			optimization_steps++;
			if(in_order)
			{
				changeOptimizerState(OptimizerState::Initialization);
			}
			else
			{
				changeOptimizerState(OptimizerState::Idle);
			}
			break;

		case OptimizerState::Initialization:
			changeOptimizerState(OptimizerState::RotationEstimation);
			rotation_estimate_start = false;
			optimization_steps++;
			break;

		case OptimizerState::RotationEstimation:
			if(rotationEstimationStoppingBarrier())
			{
				changeOptimizerState(OptimizerState::PoseEstimationInitialization);
			}
			if(current_rotation_estimate_iteration > fail_safe_steps_)
			{
				removeInactiveNeighbors();
			}
			failSafeCheck();
			optimization_steps++;
			break;

		case OptimizerState::PoseEstimationInitialization:
			initializePoseEstimation();
			pose_estimate_start = false;
			optimization_steps++;
			changeOptimizerState(OptimizerState::PoseEstimation);
			break;

		case OptimizerState::PoseEstimation:
			if(poseEstimationStoppingBarrier())
			{
				changeOptimizerState(OptimizerState::End);
			}
			failSafeCheck();
			optimization_steps++;
			break;

		case OptimizerState::End:
			endOptimization();
			changeOptimizerState(OptimizerState::PostEndingCommunicationDelay);
			optimization_steps++;
			break;

		case OptimizerState::PostEndingCommunicationDelay:
			changeOptimizerState(OptimizerState::Idle);
			optimization_steps++;
			break;
	}
}