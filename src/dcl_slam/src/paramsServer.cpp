#include "paramsServer.h"

/*
 * 构造函数
 * 作用：初始化参数服务器，从ROS参数服务器加载各种配置参数。
 * 实现机制：
 * 1. 获取机器人命名空间和ID。
 * 2. 加载机器人数量、坐标系名称、雷达配置等参数。
 * 3. 加载CPU、建图、降采样、回环检测、关键帧及可视化相关的参数。
 */
paramsServer::paramsServer(std::string node_name) : Node(node_name)
{
	// robot info
	// In ROS2, namespace is handled by node configuration, but we can get it.
	std::string ns = this->get_namespace(); 
	
	if (ns.length() >= 2 && ns[1] >= 'a' && ns[1] <= 'z')
	{
		name_ = ns;
		id_ = ns[1] - 'a';
		RCLCPP_INFO(this->get_logger(), "Robot Name: %s, ID: %d", name_.c_str(), id_);
	}
	else
	{
		// Fallback: use parameter 'robot_id'
		this->declare_parameter("robot_id", 0);
		id_ = this->get_parameter("robot_id").as_int();
		name_ = std::string("/") + (char)('a' + id_);
		RCLCPP_WARN(this->get_logger(), "Invalid namespace '%s'. Using fallback ID: %d, Name: %s", ns.c_str(), id_, name_.c_str());
	}
	
	// Param loading
	this->declare_parameter("number_of_robots", 1);
	number_of_robots_ = this->get_parameter("number_of_robots").as_int();

	if(number_of_robots_ < 1)
	{
		RCLCPP_ERROR(this->get_logger(), "Invalid robot number (must be positive number): %d", number_of_robots_);
		rclcpp::shutdown();
	}

	// Assuming params are flat or under "dcl_slam" if declared so. 
	// For simplicity, we declare them flatly as node parameters.
	
	this->declare_parameter("world_frame", "world");
	world_frame_ = this->get_parameter("world_frame").as_string();
	
	this->declare_parameter("odom_frame", "map");
	odom_frame_ = this->get_parameter("odom_frame").as_string();

	// lidar configuration
	this->declare_parameter("sensor", "velodyne");
	std::string sensorStr = this->get_parameter("sensor").as_string();
	
	if(sensorStr == "velodyne")
	{
		sensor_ = LiDARType::VELODYNE;
	}
	else if(sensorStr == "livox")
	{
		sensor_ = LiDARType::LIVOX;
	}
	else
	{
		RCLCPP_ERROR(this->get_logger(), "Invalid sensor type (must be either 'velodyne' or 'ouster'): %s ", sensorStr.c_str());
		rclcpp::shutdown();
	}
	
	this->declare_parameter("n_scan", 16);
	n_scan_ = this->get_parameter("n_scan").as_int();

	// CPU Params
	this->declare_parameter("onboard_cpu_cores_num", 4);
	onboard_cpu_cores_num_ = this->get_parameter("onboard_cpu_cores_num").as_int();
	
	this->declare_parameter("loop_closure_process_interval", 0.02);
	loop_closure_process_interval_ = this->get_parameter("loop_closure_process_interval").as_double();
	
	this->declare_parameter("map_publish_interval", 10.0);
	map_publish_interval_ = this->get_parameter("map_publish_interval").as_double();
	
	this->declare_parameter("mapping_process_interval", 0.1);
	mapping_process_interval_ = this->get_parameter("mapping_process_interval").as_double();

	// mapping
	this->declare_parameter("global_optmization_enable", false);
	global_optmization_enable_ = this->get_parameter("global_optmization_enable").as_bool();
	
	this->declare_parameter("use_pcm", false);
	use_pcm_ = this->get_parameter("use_pcm").as_bool();
	
	this->declare_parameter("pcm_threshold", 0.75);
	pcm_threshold_ = this->get_parameter("pcm_threshold").as_double();
	
	this->declare_parameter("use_between_noise", false);
	use_between_noise_ = this->get_parameter("use_between_noise").as_bool();
	
	this->declare_parameter("optmization_maximum_iteration", 100);
	optmization_maximum_iteration_ = this->get_parameter("optmization_maximum_iteration").as_int();
	
	this->declare_parameter("failsafe_wait_time", 1.0);
	fail_safe_wait_time_ = this->get_parameter("failsafe_wait_time").as_double();
	
	fail_safe_steps_ = fail_safe_wait_time_/mapping_process_interval_;
	
	this->declare_parameter("rotation_estimate_change_threshold", 0.1);
	rotation_estimate_change_threshold_ = this->get_parameter("rotation_estimate_change_threshold").as_double();
	
	this->declare_parameter("pose_estimate_change_threshold", 0.1);
	pose_estimate_change_threshold_ = this->get_parameter("pose_estimate_change_threshold").as_double();
	
	this->declare_parameter("gamma", 1.0);
	gamma_ = this->get_parameter("gamma").as_double();
	
	this->declare_parameter("use_flagged_init", true);
	use_flagged_init_ = this->get_parameter("use_flagged_init").as_bool();
	
	this->declare_parameter("use_landmarks", false);
	use_landmarks_ = this->get_parameter("use_landmarks").as_bool();
	
	this->declare_parameter("use_heuristics", true);
	use_heuristics_ = this->get_parameter("use_heuristics").as_bool();

	// downsample
	this->declare_parameter("map_leaf_size", 0.4);
	map_leaf_size_ = this->get_parameter("map_leaf_size").as_double();
	
	this->declare_parameter("descript_leaf_size", 0.1);
	descript_leaf_size_ = this->get_parameter("descript_leaf_size").as_double();
	
	// loop closure
	this->declare_parameter("intra_robot_loop_closure_enable", true);
	intra_robot_loop_closure_enable_ = this->get_parameter("intra_robot_loop_closure_enable").as_bool();
	
	this->declare_parameter("inter_robot_loop_closure_enable", true);
	inter_robot_loop_closure_enable_ = this->get_parameter("inter_robot_loop_closure_enable").as_bool();
	
	this->declare_parameter("descriptor_type", "");
	std::string descriptor_type_ = this->get_parameter("descriptor_type").as_string();
	
	if(descriptor_type_ == "ScanContext")
	{
		descriptor_type_num_ = DescriptorType::ScanContext;
	}
	else if(descriptor_type_ == "LidarIris")
	{
		descriptor_type_num_ = DescriptorType::LidarIris;
	}
	else if(descriptor_type_ == "M2DP")
	{
		descriptor_type_num_ = DescriptorType::M2DP;
	}
	else
	{
		inter_robot_loop_closure_enable_ = false;
		RCLCPP_WARN(this->get_logger(), "Invalid descriptor type: %s, turn off interloop...", descriptor_type_.c_str());
	}
	
	this->declare_parameter("knn_candidates", 10);
	knn_candidates_ = this->get_parameter("knn_candidates").as_int();
	
	this->declare_parameter("exclude_recent_frame_num", 30);
	exclude_recent_frame_num_ = this->get_parameter("exclude_recent_frame_num").as_int();
	
	this->declare_parameter("search_radius", 15.0);
	search_radius_ = this->get_parameter("search_radius").as_double();
	
	this->declare_parameter("match_mode", 2);
	match_mode_ = this->get_parameter("match_mode").as_int();
	
	this->declare_parameter("iris_row", 80);
	iris_row_ = this->get_parameter("iris_row").as_int();
	
	this->declare_parameter("iris_column", 360);
	iris_column_ = this->get_parameter("iris_column").as_int();
	
	this->declare_parameter("descriptor_distance_threshold", 0.4);
	descriptor_distance_threshold_ = this->get_parameter("descriptor_distance_threshold").as_double();
	
	this->declare_parameter("history_keyframe_search_num", 16);
	history_keyframe_search_num_ = this->get_parameter("history_keyframe_search_num").as_int();
	
	this->declare_parameter("fitness_score_threshold", 0.2);
	fitness_score_threshold_ = this->get_parameter("fitness_score_threshold").as_double();
	
	this->declare_parameter("ransac_maximum_iteration", 1000);
	ransac_maximum_iteration_ = this->get_parameter("ransac_maximum_iteration").as_int();
	
	this->declare_parameter("ransac_threshold", 0.5);
	ransac_threshold_ = this->get_parameter("ransac_threshold").as_double();
	
	this->declare_parameter("ransac_outlier_reject_threshold", 0.05);
	ransac_outlier_reject_threshold_ = this->get_parameter("ransac_outlier_reject_threshold").as_double();

	// keyframe params
	this->declare_parameter("keyframe_distance_threshold", 1.0);
	keyframe_distance_threshold_ = this->get_parameter("keyframe_distance_threshold").as_double();
	
	this->declare_parameter("keyframe_angle_threshold", 0.2);
	keyframe_angle_threshold_ = this->get_parameter("keyframe_angle_threshold").as_double();

	// visualization
	this->declare_parameter("global_map_visualization_radius", 60.0);
	global_map_visualization_radius_ = this->get_parameter("global_map_visualization_radius").as_double();

	// output directory
	this->declare_parameter("save_directory", "/dcl_output");
	save_directory_ = this->get_parameter("save_directory").as_string();
}


