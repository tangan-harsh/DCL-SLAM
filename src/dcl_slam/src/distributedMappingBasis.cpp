#include "distributedMapping.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	class distributedMapping: constructor and destructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * 构造函数
 * 作用：初始化分布式建图系统，包括机器人配置、通信接口、参数设置、优化器初始化等。
 * 实现机制：
 * 1. 初始化 Google Logging。
 * 2. 遍历所有机器人，为本机器人创建发布者，为其他机器人创建订阅者（用于回环检测和分布式优化通信）。
 * 3. 初始化用于可视化、调试的全局发布者。
 * 4. 设置描述子类型（ScanContext, LidarIris, M2DP）及相关参数。
 * 5. 初始化 iSAM2 优化器和分布式映射优化器 (DistributedMapper)。
 * 6. 启动分布式建图的主定时器 (distributed_mapping_thread)。
 */
distributedMapping::distributedMapping() : paramsServer("dcl_slam_node")
{
	string log_name = name_+"_distributed_mapping";
	google::InitGoogleLogging(log_name.c_str());
	string log_dir = "/log";
	FLAGS_log_dir = std::getenv("HOME") + log_dir;
	RCLCPP_INFO(this->get_logger(), "distributed mapping class initialization");

	/*** robot team ***/
	singleRobot robot; // each robot
	for(int it = 0; it < number_of_robots_; it++)
	{
		/*** robot information ***/
		robot.id_ = it; // robot ID and name
		robot.name_ = "/a";
		robot.name_[1] += it;
		robot.odom_frame_ = robot.name_ + "/" + odom_frame_; // odom frame

		/*** ros subscriber and publisher ***/
		// this robot
		if(it == id_)
		{
			// enable descriptor for detecting loop
			if(intra_robot_loop_closure_enable_ || inter_robot_loop_closure_enable_)
			{
				// publish global descriptor
				robot.pub_descriptors = this->create_publisher<dcl_slam::msg::GlobalDescriptor>(
					robot.name_+"/distributedMapping/globalDescriptors", 5);
				// publish loop infomation
				robot.pub_loop_info = this->create_publisher<dcl_slam::msg::LoopInfo>(
					robot.name_+"/distributedMapping/loopInfo", 5);
			}
			
			// enable DGS
			if(global_optmization_enable_)
			{
				robot.pub_optimization_state = this->create_publisher<std_msgs::msg::Int8>(
					robot.name_+"/distributedMapping/optimizationState", 50);
				robot.pub_rotation_estimate_state = this->create_publisher<std_msgs::msg::Int8>(
					robot.name_+"/distributedMapping/rotationEstimateState", 50);
				robot.pub_pose_estimate_state = this->create_publisher<std_msgs::msg::Int8>(
					robot.name_+"/distributedMapping/poseEstimateState", 50);
				robot.pub_neighbor_rotation_estimates = this->create_publisher<dcl_slam::msg::NeighborEstimate>(
					robot.name_+"/distributedMapping/neighborRotationEstimates", 50);
				robot.pub_neighbor_pose_estimates = this->create_publisher<dcl_slam::msg::NeighborEstimate>(
					robot.name_+"/distributedMapping/neighborPoseEstimates", 50);
			}
		}
		// other robot
		else
		{
			if(intra_robot_loop_closure_enable_ || inter_robot_loop_closure_enable_)
			{
				// subscribe global descriptor
				robot.sub_descriptors = this->create_subscription<dcl_slam::msg::GlobalDescriptor>(
					robot.name_+"/distributedMapping/globalDescriptors", 50,
					[this, it](const dcl_slam::msg::GlobalDescriptor::SharedPtr msg) {
						this->globalDescriptorHandler(msg, it);
					});
				// subscribe loop infomation
				robot.sub_loop_info = this->create_subscription<dcl_slam::msg::LoopInfo>(
					robot.name_+"/distributedMapping/loopInfo", 50,
					[this, it](const dcl_slam::msg::LoopInfo::SharedPtr msg) {
						this->loopInfoHandler(msg, it);
					});
			}

			if(global_optmization_enable_)
			{
				robot.sub_optimization_state = this->create_subscription<std_msgs::msg::Int8>(
					robot.name_+"/distributedMapping/optimizationState", 50,
					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
						this->optStateHandler(msg, it);
					});
				robot.sub_rotation_estimate_state = this->create_subscription<std_msgs::msg::Int8>(
					robot.name_+"/distributedMapping/rotationEstimateState", 50,
					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
						this->rotationStateHandler(msg, it);
					});
				robot.sub_pose_estimate_state = this->create_subscription<std_msgs::msg::Int8>(
					robot.name_+"/distributedMapping/poseEstimateState", 50,
					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
						this->poseStateHandler(msg, it);
					});
				robot.sub_neighbor_rotation_estimates = this->create_subscription<dcl_slam::msg::NeighborEstimate>(
					robot.name_+"/distributedMapping/neighborRotationEstimates", 50,
					[this, it](const dcl_slam::msg::NeighborEstimate::SharedPtr msg) {
						this->neighborRotationHandler(msg, it);
					});
				robot.sub_neighbor_pose_estimates = this->create_subscription<dcl_slam::msg::NeighborEstimate>(
					robot.name_+"/distributedMapping/neighborPoseEstimates", 50,
					[this, it](const dcl_slam::msg::NeighborEstimate::SharedPtr msg) {
						this->neighborPoseHandler(msg, it);
					});
			}
		}

		/*** other ***/
		robot.time_cloud_input_stamp = this->now();
		robot.time_cloud_input = 0.0;

		robot.keyframe_cloud.reset(new pcl::PointCloud<PointPose3D>());
		robot.keyframe_cloud_array.clear();

		robots.push_back(robot);
	}

	/*** ros subscriber and publisher ***/
	// loop closure visualization
	pub_loop_closure_constraints = this->create_publisher<visualization_msgs::msg::MarkerArray>(
		"distributedMapping/loopClosureConstraints", 1);
	// scan2map cloud
	pub_scan_of_scan2map = this->create_publisher<sensor_msgs::msg::PointCloud2>(
		"distributedMapping/scanOfScan2map", 1);
	pub_map_of_scan2map = this->create_publisher<sensor_msgs::msg::PointCloud2>(
		"distributedMapping/mapOfScan2map", 1);
	// global map visualization
	pub_global_map = this->create_publisher<sensor_msgs::msg::PointCloud2>(
		"distributedMapping/globalMap", 1);
	// path for independent robot
	pub_global_path = this->create_publisher<nav_msgs::msg::Path>(
		"distributedMapping/path", 1);
	pub_local_path = this->create_publisher<nav_msgs::msg::Path>(
		"distributedMapping/localPath", 1);
	// keypose cloud
	pub_keypose_cloud = this->create_publisher<sensor_msgs::msg::PointCloud2>(
		"distributedMapping/keyposeCloud", 1);

	/*** message information ***/
	cloud_for_decript_ds.reset(new pcl::PointCloud<PointPose3D>()); 

	/*** downsample filter ***/
	downsample_filter_for_descriptor.setLeafSize(descript_leaf_size_, descript_leaf_size_, descript_leaf_size_);
	downsample_filter_for_intra_loop.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
	downsample_filter_for_inter_loop.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
	downsample_filter_for_inter_loop2.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
	downsample_filter_for_inter_loop3.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);

	/*** mutex ***/
	// lock_on_call = vector<mutex>(number_of_robots_);
	global_path.poses.clear();
	local_path.poses.clear();

	/*** distributed loopclosure ***/
	inter_robot_loop_ptr = 0;
	intra_robot_loop_ptr = 0;

	intra_robot_loop_close_flag = false;

	if(descriptor_type_num_ == DescriptorType::ScanContext)
	{
		keyframe_descriptor = unique_ptr<scan_descriptor>(new scan_context_descriptor(
			20, 60, knn_candidates_, descriptor_distance_threshold_, 0, 80.0,
			exclude_recent_frame_num_, number_of_robots_, id_));
	}
	else if(descriptor_type_num_ == DescriptorType::LidarIris)
	{
		keyframe_descriptor = unique_ptr<scan_descriptor>(new lidar_iris_descriptor(
			iris_row_, iris_column_, n_scan_, descriptor_distance_threshold_,
			exclude_recent_frame_num_, match_mode_, knn_candidates_, 4, 18, 1.6, 0.75,
			number_of_robots_, id_));
	}
	else if(descriptor_type_num_ == DescriptorType::M2DP)
	{
		keyframe_descriptor = unique_ptr<scan_descriptor>(new m2dp_descriptor(
			16, 8, 4, 16, number_of_robots_, id_));
	}

	loop_closures_candidates.clear();
	loop_indexes.clear();

	// radius search
	copy_keyposes_cloud_3d.reset(new pcl::PointCloud<PointPose3D>());
	copy_keyposes_cloud_6d.reset(new pcl::PointCloud<PointPose6D>());
	kdtree_history_keyposes.reset(new pcl::KdTreeFLANN<PointPose3D>());

	/*** noise model ***/
	odometry_noise = noiseModel::Diagonal::Variances((Vector(6) << 1e-6, 1e-6, 1e-6, 1e-4, 1e-4, 1e-4).finished());
	prior_noise = noiseModel::Isotropic::Variance(6, 1e-12);

	/*** local pose graph optmazition ***/
	ISAM2Params parameters;
	parameters.relinearizeThreshold = 0.1;
	parameters.relinearizeSkip = 1;
	isam2 = new ISAM2(parameters); // isam2

	keyposes_cloud_3d.reset(new pcl::PointCloud<PointPose3D>());
	keyposes_cloud_6d.reset(new pcl::PointCloud<PointPose6D>());
	
	/*** distributed pose graph optmazition ***/
	optimizer = boost::shared_ptr<distributed_mapper::DistributedMapper>(
		new distributed_mapper::DistributedMapper(id_ + 'a'));

	steps_of_unchange_graph = 0;

	local_pose_graph = boost::make_shared<NonlinearFactorGraph>();
	initial_values = boost::make_shared<Values>();
	graph_values_vec = make_pair(local_pose_graph, initial_values);

	graph_disconnected = true;

	lowest_id_included = id_;
	lowest_id_to_included = lowest_id_included;
	prior_owner = id_;
	prior_added = false;

	adjacency_matrix = gtsam::Matrix::Zero(number_of_robots_, number_of_robots_);
	optimization_order.clear();
	in_order = false;

	optimizer_state = OptimizerState::Idle;
	optimization_steps = 0;
	sent_start_optimization_flag = false;

	current_rotation_estimate_iteration = 0;
	current_pose_estimate_iteration = 0;

	latest_change = -1;
	steps_without_change = 0;

	rotation_estimate_start = false;
	pose_estimate_start = false;
	rotation_estimate_finished = false;
	pose_estimate_finished = false;
	estimation_done = false;

	neighboring_robots.clear();
	neighbors_within_communication_range.clear();
	neighbors_started_optimization.clear();
	neighbor_state.clear();

	neighbors_rotation_estimate_finished.clear();
	neighbors_pose_estimate_finished.clear();
	neighbors_estimation_done.clear();

	neighbors_lowest_id_included.clear();
	neighbors_anchor_offset.clear();

	local_pose_graph_no_filtering = boost::make_shared<NonlinearFactorGraph>();

	pose_estimates_from_neighbors.clear();
	other_robot_keys_for_optimization.clear();

	accepted_keys.clear();
	rejected_keys.clear();
	measurements_rejected_num = 0;
	measurements_accepted_num = 0;
	
	optimizer->setUseBetweenNoiseFlag(use_between_noise_); // use between noise or not in optimizePoses
	optimizer->setUseLandmarksFlag(use_landmarks_); // use landmarks
	optimizer->loadSubgraphAndCreateSubgraphEdge(graph_values_vec); // load subgraphs
	optimizer->setVerbosity(distributed_mapper::DistributedMapper::ERROR); // verbosity level
	optimizer->setFlaggedInit(use_flagged_init_);
	optimizer->setUpdateType(distributed_mapper::DistributedMapper::incUpdate);
	optimizer->setGamma(gamma_);

	if(global_optmization_enable_)
	{
		distributed_mapping_thread = this->create_wall_timer(
			std::chrono::milliseconds((int)(mapping_process_interval_ * 1000)),
			std::bind(&distributedMapping::run, this));
	}
	
	loop_closure_thread_ = std::thread(&distributedMapping::loopClosureThread, this);

	RCLCPP_INFO(this->get_logger(), "distributed mapping class initialization finish");
}