/*
 * 函数名：gtsamPoseToAffine3f
 * 作用：将 GTSAM 的 Pose3 类型转换为 Eigen 的 Affine3f 类型。
 * 实现机制：利用 pcl::getTransformation 函数，提取 Pose3 的平移 (x, y, z) 和旋转 (roll, pitch, yaw) 分量，构建仿射变换矩阵。
 */
Eigen::Affine3f paramsServer::gtsamPoseToAffine3f(gtsam::Pose3 pose)
{ 
	return pcl::getTransformation(pose.translation().x(), pose.translation().y(), pose.translation().z(), 
		pose.rotation().roll(), pose.rotation().pitch(), pose.rotation().yaw());
}

/*
 * 函数名：gtsamPoseToTransform
 * 作用：将 GTSAM 的 Pose3 类型转换为 geometry_msgs::Transform 消息类型。
 * 实现机制：提取 Pose3 的平移部分赋值给 translation，提取旋转部分并转换为四元数赋值给 rotation。
 */
geometry_msgs::msg::Transform paramsServer::gtsamPoseToTransform(gtsam::Pose3 pose)
{
	geometry_msgs::msg::Transform transform_msg;
	transform_msg.translation.x = pose.translation().x();
	transform_msg.translation.y = pose.translation().y();
	transform_msg.translation.z = pose.translation().z();
	transform_msg.rotation.w = pose.rotation().toQuaternion().w();
	transform_msg.rotation.x = pose.rotation().toQuaternion().x();
	transform_msg.rotation.y = pose.rotation().toQuaternion().y();
	transform_msg.rotation.z = pose.rotation().toQuaternion().z();

	return transform_msg;
}


/*
 * 函数名：transformToGtsamPose
 * 作用：将 geometry_msgs::Transform 消息类型转换为 GTSAM 的 Pose3 类型。
 * 实现机制：利用消息中的四元数和平移向量，构造 gtsam::Rot3 和 gtsam::Point3，进而生成 gtsam::Pose3 对象。
 */
gtsam::Pose3 paramsServer::transformToGtsamPose(const geometry_msgs::msg::Transform& pose)
{
	return gtsam::Pose3(gtsam::Rot3::Quaternion(pose.rotation.w, pose.rotation.x, pose.rotation.y, pose.rotation.z), 
		gtsam::Point3(pose.translation.x, pose.translation.y, pose.translation.z));
}


/*
 * 函数名：pclPointTogtsamPose3
 * 作用：将自定义的点云位姿结构 PointPose6D 转换为 GTSAM 的 Pose3 类型。
 * 实现机制：利用 PointPose6D 中的欧拉角 (roll, pitch, yaw) 和位置 (x, y, z)，分别构造旋转和平移对象，生成 Pose3。
 */
gtsam::Pose3 paramsServer::pclPointTogtsamPose3(PointPose6D point)
{
	return gtsam::Pose3(gtsam::Rot3::RzRyRx(double(point.roll), double(point.pitch), double(point.yaw)),
		gtsam::Point3(double(point.x), double(point.y), double(point.z)));
}