/*
 * 析构函数
 * 作用：清理资源。
 * 实现机制：目前为空，依赖自动资源管理。
 */
distributedMapping::~distributedMapping()
{
	if(loop_closure_thread_.joinable())
	{
		loop_closure_thread_.join();
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	class distributedMapping: other function
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*
 * 函数名：lockOnCall
 * 作用：锁定互斥量，保证线程安全。
 * 实现机制：(代码中已注释掉，预留接口)
 */
void distributedMapping::lockOnCall()
{
	// lock_on_call.lock();
}

/*
 * 函数名：unlockOnCall
 * 作用：解锁互斥量。
 * 实现机制：(代码中已注释掉，预留接口)
 */
void distributedMapping::unlockOnCall()
{
	// lock_on_call.unlock();
}

/*
 * 函数名：getLocalKeyposesCloud3D
 * 作用：获取本地 3D 关键帧点云（仅包含位置信息）。
 * 实现机制：返回 keyposes_cloud_3d 指针。
 */
pcl::PointCloud<PointPose3D>::Ptr distributedMapping::getLocalKeyposesCloud3D()
{
	return keyposes_cloud_3d;
}

/*
 * 函数名：getLocalKeyposesCloud6D
 * 作用：获取本地 6D 关键帧点云（包含位置和姿态信息）。
 * 实现机制：返回 keyposes_cloud_6d 指针。
 */
pcl::PointCloud<PointPose6D>::Ptr distributedMapping::getLocalKeyposesCloud6D()
{
	return keyposes_cloud_6d;
}

/*
 * 函数名：getLocalKeyframe
 * 作用：获取指定索引的本地关键帧点云。
 * 实现机制：从 robots[id_].keyframe_cloud_array 数组中检索并返回。
 */
pcl::PointCloud<PointPose3D> distributedMapping::getLocalKeyframe(const int& index)
{
	return robots[id_].keyframe_cloud_array[index];
}

/*
 * 函数名：getLatestEstimate
 * 作用：获取 iSAM2 优化后的最新位姿估计。
 * 实现机制：返回 isam2_keypose_estimate 变量。
 */
Pose3 distributedMapping::getLatestEstimate()
{
	return isam2_keypose_estimate;
}

/*
 * 函数名：poseCovariance2msg
 * 作用：将 graph_utils::PoseWithCovariance 转换为 geometry_msgs::PoseWithCovariance 消息。
 * 实现机制：将位姿的平移、旋转（四元数）以及协方差矩阵元素逐一赋值给 ROS 消息格式。
 */
void distributedMapping::poseCovariance2msg(
	const graph_utils::PoseWithCovariance& pose,
	geometry_msgs::msg::PoseWithCovariance& msg)
{
	msg.pose.position.x = pose.pose.x();
	msg.pose.position.y = pose.pose.y();
	msg.pose.position.z = pose.pose.z();

	Vector quaternion = pose.pose.rotation().quaternion();
	msg.pose.orientation.w = quaternion(0);
	msg.pose.orientation.x = quaternion(1);
	msg.pose.orientation.y = quaternion(2);
	msg.pose.orientation.z = quaternion(3);

	for(int i = 0; i < 6; i++)
	{
		for(int j = 0; j < 6; j++)
		{
			msg.covariance[i*6 + j] = pose.covariance_matrix(i, j);
		}
	}
}

/*
 * 函数名：msg2poseCovariance
 * 作用：将 geometry_msgs::PoseWithCovariance 消息转换为 graph_utils::PoseWithCovariance 结构。
 * 实现机制：从 ROS 消息中提取平移和四元数构建 Pose3，并提取协方差矩阵数据。
 */
void distributedMapping::msg2poseCovariance(
	const geometry_msgs::msg::PoseWithCovariance& msg,
	graph_utils::PoseWithCovariance& pose)
{
	Rot3 rotation(msg.pose.orientation.w, msg.pose.orientation.x,
		msg.pose.orientation.y, msg.pose.orientation.z);
	Point3 translation(msg.pose.position.x, msg.pose.position.y, msg.pose.position.z);
	
	pose.pose = Pose3(rotation, translation);

	pose.covariance_matrix = gtsam::Matrix::Zero(6,6);
	for(int i = 0; i < 6; i++)
	{
		for(int j = 0; j < 6; j++)
		{
			pose.covariance_matrix(i, j) = msg.covariance[i*6 + j];
		}
	}
}