/*
 * 函数名：transformPointCloud
 * 作用：根据给定的位姿 PointPose6D 对点云进行坐标变换。
 * 实现机制：
 * 1. 根据 PointPose6D 构建 Eigen::Affine3f 变换矩阵。
 * 2. 使用 OpenMP 并行加速，遍历点云中的每个点，应用仿射变换公式进行坐标更新。
 */
pcl::PointCloud<PointPose3D>::Ptr paramsServer::transformPointCloud(pcl::PointCloud<PointPose3D> cloud_in, PointPose6D* pose)
{
	pcl::PointCloud<PointPose3D>::Ptr cloud_out(new pcl::PointCloud<PointPose3D>());

	int cloud_size = cloud_in.size();
	cloud_out->resize(cloud_size);

	Eigen::Affine3f trans_cur = pcl::getTransformation(pose->x, pose->y, pose->z, pose->roll, pose->pitch, pose->yaw);
	
	#pragma omp parallel for num_threads(onboard_cpu_cores_num_)
	for(int i = 0; i < cloud_size; ++i)
	{
		const auto &p_from = cloud_in.points[i];
		cloud_out->points[i].x = trans_cur(0,0)*p_from.x + trans_cur(0,1)*p_from.y + trans_cur(0,2)*p_from.z + trans_cur(0,3);
		cloud_out->points[i].y = trans_cur(1,0)*p_from.x + trans_cur(1,1)*p_from.y + trans_cur(1,2)*p_from.z + trans_cur(1,3);
		cloud_out->points[i].z = trans_cur(2,0)*p_from.x + trans_cur(2,1)*p_from.y + trans_cur(2,2)*p_from.z + trans_cur(2,3);
		cloud_out->points[i].intensity = p_from.intensity;
	}
	return cloud_out;
}

/*
 * 函数名：transformPointCloud
 * 作用：根据给定的 GTSAM Pose3 位姿对点云进行坐标变换。
 * 实现机制：
 * 1. 从 Pose3 中提取平移和旋转信息，构建 Eigen::Affine3f 变换矩阵。
 * 2. 使用 OpenMP 并行加速，遍历点云中的每个点，应用仿射变换公式进行坐标更新。
 */
pcl::PointCloud<PointPose3D>::Ptr paramsServer::transformPointCloud(pcl::PointCloud<PointPose3D> cloud_in, gtsam::Pose3 pose)
{
	pcl::PointCloud<PointPose3D>::Ptr cloud_out(new pcl::PointCloud<PointPose3D>());

	int cloud_size = cloud_in.size();
	cloud_out->resize(cloud_size);

	Eigen::Affine3f trans_cur = pcl::getTransformation(pose.translation().x(), pose.translation().y(), pose.translation().z(),
		pose.rotation().roll(), pose.rotation().pitch(), pose.rotation().yaw());
	
	#pragma omp parallel for num_threads(onboard_cpu_cores_num_)
	for(int i = 0; i < cloud_size; ++i)
	{
		const auto &p_from = cloud_in.points[i];
		cloud_out->points[i].x = trans_cur(0,0)*p_from.x + trans_cur(0,1)*p_from.y + trans_cur(0,2)*p_from.z + trans_cur(0,3);
		cloud_out->points[i].y = trans_cur(1,0)*p_from.x + trans_cur(1,1)*p_from.y + trans_cur(1,2)*p_from.z + trans_cur(1,3);
		cloud_out->points[i].z = trans_cur(2,0)*p_from.x + trans_cur(2,1)*p_from.y + trans_cur(2,2)*p_from.z + trans_cur(2,3);
		cloud_out->points[i].intensity = p_from.intensity;
	}
	return cloud_out;
}