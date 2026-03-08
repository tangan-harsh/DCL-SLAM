# 🤖 Copilot CLI Session

> **Session ID:** `5a0fa04a-ac34-47ef-91c7-e5db8f337dba`
> **Started:** 2026/3/5 16:41:24
> **Duration:** 126m 27s
> **Exported:** 2026/3/5 18:47:52

<sub>⏱️ 0s</sub>

### ℹ️ Info

💡 No copilot instructions found. Run /init to generate a copilot-instructions.md file for this project.

---

<sub>⏱️ 0s</sub>

### ℹ️ Info

Environment loaded: Visual Studio Code connected

---

<sub>⏱️ 1m 51s</sub>

### ℹ️ Info

GitHub MCP Server: Connected

---

<sub>⏱️ 1m 51s</sub>

### ℹ️ Info

Signed in successfully as tangan-harsh!

---

<sub>⏱️ 1m 51s</sub>

### ℹ️ Info

IDE connection lost: Visual Studio Code closed

---

<sub>⏱️ 2m 12s</sub>

### 👤 User

我需要你深度分析[DCL-SLAM]启动逻辑，输出结构化的启动函数调用链路，要求如下：
1. 启动链路梳理
   - 从启动文件「dcl_fast_lio_mid360.launch.py」开始，追踪所有被触发的文件和函数；
   - 标注每个函数的定义位置（文件路径+行号，若能预估）；
   - 区分「直接调用」和「间接调用」（如通过装饰器、反射、配置加载的函数）。

2. 函数关系可视化
   用Mermaid流程图语法描述核心函数调用关系，
  作出关键的中文注释
  注意mermaid语法应为函数是带括号的所以中括号里如果有函数需要加双引号
   示例格式：
   ```mermaid
   graph TD
   A[main.py:main()] --\> B[config.py:load_config()]
   A --\> C[server.py:create_app()]
   C --\> D[router.py:register_router()]
   B --\> C
3.关键函数说明对每个核心函数，说明：
    执行时机（项目启动前 / 启动中 / 启动后）；
    依赖的前置条件（如是否需要环境变量、配置文件加载完成）；
    异常处理逻辑（如函数执行失败是否导致启动中断）。

---

<sub>⏱️ 2m 23s</sub>

### 💭 Reasoning

*The user wants a deep analysis of the DCL-SLAM startup logic, starting from the launch file. Let me explore the codebase structure first.*

---

<sub>⏱️ 2m 26s</sub>

### 💬 Copilot





---

<sub>⏱️ 2m 26s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM.**

/home/isaac/DCL-SLAM

<details>
<summary>7 lines</summary>

```
.git
README_CN.md
build
install
log
replace_ros_macros.py
src
```

</details>


---

<sub>⏱️ 2m 26s</sub>

### ✅ `glob`

**/home/isaac/DCL-SLAM**

"**/*launch*" in /home/isaac/DCL-SLAM

<details>
<summary>9 matches</summary>

```
/home/isaac/DCL-SLAM/src/dcl_slam/launch/single_ugv.launch.py
/home/isaac/DCL-SLAM/src/dcl_slam/launch/single_ugv_kitti.launch
/home/isaac/DCL-SLAM/src/dcl_slam/launch/run_large.launch
/home/isaac/DCL-SLAM/src/dcl_slam/launch/dcl_fast_lio_mid360.launch.py
/home/isaac/DCL-SLAM/src/dcl_slam/launch/run.launch.py
/home/isaac/DCL-SLAM/src/dcl_slam/launch/run_kitti.launch
/home/isaac/DCL-SLAM/src/dcl_slam/launch/single_ugv.launch
/home/isaac/DCL-SLAM/src/dcl_slam/launch/run.launch
/home/isaac/DCL-SLAM/install/dcl_slam/share/dcl_slam/launch/__pycache__/dcl_fast_lio_mid360.launch.cpython-310.pyc
```

</details>


---

<sub>⏱️ 2m 26s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src.**

/home/isaac/DCL-SLAM/src

```
dcl_fast_lio
dcl_slam
distributed_mapper
libnabo
```


---

<sub>⏱️ 2m 31s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/launch/dcl_fast_lio_mid360.launch.py.**

/home/isaac/DCL-SLAM/src/dcl_slam/launch/dcl_fast_lio_mid360.launch.py

<details>
<summary>48 lines</summary>

```
1. import os
2. from ament_index_python.packages import get_package_share_directory
3. from launch import LaunchDescription
4. from launch.actions import DeclareLaunchArgument
5. from launch.substitutions import LaunchConfiguration
6. from launch.conditions import IfCondition
7. from launch_ros.actions import Node
8. 
9. 
10. def generate_launch_description():
11.     dcl_slam_dir = get_package_share_directory('dcl_slam')
12.     config_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360.yaml')
13.     rviz_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360.rviz')
14. 
15.     rviz_use = LaunchConfiguration('rviz')
16. 
17.     declare_rviz_cmd = DeclareLaunchArgument(
18.         'rviz', default_value='true',
19.         description='Launch RViz'
20.     )
21. 
22.     fastlio_node = Node(
23.         package='dcl_fast_lio',
24.         executable='fastlio_mapping',
25.         name='laserMapping',
26.         namespace='/a',
27.         parameters=[config_path],
28.         output='screen',
29.         remappings=[
30.             ('/a/livox/imu', '/livox/imu'),
31.             ('/a/livox/lidar', '/livox/lidar'),
32.         ]
33.     )
34. 
35.     rviz_node = Node(
36.         package='rviz2',
37.         executable='rviz2',
38.         arguments=['-d', rviz_path],
39.         condition=IfCondition(rviz_use)
40.     )
41. 
42.     ld = LaunchDescription()
43.     ld.add_action(declare_rviz_cmd)
44.     ld.add_action(fastlio_node)
45.     ld.add_action(rviz_node)
46. 
47.     return ld
48.
```

</details>


---

<sub>⏱️ 2m 31s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam.**

/home/isaac/DCL-SLAM/src/dcl_slam

<details>
<summary>14 lines</summary>

```
CMakeLists.txt
CMakeLists.txt.old
LICENSE
README.md
README_ROS2.md
config
dependencies.rosinstall
include
launch
log
msg
package.xml
package.xml.old
src
```

</details>


---

<sub>⏱️ 2m 31s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_fast_lio.**

/home/isaac/DCL-SLAM/src/dcl_fast_lio

<details>
<summary>12 lines</summary>

```
.gitignore
CMakeLists.txt
CMakeLists.txt.ros1
LICENSE
Log
README.md
include
msg
package.xml
package.xml.ros1
rviz_cfg
src
```

</details>


---

<sub>⏱️ 2m 36s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_fast_lio/src.**

/home/isaac/DCL-SLAM/src/dcl_fast_lio/src

```
IMU_Processing.hpp
laserMapping.cpp
preprocess.cpp
preprocess.h
```


---

<sub>⏱️ 2m 36s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src.**

/home/isaac/DCL-SLAM/src/dcl_slam/src

<details>
<summary>11 lines</summary>

```
dcl_slam_node.cpp
distributedLoopClosure.cpp
distributedMapping.cpp
distributedMappingBasis.cpp
distributedMappingVisualization.cpp
lidarIrisDescriptor.cpp
loopVisualizationNode.cpp
loopVisualizationNode.cpp.old
m2dpDescriptor.cpp
paramsServer.cpp
scanContextDescriptor.cpp
```

</details>


---

<sub>⏱️ 2m 36s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/include.**

/home/isaac/DCL-SLAM/src/dcl_slam/include

<details>
<summary>6 lines</summary>

```
descriptorBasis.h
distributedMapping.h
lidarIrisDescriptor.h
m2dpDescriptor.h
paramsServer.h
scanContextDescriptor.h
```

</details>


---

<sub>⏱️ 2m 36s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_fast_lio/include.**

/home/isaac/DCL-SLAM/src/dcl_fast_lio/include

<details>
<summary>7 lines</summary>

```
Exp_mat.h
IKFoM_toolkit
common_lib.h
ikd-Tree
matplotlibcpp.h
so3_math.h
use-ikfom.hpp
```

</details>


---

<sub>⏱️ 2m 42s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp (lines 1-80)

<details>
<summary>80 lines</summary>

```
1. // This is an advanced implementation of the algorithm described in the
2. // following paper:
3. //   J. Zhang and S. Singh. LOAM: Lidar Odometry and Mapping in Real-time.
4. //     Robotics: Science and Systems Conference (RSS). Berkeley, CA, July 2014.
5. 
6. // Modifier: Livox               dev@livoxtech.com
7. 
8. // Copyright 2013, Ji Zhang, Carnegie Mellon University
9. // Further contributions copyright (c) 2016, Southwest Research Institute
10. // All rights reserved.
11. //
12. // Redistribution and use in source and binary forms, with or without
13. // modification, are permitted provided that the following conditions are met:
14. //
15. // 1. Redistributions of source code must retain the above copyright notice,
16. //    this list of conditions and the following disclaimer.
17. // 2. Redistributions in binary form must reproduce the above copyright notice,
18. //    this list of conditions and the following disclaimer in the documentation
19. //    and/or other materials provided with the distribution.
20. // 3. Neither the name of the copyright holder nor the names of its
21. //    contributors may be used to endorse or promote products derived from this
22. //    software without specific prior written permission.
23. //
24. // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
25. // AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
26. // IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
27. // ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
28. // LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
29. // CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
30. // SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
31. // INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
32. // CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
33. // ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
34. // POSSIBILITY OF SUCH DAMAGE.
35. #include <omp.h>
36. #include <mutex>
37. #include <math.h>
38. #include <thread>
39. #include <fstream>
40. #include <csignal>
41. #include <unistd.h>
42. #include <so3_math.h>
43. #include <rclcpp/rclcpp.hpp>
44. #include <Eigen/Core>
45. #include "IMU_Processing.hpp"
46. #include <nav_msgs/msg/odometry.hpp>
47. #include <nav_msgs/msg/path.hpp>
48. #include <visualization_msgs/msg/marker.hpp>
49. #include <pcl_conversions/pcl_conversions.h>
50. #include <pcl/point_cloud.h>
51. #include <pcl/point_types.h>
52. #include <pcl/filters/voxel_grid.h>
53. #include <pcl/io/pcd_io.h>
54. #include <sensor_msgs/msg/point_cloud2.hpp>
55. #include <tf2/LinearMath/Quaternion.h>
56. #include <tf2_ros/transform_broadcaster.h>
57. #include <geometry_msgs/msg/vector3.hpp>
58. #include <geometry_msgs/msg/transform_stamped.hpp>
59. #include "preprocess.h"
60. #include <ikd-Tree/ikd_Tree.h>
61. #include "distributedMapping.h"
62. 
63. #ifdef USE_LIVOX_ROS_DRIVER2
64. #include <livox_ros_driver2/msg/custom_msg.hpp>
65. #endif
66. 
67. #define INIT_TIME           (0.1)
68. #define LASER_POINT_COV     (0.001)
69. #define MAXN                (720000)
70. #define PUBFRAME_PERIOD     (20)
71. 
72. inline builtin_interfaces::msg::Time to_ros_time(double sec)
73. {
74.     builtin_interfaces::msg::Time t;
75.     t.sec = static_cast<int32_t>(sec);
76.     t.nanosec = static_cast<uint32_t>((sec - t.sec) * 1e9);
77.     return t;
78. }
79. 
80. /*** Time Log Variables ***/
```

</details>


---

<sub>⏱️ 2m 42s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/dcl_slam_node.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/dcl_slam_node.cpp

<details>
<summary>15 lines</summary>

```
1. #include "distributedMapping.h"
2. #include <rclcpp/rclcpp.hpp>
3. 
4. int main(int argc, char** argv)
5. {
6.     rclcpp::init(argc, argv);
7.     auto node = std::make_shared<distributedMapping>();
8.     
9.     // distributedMapping has internal timers/threads, so we just spin
10.     rclcpp::spin(node);
11.     
12.     rclcpp::shutdown();
13.     return 0;
14. }
15.
```

</details>


---

<sub>⏱️ 2m 42s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/include/paramsServer.h.**

/home/isaac/DCL-SLAM/src/dcl_slam/include/paramsServer.h

<details>
<summary>169 lines</summary>

```
1. #ifndef _PARAM_SERVER_H_
2. #define _PARAM_SERVER_H_
3. 
4. #include <rclcpp/rclcpp.hpp>
5. #include <geometry_msgs/msg/transform.hpp>
6. // dcl_slam define
7. #include "dcl_slam/msg/neighbor_estimate.hpp"
8. // pcl
9. #include <pcl/registration/icp.h>
10. #include <pcl_conversions/pcl_conversions.h>
11. // mapping
12. #include <gtsam/geometry/Rot3.h>
13. #include <gtsam/geometry/Pose3.h>
14. 
15. // descriptors
16. #include "dcl_slam/msg/global_descriptor.hpp"
17. #include "dcl_slam/msg/loop_info.hpp"
18. #include "std_msgs/msg/int8.hpp"
19. 
20. using namespace gtsam;
21. using namespace std;
22. 
23. typedef pcl::PointXYZI PointPose3D;
24. struct PointPose6D
25. {
26.     float x;
27. 	float y;
28. 	float z;
29. 	float intensity;
30.     float roll;
31.     float pitch;
32.     float yaw;
33.     double time;
34. };
35. POINT_CLOUD_REGISTER_POINT_STRUCT  (PointPose6D,
36.                                    (float, x, x) (float, y, y)
37.                                    (float, z, z) (float, intensity, intensity)
38.                                    (float, roll, roll) (float, pitch, pitch) (float, yaw, yaw)
39.                                    (double, time, time))
40. 
41. struct singleRobot {
42. 	/*** robot information ***/
43. 	int id_; // robot id
44. 	std::string name_; // robot name, for example, 'a', 'b', etc.
45. 	std::string odom_frame_; // odom frame
46. 
47. 	/*** ros subscriber and publisher ***/
48. 	// mapping
49. 	rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr sub_optimization_state, sub_pose_estimate_state, sub_rotation_estimate_state;
50. 	rclcpp::Subscription<dcl_slam::msg::NeighborEstimate>::SharedPtr sub_neighbor_rotation_estimates, sub_neighbor_pose_estimates;
51. 	rclcpp::Publisher<std_msgs::msg::Int8>::SharedPtr pub_optimization_state, pub_pose_estimate_state, pub_rotation_estimate_state;
52. 	rclcpp::Publisher<dcl_slam::msg::NeighborEstimate>::SharedPtr pub_neighbor_rotation_estimates, pub_neighbor_pose_estimates;
53. 	// loop closure
54. 	rclcpp::Subscription<dcl_slam::msg::LoopInfo>::SharedPtr sub_loop_info;
55. 	rclcpp::Publisher<dcl_slam::msg::LoopInfo>::SharedPtr pub_loop_info;
56. 	// descriptors
57. 	rclcpp::Subscription<dcl_slam::msg::GlobalDescriptor>::SharedPtr sub_descriptors;
58. 	rclcpp::Publisher<dcl_slam::msg::GlobalDescriptor>::SharedPtr pub_descriptors;
59. 
60. 	/*** other ***/
61. 	rclcpp::Time time_cloud_input_stamp; // recent keyframe timestamp
62. 	double time_cloud_input; // and its double type
63. 	dcl_slam::msg::NeighborEstimate estimate_msg; // pose and rotation estimate msg
64. 	pcl::PointCloud<PointPose3D>::Ptr keyframe_cloud; // recent keyframe pointcloud
65. 	std::vector<pcl::PointCloud<PointPose3D>> keyframe_cloud_array; // and its array
66. 	Pose3 piror_odom; // piror factor
67. };
68. 
69. enum class LiDARType { VELODYNE, LIVOX };
70. enum class DescriptorType { ScanContext, LidarIris, M2DP };
71. enum class OptimizerState { Idle, Start, Initialization, RotationEstimation, 
72. 	PoseEstimationInitialization, PoseEstimation, End, PostEndingCommunicationDelay };
73. 
74. /*** class paramsServer ***/ 
75. class paramsServer : public rclcpp::Node
76. {
77. 	public:
78. 		paramsServer(std::string node_name = "dcl_slam_node");
79. 
80. 		Eigen::Affine3f gtsamPoseToAffine3f(
81. 			gtsam::Pose3 pose);
82. 
83. 		geometry_msgs::msg::Transform gtsamPoseToTransform(
84. 			gtsam::Pose3 pose);
85. 
86. 		gtsam::Pose3 transformToGtsamPose(
87. 			const geometry_msgs::msg::Transform& pose);
88. 
89. 		gtsam::Pose3 pclPointTogtsamPose3(
90. 			PointPose6D point);
91. 
92. 		pcl::PointCloud<PointPose3D>::Ptr transformPointCloud(
93. 			pcl::PointCloud<PointPose3D> cloud_in,
94. 			PointPose6D* pose);
95. 		
96. 		pcl::PointCloud<PointPose3D>::Ptr transformPointCloud(
97. 			pcl::PointCloud<PointPose3D> cloud_in,
98. 			gtsam::Pose3 pose);
99. 
100. 	protected:
101. 		// robot team
102. 		int number_of_robots_; // number of robots in robot team
103. 
104. 		// robot info
105. 		std::string name_; // this robot name
106. 		int id_; // this robot id
107. 
108. 		// frames name
109. 		std::string world_frame_; // global frame
110. 		std::string odom_frame_; // local frame
111. 
112. 		// lidar Sensor Configuration
113. 		LiDARType sensor_; // lidar type, support 'velodyne 16/64' or 'livox 6'
114. 		int n_scan_; // number of lidar channel (i.e., 6, 16, 64)
115. 
116. 		// CPU params
117. 		int onboard_cpu_cores_num_; // cores number of onboard unit
118. 		float loop_closure_process_interval_; // interval of detecting loop (in second)
119. 		float map_publish_interval_; // interval of publish global maps (in second)
120. 		float mapping_process_interval_; // interval of optmization (in second)
121. 
122. 		// Mapping
123. 		bool global_optmization_enable_; // enable distributed DGS
124. 		bool use_pcm_; // enable pairwise consistency maximization (PCM)
125. 		float pcm_threshold_; // confidence probability for PCM (i.e., 0.01, 0.05, 0.1, 0.25, 0.5, 0.75)
126. 		int optmization_maximum_iteration_; // maximum iterations time of optimization
127. 		bool use_between_noise_; // use between noise flag
128. 		int fail_safe_steps_; // steps of fail safe to abort (depend on both fail_safe_wait_time_ and mapping_process_interval_)
129. 		float fail_safe_wait_time_; // wait time for fail safe (in second)
130. 		float rotation_estimate_change_threshold_;  // difference between rotation estimate provides an early stopping condition
131. 		float pose_estimate_change_threshold_; // difference between pose estimate provides an early stopping condition
132. 		float gamma_; // gamma value for over relaxation methods
133. 		bool use_flagged_init_; // to use flagged initialization or not
134. 		bool use_landmarks_; // use landmarks -- landmarks are given symbols as upper case of robot name
135. 		bool use_heuristics_; // use heuristics-based algorithm for the max-clique solver
136. 
137. 		// keyframe
138. 		float keyframe_distance_threshold_; // keyframe distance threshold (in meter)
139. 		float keyframe_angle_threshold_; // keyframe angle threshold (in rad)
140. 
141. 		// downsample
142. 		float map_leaf_size_; // scan to map matching downsample rate (default 0.4)
143. 		float descript_leaf_size_; // descriptor downsample rate (default 0.1)
144. 
145. 		// loop closure
146. 		bool intra_robot_loop_closure_enable_; // enable to search intra-robot loop closre with global descriptor
147. 		bool inter_robot_loop_closure_enable_; // enable to search intra-robot loop closre with global descriptor
148. 		DescriptorType descriptor_type_num_; // descriptor type: ScanContext, LidarIris, M2DP
149. 		int knn_candidates_; // k nearest neighbor search of row key
150. 		int exclude_recent_frame_num_; // exclude recent keyframe in intra-robot loop closure
151. 		float search_radius_; // radius of radius search based intra-robot loop closure
152. 		int match_mode_; // iris-feature matching mode, (i.e., 0, 1, 2; default 2) 
153. 		int iris_row_; // iris-image row
154. 		int iris_column_; // iris-image column
155. 		float descriptor_distance_threshold_; // iris-feature match threshold
156. 		int ransac_maximum_iteration_; // RANSAC maximum iteration time
157. 		float ransac_threshold_; // RANSAC threshold (rate: [0 1])
158. 		float ransac_outlier_reject_threshold_; // RANSAC outlier rejection distancce
159. 		int history_keyframe_search_num_; // number of history frames in submap for scan-to-map matching
160. 		float fitness_score_threshold_; // ICP fitness score threshold
161. 
162. 		// visualization
163. 		float global_map_visualization_radius_; // radius of radius search based intra-robot loop closure
164. 
165. 		// Save pcd
166. 		std::string save_directory_;
167. };
168. 
169. #endif
```

</details>


---

<sub>⏱️ 2m 42s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/include/distributedMapping.h.**

/home/isaac/DCL-SLAM/src/dcl_slam/include/distributedMapping.h

<details>
<summary>332 lines</summary>

```
1. #ifndef _DISTRIBUTED_MAPPING_H_
2. #define _DISTRIBUTED_MAPPING_H_
3. 
4. #include <rclcpp/rclcpp.hpp>
5. #include <nav_msgs/msg/odometry.hpp>
6. #include <nav_msgs/msg/path.hpp>
7. #include <std_msgs/msg/int8.hpp>
8. #include <visualization_msgs/msg/marker.hpp>
9. #include <visualization_msgs/msg/marker_array.hpp>
10. #include <tf2/LinearMath/Quaternion.h>
11. #include <tf2_ros/transform_listener.h>
12. #include <tf2_ros/transform_broadcaster.h>
13. #include <flann/flann.hpp>
14. #include <thread>
15. #include <deque>
16. // dcl_slam define
17. #include "paramsServer.h"
18. #include "scanContextDescriptor.h"
19. #include "lidarIrisDescriptor.h"
20. #include "m2dpDescriptor.h"
21. #include "dcl_slam/msg/loop_info.hpp"
22. #include "dcl_slam/msg/global_descriptor.hpp"
23. #include "dcl_slam/msg/neighbor_estimate.hpp"
24. // pcl
25. #include <pcl/filters/voxel_grid.h>
26. #include <pcl/kdtree/kdtree_flann.h>
27. #include <pcl/registration/icp.h>
28. #include <pcl_conversions/pcl_conversions.h>
29. #include <pcl/registration/correspondence_estimation.h>
30. #include <pcl/registration/correspondence_rejection_sample_consensus.h>
31. // mapping
32. #include "distributed_mapper/distributed_mapper.h"
33. #include "distributed_mapper/distributed_mapper_utils.h"
34. #include <gtsam/nonlinear/ISAM2.h>
35. // file iostream
36. #include <fstream>
37. #include <iostream>
38. // log
39. #include <glog/logging.h>
40. 
41. using namespace gtsam;
42. using namespace std;
43. 
44. class distributedMapping : public paramsServer
45. {
46. 	public:
47. 		distributedMapping();
48. 
49. 		~distributedMapping();
50. 
51. 		pcl::PointCloud<PointPose3D>::Ptr getLocalKeyposesCloud3D();
52. 
53. 		pcl::PointCloud<PointPose6D>::Ptr getLocalKeyposesCloud6D();
54. 
55. 		pcl::PointCloud<PointPose3D> getLocalKeyframe(const int& index);
56. 
57. 		Pose3 getLatestEstimate();
58. 
59. 		void lockOnCall();
60. 
61. 		void unlockOnCall();
62. 
63. 		void performDistributedMapping(
64. 			const Pose3& pose_to,
65. 			const pcl::PointCloud<PointPose3D>::Ptr frame_to,
66. 			const rclcpp::Time& timestamp);
67. 
68. 		bool saveFrame(
69. 			const Pose3& pose_to);
70. 
71. 		void updateLocalPath(
72. 			const PointPose6D& pose);
73. 
74. 		bool updatePoses();
75. 
76. 		void makeDescriptors();
77. 
78. 		void publishPath();
79. 
80. 		void publishTransformation(
81. 			const rclcpp::Time& timestamp);
82. 
83. 		void loopClosureThread();
84. 
85. 		void globalMapThread();
86. 
87. 	private:
88. 		void poseCovariance2msg(
89. 			const graph_utils::PoseWithCovariance& pose,
90. 			geometry_msgs::msg::PoseWithCovariance& msg);
91. 
92. 		void msg2poseCovariance(
93. 			const geometry_msgs::msg::PoseWithCovariance& msg,
94. 			graph_utils::PoseWithCovariance& pose);
95. 
96. 		void globalDescriptorHandler(
97. 			const dcl_slam::msg::GlobalDescriptor::SharedPtr msg,
98. 			const int& id);
99. 
100. 		void loopInfoHandler(
101. 			const dcl_slam::msg::LoopInfo::SharedPtr msg,
102. 			const int& id);
103. 
104. 		void optStateHandler(
105. 			const std_msgs::msg::Int8::SharedPtr msg,
106. 			const int& id);
107. 
108. 		void rotationStateHandler(
109. 			const std_msgs::msg::Int8::SharedPtr msg,
110. 			const int& id);
111. 
112. 		void poseStateHandler(
113. 			const std_msgs::msg::Int8::SharedPtr msg,
114. 			const int& id);
115. 
116. 		void neighborRotationHandler(
117. 			const dcl_slam::msg::NeighborEstimate::SharedPtr msg,
118. 			const int& id);
119. 
120. 		void neighborPoseHandler(
121. 			const dcl_slam::msg::NeighborEstimate::SharedPtr msg,
122. 			const int& id);
123. 
124. 		void updatePoseEstimateFromNeighbor(
125. 			const int& rid,
126. 			const Key& key,
127. 			const graph_utils::PoseWithCovariance& pose);
128. 
129. 		bool startOptimizationCondition();
130. 
131. 		void updateOptimizer();
132. 
133. 		void outliersFiltering();
134. 
135. 		void computeOptimizationOrder();
136. 
137. 		void initializePoseGraphOptimization();
138. 
139. 		bool rotationEstimationStoppingBarrier();
140. 
141. 		void abortOptimization(
142. 			const bool& log_info);
143. 
144. 		void removeInactiveNeighbors();
145. 
146. 		void failSafeCheck();
147. 
148. 		void initializePoseEstimation();
149. 
150. 		bool poseEstimationStoppingBarrier();
151. 
152. 		void updateGlobalPath(
153. 			const Pose3& pose_in);
154. 
155. 		void incrementalInitialGuessUpdate();
156. 
157. 		void endOptimization();
158. 
159. 		void changeOptimizerState(
160. 			const OptimizerState& state);
161. 
162. 		void run();
163. 
164. 		void performRSIntraLoopClosure();
165. 
166. 		int detectLoopClosureDistance(
167. 			const int& cur_ptr);
168. 		
169. 		void performIntraLoopClosure();
170. 
171. 		void calculateTransformation(
172. 			const int& loop_key_cur,
173. 			const int& loop_key_pre);
174. 
175. 		void loopFindNearKeyframes(
176. 			pcl::PointCloud<PointPose3D>::Ptr& near_keyframes,
177. 			const int& key, const int& search_num);
178. 
179. 		void performInterLoopClosure();
180. 
181. 		void performExternLoopClosure();
182. 
183. 		void loopFindGlobalNearKeyframes(
184. 			pcl::PointCloud<PointPose3D>::Ptr& near_keyframes,
185. 			const int& key, const int& search_num);
186. 
187. 		void publishGlobalMap();
188. 
189. 		void publishLoopClosureConstraint();
190. 
191. 	public:
192. 		mutex lock_on_call; // lock on odometry
193. 
194. 	private:
195. 		/*** robot team ***/
196. 		vector<singleRobot> robots;
197. 
198. 		/*** ros subscriber and publisher ***/
199. 		rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr pub_loop_closure_constraints;
200. 		rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_scan_of_scan2map, pub_map_of_scan2map;
201. 		rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_global_map;
202. 		rclcpp::Publisher<nav_msgs::msg::Path>::SharedPtr pub_global_path, pub_local_path;
203. 		rclcpp::Publisher<sensor_msgs::msg::PointCloud2>::SharedPtr pub_keypose_cloud;
204. 
205. 		/*** ros service ***/
206. 
207. 		/*** message information ***/
208. 		pcl::PointCloud<PointPose3D>::Ptr cloud_for_decript_ds; // input cloud for descriptor
209. 		deque<pair<int, dcl_slam::msg::GlobalDescriptor>> store_descriptors;
210. 
211. 		std_msgs::msg::Int8 state_msg; // optimization state msg
212. 
213. 		dcl_slam::msg::GlobalDescriptor global_descriptor_msg; // descriptor message
214. 		
215. 		nav_msgs::msg::Path local_path; // path in local frame
216. 		nav_msgs::msg::Path global_path; // path in global frame
217. 
218. 		/*** downsample filter ***/
219. 		pcl::VoxelGrid<PointPose3D> downsample_filter_for_descriptor;
220. 		pcl::VoxelGrid<PointPose3D> downsample_filter_for_intra_loop;
221. 		pcl::VoxelGrid<PointPose3D> downsample_filter_for_inter_loop;
222. 		pcl::VoxelGrid<PointPose3D> downsample_filter_for_inter_loop2;
223. 		pcl::VoxelGrid<PointPose3D> downsample_filter_for_inter_loop3;
224. 
225. 		/*** mutex ***/
226. 		// vector<mutex> lock_on_call; // lock on odometry
227. 
228. 		/*** distributed loopclosure ***/
229. 		int intra_robot_loop_ptr; // current position pointer for intra-robot loop
230. 		int inter_robot_loop_ptr; // current position pointer for inter-robot loop
231. 
232. 		bool intra_robot_loop_close_flag; // intra-robot loop is detected
233. 
234. 		unique_ptr<scan_descriptor> keyframe_descriptor; // descriptor for keyframe pointcloud
235. 
236. 		deque<dcl_slam::msg::LoopInfo> loop_closures_candidates; // loop closures need to verify
237. 
238. 		// radius search for intra-robot loop closure
239. 		pcl::PointCloud<PointPose3D>::Ptr copy_keyposes_cloud_3d; // copy of local 3-dof keyposes
240. 		pcl::PointCloud<PointPose6D>::Ptr copy_keyposes_cloud_6d; // copy of local 6-dof keyposes
241. 
242. 		pcl::KdTreeFLANN<PointPose3D>::Ptr kdtree_history_keyposes; // kdtree for searching history keyposes
243. 
244. 		map<int, int> loop_indexs;
245. 		map<Symbol, Symbol> loop_indexes;
246. 
247. 		/*** noise model ***/
248. 		noiseModel::Diagonal::shared_ptr odometry_noise; // odometry factor noise
249. 		noiseModel::Diagonal::shared_ptr prior_noise; // prior factor noise
250. 
251. 		/*** local pose graph optmazition ***/
252. 		ISAM2 *isam2; // isam2 optimizer
253. 
254. 		NonlinearFactorGraph isam2_graph; // local pose graph for isam2
255. 		Values isam2_initial_values; // local initial values for isam2
256. 
257. 		Values isam2_current_estimates; // current estimates for isam2
258. 		Pose3 isam2_keypose_estimate; // keypose estimate for isam2
259. 
260. 		pcl::PointCloud<PointPose3D>::Ptr keyposes_cloud_3d; // 3-dof keyposes in local frame
261. 		pcl::PointCloud<PointPose6D>::Ptr keyposes_cloud_6d; // 6-dof keyposes in local frame
262. 
263. 		/*** distributed pose graph optmazition ***/
264. 		rclcpp::TimerBase::SharedPtr distributed_mapping_thread; // thread for running distributed mapping
265. 		std::thread loop_closure_thread_; // thread for loop closure
266. 		boost::shared_ptr<distributed_mapper::DistributedMapper> optimizer; // distributed mapper (DGS)
267. 
268. 		int steps_of_unchange_graph; // stop optimization 
269. 
270. 		// measurements
271. 		boost::shared_ptr<NonlinearFactorGraph> local_pose_graph; // pose graph for distributed mapping
272. 		boost::shared_ptr<Values> initial_values; // initial values for distributed mapping
273. 		GraphAndValues graph_values_vec; // vector of pose graph and initial values
274. 
275. 		bool graph_disconnected; // pose graph is not connected to others
276. 
277. 		int lowest_id_included; // lowest id in this robot
278. 		int lowest_id_to_included; // lowest id to be included in this robot
279. 		int prior_owner; // the robot that own prior factor
280. 		bool prior_added; // this robot have add prior factor
281. 
282. 		gtsam::Matrix adjacency_matrix; // adjacency matrix of robot team
283. 		vector<int> optimization_order; // optimization order of robot team
284. 		bool in_order; // this robot in optimization order
285. 
286. 		// this robot
287. 		OptimizerState optimizer_state; // current state of optimizer
288. 		int optimization_steps; // steps in optimization
289. 		bool sent_start_optimization_flag; // ready for optimization
290. 
291. 		int current_rotation_estimate_iteration; // current iteration time of rotation estimate
292. 		int current_pose_estimate_iteration; // current iteration time of pose estimate
293. 
294. 		double latest_change; // latest change of estimate
295. 		int steps_without_change; // setps of estimate without change
296. 
297. 		bool rotation_estimate_start; // rotation estimate is start
298. 		bool pose_estimate_start; // pose estimate is start
299. 		bool rotation_estimate_finished; // rotation estimate is finished
300. 		bool pose_estimate_finished; // pose estimate is finished
301. 		bool estimation_done; // estimate is done
302. 
303. 		Point3 anchor_offset, anchor_point; // anchor offset
304. 
305. 		// neighbors
306. 		set<char> neighboring_robots; // neighbors (name) within communication range
307. 		set<int> neighbors_within_communication_range; // neighbors (id) within communication range
308. 		map<int, bool> neighbors_started_optimization; // neighbors ready for optimization
309. 		map<int, OptimizerState> neighbor_state; // current state of neighbors optimizer
310. 
311. 		map<int, bool> neighbors_rotation_estimate_finished; // neighbors rotation estimate is finished
312. 		map<int, bool> neighbors_pose_estimate_finished; // neighbors pose estimate is finished
313. 		map<int, bool> neighbors_estimation_done; // neighbors estimate is done
314. 
315. 		map<int, int> neighbors_lowest_id_included; // lowest id in neighbors
316. 		map<int, Point3> neighbors_anchor_offset; // neighbors anchor offset
317. 
318. 		// distributed pairwise consistency maximization
319. 		robot_measurements::RobotLocalMap robot_local_map; // local loop closures and transform 
320. 		robot_measurements::RobotLocalMap robot_local_map_backup; // backups in case of abort
321. 
322. 		boost::shared_ptr<NonlinearFactorGraph> local_pose_graph_no_filtering; // pose graph without pcm
323. 
324. 		map<int, graph_utils::Trajectory> pose_estimates_from_neighbors; // pose estimates of neighbors
325. 		set<Key> other_robot_keys_for_optimization; // keys of neighbors for optimization
326. 
327. 		set<pair<Key, Key>> accepted_keys, rejected_keys; // accepted and rejected pairs
328. 		int measurements_accepted_num, measurements_rejected_num;
329. };
330. 
331. #endif
332.
```

</details>


---

<sub>⏱️ 2m 45s</sub>

### 💬 Copilot

Now let me look at the key implementation files for the constructor and initialization logic:

---

<sub>⏱️ 2m 47s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/paramsServer.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/paramsServer.cpp

<details>
<summary>322 lines</summary>

```
1. #include "paramsServer.h"
2. 
3. /*
4.  * 构造函数
5.  * 作用：初始化参数服务器，从ROS参数服务器加载各种配置参数。
6.  * 实现机制：
7.  * 1. 获取机器人命名空间和ID。
8.  * 2. 加载机器人数量、坐标系名称、雷达配置等参数。
9.  * 3. 加载CPU、建图、降采样、回环检测、关键帧及可视化相关的参数。
10.  */
11. paramsServer::paramsServer(std::string node_name) : Node(node_name)
12. {
13. 	// robot info
14. 	// In ROS2, namespace is handled by node configuration, but we can get it.
15. 	std::string ns = this->get_namespace(); 
16. 	
17. 	if (ns.length() >= 2 && ns[1] >= 'a' && ns[1] <= 'z')
18. 	{
19. 		name_ = ns;
20. 		id_ = ns[1] - 'a';
21. 		RCLCPP_INFO(this->get_logger(), "Robot Name: %s, ID: %d", name_.c_str(), id_);
22. 	}
23. 	else
24. 	{
25. 		// Fallback: use parameter 'robot_id'
26. 		this->declare_parameter("robot_id", 0);
27. 		id_ = this->get_parameter("robot_id").as_int();
28. 		name_ = std::string("/") + (char)('a' + id_);
29. 		RCLCPP_WARN(this->get_logger(), "Invalid namespace '%s'. Using fallback ID: %d, Name: %s", ns.c_str(), id_, name_.c_str());
30. 	}
31. 	
32. 	// Param loading
33. 	this->declare_parameter("number_of_robots", 1);
34. 	number_of_robots_ = this->get_parameter("number_of_robots").as_int();
35. 
36. 	if(number_of_robots_ < 1)
37. 	{
38. 		RCLCPP_ERROR(this->get_logger(), "Invalid robot number (must be positive number): %d", number_of_robots_);
39. 		rclcpp::shutdown();
40. 	}
41. 
42. 	// Assuming params are flat or under "dcl_slam" if declared so. 
43. 	// For simplicity, we declare them flatly as node parameters.
44. 	
45. 	this->declare_parameter("world_frame", "world");
46. 	world_frame_ = this->get_parameter("world_frame").as_string();
47. 	
48. 	this->declare_parameter("odom_frame", "map");
49. 	odom_frame_ = this->get_parameter("odom_frame").as_string();
50. 
51. 	// lidar configuration
52. 	this->declare_parameter("sensor", "velodyne");
53. 	std::string sensorStr = this->get_parameter("sensor").as_string();
54. 	
55. 	if(sensorStr == "velodyne")
56. 	{
57. 		sensor_ = LiDARType::VELODYNE;
58. 	}
59. 	else if(sensorStr == "livox")
60. 	{
61. 		sensor_ = LiDARType::LIVOX;
62. 	}
63. 	else
64. 	{
65. 		RCLCPP_ERROR(this->get_logger(), "Invalid sensor type (must be either 'velodyne' or 'ouster'): %s ", sensorStr.c_str());
66. 		rclcpp::shutdown();
67. 	}
68. 	
69. 	this->declare_parameter("n_scan", 16);
70. 	n_scan_ = this->get_parameter("n_scan").as_int();
71. 
72. 	// CPU Params
73. 	this->declare_parameter("onboard_cpu_cores_num", 4);
74. 	onboard_cpu_cores_num_ = this->get_parameter("onboard_cpu_cores_num").as_int();
75. 	
76. 	this->declare_parameter("loop_closure_process_interval", 0.02);
77. 	loop_closure_process_interval_ = this->get_parameter("loop_closure_process_interval").as_double();
78. 	
79. 	this->declare_parameter("map_publish_interval", 10.0);
80. 	map_publish_interval_ = this->get_parameter("map_publish_interval").as_double();
81. 	
82. 	this->declare_parameter("mapping_process_interval", 0.1);
83. 	mapping_process_interval_ = this->get_parameter("mapping_process_interval").as_double();
84. 
85. 	// mapping
86. 	this->declare_parameter("global_optmization_enable", false);
87. 	global_optmization_enable_ = this->get_parameter("global_optmization_enable").as_bool();
88. 	
89. 	this->declare_parameter("use_pcm", false);
90. 	use_pcm_ = this->get_parameter("use_pcm").as_bool();
91. 	
92. 	this->declare_parameter("pcm_threshold", 0.75);
93. 	pcm_threshold_ = this->get_parameter("pcm_threshold").as_double();
94. 	
95. 	this->declare_parameter("use_between_noise", false);
96. 	use_between_noise_ = this->get_parameter("use_between_noise").as_bool();
97. 	
98. 	this->declare_parameter("optmization_maximum_iteration", 100);
99. 	optmization_maximum_iteration_ = this->get_parameter("optmization_maximum_iteration").as_int();
100. 	
101. 	this->declare_parameter("failsafe_wait_time", 1.0);
102. 	fail_safe_wait_time_ = this->get_parameter("failsafe_wait_time").as_double();
103. 	
104. 	fail_safe_steps_ = fail_safe_wait_time_/mapping_process_interval_;
105. 	
106. 	this->declare_parameter("rotation_estimate_change_threshold", 0.1);
107. 	rotation_estimate_change_threshold_ = this->get_parameter("rotation_estimate_change_threshold").as_double();
108. 	
109. 	this->declare_parameter("pose_estimate_change_threshold", 0.1);
110. 	pose_estimate_change_threshold_ = this->get_parameter("pose_estimate_change_threshold").as_double();
111. 	
112. 	this->declare_parameter("gamma", 1.0);
113. 	gamma_ = this->get_parameter("gamma").as_double();
114. 	
115. 	this->declare_parameter("use_flagged_init", true);
116. 	use_flagged_init_ = this->get_parameter("use_flagged_init").as_bool();
117. 	
118. 	this->declare_parameter("use_landmarks", false);
119. 	use_landmarks_ = this->get_parameter("use_landmarks").as_bool();
120. 	
121. 	this->declare_parameter("use_heuristics", true);
122. 	use_heuristics_ = this->get_parameter("use_heuristics").as_bool();
123. 
124. 	// downsample
125. 	this->declare_parameter("map_leaf_size", 0.4);
126. 	map_leaf_size_ = this->get_parameter("map_leaf_size").as_double();
127. 	
128. 	this->declare_parameter("descript_leaf_size", 0.1);
129. 	descript_leaf_size_ = this->get_parameter("descript_leaf_size").as_double();
130. 	
131. 	// loop closure
132. 	this->declare_parameter("intra_robot_loop_closure_enable", true);
133. 	intra_robot_loop_closure_enable_ = this->get_parameter("intra_robot_loop_closure_enable").as_bool();
134. 	
135. 	this->declare_parameter("inter_robot_loop_closure_enable", true);
136. 	inter_robot_loop_closure_enable_ = this->get_parameter("inter_robot_loop_closure_enable").as_bool();
137. 	
138. 	this->declare_parameter("descriptor_type", "");
139. 	std::string descriptor_type_ = this->get_parameter("descriptor_type").as_string();
140. 	
141. 	if(descriptor_type_ == "ScanContext")
142. 	{
143. 		descriptor_type_num_ = DescriptorType::ScanContext;
144. 	}
145. 	else if(descriptor_type_ == "LidarIris")
146. 	{
147. 		descriptor_type_num_ = DescriptorType::LidarIris;
148. 	}
149. 	else if(descriptor_type_ == "M2DP")
150. 	{
151. 		descriptor_type_num_ = DescriptorType::M2DP;
152. 	}
153. 	else
154. 	{
155. 		inter_robot_loop_closure_enable_ = false;
156. 		RCLCPP_WARN(this->get_logger(), "Invalid descriptor type: %s, turn off interloop...", descriptor_type_.c_str());
157. 	}
158. 	
159. 	this->declare_parameter("knn_candidates", 10);
160. 	knn_candidates_ = this->get_parameter("knn_candidates").as_int();
161. 	
162. 	this->declare_parameter("exclude_recent_frame_num", 30);
163. 	exclude_recent_frame_num_ = this->get_parameter("exclude_recent_frame_num").as_int();
164. 	
165. 	this->declare_parameter("search_radius", 15.0);
166. 	search_radius_ = this->get_parameter("search_radius").as_double();
167. 	
168. 	this->declare_parameter("match_mode", 2);
169. 	match_mode_ = this->get_parameter("match_mode").as_int();
170. 	
171. 	this->declare_parameter("iris_row", 80);
172. 	iris_row_ = this->get_parameter("iris_row").as_int();
173. 	
174. 	this->declare_parameter("iris_column", 360);
175. 	iris_column_ = this->get_parameter("iris_column").as_int();
176. 	
177. 	this->declare_parameter("descriptor_distance_threshold", 0.4);
178. 	descriptor_distance_threshold_ = this->get_parameter("descriptor_distance_threshold").as_double();
179. 	
180. 	this->declare_parameter("history_keyframe_search_num", 16);
181. 	history_keyframe_search_num_ = this->get_parameter("history_keyframe_search_num").as_int();
182. 	
183. 	this->declare_parameter("fitness_score_threshold", 0.2);
184. 	fitness_score_threshold_ = this->get_parameter("fitness_score_threshold").as_double();
185. 	
186. 	this->declare_parameter("ransac_maximum_iteration", 1000);
187. 	ransac_maximum_iteration_ = this->get_parameter("ransac_maximum_iteration").as_int();
188. 	
189. 	this->declare_parameter("ransac_threshold", 0.5);
190. 	ransac_threshold_ = this->get_parameter("ransac_threshold").as_double();
191. 	
192. 	this->declare_parameter("ransac_outlier_reject_threshold", 0.05);
193. 	ransac_outlier_reject_threshold_ = this->get_parameter("ransac_outlier_reject_threshold").as_double();
194. 
195. 	// keyframe params
196. 	this->declare_parameter("keyframe_distance_threshold", 1.0);
197. 	keyframe_distance_threshold_ = this->get_parameter("keyframe_distance_threshold").as_double();
198. 	
199. 	this->declare_parameter("keyframe_angle_threshold", 0.2);
200. 	keyframe_angle_threshold_ = this->get_parameter("keyframe_angle_threshold").as_double();
201. 
202. 	// visualization
203. 	this->declare_parameter("global_map_visualization_radius", 60.0);
204. 	global_map_visualization_radius_ = this->get_parameter("global_map_visualization_radius").as_double();
205. 
206. 	// output directory
207. 	this->declare_parameter("save_directory", "/dcl_output");
208. 	save_directory_ = this->get_parameter("save_directory").as_string();
209. }
210. 
211. 
212. /*
213.  * 函数名：gtsamPoseToAffine3f
214.  * 作用：将 GTSAM 的 Pose3 类型转换为 Eigen 的 Affine3f 类型。
215.  * 实现机制：利用 pcl::getTransformation 函数，提取 Pose3 的平移 (x, y, z) 和旋转 (roll, pitch, yaw) 分量，构建仿射变换矩阵。
216.  */
217. Eigen::Affine3f paramsServer::gtsamPoseToAffine3f(gtsam::Pose3 pose)
218. { 
219. 	return pcl::getTransformation(pose.translation().x(), pose.translation().y(), pose.translation().z(), 
220. 		pose.rotation().roll(), pose.rotation().pitch(), pose.rotation().yaw());
221. }
222. 
223. /*
224.  * 函数名：gtsamPoseToTransform
225.  * 作用：将 GTSAM 的 Pose3 类型转换为 geometry_msgs::Transform 消息类型。
226.  * 实现机制：提取 Pose3 的平移部分赋值给 translation，提取旋转部分并转换为四元数赋值给 rotation。
227.  */
228. geometry_msgs::msg::Transform paramsServer::gtsamPoseToTransform(gtsam::Pose3 pose)
229. {
230. 	geometry_msgs::msg::Transform transform_msg;
231. 	transform_msg.translation.x = pose.translation().x();
232. 	transform_msg.translation.y = pose.translation().y();
233. 	transform_msg.translation.z = pose.translation().z();
234. 	transform_msg.rotation.w = pose.rotation().toQuaternion().w();
235. 	transform_msg.rotation.x = pose.rotation().toQuaternion().x();
236. 	transform_msg.rotation.y = pose.rotation().toQuaternion().y();
237. 	transform_msg.rotation.z = pose.rotation().toQuaternion().z();
238. 
239. 	return transform_msg;
240. }
241. 
242. 
243. /*
244.  * 函数名：transformToGtsamPose
245.  * 作用：将 geometry_msgs::Transform 消息类型转换为 GTSAM 的 Pose3 类型。
246.  * 实现机制：利用消息中的四元数和平移向量，构造 gtsam::Rot3 和 gtsam::Point3，进而生成 gtsam::Pose3 对象。
247.  */
248. gtsam::Pose3 paramsServer::transformToGtsamPose(const geometry_msgs::msg::Transform& pose)
249. {
250. 	return gtsam::Pose3(gtsam::Rot3::Quaternion(pose.rotation.w, pose.rotation.x, pose.rotation.y, pose.rotation.z), 
251. 		gtsam::Point3(pose.translation.x, pose.translation.y, pose.translation.z));
252. }
253. 
254. 
255. /*
256.  * 函数名：pclPointTogtsamPose3
257.  * 作用：将自定义的点云位姿结构 PointPose6D 转换为 GTSAM 的 Pose3 类型。
258.  * 实现机制：利用 PointPose6D 中的欧拉角 (roll, pitch, yaw) 和位置 (x, y, z)，分别构造旋转和平移对象，生成 Pose3。
259.  */
260. gtsam::Pose3 paramsServer::pclPointTogtsamPose3(PointPose6D point)
261. {
262. 	return gtsam::Pose3(gtsam::Rot3::RzRyRx(double(point.roll), double(point.pitch), double(point.yaw)),
263. 		gtsam::Point3(double(point.x), double(point.y), double(point.z)));
264. }
265. 
266. 
267. /*
268.  * 函数名：transformPointCloud
269.  * 作用：根据给定的位姿 PointPose6D 对点云进行坐标变换。
270.  * 实现机制：
271.  * 1. 根据 PointPose6D 构建 Eigen::Affine3f 变换矩阵。
272.  * 2. 使用 OpenMP 并行加速，遍历点云中的每个点，应用仿射变换公式进行坐标更新。
273.  */
274. pcl::PointCloud<PointPose3D>::Ptr paramsServer::transformPointCloud(pcl::PointCloud<PointPose3D> cloud_in, PointPose6D* pose)
275. {
276. 	pcl::PointCloud<PointPose3D>::Ptr cloud_out(new pcl::PointCloud<PointPose3D>());
277. 
278. 	int cloud_size = cloud_in.size();
279. 	cloud_out->resize(cloud_size);
280. 
281. 	Eigen::Affine3f trans_cur = pcl::getTransformation(pose->x, pose->y, pose->z, pose->roll, pose->pitch, pose->yaw);
282. 	
283. 	#pragma omp parallel for num_threads(onboard_cpu_cores_num_)
284. 	for(int i = 0; i < cloud_size; ++i)
285. 	{
286. 		const auto &p_from = cloud_in.points[i];
287. 		cloud_out->points[i].x = trans_cur(0,0)*p_from.x + trans_cur(0,1)*p_from.y + trans_cur(0,2)*p_from.z + trans_cur(0,3);
288. 		cloud_out->points[i].y = trans_cur(1,0)*p_from.x + trans_cur(1,1)*p_from.y + trans_cur(1,2)*p_from.z + trans_cur(1,3);
289. 		cloud_out->points[i].z = trans_cur(2,0)*p_from.x + trans_cur(2,1)*p_from.y + trans_cur(2,2)*p_from.z + trans_cur(2,3);
290. 		cloud_out->points[i].intensity = p_from.intensity;
291. 	}
292. 	return cloud_out;
293. }
294. 
295. /*
296.  * 函数名：transformPointCloud
297.  * 作用：根据给定的 GTSAM Pose3 位姿对点云进行坐标变换。
298.  * 实现机制：
299.  * 1. 从 Pose3 中提取平移和旋转信息，构建 Eigen::Affine3f 变换矩阵。
300.  * 2. 使用 OpenMP 并行加速，遍历点云中的每个点，应用仿射变换公式进行坐标更新。
301.  */
302. pcl::PointCloud<PointPose3D>::Ptr paramsServer::transformPointCloud(pcl::PointCloud<PointPose3D> cloud_in, gtsam::Pose3 pose)
303. {
304. 	pcl::PointCloud<PointPose3D>::Ptr cloud_out(new pcl::PointCloud<PointPose3D>());
305. 
306. 	int cloud_size = cloud_in.size();
307. 	cloud_out->resize(cloud_size);
308. 
309. 	Eigen::Affine3f trans_cur = pcl::getTransformation(pose.translation().x(), pose.translation().y(), pose.translation().z(),
310. 		pose.rotation().roll(), pose.rotation().pitch(), pose.rotation().yaw());
311. 	
312. 	#pragma omp parallel for num_threads(onboard_cpu_cores_num_)
313. 	for(int i = 0; i < cloud_size; ++i)
314. 	{
315. 		const auto &p_from = cloud_in.points[i];
316. 		cloud_out->points[i].x = trans_cur(0,0)*p_from.x + trans_cur(0,1)*p_from.y + trans_cur(0,2)*p_from.z + trans_cur(0,3);
317. 		cloud_out->points[i].y = trans_cur(1,0)*p_from.x + trans_cur(1,1)*p_from.y + trans_cur(1,2)*p_from.z + trans_cur(1,3);
318. 		cloud_out->points[i].z = trans_cur(2,0)*p_from.x + trans_cur(2,1)*p_from.y + trans_cur(2,2)*p_from.z + trans_cur(2,3);
319. 		cloud_out->points[i].intensity = p_from.intensity;
320. 	}
321. 	return cloud_out;
322. }
```

</details>


---

<sub>⏱️ 2m 47s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingBasis.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingBasis.cpp

<details>
<summary>413 lines</summary>

```
1. #include "distributedMapping.h"
2. 
3. /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
4. 	class distributedMapping: constructor and destructor
5. * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
6. /*
7.  * 构造函数
8.  * 作用：初始化分布式建图系统，包括机器人配置、通信接口、参数设置、优化器初始化等。
9.  * 实现机制：
10.  * 1. 初始化 Google Logging。
11.  * 2. 遍历所有机器人，为本机器人创建发布者，为其他机器人创建订阅者（用于回环检测和分布式优化通信）。
12.  * 3. 初始化用于可视化、调试的全局发布者。
13.  * 4. 设置描述子类型（ScanContext, LidarIris, M2DP）及相关参数。
14.  * 5. 初始化 iSAM2 优化器和分布式映射优化器 (DistributedMapper)。
15.  * 6. 启动分布式建图的主定时器 (distributed_mapping_thread)。
16.  */
17. distributedMapping::distributedMapping() : paramsServer("dcl_slam_node")
18. {
19. 	string log_name = name_+"_distributed_mapping";
20. 	google::InitGoogleLogging(log_name.c_str());
21. 	string log_dir = "/log";
22. 	FLAGS_log_dir = std::getenv("HOME") + log_dir;
23. 	RCLCPP_INFO(this->get_logger(), "distributed mapping class initialization");
24. 
25. 	/*** robot team ***/
26. 	singleRobot robot; // each robot
27. 	for(int it = 0; it < number_of_robots_; it++)
28. 	{
29. 		/*** robot information ***/
30. 		robot.id_ = it; // robot ID and name
31. 		robot.name_ = "/a";
32. 		robot.name_[1] += it;
33. 		robot.odom_frame_ = robot.name_ + "/" + odom_frame_; // odom frame
34. 
35. 		/*** ros subscriber and publisher ***/
36. 		// this robot
37. 		if(it == id_)
38. 		{
39. 			// enable descriptor for detecting loop
40. 			if(intra_robot_loop_closure_enable_ || inter_robot_loop_closure_enable_)
41. 			{
42. 				// publish global descriptor
43. 				robot.pub_descriptors = this->create_publisher<dcl_slam::msg::GlobalDescriptor>(
44. 					robot.name_+"/distributedMapping/globalDescriptors", 5);
45. 				// publish loop infomation
46. 				robot.pub_loop_info = this->create_publisher<dcl_slam::msg::LoopInfo>(
47. 					robot.name_+"/distributedMapping/loopInfo", 5);
48. 			}
49. 			
50. 			// enable DGS
51. 			if(global_optmization_enable_)
52. 			{
53. 				robot.pub_optimization_state = this->create_publisher<std_msgs::msg::Int8>(
54. 					robot.name_+"/distributedMapping/optimizationState", 50);
55. 				robot.pub_rotation_estimate_state = this->create_publisher<std_msgs::msg::Int8>(
56. 					robot.name_+"/distributedMapping/rotationEstimateState", 50);
57. 				robot.pub_pose_estimate_state = this->create_publisher<std_msgs::msg::Int8>(
58. 					robot.name_+"/distributedMapping/poseEstimateState", 50);
59. 				robot.pub_neighbor_rotation_estimates = this->create_publisher<dcl_slam::msg::NeighborEstimate>(
60. 					robot.name_+"/distributedMapping/neighborRotationEstimates", 50);
61. 				robot.pub_neighbor_pose_estimates = this->create_publisher<dcl_slam::msg::NeighborEstimate>(
62. 					robot.name_+"/distributedMapping/neighborPoseEstimates", 50);
63. 			}
64. 		}
65. 		// other robot
66. 		else
67. 		{
68. 			if(intra_robot_loop_closure_enable_ || inter_robot_loop_closure_enable_)
69. 			{
70. 				// subscribe global descriptor
71. 				robot.sub_descriptors = this->create_subscription<dcl_slam::msg::GlobalDescriptor>(
72. 					robot.name_+"/distributedMapping/globalDescriptors", 50,
73. 					[this, it](const dcl_slam::msg::GlobalDescriptor::SharedPtr msg) {
74. 						this->globalDescriptorHandler(msg, it);
75. 					});
76. 				// subscribe loop infomation
77. 				robot.sub_loop_info = this->create_subscription<dcl_slam::msg::LoopInfo>(
78. 					robot.name_+"/distributedMapping/loopInfo", 50,
79. 					[this, it](const dcl_slam::msg::LoopInfo::SharedPtr msg) {
80. 						this->loopInfoHandler(msg, it);
81. 					});
82. 			}
83. 
84. 			if(global_optmization_enable_)
85. 			{
86. 				robot.sub_optimization_state = this->create_subscription<std_msgs::msg::Int8>(
87. 					robot.name_+"/distributedMapping/optimizationState", 50,
88. 					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
89. 						this->optStateHandler(msg, it);
90. 					});
91. 				robot.sub_rotation_estimate_state = this->create_subscription<std_msgs::msg::Int8>(
92. 					robot.name_+"/distributedMapping/rotationEstimateState", 50,
93. 					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
94. 						this->rotationStateHandler(msg, it);
95. 					});
96. 				robot.sub_pose_estimate_state = this->create_subscription<std_msgs::msg::Int8>(
97. 					robot.name_+"/distributedMapping/poseEstimateState", 50,
98. 					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
99. 						this->poseStateHandler(msg, it);
100. 					});
101. 				robot.sub_neighbor_rotation_estimates = this->create_subscription<dcl_slam::msg::NeighborEstimate>(
102. 					robot.name_+"/distributedMapping/neighborRotationEstimates", 50,
103. 					[this, it](const dcl_slam::msg::NeighborEstimate::SharedPtr msg) {
104. 						this->neighborRotationHandler(msg, it);
105. 					});
106. 				robot.sub_neighbor_pose_estimates = this->create_subscription<dcl_slam::msg::NeighborEstimate>(
107. 					robot.name_+"/distributedMapping/neighborPoseEstimates", 50,
108. 					[this, it](const dcl_slam::msg::NeighborEstimate::SharedPtr msg) {
109. 						this->neighborPoseHandler(msg, it);
110. 					});
111. 			}
112. 		}
113. 
114. 		/*** other ***/
115. 		robot.time_cloud_input_stamp = this->now();
116. 		robot.time_cloud_input = 0.0;
117. 
118. 		robot.keyframe_cloud.reset(new pcl::PointCloud<PointPose3D>());
119. 		robot.keyframe_cloud_array.clear();
120. 
121. 		robots.push_back(robot);
122. 	}
123. 
124. 	/*** ros subscriber and publisher ***/
125. 	// loop closure visualization
126. 	pub_loop_closure_constraints = this->create_publisher<visualization_msgs::msg::MarkerArray>(
127. 		"distributedMapping/loopClosureConstraints", 1);
128. 	// scan2map cloud
129. 	pub_scan_of_scan2map = this->create_publisher<sensor_msgs::msg::PointCloud2>(
130. 		"distributedMapping/scanOfScan2map", 1);
131. 	pub_map_of_scan2map = this->create_publisher<sensor_msgs::msg::PointCloud2>(
132. 		"distributedMapping/mapOfScan2map", 1);
133. 	// global map visualization
134. 	pub_global_map = this->create_publisher<sensor_msgs::msg::PointCloud2>(
135. 		"distributedMapping/globalMap", 1);
136. 	// path for independent robot
137. 	pub_global_path = this->create_publisher<nav_msgs::msg::Path>(
138. 		"distributedMapping/path", 1);
139. 	pub_local_path = this->create_publisher<nav_msgs::msg::Path>(
140. 		"distributedMapping/localPath", 1);
141. 	// keypose cloud
142. 	pub_keypose_cloud = this->create_publisher<sensor_msgs::msg::PointCloud2>(
143. 		"distributedMapping/keyposeCloud", 1);
144. 
145. 	/*** message information ***/
146. 	cloud_for_decript_ds.reset(new pcl::PointCloud<PointPose3D>()); 
147. 
148. 	/*** downsample filter ***/
149. 	downsample_filter_for_descriptor.setLeafSize(descript_leaf_size_, descript_leaf_size_, descript_leaf_size_);
150. 	downsample_filter_for_intra_loop.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
151. 	downsample_filter_for_inter_loop.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
152. 	downsample_filter_for_inter_loop2.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
153. 	downsample_filter_for_inter_loop3.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
154. 
155. 	/*** mutex ***/
156. 	// lock_on_call = vector<mutex>(number_of_robots_);
157. 	global_path.poses.clear();
158. 	local_path.poses.clear();
159. 
160. 	/*** distributed loopclosure ***/
161. 	inter_robot_loop_ptr = 0;
162. 	intra_robot_loop_ptr = 0;
163. 
164. 	intra_robot_loop_close_flag = false;
165. 
166. 	if(descriptor_type_num_ == DescriptorType::ScanContext)
167. 	{
168. 		keyframe_descriptor = unique_ptr<scan_descriptor>(new scan_context_descriptor(
169. 			20, 60, knn_candidates_, descriptor_distance_threshold_, 0, 80.0,
170. 			exclude_recent_frame_num_, number_of_robots_, id_));
171. 	}
172. 	else if(descriptor_type_num_ == DescriptorType::LidarIris)
173. 	{
174. 		keyframe_descriptor = unique_ptr<scan_descriptor>(new lidar_iris_descriptor(
175. 			iris_row_, iris_column_, n_scan_, descriptor_distance_threshold_,
176. 			exclude_recent_frame_num_, match_mode_, knn_candidates_, 4, 18, 1.6, 0.75,
177. 			number_of_robots_, id_));
178. 	}
179. 	else if(descriptor_type_num_ == DescriptorType::M2DP)
180. 	{
181. 		keyframe_descriptor = unique_ptr<scan_descriptor>(new m2dp_descriptor(
182. 			16, 8, 4, 16, number_of_robots_, id_));
183. 	}
184. 
185. 	loop_closures_candidates.clear();
186. 	loop_indexes.clear();
187. 
188. 	// radius search
189. 	copy_keyposes_cloud_3d.reset(new pcl::PointCloud<PointPose3D>());
190. 	copy_keyposes_cloud_6d.reset(new pcl::PointCloud<PointPose6D>());
191. 	kdtree_history_keyposes.reset(new pcl::KdTreeFLANN<PointPose3D>());
192. 
193. 	/*** noise model ***/
194. 	odometry_noise = noiseModel::Diagonal::Variances((Vector(6) << 1e-6, 1e-6, 1e-6, 1e-4, 1e-4, 1e-4).finished());
195. 	prior_noise = noiseModel::Isotropic::Variance(6, 1e-12);
196. 
197. 	/*** local pose graph optmazition ***/
198. 	ISAM2Params parameters;
199. 	parameters.relinearizeThreshold = 0.1;
200. 	parameters.relinearizeSkip = 1;
201. 	isam2 = new ISAM2(parameters); // isam2
202. 
203. 	keyposes_cloud_3d.reset(new pcl::PointCloud<PointPose3D>());
204. 	keyposes_cloud_6d.reset(new pcl::PointCloud<PointPose6D>());
205. 	
206. 	/*** distributed pose graph optmazition ***/
207. 	optimizer = boost::shared_ptr<distributed_mapper::DistributedMapper>(
208. 		new distributed_mapper::DistributedMapper(id_ + 'a'));
209. 
210. 	steps_of_unchange_graph = 0;
211. 
212. 	local_pose_graph = boost::make_shared<NonlinearFactorGraph>();
213. 	initial_values = boost::make_shared<Values>();
214. 	graph_values_vec = make_pair(local_pose_graph, initial_values);
215. 
216. 	graph_disconnected = true;
217. 
218. 	lowest_id_included = id_;
219. 	lowest_id_to_included = lowest_id_included;
220. 	prior_owner = id_;
221. 	prior_added = false;
222. 
223. 	adjacency_matrix = gtsam::Matrix::Zero(number_of_robots_, number_of_robots_);
224. 	optimization_order.clear();
225. 	in_order = false;
226. 
227. 	optimizer_state = OptimizerState::Idle;
228. 	optimization_steps = 0;
229. 	sent_start_optimization_flag = false;
230. 
231. 	current_rotation_estimate_iteration = 0;
232. 	current_pose_estimate_iteration = 0;
233. 
234. 	latest_change = -1;
235. 	steps_without_change = 0;
236. 
237. 	rotation_estimate_start = false;
238. 	pose_estimate_start = false;
239. 	rotation_estimate_finished = false;
240. 	pose_estimate_finished = false;
241. 	estimation_done = false;
242. 
243. 	neighboring_robots.clear();
244. 	neighbors_within_communication_range.clear();
245. 	neighbors_started_optimization.clear();
246. 	neighbor_state.clear();
247. 
248. 	neighbors_rotation_estimate_finished.clear();
249. 	neighbors_pose_estimate_finished.clear();
250. 	neighbors_estimation_done.clear();
251. 
252. 	neighbors_lowest_id_included.clear();
253. 	neighbors_anchor_offset.clear();
254. 
255. 	local_pose_graph_no_filtering = boost::make_shared<NonlinearFactorGraph>();
256. 
257. 	pose_estimates_from_neighbors.clear();
258. 	other_robot_keys_for_optimization.clear();
259. 
260. 	accepted_keys.clear();
261. 	rejected_keys.clear();
262. 	measurements_rejected_num = 0;
263. 	measurements_accepted_num = 0;
264. 	
265. 	optimizer->setUseBetweenNoiseFlag(use_between_noise_); // use between noise or not in optimizePoses
266. 	optimizer->setUseLandmarksFlag(use_landmarks_); // use landmarks
267. 	optimizer->loadSubgraphAndCreateSubgraphEdge(graph_values_vec); // load subgraphs
268. 	optimizer->setVerbosity(distributed_mapper::DistributedMapper::ERROR); // verbosity level
269. 	optimizer->setFlaggedInit(use_flagged_init_);
270. 	optimizer->setUpdateType(distributed_mapper::DistributedMapper::incUpdate);
271. 	optimizer->setGamma(gamma_);
272. 
273. 	if(global_optmization_enable_)
274. 	{
275. 		distributed_mapping_thread = this->create_wall_timer(
276. 			std::chrono::milliseconds((int)(mapping_process_interval_ * 1000)),
277. 			std::bind(&distributedMapping::run, this));
278. 	}
279. 	
280. 	loop_closure_thread_ = std::thread(&distributedMapping::loopClosureThread, this);
281. 
282. 	RCLCPP_INFO(this->get_logger(), "distributed mapping class initialization finish");
283. }
284. 
285. /*
286.  * 析构函数
287.  * 作用：清理资源。
288.  * 实现机制：目前为空，依赖自动资源管理。
289.  */
290. distributedMapping::~distributedMapping()
291. {
292. 	if(loop_closure_thread_.joinable())
293. 	{
294. 		loop_closure_thread_.join();
295. 	}
296. }
297. 
298. /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
299. 	class distributedMapping: other function
300. * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
301. /*
302.  * 函数名：lockOnCall
303.  * 作用：锁定互斥量，保证线程安全。
304.  * 实现机制：(代码中已注释掉，预留接口)
305.  */
306. void distributedMapping::lockOnCall()
307. {
308. 	// lock_on_call.lock();
309. }
310. 
311. /*
312.  * 函数名：unlockOnCall
313.  * 作用：解锁互斥量。
314.  * 实现机制：(代码中已注释掉，预留接口)
315.  */
316. void distributedMapping::unlockOnCall()
317. {
318. 	// lock_on_call.unlock();
319. }
320. 
321. /*
322.  * 函数名：getLocalKeyposesCloud3D
323.  * 作用：获取本地 3D 关键帧点云（仅包含位置信息）。
324.  * 实现机制：返回 keyposes_cloud_3d 指针。
325.  */
326. pcl::PointCloud<PointPose3D>::Ptr distributedMapping::getLocalKeyposesCloud3D()
327. {
328. 	return keyposes_cloud_3d;
329. }
330. 
331. /*
332.  * 函数名：getLocalKeyposesCloud6D
333.  * 作用：获取本地 6D 关键帧点云（包含位置和姿态信息）。
334.  * 实现机制：返回 keyposes_cloud_6d 指针。
335.  */
336. pcl::PointCloud<PointPose6D>::Ptr distributedMapping::getLocalKeyposesCloud6D()
337. {
338. 	return keyposes_cloud_6d;
339. }
340. 
341. /*
342.  * 函数名：getLocalKeyframe
343.  * 作用：获取指定索引的本地关键帧点云。
344.  * 实现机制：从 robots[id_].keyframe_cloud_array 数组中检索并返回。
345.  */
346. pcl::PointCloud<PointPose3D> distributedMapping::getLocalKeyframe(const int& index)
347. {
348. 	return robots[id_].keyframe_cloud_array[index];
349. }
350. 
351. /*
352.  * 函数名：getLatestEstimate
353.  * 作用：获取 iSAM2 优化后的最新位姿估计。
354.  * 实现机制：返回 isam2_keypose_estimate 变量。
355.  */
356. Pose3 distributedMapping::getLatestEstimate()
357. {
358. 	return isam2_keypose_estimate;
359. }
360. 
361. /*
362.  * 函数名：poseCovariance2msg
363.  * 作用：将 graph_utils::PoseWithCovariance 转换为 geometry_msgs::PoseWithCovariance 消息。
364.  * 实现机制：将位姿的平移、旋转（四元数）以及协方差矩阵元素逐一赋值给 ROS 消息格式。
365.  */
366. void distributedMapping::poseCovariance2msg(
367. 	const graph_utils::PoseWithCovariance& pose,
368. 	geometry_msgs::msg::PoseWithCovariance& msg)
369. {
370. 	msg.pose.position.x = pose.pose.x();
371. 	msg.pose.position.y = pose.pose.y();
372. 	msg.pose.position.z = pose.pose.z();
373. 
374. 	Vector quaternion = pose.pose.rotation().quaternion();
375. 	msg.pose.orientation.w = quaternion(0);
376. 	msg.pose.orientation.x = quaternion(1);
377. 	msg.pose.orientation.y = quaternion(2);
378. 	msg.pose.orientation.z = quaternion(3);
379. 
380. 	for(int i = 0; i < 6; i++)
381. 	{
382. 		for(int j = 0; j < 6; j++)
383. 		{
384. 			msg.covariance[i*6 + j] = pose.covariance_matrix(i, j);
385. 		}
386. 	}
387. }
388. 
389. /*
390.  * 函数名：msg2poseCovariance
391.  * 作用：将 geometry_msgs::PoseWithCovariance 消息转换为 graph_utils::PoseWithCovariance 结构。
392.  * 实现机制：从 ROS 消息中提取平移和四元数构建 Pose3，并提取协方差矩阵数据。
393.  */
394. void distributedMapping::msg2poseCovariance(
395. 	const geometry_msgs::msg::PoseWithCovariance& msg,
396. 	graph_utils::PoseWithCovariance& pose)
397. {
398. 	Rot3 rotation(msg.pose.orientation.w, msg.pose.orientation.x,
399. 		msg.pose.orientation.y, msg.pose.orientation.z);
400. 	Point3 translation(msg.pose.position.x, msg.pose.position.y, msg.pose.position.z);
401. 	
402. 	pose.pose = Pose3(rotation, translation);
403. 
404. 	pose.covariance_matrix = gtsam::Matrix::Zero(6,6);
405. 	for(int i = 0; i < 6; i++)
406. 	{
407. 		for(int j = 0; j < 6; j++)
408. 		{
409. 			pose.covariance_matrix(i, j) = msg.covariance[i*6 + j];
410. 		}
411. 	}
412. }
413.
```

</details>


---

<sub>⏱️ 2m 51s</sub>

### 💬 Copilot

Now let me look at the laserMapping.cpp main function and the key runtime functions:

---

<sub>⏱️ 2m 55s</sub>

### ✅ `bash`

**Find main and thread functions in laserMapping**

$ grep -n "int main\|void.*Thread\|void.*thread\|rclcpp::spin\|create_wall_timer\|create_timer\|std::thread" /home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp | head -30

```
796:int main(int argc, char** argv)
813:	std::thread visualize_map_thread(&distributedMapping::globalMapThread, &dm);
973:        rclcpp::spin_some(node);
<exited with exit code 0>
```


---

<sub>⏱️ 2m 55s</sub>

### ✅ `bash`

**Find run and thread functions**

$ grep -n "void distributedMapping::run\|void distributedMapping::loopClosureThread\|void distributedMapping::globalMapThread" /home/isaac/DCL-SLAM/src/dcl_slam/src/*.cpp

```
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp:674:void distributedMapping::loopClosureThread()
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:1444:void distributedMapping::run()
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingVisualization.cpp:6:void distributedMapping::globalMapThread()
<exited with exit code 0>
```


---

<sub>⏱️ 3m 2s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp (lines 796-980)

<details>
<summary>185 lines</summary>

```
796. int main(int argc, char** argv)
797. {
798.     rclcpp::init(argc, argv);
799.     node = std::make_shared<rclcpp::Node>("laserMapping");
800. 
801. 	distributedMapping dm;
802. 
803. 	// Robot info
804. 	std::string ns = node->get_namespace();
805. 	if(ns.length() != 2)
806. 	{
807. 		RCLCPP_ERROR(node->get_logger(), "Invalid robot prefix (should be either 'a-z' or 'A-Z'): %s", ns.c_str());
808. 		rclcpp::shutdown();
809. 		return 1;
810. 	}
811. 	name = ns.substr(1, 1);
812. 
813. 	std::thread visualize_map_thread(&distributedMapping::globalMapThread, &dm);
814. 
815.     // Declare and get parameters
816.     node->declare_parameter<bool>("publish.path_en", true);
817.     node->declare_parameter<bool>("publish.scan_publish_en", true);
818.     node->declare_parameter<bool>("publish.dense_publish_en", true);
819.     node->declare_parameter<bool>("publish.scan_bodyframe_pub_en", true);
820.     node->declare_parameter<int>("max_iteration", 4);
821.     node->declare_parameter<std::string>("map_file_path", "");
822.     node->declare_parameter<std::string>("common.lid_topic", "/livox/lidar");
823.     node->declare_parameter<std::string>("common.imu_topic", "/livox/imu");
824.     node->declare_parameter<bool>("common.time_sync_en", false);
825.     node->declare_parameter<double>("filter_size_corner", 0.5);
826.     node->declare_parameter<double>("filter_size_surf", 0.5);
827.     node->declare_parameter<double>("filter_size_map", 0.5);
828.     node->declare_parameter<double>("cube_side_length", 200.0);
829.     node->declare_parameter<double>("mapping.det_range", 260.0);
830.     node->declare_parameter<double>("mapping.fov_degree", 100.0);
831.     node->declare_parameter<double>("mapping.gyr_cov", 0.1);
832.     node->declare_parameter<double>("mapping.acc_cov", 0.1);
833.     node->declare_parameter<double>("mapping.b_gyr_cov", 0.0001);
834.     node->declare_parameter<double>("mapping.b_acc_cov", 0.0001);
835.     node->declare_parameter<double>("preprocess.blind", 4.0);
836.     node->declare_parameter<int>("preprocess.lidar_type", AVIA);
837.     node->declare_parameter<int>("preprocess.scan_line", 6);
838.     node->declare_parameter<int>("preprocess.timestamp_unit", US);
839.     node->declare_parameter<int>("preprocess.scan_rate", 10);
840.     node->declare_parameter<int>("point_filter_num", 2);
841.     node->declare_parameter<bool>("feature_extract_enable", false);
842.     node->declare_parameter<bool>("runtime_pos_log_enable", false);
843.     node->declare_parameter<bool>("mapping.extrinsic_est_en", true);
844.     node->declare_parameter<bool>("pcd_save.pcd_save_en", true);
845.     node->declare_parameter<int>("pcd_save.interval", -1);
846.     node->declare_parameter<std::vector<double>>("mapping.extrinsic_T", std::vector<double>{0.0, 0.0, 0.0});
847.     node->declare_parameter<std::vector<double>>("mapping.extrinsic_R", std::vector<double>{1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0});
848. 
849.     node->get_parameter("publish.path_en", path_en);
850.     node->get_parameter("publish.scan_publish_en", scan_pub_en);
851.     node->get_parameter("publish.dense_publish_en", dense_pub_en);
852.     node->get_parameter("publish.scan_bodyframe_pub_en", scan_body_pub_en);
853.     node->get_parameter("max_iteration", NUM_MAX_ITERATIONS);
854.     node->get_parameter("map_file_path", map_file_path);
855.     node->get_parameter("common.lid_topic", lid_topic);
856.     node->get_parameter("common.imu_topic", imu_topic);
857.     node->get_parameter("common.time_sync_en", time_sync_en);
858.     node->get_parameter("filter_size_corner", filter_size_corner_min);
859.     node->get_parameter("filter_size_surf", filter_size_surf_min);
860.     node->get_parameter("filter_size_map", filter_size_map_min);
861.     node->get_parameter("cube_side_length", cube_len);
862.     double det_range_d = 260.0;
863.     node->get_parameter("mapping.det_range", det_range_d);
864.     DET_RANGE = static_cast<float>(det_range_d);
865.     node->get_parameter("mapping.fov_degree", fov_deg);
866.     node->get_parameter("mapping.gyr_cov", gyr_cov);
867.     node->get_parameter("mapping.acc_cov", acc_cov);
868.     node->get_parameter("mapping.b_gyr_cov", b_gyr_cov);
869.     node->get_parameter("mapping.b_acc_cov", b_acc_cov);
870.     node->get_parameter("preprocess.blind", p_pre->blind);
871.     node->get_parameter("preprocess.lidar_type", p_pre->lidar_type);
872.     node->get_parameter("preprocess.scan_line", p_pre->N_SCANS);
873.     node->get_parameter("preprocess.timestamp_unit", p_pre->time_unit);
874.     node->get_parameter("preprocess.scan_rate", p_pre->SCAN_RATE);
875.     node->get_parameter("point_filter_num", p_pre->point_filter_num);
876.     node->get_parameter("feature_extract_enable", p_pre->feature_enabled);
877.     node->get_parameter("runtime_pos_log_enable", runtime_pos_log);
878.     node->get_parameter("mapping.extrinsic_est_en", extrinsic_est_en);
879.     node->get_parameter("pcd_save.pcd_save_en", pcd_save_en);
880.     node->get_parameter("pcd_save.interval", pcd_save_interval);
881.     node->get_parameter("mapping.extrinsic_T", extrinT);
882.     node->get_parameter("mapping.extrinsic_R", extrinR);
883. 
884.     cout<<"p_pre->lidar_type "<<p_pre->lidar_type<<" "<<p_pre->N_SCANS<<endl;
885.     
886.     path.header.stamp = node->now();
887.     path.header.frame_id = name +"/"+ "camera_init";
888. 
889.     /*** variables definition ***/
890.     int effect_feat_num = 0, frame_num = 0;
891.     double deltaT, deltaR, aver_time_consu = 0, aver_time_icp = 0, aver_time_match = 0, aver_time_incre = 0, aver_time_solve = 0, aver_time_const_H_time = 0;
892.     bool flg_EKF_converged, EKF_stop_flg = 0;
893.     
894.     FOV_DEG = (fov_deg + 10.0) > 179.9 ? 179.9 : (fov_deg + 10.0);
895.     HALF_FOV_COS = cos((FOV_DEG) * 0.5 * PI_M / 180.0);
896. 
897.     _featsArray.reset(new PointCloudXYZI());
898. 
899.     memset(point_selected_surf, true, sizeof(point_selected_surf));
900.     memset(res_last, -1000.0f, sizeof(res_last));
901.     downSizeFilterSurf.setLeafSize(filter_size_surf_min, filter_size_surf_min, filter_size_surf_min);
902.     downSizeFilterMap.setLeafSize(filter_size_map_min, filter_size_map_min, filter_size_map_min);
903.     memset(point_selected_surf, true, sizeof(point_selected_surf));
904.     memset(res_last, -1000.0f, sizeof(res_last));
905. 
906.     Lidar_T_wrt_IMU<<VEC_FROM_ARRAY(extrinT);
907.     Lidar_R_wrt_IMU<<MAT_FROM_ARRAY(extrinR);
908.     p_imu->set_extrinsic(Lidar_T_wrt_IMU, Lidar_R_wrt_IMU);
909.     p_imu->set_gyr_cov(V3D(gyr_cov, gyr_cov, gyr_cov));
910.     p_imu->set_acc_cov(V3D(acc_cov, acc_cov, acc_cov));
911.     p_imu->set_gyr_bias_cov(V3D(b_gyr_cov, b_gyr_cov, b_gyr_cov));
912.     p_imu->set_acc_bias_cov(V3D(b_acc_cov, b_acc_cov, b_acc_cov));
913. 
914.     double epsi[23] = {0.001};
915.     fill(epsi, epsi+23, 0.001);
916.     kf.init_dyn_share(get_f, df_dx, df_dw, h_share_model, NUM_MAX_ITERATIONS, epsi);
917. 
918.     /*** debug record ***/
919.     FILE *fp;
920.     string pos_log_dir = root_dir + "/Log/pos_log.txt";
921.     fp = fopen(pos_log_dir.c_str(),"w");
922. 
923.     ofstream fout_pre, fout_out, fout_dbg;
924.     fout_pre.open(DEBUG_FILE_DIR("mat_pre.txt"),ios::out);
925.     fout_out.open(DEBUG_FILE_DIR("mat_out.txt"),ios::out);
926.     fout_dbg.open(DEBUG_FILE_DIR("dbg.txt"),ios::out);
927.     if (fout_pre && fout_out)
928.         cout << "~~~~"<<ROOT_DIR<<" file opened" << endl;
929.     else
930.         cout << "~~~~"<<ROOT_DIR<<" doesn't exist" << endl;
931. 
932.     /*** ROS2 subscribe initialization ***/
933.     rclcpp::QoS qos(rclcpp::KeepLast(200000));
934.     qos.best_effort();
935. 
936.     rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr sub_pcl;
937. #ifdef USE_LIVOX_ROS_DRIVER2
938.     rclcpp::Subscription<livox_ros_driver2::msg::CustomMsg>::SharedPtr sub_livox;
939.     if (p_pre->lidar_type == AVIA) {
940.         sub_livox = node->create_subscription<livox_ros_driver2::msg::CustomMsg>(
941.             lid_topic, qos, livox_pcl_cbk);
942.     } else
943. #endif
944.     {
945.         sub_pcl = node->create_subscription<sensor_msgs::msg::PointCloud2>(
946.             lid_topic, qos, standard_pcl_cbk);
947.     }
948. 
949.     auto sub_imu = node->create_subscription<sensor_msgs::msg::Imu>(
950.         imu_topic, qos, imu_cbk);
951. 
952.     auto pubLaserCloudFull = node->create_publisher<sensor_msgs::msg::PointCloud2>(
953.         "cloud_registered", 100000);
954.     auto pubLaserCloudFull_body = node->create_publisher<sensor_msgs::msg::PointCloud2>(
955.         "cloud_registered_body", 100000);
956.     auto pubLaserCloudEffect = node->create_publisher<sensor_msgs::msg::PointCloud2>(
957.         "cloud_effected", 100000);
958.     auto pubLaserCloudMap = node->create_publisher<sensor_msgs::msg::PointCloud2>(
959.         "Laser_map", 100000);
960.     auto pubOdomAftMapped = node->create_publisher<nav_msgs::msg::Odometry>(
961.         "Odometry", 100000);
962.     auto pubPath = node->create_publisher<nav_msgs::msg::Path>(
963.         "path", 100000);
964. 
965.     auto tf_broadcaster = std::make_unique<tf2_ros::TransformBroadcaster>(*node);
966. 
967. //------------------------------------------------------------------------------------------------------
968.     signal(SIGINT, SigHandle);
969.     rclcpp::Rate rate(5000);
970.     while (rclcpp::ok())
971.     {
972.         if (flg_exit) break;
973.         rclcpp::spin_some(node);
974.         if(sync_packages(Measures)) 
975.         {
976.             if (flg_first_scan)
977.             {
978.                 first_lidar_time = Measures.lidar_beg_time;
979.                 p_imu->first_lidar_time = first_lidar_time;
980.                 flg_first_scan = false;
```

</details>


---

<sub>⏱️ 3m 2s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp (lines 1444-1530)

<details>
<summary>87 lines</summary>

```
1444. void distributedMapping::run()
1445. {
1446. 	// update optimizer state
1447. 	switch(optimizer_state)
1448. 	{
1449. 		case OptimizerState::Idle:
1450. 			if(startOptimizationCondition())
1451. 			{
1452. 				current_rotation_estimate_iteration = 0;
1453. 				current_pose_estimate_iteration = 0;
1454. 				// neighbors_within_communication_range.clear();
1455. 				neighbors_rotation_estimate_finished.clear();
1456. 				neighbors_pose_estimate_finished.clear();
1457. 				neighbors_anchor_offset.clear();
1458. 				neighbors_estimation_done.clear();
1459. 				latest_change = -1;
1460. 				steps_without_change = 0;
1461. 				lowest_id_to_included = lowest_id_included;
1462. 				neighbors_started_optimization.clear();
1463. 				if(prior_added)
1464. 				{
1465. 					optimizer->removePrior();
1466. 					prior_added = false;
1467. 				}
1468. 				optimization_steps = 0;
1469. 				changeOptimizerState(OptimizerState::Start);
1470. 			}
1471. 			else
1472. 			{
1473. 				changeOptimizerState(OptimizerState::Idle);
1474. 			}
1475. 			state_msg.data = (int)optimizer_state;
1476. 			robots[id_].pub_optimization_state->publish(state_msg);
1477. 			break;
1478. 
1479. 		case OptimizerState::Start:
1480. 			initializePoseGraphOptimization();
1481. 			optimization_steps++;
1482. 			if(in_order)
1483. 			{
1484. 				changeOptimizerState(OptimizerState::Initialization);
1485. 			}
1486. 			else
1487. 			{
1488. 				changeOptimizerState(OptimizerState::Idle);
1489. 			}
1490. 			break;
1491. 
1492. 		case OptimizerState::Initialization:
1493. 			changeOptimizerState(OptimizerState::RotationEstimation);
1494. 			rotation_estimate_start = false;
1495. 			optimization_steps++;
1496. 			break;
1497. 
1498. 		case OptimizerState::RotationEstimation:
1499. 			if(rotationEstimationStoppingBarrier())
1500. 			{
1501. 				changeOptimizerState(OptimizerState::PoseEstimationInitialization);
1502. 			}
1503. 			if(current_rotation_estimate_iteration > fail_safe_steps_)
1504. 			{
1505. 				removeInactiveNeighbors();
1506. 			}
1507. 			failSafeCheck();
1508. 			optimization_steps++;
1509. 			break;
1510. 
1511. 		case OptimizerState::PoseEstimationInitialization:
1512. 			initializePoseEstimation();
1513. 			pose_estimate_start = false;
1514. 			optimization_steps++;
1515. 			changeOptimizerState(OptimizerState::PoseEstimation);
1516. 			break;
1517. 
1518. 		case OptimizerState::PoseEstimation:
1519. 			if(poseEstimationStoppingBarrier())
1520. 			{
1521. 				changeOptimizerState(OptimizerState::End);
1522. 			}
1523. 			failSafeCheck();
1524. 			optimization_steps++;
1525. 			break;
1526. 
1527. 		case OptimizerState::End:
1528. 			endOptimization();
1529. 			changeOptimizerState(OptimizerState::PostEndingCommunicationDelay);
1530. 			optimization_steps++;
```

</details>


---

<sub>⏱️ 3m 2s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp (lines 674-740)

<details>
<summary>23 lines</summary>

```
674. void distributedMapping::loopClosureThread()
675. {
676. 	// Terminate the thread if loop closure are not needed
677. 	if(!intra_robot_loop_closure_enable_ && !inter_robot_loop_closure_enable_)
678. 	{
679. 		return;
680. 	}
681. 
682. 	rclcpp::Rate rate(1.0/loop_closure_process_interval_);
683. 
684. 	while(rclcpp::ok())
685. 	{
686. 		rate.sleep();
687. 
688. 		performRSIntraLoopClosure(); // find intra-loop with radius search
689. 
690. 		performIntraLoopClosure(); // find intra-loop with descriptor
691. 
692. 		performInterLoopClosure(); // find inter-loop with descriptor
693. 
694. 		performExternLoopClosure(); // verify all inter-loop here
695. 	}
696. }
```

</details>


---

<sub>⏱️ 3m 2s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingVisualization.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingVisualization.cpp

<details>
<summary>148 lines</summary>

```
1. #include "distributedMapping.h"
2. 
3. /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
4. 	class distributedMapping: publish visualization msg
5. * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
6. void distributedMapping::globalMapThread()
7. {
8. 	rclcpp::Rate rate(1.0/map_publish_interval_); // update global map per 4s (default)
9. 
10. 	while(rclcpp::ok())
11. 	{
12. 		rate.sleep();
13. 
14. 		publishGlobalMap(); // global map visualization
15. 
16. 		publishLoopClosureConstraint(); // loop closure visualization
17. 	}
18. }
19. 
20. void distributedMapping::publishGlobalMap()
21. {
22. 	// early return
23. 	if(pub_global_map->get_subscription_count() == 0 || initial_values->empty() == true)
24. 	{
25. 		return;
26. 	}
27. 
28. 	// copy the poses and change to cloud type
29. 	Values poses_initial_guess_copy = *initial_values;
30. 	pcl::PointCloud<PointPose3D>::Ptr poses_3d_cloud_copy(new pcl::PointCloud<PointPose3D>());
31. 	pcl::PointCloud<PointPose6D>::Ptr poses_6d_cloud_copy(new pcl::PointCloud<PointPose6D>());
32. 	for(const Values::ConstKeyValuePair &key_value: poses_initial_guess_copy)
33. 	{
34. 		Symbol key = key_value.key;
35. 		Pose3 pose = poses_initial_guess_copy.at<Pose3>(key);
36. 
37. 		PointPose3D pose_3d;
38. 		pose_3d.x = pose.translation().x();
39. 		pose_3d.y = pose.translation().y();
40. 		pose_3d.z = pose.translation().z();
41. 		pose_3d.intensity = key.index();
42. 
43. 		PointPose6D pose_6d;
44. 		pose_6d.x = pose_3d.x;
45. 		pose_6d.y = pose_3d.y;
46. 		pose_6d.z = pose_3d.z;
47. 		pose_6d.intensity = pose_3d.intensity;
48. 		pose_6d.roll = pose.rotation().roll();
49. 		pose_6d.pitch = pose.rotation().pitch();
50. 		pose_6d.yaw = pose.rotation().yaw();
51. 
52. 		poses_3d_cloud_copy->push_back(pose_3d);
53. 		poses_6d_cloud_copy->push_back(pose_6d);
54. 	}
55. 
56. 	// find the closest history key frame
57. 	std::vector<int> indices;
58. 	std::vector<float> distances;
59. 	kdtree_history_keyposes->setInputCloud(poses_3d_cloud_copy);
60. 	kdtree_history_keyposes->radiusSearch(poses_3d_cloud_copy->back(),
61. 		global_map_visualization_radius_, indices, distances, 0);
62. 
63. 	// extract visualized key frames
64. 	pcl::PointCloud<PointPose3D>::Ptr global_map_keyframes(new pcl::PointCloud<PointPose3D>());
65. 	pcl::PointCloud<PointPose3D>::Ptr global_map_keyframes_ds(new pcl::PointCloud<PointPose3D>());
66. 	for (int i = 0; i < (int)indices.size(); ++i)
67. 	{
68. 		PointPose6D pose_6d_tmp = poses_6d_cloud_copy->points[indices[i]];
69. 		*global_map_keyframes += *transformPointCloud(robots[id_].keyframe_cloud_array[pose_6d_tmp.intensity],
70. 			&pose_6d_tmp);
71. 	}
72. 
73. 	// downsample visualized points
74. 	pcl::VoxelGrid<PointPose3D> downsample_filter_for_global_map; // for global map visualization
75. 	downsample_filter_for_global_map.setLeafSize(map_leaf_size_, map_leaf_size_, map_leaf_size_);
76. 	downsample_filter_for_global_map.setInputCloud(global_map_keyframes);
77. 	downsample_filter_for_global_map.filter(*global_map_keyframes_ds);
78. 
79. 	// publish global map
80. 	sensor_msgs::msg::PointCloud2 global_map_msg;
81. 	pcl::toROSMsg(*global_map_keyframes_ds, global_map_msg);
82. 	global_map_msg.header.stamp = robots[id_].time_cloud_input_stamp;
83. 	global_map_msg.header.frame_id = world_frame_;
84. 	pub_global_map->publish(global_map_msg);
85. }
86. 
87. void distributedMapping::publishLoopClosureConstraint()
88. {
89. 	if(loop_indexs.empty())
90. 	{
91. 		return;
92. 	}
93. 
94. 	// loop nodes
95. 	visualization_msgs::msg::Marker nodes;
96. 	nodes.header.frame_id = world_frame_;
97. 	nodes.header.stamp = this->now();
98. 	nodes.action = visualization_msgs::msg::Marker::ADD;
99. 	nodes.type = visualization_msgs::msg::Marker::SPHERE_LIST;
100. 	nodes.ns = "loop_nodes";
101. 	nodes.id = 0;
102. 	nodes.pose.orientation.w = 1;
103. 	nodes.scale.x = 0.3; nodes.scale.y = 0.3; nodes.scale.z = 0.3; 
104. 	nodes.color.r = 0; nodes.color.g = 0.8; nodes.color.b = 1;
105. 	nodes.color.a = 1;
106. 
107. 	// loop edges
108. 	visualization_msgs::msg::Marker constraints;
109. 	constraints.header.frame_id = world_frame_;
110. 	constraints.header.stamp = this->now();
111. 	constraints.action = visualization_msgs::msg::Marker::ADD;
112. 	constraints.type = visualization_msgs::msg::Marker::LINE_LIST;
113. 	constraints.ns = "loop_constraints";
114. 	constraints.id = 1;
115. 	constraints.pose.orientation.w = 1;
116. 	constraints.scale.x = 0.1;
117. 	constraints.color.r = 0.9; constraints.color.g = 0.9; constraints.color.b = 0;
118. 	constraints.color.a = 1;
119. 
120. 	// check all accepted loop closure
121. 	Pose3 pose;
122. 	int index0, index1;
123. 	for(auto it = loop_indexs.begin(); it != loop_indexs.end(); ++it)
124.     {
125. 		index0 = it->first;
126. 		index1 = it->second;
127. 
128. 		geometry_msgs::msg::Point p;
129. 		pose = initial_values->at<Pose3>(Symbol('a'+id_, index0));
130. 		p.x = pose.translation().x();
131. 		p.y = pose.translation().y();
132. 		p.z = pose.translation().z();
133. 		nodes.points.push_back(p);
134. 		constraints.points.push_back(p);
135. 		pose = initial_values->at<Pose3>(Symbol('a'+id_, index1));
136. 		p.x = pose.translation().x();
137. 		p.y = pose.translation().y();
138. 		p.z = pose.translation().z();
139. 		nodes.points.push_back(p);
140. 		constraints.points.push_back(p);
141. 	}
142. 
143. 	// publish loop closure markers
144. 	visualization_msgs::msg::MarkerArray markers_array;
145. 	markers_array.markers.push_back(nodes);
146. 	markers_array.markers.push_back(constraints);
147. 	pub_loop_closure_constraints->publish(markers_array);
148. }
```

</details>


---

<sub>⏱️ 3m 5s</sub>

### 💬 Copilot

Now let me check the remaining key functions - the main loop processing and `performDistributedMapping`:

---

<sub>⏱️ 3m 10s</sub>

### ✅ `bash`

**Find key function calls in main loop**

$ grep -n "performDistributedMapping\|saveFrame\|updatePoses\|makeDescriptors\|publishPath\|publishTransformation\|sync_packages\|livox_pcl_cbk\|standard_pcl_cbk\|imu_cbk" /home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp | head -30

<details>
<summary>15 lines</summary>

```
300:void standard_pcl_cbk(const sensor_msgs::msg::PointCloud2::ConstSharedPtr msg) 
325:void livox_pcl_cbk(const livox_ros_driver2::msg::CustomMsg::ConstSharedPtr msg) 
361:void imu_cbk(const sensor_msgs::msg::Imu::ConstSharedPtr msg_in) 
392:bool sync_packages(MeasureGroup &meas)
941:            lid_topic, qos, livox_pcl_cbk);
946:            lid_topic, qos, standard_pcl_cbk);
950:        imu_topic, qos, imu_cbk);
974:        if(sync_packages(Measures)) 
1087:            if(dm.saveFrame(pose_to) == true)
1093:                dm.performDistributedMapping(pose_to, keyframe, rclcpp::Time(to_ros_time(lidar_end_time)));
1095:                if(dm.updatePoses())
1147:                dm.makeDescriptors();
1149:                dm.publishPath();
1151:            dm.publishTransformation(rclcpp::Time(to_ros_time(lidar_end_time)));
<exited with exit code 0>
```

</details>


---

<sub>⏱️ 3m 14s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_fast_lio/src/laserMapping.cpp (lines 980-1160)

<details>
<summary>181 lines</summary>

```
980.                 flg_first_scan = false;
981.                 continue;
982.             }
983. 
984.             double t0,t1,t2,t3,t4,t5,match_start, solve_start, svd_time;
985. 
986.             match_time = 0;
987.             kdtree_search_time = 0.0;
988.             solve_time = 0;
989.             solve_const_H_time = 0;
990.             svd_time   = 0;
991.             t0 = omp_get_wtime();
992. 
993.             p_imu->Process(Measures, kf, feats_undistort);
994.             state_point = kf.get_x();
995.             pos_lid = state_point.pos + state_point.rot * state_point.offset_T_L_I;
996. 
997.             if (feats_undistort->empty() || (feats_undistort == NULL))
998.             {
999.                 printf("No point, skip this scan\n");
1000.                 continue;
1001.             }
1002. 
1003.             flg_EKF_inited = (Measures.lidar_beg_time - first_lidar_time) < INIT_TIME ? \
1004.                             false : true;
1005. 
1006.             /*** Segment the map in lidar FOV ***/
1007.             lasermap_fov_segment();
1008. 
1009.             /*** downsample the feature points in a scan ***/
1010.             downSizeFilterSurf.setInputCloud(feats_undistort);
1011.             downSizeFilterSurf.filter(*feats_down_body);
1012.             t1 = omp_get_wtime();
1013.             feats_down_size = feats_down_body->points.size();
1014.             /*** initialize the map kdtree ***/
1015.             if(ikdtree.Root_Node == nullptr)
1016.             {
1017.                 if(feats_down_size > 5)
1018.                 {
1019.                     ikdtree.set_downsample_param(filter_size_map_min);
1020.                     feats_down_world->resize(feats_down_size);
1021.                     for(int i = 0; i < feats_down_size; i++)
1022.                     {
1023.                         pointBodyToWorld(&(feats_down_body->points[i]), &(feats_down_world->points[i]));
1024.                     }
1025.                     ikdtree.Build(feats_down_world->points);
1026.                     cout<<"[ mapping ]: ikdtree initialized with "<<feats_down_size<<" points, map valid: "<<ikdtree.validnum()<<endl;
1027.                 }
1028.                 else
1029.                 {
1030.                     cout<<"[ mapping ]: ikdtree init skipped, feats_down_size="<<feats_down_size<<endl;
1031.                 }
1032.                 continue;
1033.             }
1034.             int featsFromMapNum = ikdtree.validnum();
1035.             kdtree_size_st = ikdtree.size();
1036.             
1037.             cout<<"[ mapping ]: In num: "<<feats_undistort->points.size()<<" downsamp "<<feats_down_size<<" Map num: "<<featsFromMapNum<<"effect num:"<<effct_feat_num<<endl;
1038. 
1039.             /*** ICP and iterated Kalman filter update ***/
1040.             if (feats_down_size < 5)
1041.             {
1042.                 printf("No point, skip this scan!\n");
1043.                 continue;
1044.             }
1045.             
1046.             normvec->resize(feats_down_size);
1047.             feats_down_world->resize(feats_down_size);
1048. 
1049.             V3D ext_euler = SO3ToEuler(state_point.offset_R_L_I);
1050.             fout_pre<<setw(20)<<Measures.lidar_beg_time - first_lidar_time<<" "<<euler_cur.transpose()<<" "<< state_point.pos.transpose()<<" "<<ext_euler.transpose() << " "<<state_point.offset_T_L_I.transpose()<< " " << state_point.vel.transpose() \
1051.             <<" "<<state_point.bg.transpose()<<" "<<state_point.ba.transpose()<<" "<<state_point.grav<< endl;
1052. 
1053.             if(0) // If you need to see map point, change to "if(1)"
1054.             {
1055.                 PointVector ().swap(ikdtree.PCL_Storage);
1056.                 ikdtree.flatten(ikdtree.Root_Node, ikdtree.PCL_Storage, NOT_RECORD);
1057.                 featsFromMap->clear();
1058.                 featsFromMap->points = ikdtree.PCL_Storage;
1059.             }
1060. 
1061.             pointSearchInd_surf.resize(feats_down_size);
1062.             Nearest_Points.resize(feats_down_size);
1063.             int  rematch_num = 0;
1064.             bool nearest_search_en = true; //
1065. 
1066.             t2 = omp_get_wtime();
1067.             
1068.             /*** iterated state estimation ***/
1069.             double t_update_start = omp_get_wtime();
1070.             double solve_H_time = 0;
1071.             kf.update_iterated_dyn_share_modified(LASER_POINT_COV, solve_H_time);
1072.             state_point = kf.get_x();
1073.             euler_cur = SO3ToEuler(state_point.rot);
1074.             pos_lid = state_point.pos + state_point.rot * state_point.offset_T_L_I;
1075.             geoQuat.x = state_point.rot.coeffs()[0];
1076.             geoQuat.y = state_point.rot.coeffs()[1];
1077.             geoQuat.z = state_point.rot.coeffs()[2];
1078.             geoQuat.w = state_point.rot.coeffs()[3];
1079. 
1080.             double t_update_end = omp_get_wtime();
1081. 
1082. 			/*** dcl-slam ***/
1083.             gtsam::Pose3 pose_to = Pose3(
1084.                     Rot3::Quaternion(state_point.rot.coeffs()[3], state_point.rot.coeffs()[0],
1085.                     state_point.rot.coeffs()[1], state_point.rot.coeffs()[2]),
1086.                     Point3(state_point.pos(0), state_point.pos(1), state_point.pos(2)));
1087.             if(dm.saveFrame(pose_to) == true)
1088.             {
1089.                 // keyframe
1090.                 pcl::PointCloud<pcl::PointXYZI>::Ptr keyframe(new pcl::PointCloud<pcl::PointXYZI>());
1091.                 pcl::copyPointCloud(*feats_undistort, *keyframe);
1092. 
1093.                 dm.performDistributedMapping(pose_to, keyframe, rclcpp::Time(to_ros_time(lidar_end_time)));
1094. 
1095.                 if(dm.updatePoses())
1096.                 {
1097.                     // surrounding poses
1098.                     pcl::PointCloud<pcl::PointXYZI>::Ptr surrounding_keyposes(new pcl::PointCloud<pcl::PointXYZI>());
1099.                     static double search_radius = 5;
1100.                     std::vector<int> indice;
1101.                     std::vector<float> distance;
1102.                     kdtree_surrounding_keyposes->setInputCloud(dm.getLocalKeyposesCloud3D()); 
1103.                     kdtree_surrounding_keyposes->radiusSearch(
1104.                         dm.getLocalKeyposesCloud3D()->back(), search_radius, indice, distance);
1105.                     for(int i = 0; i < (int)indice.size(); ++i)
1106.                     {
1107.                         int index = indice[i];
1108.                         surrounding_keyposes->push_back(dm.getLocalKeyposesCloud3D()->points[index]);
1109.                     }
1110. 
1111.                     // surrounding map
1112.                     PointCloudXYZI::Ptr surrounding_map(new PointCloudXYZI());
1113.                     for(int i = 0; i < (int)surrounding_keyposes->size(); ++i)
1114.                     {
1115.                         int key = surrounding_keyposes->points[i].intensity;
1116.                         PointCloudXYZI::Ptr keyframe_tmp2(new PointCloudXYZI());
1117.                         PointCloudXYZI::Ptr keyframesTmp2(new PointCloudXYZI());
1118.                         pcl::copyPointCloud(dm.getLocalKeyframe(key), *keyframesTmp2);
1119.                         downSizeFilterMap.setInputCloud(keyframesTmp2);
1120.                         downSizeFilterMap.filter(*keyframe_tmp2);
1121. 
1122.                         *surrounding_map += *transformPointCloud(keyframe_tmp2, &(dm.getLocalKeyposesCloud6D()->points[key]));
1123.                     }
1124.                     // downsample
1125.                     downSizeFilterMap.setInputCloud(surrounding_map);
1126.                     downSizeFilterMap.filter(*surrounding_map);
1127. 
1128.                     // recontruct ikd tree
1129.                     ikdtree.reconstruct(surrounding_map->points);
1130.                 }
1131.                 // save updated transform
1132.                 state_ikfom state_updated = kf.get_x();
1133.                 gtsam::Pose3 latest_estimate = dm.getLatestEstimate();
1134.                 Eigen::Quaterniond lastPoseQuat(
1135.                     latest_estimate.rotation().toQuaternion().w(),
1136.                     latest_estimate.rotation().toQuaternion().x(),
1137.                     latest_estimate.rotation().toQuaternion().y(),
1138.                     latest_estimate.rotation().toQuaternion().z());
1139.                 Eigen::Vector3d lastPoseQuatPos(
1140.                     latest_estimate.translation().x(),
1141.                     latest_estimate.translation().y(),
1142.                     latest_estimate.translation().z());
1143.                 state_updated.rot = lastPoseQuat;
1144.                 state_updated.pos = lastPoseQuatPos;
1145.                 kf.change_x(state_updated);
1146. 
1147.                 dm.makeDescriptors();
1148. 
1149.                 dm.publishPath();
1150.             }
1151.             dm.publishTransformation(rclcpp::Time(to_ros_time(lidar_end_time)));
1152. 			/*** dcl-slam ***/
1153. 
1154.             /******* Publish odometry *******/
1155.             publish_odometry(pubOdomAftMapped, tf_broadcaster);
1156. 
1157.             /*** add the feature points to map kdtree ***/
1158.             t3 = omp_get_wtime();
1159.             map_incremental();
1160.             t5 = omp_get_wtime();
```

</details>


---

<sub>⏱️ 3m 18s</sub>

### 💬 Copilot

Now let me look at key function implementations:

---

<sub>⏱️ 3m 21s</sub>

### ✅ `bash`

**Find function locations**

$ grep -n "void distributedMapping::performDistributedMapping\|bool distributedMapping::saveFrame\|bool distributedMapping::updatePoses\|void distributedMapping::makeDescriptors\|void distributedMapping::publishPath\|void distributedMapping::publishTransformation\|void distributedMapping::updateLocalPath" /home/isaac/DCL-SLAM/src/dcl_slam/src/*.cpp

<details>
<summary>8 lines</summary>

```
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:374:void distributedMapping::performDistributedMapping(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:488:bool distributedMapping::saveFrame(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:521:void distributedMapping::updateLocalPath(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:548:bool distributedMapping::updatePoses()
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:609:void distributedMapping::makeDescriptors()
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:657:void distributedMapping::publishPath()
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:680:void distributedMapping::publishTransformation(
<exited with exit code 0>
```

</details>


---

<sub>⏱️ 3m 25s</sub>

### 💬 Copilot

Now let me check the config file to complete the picture:

---

<sub>⏱️ 3m 27s</sub>

### ✅ `bash`

**List config files**

$ ls /home/isaac/DCL-SLAM/src/dcl_slam/config/

```
dcl_fast_lio_mid360.rviz      dcl_lio_sam_params.yaml	       dcl_lio_sam_vlp16_params.yaml  dcl_rviz.rviz
dcl_fast_lio_mid360.yaml      dcl_lio_sam_vlp16_params_a.yaml  dcl_rviz_a.rviz		      lio_sam_robot.urdf.xacro
dcl_fast_lio_velodyne16.yaml  dcl_lio_sam_vlp16_params_b.yaml  dcl_rviz_b.rviz
dcl_fast_lio_velodyne.yaml    dcl_lio_sam_vlp16_params_c.yaml  dcl_rviz_c.rviz
<exited with exit code 0>
```


---

<sub>⏱️ 3m 27s</sub>

### ✅ `bash`

**Check performDistributedMapping location**

$ grep -n "void distributedMapping::performDistributedMapping" /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp

```
374:void distributedMapping::performDistributedMapping(
<exited with exit code 0>
```


---

<sub>⏱️ 3m 31s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp (lines 374-530)

<details>
<summary>157 lines</summary>

```
374. void distributedMapping::performDistributedMapping(
375. 	const Pose3& pose_to,
376. 	const pcl::PointCloud<PointPose3D>::Ptr frame_to,
377. 	const rclcpp::Time& timestamp)
378. {
379. 	// save keyframe cloud
380. 	pcl::copyPointCloud(*frame_to, *robots[id_].keyframe_cloud);
381. 	robots[id_].keyframe_cloud_array.push_back(*robots[id_].keyframe_cloud);
382. 	// save timestamp
383. 	robots[id_].time_cloud_input_stamp = timestamp;
384. 	robots[id_].time_cloud_input = timestamp.seconds();
385. 
386. 	// add piror factor
387. 	Pose3 new_pose_to;
388. 	int poses_num = initial_values->size();
389. 	Symbol current_symbol = Symbol('a'+id_, poses_num);
390. 	if(poses_num == 0)
391. 	{
392. 		// save piror value
393. 		robots[id_].piror_odom = pose_to;
394. 
395. 		// add piror factor to graph
396. 		auto prior_factor = PriorFactor<Pose3>(current_symbol, pose_to, prior_noise);
397. 		local_pose_graph_no_filtering->add(prior_factor);
398. 		isam2_graph.add(prior_factor);
399. 
400. 		// add piror value
401. 		initial_values->insert(current_symbol, pose_to);
402. 		isam2_initial_values.insert(current_symbol, pose_to);
403. 		new_pose_to = pose_to;
404. 
405. 		LOG(INFO) << "createPrior:[" << id_ << "]" << new_pose_to.translation().x() << " " << new_pose_to.translation().y()
406. 			<< " " << new_pose_to.translation().z() << new_pose_to.rotation().roll() << " " << new_pose_to.rotation().pitch()
407. 			<< " " << new_pose_to.rotation().yaw() << "." << endl;
408. 	}
409. 	// add odometry factor
410. 	else
411. 	{
412. 		// incremental odom in local frame
413. 		auto pose_from = pclPointTogtsamPose3(keyposes_cloud_6d->points[poses_num - 1]);
414. 		auto pose_increment = pose_from.between(pose_to);
415. 		Symbol previous_symbol = Symbol('a'+id_, poses_num - 1);
416. 		Matrix covariance = odometry_noise->covariance();
417. 		
418. 		// add odometry factor to graph
419. 		NonlinearFactor::shared_ptr factor(new BetweenFactor<Pose3>(
420. 			previous_symbol, current_symbol, pose_increment, odometry_noise));
421. 		local_pose_graph->add(factor);
422. 		local_pose_graph_no_filtering->add(factor);
423. 		isam2_graph.add(factor);
424. 
425. 		// add odometry value
426. 		isam2_initial_values.insert(current_symbol, pose_to);
427. 		// incremental odom in global graph frame
428. 		pose_from = initial_values->at<Pose3>(previous_symbol);
429. 		new_pose_to = pose_from * pose_increment;
430. 		initial_values->insert(current_symbol, new_pose_to);
431. 
432. 		// save factor in local map (for PCM)
433. 		auto new_factor = boost::dynamic_pointer_cast<BetweenFactor<Pose3>>(factor);
434. 		robot_local_map.addTransform(*new_factor, covariance);
435. 
436. 		LOG(INFO) << "createOdom:[" << id_ << "]" << "[" << poses_num - 1 << "-" << poses_num << "]--["
437. 			<< pose_from.translation().x() << " " << pose_from.translation().y() << " " << pose_from.translation().z()
438. 			<< pose_from.rotation().roll() << " " << pose_from.rotation().pitch() << " " << pose_from.rotation().yaw() << "],["
439. 			<< new_pose_to.translation().x() << " " << new_pose_to.translation().y() << " " << new_pose_to.translation().z()
440. 			<< new_pose_to.rotation().roll() << " " << new_pose_to.rotation().pitch() << " " << new_pose_to.rotation().yaw() << "]." << endl;
441. 	}
442. 
443. 	// optimizing
444. 	// isam2_graph.print("GTSAM Graph:\n");
445. 	isam2->update(isam2_graph, isam2_initial_values);
446. 	isam2_graph.resize(0);
447. 	isam2_initial_values.clear();
448. 	isam2_current_estimates = isam2->calculateEstimate();
449. 	isam2_keypose_estimate = isam2_current_estimates.at<Pose3>(current_symbol);
450. 
451. 	// save pose in local frame
452. 	static PointPose3D pose_3d;
453. 	pose_3d.x = isam2_keypose_estimate.translation().x();
454. 	pose_3d.y = isam2_keypose_estimate.translation().y();
455. 	pose_3d.z = isam2_keypose_estimate.translation().z();
456. 	pose_3d.intensity = poses_num; // keyframe index
457. 	keyposes_cloud_3d->push_back(pose_3d);
458. 
459. 	static PointPose6D pose_6d;
460. 	pose_6d.x = pose_3d.x;
461. 	pose_6d.y = pose_3d.y;
462. 	pose_6d.z = pose_3d.z;
463. 	pose_6d.intensity = pose_3d.intensity;
464. 	pose_6d.roll = isam2_keypose_estimate.rotation().roll();
465. 	pose_6d.pitch = isam2_keypose_estimate.rotation().pitch();
466. 	pose_6d.yaw = isam2_keypose_estimate.rotation().yaw();
467. 	pose_6d.time = robots[id_].time_cloud_input; // keyframe timestamp
468. 	keyposes_cloud_6d->push_back(pose_6d);
469. 
470. 	LOG(INFO) << "save:[" << id_ << "]--[" << poses_num << "]--" << isam2_keypose_estimate.translation().x()
471. 		<< " " << isam2_keypose_estimate.translation().y() << " " << isam2_keypose_estimate.translation().z()
472. 		<< " " << isam2_keypose_estimate.rotation().roll() << " " << isam2_keypose_estimate.rotation().pitch()
473. 		<< " " << isam2_keypose_estimate.rotation().yaw() << "." << endl;
474. 
475. 	// save path for visualization
476. 	updateLocalPath(pose_6d);
477. 	updateGlobalPath(new_pose_to);
478. }
479. 
480. /*
481.  * 函数名：saveFrame
482.  * 作用：判断是否需要保存当前帧为关键帧。
483.  * 实现机制：
484.  * 1. 如果是第一帧，直接保存。
485.  * 2. 计算当前帧与上一关键帧的相对位姿变化（距离和角度）。
486.  * 3. 如果变化量小于设定的阈值 (keyframe_distance_threshold_, keyframe_angle_threshold_)，则丢弃该帧。
487.  */
488. bool distributedMapping::saveFrame(
489. 	const Pose3& pose_to)
490. {
491. 	if(keyposes_cloud_3d->empty())
492. 	{
493. 		return true;
494. 	}
495. 
496. 	auto last_keypose = pclPointTogtsamPose3(keyposes_cloud_6d->back());
497. 	auto pose_increment = last_keypose.inverse() * pose_to;
498. 
499. 	float x = pose_increment.translation().x();
500. 	float y = pose_increment.translation().y();
501. 	float z = pose_increment.translation().z();
502. 	float roll = pose_increment.rotation().roll();
503. 	float pitch = pose_increment.rotation().pitch();
504. 	float yaw = pose_increment.rotation().yaw();
505. 
506. 	// select keyframe
507. 	if(abs(roll) < keyframe_angle_threshold_ && abs(pitch) < keyframe_angle_threshold_ && 
508. 		abs(yaw) < keyframe_angle_threshold_ && sqrt(x*x + y*y + z*z) < keyframe_distance_threshold_)
509. 	{
510. 		return false;
511. 	}
512. 
513. 	return true;
514. }
515. 
516. /*
517.  * 函数名：updateLocalPath
518.  * 作用：更新局部路径 (Local Path)。
519.  * 实现机制：将新的位姿转换为 geometry_msgs::PoseStamped 消息，添加到 local_path 消息中。
520.  */
521. void distributedMapping::updateLocalPath(
522. 	const PointPose6D& pose)
523. {
524. 	static geometry_msgs::msg::PoseStamped pose_stamped_msg;
525. 	pose_stamped_msg.header.stamp = this->now();
526. 	pose_stamped_msg.header.frame_id = world_frame_;
527. 	pose_stamped_msg.pose.position.x = pose.x;
528. 	pose_stamped_msg.pose.position.y = pose.y;
529. 	pose_stamped_msg.pose.position.z = pose.z;
530. 	tf2::Quaternion q;
```

</details>


---

<sub>⏱️ 3m 35s</sub>

### 💬 Copilot

Now I have enough information to write the analysis. Let me create the plan file with the complete analysis:

---

<sub>⏱️ 5m 25s</sub>

### ✅ `create`

**create a new file at /home/isaac/.copilot/session-state/5a0fa04a-ac34-47ef-91c7-e5db8f337dba/plan.md.**

/home/isaac/.copilot/session-state/5a0fa04a-ac34-47ef-91c7-e5db8f337dba/plan.md

```
Created file /home/isaac/.copilot/session-state/5a0fa04a-ac34-47ef-91c7-e5db8f337dba/plan.md with 10542 characters
```


---

<sub>⏱️ 5m 30s</sub>

### 💬 Copilot

分析完成。以下是 DCL-SLAM 启动逻辑的结构化总结：

## 启动链路概览

**Launch → main() → distributedMapping构造 → 主循环 + 3个并行线程**

```
dcl_fast_lio_mid360.launch.py
  └─ fastlio_mapping (laserMapping.cpp:main)
       ├─ rclcpp::init()
       ├─ distributedMapping dm  ← 核心初始化
       │    ├─ paramsServer() ← 加载60+参数
       │    ├─ 创建pub/sub通信接口
       │    ├─ 初始化描述子(ScanContext/LidarIris/M2DP)
       │    ├─ 初始化iSAM2 + DistributedMapper
       │    ├─ 启动定时器 → run() 分布式优化状态机
       │    └─ 启动线程 → loopClosureThread()
       ├─ 启动线程 → globalMapThread()
       ├─ 加载FastLIO参数 + 初始化EKF
       ├─ 注册LiDAR/IMU回调
       └─ 主循环: spin → sync → IMU处理 → EKF → saveFrame → performDistributedMapping
```

**4个并发执行单元**：
1. **主循环** — LiDAR帧处理 + DCL-SLAM关键帧建图
2. **回环检测线程** — 内部回环(半径搜索+描述子) + 跨机器人回环
3. **分布式优化定时器** — DGS状态机(Idle→Rotation→Pose→End)
4. **可视化线程** — 全局地图 + 回环约束发布

完整分析文档（含Mermaid图、函数表、异常处理说明）已写入会话文件。关键发现：`paramsServer`中`rclcpp::shutdown()`不会立即终止进程，`lock_on_call`互斥锁已被注释为空操作。

---

<sub>⏱️ 8m 29s</sub>

### 👤 User

会话文件在哪里

---

<sub>⏱️ 8m 34s</sub>

### 💬 Copilot

会话文件路径：

```
/home/isaac/.copilot/session-state/5a0fa04a-ac34-47ef-91c7-e5db8f337dba/plan.md
```

这是 Copilot 的会话工作区目录，不在你的项目仓库内。如果你需要把分析文档放到项目中，我可以将其复制到 DCL-SLAM 仓库下你指定的位置。

---

<sub>⏱️ 113m 28s</sub>

### 👤 User

本机器人是怎么和其他机器人通信的

---

<sub>⏱️ 113m 34s</sub>

### 💭 Reasoning

*The user wants to understand how the robot communicates with other robots in DCL-SLAM. Let me look at the communication mechanisms more closely.*

---

<sub>⏱️ 113m 38s</sub>

### 💬 Copilot





---

<sub>⏱️ 113m 38s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingBasis.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMappingBasis.cpp (lines 27-122)

<details>
<summary>96 lines</summary>

```
27. 	for(int it = 0; it < number_of_robots_; it++)
28. 	{
29. 		/*** robot information ***/
30. 		robot.id_ = it; // robot ID and name
31. 		robot.name_ = "/a";
32. 		robot.name_[1] += it;
33. 		robot.odom_frame_ = robot.name_ + "/" + odom_frame_; // odom frame
34. 
35. 		/*** ros subscriber and publisher ***/
36. 		// this robot
37. 		if(it == id_)
38. 		{
39. 			// enable descriptor for detecting loop
40. 			if(intra_robot_loop_closure_enable_ || inter_robot_loop_closure_enable_)
41. 			{
42. 				// publish global descriptor
43. 				robot.pub_descriptors = this->create_publisher<dcl_slam::msg::GlobalDescriptor>(
44. 					robot.name_+"/distributedMapping/globalDescriptors", 5);
45. 				// publish loop infomation
46. 				robot.pub_loop_info = this->create_publisher<dcl_slam::msg::LoopInfo>(
47. 					robot.name_+"/distributedMapping/loopInfo", 5);
48. 			}
49. 			
50. 			// enable DGS
51. 			if(global_optmization_enable_)
52. 			{
53. 				robot.pub_optimization_state = this->create_publisher<std_msgs::msg::Int8>(
54. 					robot.name_+"/distributedMapping/optimizationState", 50);
55. 				robot.pub_rotation_estimate_state = this->create_publisher<std_msgs::msg::Int8>(
56. 					robot.name_+"/distributedMapping/rotationEstimateState", 50);
57. 				robot.pub_pose_estimate_state = this->create_publisher<std_msgs::msg::Int8>(
58. 					robot.name_+"/distributedMapping/poseEstimateState", 50);
59. 				robot.pub_neighbor_rotation_estimates = this->create_publisher<dcl_slam::msg::NeighborEstimate>(
60. 					robot.name_+"/distributedMapping/neighborRotationEstimates", 50);
61. 				robot.pub_neighbor_pose_estimates = this->create_publisher<dcl_slam::msg::NeighborEstimate>(
62. 					robot.name_+"/distributedMapping/neighborPoseEstimates", 50);
63. 			}
64. 		}
65. 		// other robot
66. 		else
67. 		{
68. 			if(intra_robot_loop_closure_enable_ || inter_robot_loop_closure_enable_)
69. 			{
70. 				// subscribe global descriptor 订阅其他机器全局描述子
71. 				robot.sub_descriptors = this->create_subscription<dcl_slam::msg::GlobalDescriptor>(
72. 					robot.name_+"/distributedMapping/globalDescriptors", 50,
73. 					[this, it](const dcl_slam::msg::GlobalDescriptor::SharedPtr msg) {
74. 						this->globalDescriptorHandler(msg, it);
75. 					});
76. 				// subscribe loop infomation 
77. 				robot.sub_loop_info = this->create_subscription<dcl_slam::msg::LoopInfo>(
78. 					robot.name_+"/distributedMapping/loopInfo", 50,
79. 					[this, it](const dcl_slam::msg::LoopInfo::SharedPtr msg) {
80. 						this->loopInfoHandler(msg, it);
81. 					});
82. 			}
83. 
84. 			if(global_optmization_enable_)
85. 			{
86. 				robot.sub_optimization_state = this->create_subscription<std_msgs::msg::Int8>(
87. 					robot.name_+"/distributedMapping/optimizationState", 50,
88. 					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
89. 						this->optStateHandler(msg, it);
90. 					});
91. 				robot.sub_rotation_estimate_state = this->create_subscription<std_msgs::msg::Int8>(
92. 					robot.name_+"/distributedMapping/rotationEstimateState", 50,
93. 					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
94. 						this->rotationStateHandler(msg, it);
95. 					});
96. 				robot.sub_pose_estimate_state = this->create_subscription<std_msgs::msg::Int8>(
97. 					robot.name_+"/distributedMapping/poseEstimateState", 50,
98. 					[this, it](const std_msgs::msg::Int8::SharedPtr msg) {
99. 						this->poseStateHandler(msg, it);
100. 					});
101. 				robot.sub_neighbor_rotation_estimates = this->create_subscription<dcl_slam::msg::NeighborEstimate>(
102. 					robot.name_+"/distributedMapping/neighborRotationEstimates", 50,
103. 					[this, it](const dcl_slam::msg::NeighborEstimate::SharedPtr msg) {
104. 						this->neighborRotationHandler(msg, it);
105. 					});
106. 				robot.sub_neighbor_pose_estimates = this->create_subscription<dcl_slam::msg::NeighborEstimate>(
107. 					robot.name_+"/distributedMapping/neighborPoseEstimates", 50,
108. 					[this, it](const dcl_slam::msg::NeighborEstimate::SharedPtr msg) {
109. 						this->neighborPoseHandler(msg, it);
110. 					});
111. 			}
112. 		}
113. 
114. 		/*** other ***/
115. 		robot.time_cloud_input_stamp = this->now();
116. 		robot.time_cloud_input = 0.0;
117. 
118. 		robot.keyframe_cloud.reset(new pcl::PointCloud<PointPose3D>());
119. 		robot.keyframe_cloud_array.clear();
120. 
121. 		robots.push_back(robot);
122. 	}
```

</details>


---

<sub>⏱️ 113m 39s</sub>

### ✅ `bash`

**Find handler implementations**

$ grep -n "globalDescriptorHandler\|loopInfoHandler\|optStateHandler\|rotationStateHandler\|poseStateHandler\|neighborRotationHandler\|neighborPoseHandler" /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp | head -30

<details>
<summary>19 lines</summary>

```
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:7: * 函数名：globalDescriptorHandler
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:11:void distributedMapping::globalDescriptorHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:20:	LOG(INFO) << "[globalDescriptorHandler(" << id << ")]" <<
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:27: * 函数名：optStateHandler
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:31:void distributedMapping::optStateHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:45: * 函数名：rotationStateHandler
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:49:void distributedMapping::rotationStateHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:57: * 函数名：poseStateHandler
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:61:void distributedMapping::poseStateHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:70: * 函数名：neighborRotationHandler
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:80:void distributedMapping::neighborRotationHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:115:	RCLCPP_DEBUG(this->get_logger(), "neighborRotationHandler<%d> from robot %d done? %d (%d/%d]", id_, id, msg->estimation_done,
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:212: * 函数名：neighborPoseHandler
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:219:void distributedMapping::neighborPoseHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp:255:	RCLCPP_DEBUG(this->get_logger(), "neighborPoseHandler<%d> from robot %d, done? %d [%d/%d]", id_, id, msg->estimation_done,
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp:6:void distributedMapping::loopInfoHandler(
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp:50:		LOG(INFO) << "[loopInfoHandler(" << id << ")]" << " check loop "
/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp:58:		LOG(INFO) << "[loopInfoHandler(" << id << ")] add loop "
<exited with exit code 0>
```

</details>


---

<sub>⏱️ 113m 46s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp (lines 1-120)

<details>
<summary>120 lines</summary>

```
1. #include "distributedMapping.h"
2. 
3. /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
4. 	class distributedMapping: handle message callback 
5. * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
6. /*
7.  * 函数名：globalDescriptorHandler
8.  * 作用：接收来自其他机器人的全局描述子消息。
9.  * 实现机制：将接收到的描述子及其对应的时间戳和索引存储到 robots[id] 结构和 store_descriptors 队列中，用于后续的回环检测。
10.  */
11. void distributedMapping::globalDescriptorHandler(
12. 	const dcl_slam::msg::GlobalDescriptor::SharedPtr msg,
13. 	const int& id)
14. {
15. 	// save timestamp
16. 	robots[id].time_cloud_input_stamp = msg->header.stamp;
17. 	robots[id].time_cloud_input = robots[id].time_cloud_input_stamp.seconds();
18. 
19. 	// save descriptors
20. 	LOG(INFO) << "[globalDescriptorHandler(" << id << ")]" <<
21. 		" saveDescriptorAndKey:" << msg->index <<  "." << endl;
22. 	// keyframe_descriptor->saveDescriptorAndKey(msg->values.data(), id, msg->index);
23. 	store_descriptors.emplace_back(make_pair(id,*msg));
24. }
25. 
26. /*
27.  * 函数名：optStateHandler
28.  * 作用：接收邻居机器人的优化状态消息。
29.  * 实现机制：更新邻居机器人的优化状态（是否开始优化、当前状态枚举值），并将其加入通信范围内的邻居集合。
30.  */
31. void distributedMapping::optStateHandler(
32. 	const std_msgs::msg::Int8::SharedPtr msg,
33. 	const int& id)
34. {
35. 	neighbors_started_optimization[id] = (OptimizerState)msg->data <= OptimizerState::Start;
36. 	neighbor_state[id] = (OptimizerState)msg->data;
37. 	neighbors_lowest_id_included[id] = lowest_id_included;
38. 	if(neighbors_within_communication_range.find(id) == neighbors_within_communication_range.end())
39. 	{
40. 		neighbors_within_communication_range.insert(id);
41. 	}
42. }
43. 
44. /*
45.  * 函数名：rotationStateHandler
46.  * 作用：接收邻居机器人的旋转估计完成状态。
47.  * 实现机制：更新 neighbors_rotation_estimate_finished 标志位。
48.  */
49. void distributedMapping::rotationStateHandler(
50. 	const std_msgs::msg::Int8::SharedPtr msg,
51. 	const int& id)
52. {
53. 	neighbors_rotation_estimate_finished[id] = msg->data;
54. }
55. 
56. /*
57.  * 函数名：poseStateHandler
58.  * 作用：接收邻居机器人的位姿估计完成状态。
59.  * 实现机制：更新 neighbors_pose_estimate_finished 和 neighbors_estimation_done 标志位。
60.  */
61. void distributedMapping::poseStateHandler(
62. 	const std_msgs::msg::Int8::SharedPtr msg,
63. 	const int& id)
64. {
65. 	neighbors_pose_estimate_finished[id] = msg->data;
66. 	neighbors_estimation_done[id] = msg->data;
67. }
68. 
69. /*
70.  * 函数名：neighborRotationHandler
71.  * 作用：接收邻居机器人的旋转估计值，并进行分布式旋转优化。
72.  * 实现机制：
73.  * 1. 检查消息来源和当前优化状态。
74.  * 2. 更新优化器中邻居的线性化旋转值。
75.  * 3. 检查所有邻居是否完成初始化。
76.  * 4. 如果所有邻居就绪且自身未完成，执行旋转估计 (estimateRotation) 和更新。
77.  * 5. 检查收敛条件或最大迭代次数，判断旋转估计是否完成。
78.  * 6. 向后序机器人发送旋转估计值，或者如果所有邻居都完成，发布完成状态。
79.  */
80. void distributedMapping::neighborRotationHandler(
81. 	const dcl_slam::msg::NeighborEstimate::SharedPtr msg,
82. 	const int& id)
83. {
84. 	if(msg->receiver_id != id_ || optimizer_state != OptimizerState::RotationEstimation)
85. 	{
86. 		return;
87. 	}
88. 
89. 	/*** prepare for optimization ***/
90. 	// update neighbor roatation estimates
91. 	for(int i = 0; i < msg->pose_id.size(); i++)
92. 	{
93. 		Symbol symbol((id + 'a'), msg->pose_id[i]);
94. 		if(!use_pcm_ || other_robot_keys_for_optimization.find(symbol.key()) != other_robot_keys_for_optimization.end())
95. 		{
96. 			Vector rotation_matrix(9);
97. 			rotation_matrix << msg->estimate[0 + 9*i], msg->estimate[1 + 9*i], msg->estimate[2 + 9*i],
98. 				msg->estimate[3 + 9*i], msg->estimate[4 + 9*i], msg->estimate[5 + 9*i],
99. 				msg->estimate[6 + 9*i], msg->estimate[7 + 9*i], msg->estimate[8 + 9*i];
100. 			optimizer->updateNeighborLinearizedRotations(symbol.key(), rotation_matrix);
101. 		}
102. 		else
103. 		{
104. 			RCLCPP_DEBUG(this->get_logger(), "Stop optimization<%d>. Key %d %d doesn't exist.", id_, symbol.chr(), symbol.index());
105. 			abortOptimization(false);
106. 		}
107. 	}
108. 	// update neighbor flags
109. 	if(optimizer_state == OptimizerState::RotationEstimation)
110. 	{
111. 		// used only with flagged initialization
112. 		optimizer->updateNeighboringRobotInitialized(char(id + 'a'), msg->initialized);
113. 		neighbors_rotation_estimate_finished[id] = msg->estimation_done;
114. 	}
115. 	RCLCPP_DEBUG(this->get_logger(), "neighborRotationHandler<%d> from robot %d done? %d (%d/%d]", id_, id, msg->estimation_done,
116. 		optimizer->getNeighboringRobotsInit().size(), optimization_order.size() - 1);
117. 
118. 	/*** perform rotation optimization ***/
119. 	// all other robot is finished rotation optimization
120. 	if(optimizer->getNeighboringRobotsInit().size() == optimization_order.size() - 1)
```

</details>


---

<sub>⏱️ 113m 46s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp (lines 1-70)

<details>
<summary>70 lines</summary>

```
1. #include "distributedMapping.h"
2. 
3. /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
4. 	class distributedMapping: handle message callback 
5. * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
6. void distributedMapping::loopInfoHandler(
7. 	const dcl_slam::msg::LoopInfo::SharedPtr msg,
8. 	const int& id)
9. {
10. 	// Situation 1: need to add pointcloud for loop closure verification
11. 	if((int)msg->noise == 999)
12. 	{
13. 		if(msg->robot0 != id_)
14. 		{
15. 			return;
16. 		}
17. 
18. 		// copy message
19. 		dcl_slam::msg::LoopInfo loop_msg;
20. 		loop_msg.robot0 = msg->robot0;
21. 		loop_msg.robot1 = msg->robot1;
22. 		loop_msg.index0 = msg->index0;
23. 		loop_msg.index1 = msg->index1;
24. 		loop_msg.init_yaw = msg->init_yaw;
25. 		loop_msg.noise = 888.0; // this loop need verification
26. 
27. 		CHECK_LT(loop_msg.index0,keyposes_cloud_6d->size());
28. 		CHECK_LT(loop_msg.index0,robots[id_].keyframe_cloud_array.size());
29. 
30. 		// filtered pointcloud
31. 		pcl::PointCloud<PointPose3D>::Ptr cloudTemp(new pcl::PointCloud<PointPose3D>());
32. 		*cloudTemp = robots[id_].keyframe_cloud_array[loop_msg.index0];
33. 		downsample_filter_for_inter_loop2.setInputCloud(cloudTemp);
34. 		downsample_filter_for_inter_loop2.filter(*cloudTemp);
35. 		pcl::toROSMsg(*cloudTemp, loop_msg.scan_cloud);
36. 		// relative pose
37. 		loop_msg.pose0 = gtsamPoseToTransform(pclPointTogtsamPose3(keyposes_cloud_6d->points[loop_msg.index0]));
38. 
39. 		// publish to others for verification
40. 		robots[id_].pub_loop_info->publish(loop_msg);
41. 	}
42. 	// Situation 2: need to verify loop closure in this robot
43. 	else if((int)msg->noise == 888)
44. 	{
45. 		if(msg->robot1 != id_)
46. 		{
47. 			return;
48. 		}
49. 
50. 		LOG(INFO) << "[loopInfoHandler(" << id << ")]" << " check loop "
51. 			<< msg->robot0 << "-" << msg->index0 << " " << msg->robot1 << "-" << msg->index1 << "." << endl;
52. 
53. 		loop_closures_candidates.push_back(*msg);
54. 	}
55. 	// Situation 3: add verified loop closure
56. 	else
57. 	{
58. 		LOG(INFO) << "[loopInfoHandler(" << id << ")] add loop "
59. 			<< msg->robot0 << "-" << msg->index0 << " " << msg->robot1 << "-" << msg->index1 << "." << endl;
60. 
61. 		// extract loop
62. 		Vector Vector6(6);
63. 		Vector6 << msg->noise, msg->noise, msg->noise, msg->noise, msg->noise, msg->noise;
64. 		noiseModel::Diagonal::shared_ptr loop_noise = noiseModel::Diagonal::Variances(Vector6);
65. 		Pose3 pose_between = transformToGtsamPose(msg->pose_between);
66. 		NonlinearFactor::shared_ptr factor(new BetweenFactor<Pose3>(
67. 			Symbol('a'+msg->robot0, msg->index0), Symbol('a'+msg->robot1, msg->index1), pose_between, loop_noise));
68. 		
69. 		// update adjacency matrix
70. 		adjacency_matrix(msg->robot0, msg->robot1) += 1;
```

</details>


---

<sub>⏱️ 113m 51s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp (lines 120-260)

<details>
<summary>141 lines</summary>

```
120. 	if(optimizer->getNeighboringRobotsInit().size() == optimization_order.size() - 1)
121. 	{
122. 		if(!estimation_done)
123. 		{
124. 			// rotation estimation
125. 			try
126. 			{
127. 				optimizer->estimateRotation();
128. 				optimizer->updateRotation();
129. 				optimizer->updateInitialized(true);
130. 				current_rotation_estimate_iteration++;
131. 			}
132. 			catch(const std::exception& ex)
133. 			{
134. 				RCLCPP_WARN(this->get_logger(), "Stopping rotation optimization<%d> : %s.", id_, ex.what());
135. 				abortOptimization(true);
136. 			}
137. 
138. 			// if change is small enough, end rotation optimization
139. 			if((optimizer->latestChange() <= rotation_estimate_change_threshold_) ||
140. 				(current_rotation_estimate_iteration >= optmization_maximum_iteration_))
141. 			{
142. 				rotation_estimate_finished = true;
143. 				estimation_done = true;
144. 			}
145. 			RCLCPP_DEBUG(this->get_logger(), "--->Rotation estimation<%d> iter:[%d/%d] change:%.4f.", id_,
146. 				current_rotation_estimate_iteration, optmization_maximum_iteration_, optimizer->latestChange());
147. 		}
148. 
149. 		// check neigbors rotation optimization state
150. 		bool send_flag = estimation_done;
151. 		for(int i = 0; i < optimization_order.size(); i++)
152. 		{
153. 			int other_robot = optimization_order[i];
154. 			if(!neighbors_rotation_estimate_finished[other_robot] && other_robot != id_)
155. 			{
156. 				send_flag = false;
157. 			}
158. 		}
159. 		// send rotation estimate to the aft-order robot
160. 		if(!send_flag)
161. 		{
162. 			// clear buffer
163. 			for(const auto& neighbor : neighbors_within_communication_range)
164. 			{
165. 				robots[neighbor].estimate_msg.pose_id.clear();
166. 				robots[neighbor].estimate_msg.estimate.clear();
167. 			}
168. 			// extract rotation estimate for each loop closure
169. 			for(const std::pair<Symbol, Symbol>& separator_symbols: optimizer->separatorsSymbols())
170. 			{
171. 				// robot id
172. 				int other_robot = (int)(separator_symbols.first.chr() - 'a');
173. 				// pose id
174. 				robots[other_robot].estimate_msg.pose_id.push_back(separator_symbols.second.index());
175. 				// rotation estimates
176. 				Vector rotation_estimate = optimizer->linearizedRotationAt(separator_symbols.second.key());
177. 				for(int it = 0; it < 9; it++)
178. 				{
179. 					robots[other_robot].estimate_msg.estimate.push_back(rotation_estimate[it]);
180. 				}
181. 			}
182. 			// send rotation estimate
183. 			int publish_flag = false;
184. 			for(int i = 0; i < optimization_order.size(); i++)
185. 			{
186. 				int other_robot = optimization_order[i];
187. 				if(other_robot == id_)
188. 				{
189. 					publish_flag = true;
190. 					continue;
191. 				}
192. 
193. 				if(publish_flag)
194. 				{
195. 					robots[other_robot].estimate_msg.initialized = optimizer->isRobotInitialized();
196. 					robots[other_robot].estimate_msg.receiver_id = other_robot;
197. 					robots[other_robot].estimate_msg.estimation_done = estimation_done;
198. 					robots[id_].pub_neighbor_rotation_estimates->publish(robots[other_robot].estimate_msg);
199. 				}
200. 			}
201. 
202. 			rotation_estimate_start = false;
203. 			optimizer->clearNeighboringRobotInit();
204. 		}
205. 		// send rotation optimization state
206. 		state_msg.data = estimation_done? 1:0;
207. 		robots[id_].pub_rotation_estimate_state->publish(state_msg);
208. 	}
209. }
210. 
211. /*
212.  * 函数名：neighborPoseHandler
213.  * 作用：接收邻居机器人的位姿估计值，并进行分布式位姿优化。
214.  * 实现机制：
215.  * 1. 保存邻居的锚点偏移。
216.  * 2. 更新来自邻居的位姿估计值。
217.  * 3. 执行分布式位姿优化逻辑 (具体实现依赖后续代码块)。
218.  */
219. void distributedMapping::neighborPoseHandler(
220. 	const dcl_slam::msg::NeighborEstimate::SharedPtr msg,
221. 	const int& id)
222. {
223. 	if(msg->receiver_id != id_ || optimizer_state != OptimizerState::PoseEstimation)
224. 	{
225. 		return;
226. 	}
227. 
228. 	/*** prepare for optimization ***/
229. 	// save neighbor anchor offset
230. 	if(msg->estimation_done)
231. 	{
232. 		Point3 offset(msg->anchor_offset[0], msg->anchor_offset[1], msg->anchor_offset[2]);
233. 		neighbors_anchor_offset[id] = offset;
234. 	}
235. 	// update neighbor pose estimates
236. 	for(int i = 0; i < msg->pose_id.size(); i++)
237. 	{
238. 		if(neighbors_within_communication_range.find(id) != neighbors_within_communication_range.end())
239. 		{
240. 			Symbol symbol((id + 'a'), msg->pose_id[i]);
241. 			Vector pose_vector(6);
242. 			pose_vector << msg->estimate[0 + 6*i], msg->estimate[1 + 6*i], msg->estimate[2 + 6*i],
243. 				msg->estimate[3 + 6*i], msg->estimate[4 + 6*i], msg->estimate[5 + 6*i];
244. 			optimizer->updateNeighborLinearizedPoses(symbol.key(), pose_vector);
245. 		}
246. 	}
247. 	// update neighbor flags
248. 	if(optimizer_state == OptimizerState::PoseEstimation)
249. 	{
250. 		// used only with flagged initialization
251. 		optimizer->updateNeighboringRobotInitialized(char(id + 'a'), msg->initialized);
252. 		neighbors_estimation_done[id] = msg->estimation_done;
253. 		neighbors_pose_estimate_finished[id] = msg->estimation_done;
254. 	}
255. 	RCLCPP_DEBUG(this->get_logger(), "neighborPoseHandler<%d> from robot %d, done? %d [%d/%d]", id_, id, msg->estimation_done,
256. 		optimizer->getNeighboringRobotsInit().size(), optimization_order.size() - 1);
257. 
258. 	/*** perform pose estimation ***/
259. 	if(optimizer->getNeighboringRobotsInit().size() == optimization_order.size() - 1)
260. 	{
```

</details>


---

<sub>⏱️ 113m 51s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedLoopClosure.cpp (lines 400-500)

<details>
<summary>101 lines</summary>

```
400. 		loop_closures_candidates.push_back(inter_loop);
401. 		return;
402. 	}
403. 
404. 	// logging
405. 	LOG(INFO) << "[performExternLoopClosure<" << id_ << ">] Loop: "
406. 		<< inter_loop.robot0 << " " << inter_loop.index0 << " "
407. 		<< inter_loop.robot1 << " " << inter_loop.index1 << endl;
408. 	
409. 	// get initial pose
410. 	CHECK_LT(inter_loop.index1, initial_values->size());
411. 	double initial_yaw_;
412. 	if (descriptor_type_num_ == DescriptorType::LidarIris)
413. 	{
414. 		initial_yaw_ = (inter_loop.init_yaw+1)*2*M_PI/60.0;
415. 	}
416. 	else
417. 	{
418. 		initial_yaw_ = inter_loop.init_yaw*M_PI/180.0;
419. 	}
420. 	if(initial_yaw_ > M_PI)
421. 		initial_yaw_ -= 2*M_PI;
422. 	
423. 	auto initial_loop_pose0 = initial_values->at<Pose3>(loop_symbol1);
424. 
425. 	auto loop_pose0 = Pose3(
426. 		Rot3::RzRyRx(
427. 			initial_loop_pose0.rotation().roll(),
428. 			initial_loop_pose0.rotation().pitch(),
429. 			initial_loop_pose0.rotation().yaw() + initial_yaw_),
430. 		Point3(
431. 			initial_loop_pose0.translation().x(),
432. 			initial_loop_pose0.translation().y(),
433. 			initial_loop_pose0.translation().z()));
434. 
435. 	auto loop_pose1 = initial_values->at<Pose3>(loop_symbol1);
436. 
437. 	// extract cloud
438. 	pcl::PointCloud<PointPose3D>::Ptr scan_cloud_ds(new pcl::PointCloud<PointPose3D>());
439. 	pcl::fromROSMsg(inter_loop.scan_cloud, *scan_cloud_ds);
440. 	*scan_cloud_ds = *transformPointCloud(*scan_cloud_ds, loop_pose0);
441. 	pcl::PointCloud<PointPose3D>::Ptr map_cloud(new pcl::PointCloud<PointPose3D>());
442. 	pcl::PointCloud<PointPose3D>::Ptr map_cloud_ds(new pcl::PointCloud<PointPose3D>());
443. 	loopFindGlobalNearKeyframes(map_cloud, inter_loop.index1, history_keyframe_search_num_);
444. 	downsample_filter_for_inter_loop.setInputCloud(map_cloud); // downsample near keyframes
445. 	downsample_filter_for_inter_loop.filter(*map_cloud_ds);
446. 
447. 	// safe check for cloud
448. 	if (scan_cloud_ds->size() < 300 || map_cloud_ds->size() < 1000)
449. 	{
450. 		RCLCPP_WARN(this->get_logger(), "keyFrameCloud too little points 2");
451. 		return;
452. 	}
453. 	if (!scan_cloud_ds->is_dense || !map_cloud_ds->is_dense)
454. 	{
455. 		RCLCPP_WARN(this->get_logger(), "keyFrameCloud is not dense");
456. 		return;
457. 	}
458. 
459. 	// publish cloud
460. 	if(pub_scan_of_scan2map->get_subscription_count() != 0)
461. 	{
462. 		sensor_msgs::msg::PointCloud2 scan_cloud_msg;
463. 		pcl::toROSMsg(*scan_cloud_ds, scan_cloud_msg);
464. 		scan_cloud_msg.header.stamp = this->now();
465. 		scan_cloud_msg.header.frame_id = world_frame_;
466. 		pub_scan_of_scan2map->publish(scan_cloud_msg);
467. 	}
468. 	if(pub_map_of_scan2map->get_subscription_count() != 0)
469. 	{
470. 		sensor_msgs::msg::PointCloud2 map_cloud_msg;
471. 		pcl::toROSMsg(*map_cloud_ds, map_cloud_msg);
472. 		map_cloud_msg.header.stamp = this->now();
473. 		map_cloud_msg.header.frame_id = world_frame_;
474. 		pub_map_of_scan2map->publish(map_cloud_msg);
475. 	}
476. 
477. 	/*** calculate transform using icp ***/
478. 	// ICP Settings
479. 	static pcl::IterativeClosestPoint<PointPose3D, PointPose3D> icp;
480. 	icp.setMaxCorrespondenceDistance(30);
481. 	icp.setMaximumIterations(100);	
482. 	icp.setTransformationEpsilon(1e-6);
483. 	icp.setEuclideanFitnessEpsilon(1e-6);
484. 	icp.setRANSACIterations(ransac_maximum_iteration_);
485. 	icp.setRANSACOutlierRejectionThreshold(ransac_outlier_reject_threshold_);
486. 
487. 	// Align clouds
488. 	icp.setInputSource(scan_cloud_ds);
489. 	icp.setInputTarget(map_cloud_ds);
490. 	pcl::PointCloud<PointPose3D>::Ptr correct_scan_cloud_ds(new pcl::PointCloud<PointPose3D>());
491. 	icp.align(*correct_scan_cloud_ds);
492. 	inter_loop.noise = icp.getFitnessScore();
493. 
494. 	/*** verification using RANSAC ***/
495. 	// initial matching
496. 	pcl::CorrespondencesPtr correspondences(new pcl::Correspondences);
497. 	pcl::registration::CorrespondenceEstimation<PointPose3D, PointPose3D> correspondence_estimation;
498. 	correspondence_estimation.setInputCloud(correct_scan_cloud_ds);
499. 	correspondence_estimation.setInputTarget(map_cloud_ds);
500. 	correspondence_estimation.determineCorrespondences(*correspondences);
```

</details>


---

<sub>⏱️ 113m 55s</sub>

### 💬 Copilot

DCL-SLAM 的多机器人通信机制是 **纯ROS2 Topic 发布/订阅模式**，没有使用 Service、Action 或自定义网络协议。核心设计如下：

---

### 通信架构

每个机器人运行独立的 `fastlio_mapping` 进程，namespace 不同（`/a`、`/b`、`/c`...）。通信基于**命名约定**：

```
本机(id=0, /a): 只 publish 自己的 topic → /a/distributedMapping/...
                 subscribe 所有其他机器人的 topic → /b/distributedMapping/..., /c/...
```

构造函数中的循环逻辑（`distributedMappingBasis.cpp:27-122`）：
- `it == id_` → 创建 **publisher**（向外广播自己的数据）
- `it != id_` → 创建 **subscriber**（监听其他机器人的数据）

---

### 7 类通信 Topic

| Topic | 消息类型 | 方向 | 用途 |
|-------|---------|------|------|
| `globalDescriptors` | `GlobalDescriptor` | 广播描述子 → 他人接收 | **回环检测**：交换关键帧全局描述子 |
| `loopInfo` | `LoopInfo` | 双向（3阶段协议） | **回环验证**：请求点云 → 发送点云 → 确认回环 |
| `optimizationState` | `Int8` | 广播 → 他人接收 | **优化同步**：通知邻居自己的优化状态 |
| `rotationEstimateState` | `Int8` | 广播 → 他人接收 | **旋转同步**：旋转估计是否完成 |
| `poseEstimateState` | `Int8` | 广播 → 他人接收 | **位姿同步**：位姿估计是否完成 |
| `neighborRotationEstimates` | `NeighborEstimate` | 定向发送（带receiver_id） | **旋转优化**：交换旋转估计值（9维旋转矩阵） |
| `neighborPoseEstimates` | `NeighborEstimate` | 定向发送（带receiver_id） | **位姿优化**：交换位姿估计值（6维向量） |

---

### 三大通信协议

**1. 跨机器人回环检测（描述子交换）**
```
机器人A 生成关键帧 → publish GlobalDescriptor
机器人B 收到 → globalDescriptorHandler → 存入 store_descriptors
机器人B 回环线程 → performInterLoopClosure → 描述子匹配
```

**2. 跨机器人回环验证（3阶段握手）**
```
阶段1: B检测到与A的回环候选 → publish LoopInfo(noise=999) 请求A的点云
阶段2: A收到 → loopInfoHandler → 回传点云 publish LoopInfo(noise=888)
阶段3: B收到点云 → performExternLoopClosure → ICP+RANSAC验证
       → 验证通过 → publish LoopInfo(noise=实际值) 添加回环约束
```

**3. 分布式位姿图优化（DGS顺序传递）**
```
按 optimization_order 顺序，每个机器人：
  1. 收到前序机器人的估计值 → neighborRotationHandler / neighborPoseHandler
  2. 执行本地优化 → estimateRotation/estimatePoses
  3. 向后序机器人发送估计值 → publish NeighborEstimate(receiver_id=后序)
  4. 迭代直到收敛或达到最大迭代次数
```

关键点：`NeighborEstimate` 消息包含 `receiver_id` 字段，虽然通过广播 topic 发送，但接收端会**过滤非目标消息**（`msg-\>receiver_id != id_` 时直接 return），实现了逻辑上的点对点通信。

---

<sub>⏱️ 120m 37s</sub>

### 👤 User

每个机器人需要怎么做才能通信,怎么验证通信正常

---

<sub>⏱️ 120m 43s</sub>

### 💭 Reasoning

*The user wants to know the practical steps for setting up communication between robots and how to verify it's working. Let me look at the config files and launch files to understand the deployment requirements.*

---

<sub>⏱️ 120m 47s</sub>

### 💬 Copilot





---

<sub>⏱️ 120m 47s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/config/dcl_fast_lio_mid360.yaml.**

/home/isaac/DCL-SLAM/src/dcl_slam/config/dcl_fast_lio_mid360.yaml

<details>
<summary>85 lines</summary>

```
1. # ROS2 参数文件格式：必须包裹在 节点名/ros__parameters 下
2. /**:
3.   ros__parameters:
4.     # ===== DCL-SLAM 后端参数 =====
5.     # frames
6.     world_frame: "world"                    # 全局坐标系
7.     odom_frame: "camera_init"               # 局部坐标系
8. 
9.     # lidar configuration
10.     sensor: "livox"                         # 雷达类型: velodyne / livox / ouster
11.     n_scan: 4                               # MID-360 有 4 线非重复扫描
12. 
13.     # CPU params
14.     onboard_cpu_cores_num: 4                # 机载CPU核数
15.     loop_closure_process_interval: 0.01     # 闭环检测间隔（秒）
16.     map_publish_interval: 10.0              # 全局地图发布间隔（秒）
17.     mapping_process_interval: 0.02          # 优化间隔（秒）
18. 
19.     # keyframe params
20.     keyframe_distance_threshold: 1.0        # 关键帧距离阈值（米）
21.     keyframe_angle_threshold: 0.2           # 关键帧角度阈值（弧度）
22. 
23.     # mapping
24.     global_optmization_enable: true         # 开启分布式 DGS 优化
25.     use_between_noise: true                 # 使用 between noise
26.     optmization_maximum_iteration: 30       # 最大优化迭代次数
27.     use_pcm: false                          # 开启 PCM 一致性校验
28.     pcm_threshold: 0.5                      # PCM 置信度
29. 
30.     # downsample
31.     descript_leaf_size: 0.1                 # 描述子降采样分辨率
32.     map_leaf_size: 0.4                      # 扫描匹配降采样分辨率
33. 
34.     # loop closure
35.     intra_robot_loop_closure_enable: false  # 单机闭环
36.     inter_robot_loop_closure_enable: true   # 多机闭环
37.     descriptor_type: "LidarIris"            # 描述子类型: ScanContext / LidarIris / M2DP
38.     knn_candidates: 6                       # KNN 候选数
39.     match_mode: 2                           # Iris 匹配模式 (0/1/2)
40.     descriptor_distance_threshold: 0.35     # 描述子匹配阈值
41.     exclude_recent_frame_num: 50            # 排除最近N帧（避免误匹配）
42.     search_radius: 15.0                     # 闭环搜索半径（米）
43.     history_keyframe_search_num: 18         # 子地图历史帧数
44.     fitness_score_threshold: 0.2            # ICP 匹配得分阈值
45.     ransac_maximum_iteration: 2000          # RANSAC 最大迭代
46.     ransac_threshold: 0.3                   # RANSAC 阈值
47.     ransac_outlier_reject_threshold: 0.3    # RANSAC 外点剔除距离
48. 
49.     # ===== FAST-LIO 前端参数 =====
50.     common:
51.       lid_topic: "/livox/lidar"             # MID-360 默认话题名（绝对路径，避免命名空间前缀）
52.       imu_topic: "/livox/imu"              # MID-360 内置 IMU 话题名（绝对路径）
53.       time_sync_en: false                   # MID-360 硬件同步，无需软件同步
54. 
55.     preprocess:
56.       lidar_type: 1              # 1=Livox, 2=Velodyne, 3=Ouster
57.       scan_line: 4               # MID-360: 4 线
58.       scan_rate: 10              # 扫描频率 10Hz
59.       timestamp_unit: 3          # Livox 时间戳单位: 3=纳秒（与原版 FAST-LIO mid360.yaml 一致）
60.       blind: 0.5                 # 最小检测距离 0.5m（MID-360 近距有效）
61. 
62.     mapping:
63.       acc_cov: 0.1               # 加速度计噪声协方差
64.       gyr_cov: 0.1               # 陀螺仪噪声协方差
65.       b_acc_cov: 0.0001          # 加速度计偏置协方差
66.       b_gyr_cov: 0.0001          # 陀螺仪偏置协方差
67.       fov_degree: 360.0          # MID-360 水平视场角 360°
68.       det_range: 40.0            # MID-360 最大检测距离 40m
69.       extrinsic_est_en: false    # 是否在线估计外参
70.       # MID-360 的 IMU 和雷达一体化，外参接近单位阵
71.       extrinsic_T: [0.04165, 0.02326, -0.0284]
72.       extrinsic_R: [1.0, 0.0, 0.0,
73.                      0.0, 1.0, 0.0,
74.                      0.0, 0.0, 1.0]
75. 
76.     publish:
77.       path_en: true              # 发布轨迹路径
78.       scan_publish_en: true      # 发布点云
79.       dense_publish_en: true     # 发布稠密点云
80.       scan_bodyframe_pub_en: true  # 在 IMU 体坐标系下发布点云
81. 
82.     pcd_save:
83.       pcd_save_en: false         # 是否保存 PCD 文件
84.       interval: -1               # -1 表示全部保存到一个文件
85.
```

</details>


---

<sub>⏱️ 120m 47s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/launch/dcl_fast_lio_mid360.launch.py.**

/home/isaac/DCL-SLAM/src/dcl_slam/launch/dcl_fast_lio_mid360.launch.py

<details>
<summary>48 lines</summary>

```
1. import os
2. from ament_index_python.packages import get_package_share_directory
3. from launch import LaunchDescription
4. from launch.actions import DeclareLaunchArgument
5. from launch.substitutions import LaunchConfiguration
6. from launch.conditions import IfCondition
7. from launch_ros.actions import Node
8. 
9. 
10. def generate_launch_description():
11.     dcl_slam_dir = get_package_share_directory('dcl_slam')
12.     config_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360.yaml')
13.     rviz_path = os.path.join(dcl_slam_dir, 'config', 'dcl_fast_lio_mid360.rviz')
14. 
15.     rviz_use = LaunchConfiguration('rviz')
16. 
17.     declare_rviz_cmd = DeclareLaunchArgument(
18.         'rviz', default_value='true',
19.         description='Launch RViz'
20.     )
21. 
22.     fastlio_node = Node(
23.         package='dcl_fast_lio',
24.         executable='fastlio_mapping',
25.         name='laserMapping',
26.         namespace='/a',
27.         parameters=[config_path],
28.         output='screen',
29.         remappings=[
30.             ('/a/livox/imu', '/livox/imu'),
31.             ('/a/livox/lidar', '/livox/lidar'),
32.         ]
33.     )
34. 
35.     rviz_node = Node(
36.         package='rviz2',
37.         executable='rviz2',
38.         arguments=['-d', rviz_path],
39.         condition=IfCondition(rviz_use)
40.     )
41. 
42.     ld = LaunchDescription()
43.     ld.add_action(declare_rviz_cmd)
44.     ld.add_action(fastlio_node)
45.     ld.add_action(rviz_node)
46. 
47.     return ld
48.
```

</details>


---

<sub>⏱️ 120m 47s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/launch/run.launch.py.**

/home/isaac/DCL-SLAM/src/dcl_slam/launch/run.launch.py

<details>
<summary>105 lines</summary>

```
1. import os
2. from ament_index_python.packages import get_package_share_directory
3. from launch import LaunchDescription
4. from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, GroupAction
5. from launch.launch_description_sources import PythonLaunchDescriptionSource
6. from launch.substitutions import LaunchConfiguration, EnvironmentVariable
7. from launch_ros.actions import Node
8. 
9. def generate_launch_description():
10.     # Get the package directory
11.     dcl_slam_dir = get_package_share_directory('dcl_slam')
12.     
13.     # Arguments
14.     set_lio_type = LaunchConfiguration('set_lio_type')
15.     set_respawn_value = LaunchConfiguration('set_respawn_value')
16.     
17.     declare_lio_type = DeclareLaunchArgument(
18.         'set_lio_type',
19.         default_value='1',
20.         description='1 for LIO-SAM, 2 for FAST-LIO2'
21.     )
22.     
23.     declare_respawn_value = DeclareLaunchArgument(
24.         'set_respawn_value',
25.         default_value='false',
26.         description='Respawn nodes if they crash'
27.     )
28. 
29.     # Global Parameters
30.     # Note: In ROS2, global parameters are less common. We usually pass them to nodes.
31.     # But for compatibility, we can use a shared yaml or pass to each node.
32.     # Here we assume single_ugv.launch.py handles passing params to its nodes.
33. 
34.     # Nodes
35.     rviz_config = os.path.join(dcl_slam_dir, 'config', 'dcl_rviz.rviz')
36.     rviz_node = Node(
37.         package='rviz2',
38.         executable='rviz2',
39.         name='dcl_rviz',
40.         arguments=['-d', rviz_config],
41.         output='screen'
42.     )
43.     
44.     loop_vis_node = Node(
45.         package='dcl_slam',
46.         executable='dcl_slam_loopVisualizationNode',
47.         name='dcl_slam_loopVisualizationNode',
48.         output='screen',
49.         parameters=[{'number_of_robots': 3}]
50.     )
51. 
52.     # Launch robots (a, b, c)
53.     # We need to convert single_ugv.launch to Python first, assuming it exists or we mock it.
54.     # Since single_ugv.launch is complex and calls other packages, we'll assume it's migrated too.
55.     
56.     launch_robot_a = IncludeLaunchDescription(
57.         PythonLaunchDescriptionSource(os.path.join(dcl_slam_dir, 'launch', 'single_ugv.launch.py')),
58.         launch_arguments={
59.             'robotPrefix': 'a',
60.             'respawnValue': set_respawn_value,
61.             'lioType': set_lio_type
62.         }.items()
63.     )
64.     
65.     launch_robot_b = IncludeLaunchDescription(
66.         PythonLaunchDescriptionSource(os.path.join(dcl_slam_dir, 'launch', 'single_ugv.launch.py')),
67.         launch_arguments={
68.             'robotPrefix': 'b',
69.             'respawnValue': set_respawn_value,
70.             'lioType': set_lio_type
71.         }.items()
72.     )
73.     
74.     launch_robot_c = IncludeLaunchDescription(
75.         PythonLaunchDescriptionSource(os.path.join(dcl_slam_dir, 'launch', 'single_ugv.launch.py')),
76.         launch_arguments={
77.             'robotPrefix': 'c',
78.             'respawnValue': set_respawn_value,
79.             'lioType': set_lio_type
80.         }.items()
81.     )
82.     
83.     # Bag players
84.     # Using ros2bag play. Syntax is different from rosbag play.
85.     # We need to find the bag files.
86.     home_dir = EnvironmentVariable('HOME')
87.     
88.     # Example bag file path construction (adjust as needed)
89.     # bag_path = [home_dir, '/rosbag-data/S3E/SYSU_LIBRARY.bag'] 
90.     
91.     # Note: 'rosbag play' in ROS1 supports -p for prefix. ROS2 'ros2 bag play' does NOT support prefix remapping easily in the same way.
92.     # We usually use topic remapping.
93.     # This part requires careful manual adjustment by the user.
94.     
95.     return LaunchDescription([
96.         declare_lio_type,
97.         declare_respawn_value,
98.         rviz_node,
99.         loop_vis_node,
100.         launch_robot_a,
101.         launch_robot_b,
102.         launch_robot_c,
103.         # Bag players omitted as they require complex remapping setup in ROS2
104.     ])
105.
```

</details>


---

<sub>⏱️ 120m 54s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/launch/single_ugv.launch.py.**

/home/isaac/DCL-SLAM/src/dcl_slam/launch/single_ugv.launch.py

<details>
<summary>100 lines</summary>

```
1. import os
2. from ament_index_python.packages import get_package_share_directory
3. from launch import LaunchDescription
4. from launch.actions import DeclareLaunchArgument, GroupAction, OpaqueFunction
5. from launch.conditions import IfCondition, LaunchConfigurationEquals
6. from launch.substitutions import LaunchConfiguration
7. from launch_ros.actions import Node, PushRosNamespace
8. 
9. def launch_setup(context, *args, **kwargs):
10.     dcl_slam_dir = get_package_share_directory('dcl_slam')
11.     robot_prefix = LaunchConfiguration('robotPrefix').perform(context)
12.     respawn_value = LaunchConfiguration('respawnValue')
13.     
14.     # Construct config file path
15.     # e.g., config/dcl_lio_sam_vlp16_params_a.yaml
16.     config_file_name = f'dcl_lio_sam_vlp16_params_{robot_prefix}.yaml'
17.     config_file_path = os.path.join(dcl_slam_dir, 'config', config_file_name)
18.     
19.     # Note: We are not checking if file exists here, assuming user configuration is correct.
20. 
21.     # Group for LIO-SAM (lioType == '1')
22.     lio_sam_group = GroupAction(
23.         condition=LaunchConfigurationEquals('lioType', '1'),
24.         actions=[
25.             PushRosNamespace(robot_prefix),
26.             
27.             # Note: The original launch file used <rosparam command="load"> which loads params into the namespace.
28.             # In ROS2, we usually pass parameters directly to nodes.
29.             # Since we don't know the exact nodes of dcl_lio_sam (it's external),
30.             # we will assume we pass the config file to the known nodes we invoke.
31.             # OR we can attempt to load it globally for the namespace (less common in ROS2 but possible via 'parameters' list).
32.             
33.             # Nodes
34.             Node(
35.                 package='dcl_lio_sam',
36.                 executable='dcl_lio_sam_imuPreintegration',
37.                 name='dcl_lio_sam_imuPreintegration',
38.                 output='screen',
39.                 respawn=True, # simplified
40.                 parameters=[config_file_path]
41.             ),
42.             Node(
43.                 package='dcl_lio_sam',
44.                 executable='dcl_lio_sam_imageProjection',
45.                 name='dcl_lio_sam_imageProjection',
46.                 output='screen',
47.                 respawn=True,
48.                 parameters=[config_file_path]
49.             ),
50.             Node(
51.                 package='dcl_lio_sam',
52.                 executable='dcl_lio_sam_featureExtraction',
53.                 name='dcl_lio_sam_featureExtraction',
54.                 output='screen',
55.                 respawn=True,
56.                 parameters=[config_file_path]
57.             ),
58.             Node(
59.                 package='dcl_lio_sam',
60.                 executable='dcl_lio_sam_mapOptmization',
61.                 name='dcl_lio_sam_mapOptmization',
62.                 output='screen',
63.                 respawn=True,
64.                 parameters=[config_file_path]
65.             ),
66.             
67.             # DCL-SLAM Distributed Mapping Node
68.             Node(
69.                 package='dcl_slam',
70.                 executable='dcl_slam_node',
71.                 name='dcl_slam_node',
72.                 output='screen',
73.                 parameters=[config_file_path]
74.             ),
75.             
76.             # Robot State Publisher
77.             Node(
78.                 package='robot_state_publisher',
79.                 executable='robot_state_publisher',
80.                 name='robot_state_publisher',
81.                 parameters=[{'tf_prefix': robot_prefix}],
82.                 arguments=[os.path.join(dcl_slam_dir, 'config', 'lio_sam_robot.urdf.xacro')]
83.             )
84.         ]
85.     )
86.     
87.     return [lio_sam_group]
88. 
89. def generate_launch_description():
90.     declare_lio_type = DeclareLaunchArgument('lioType', default_value='1')
91.     declare_robot_prefix = DeclareLaunchArgument('robotPrefix', default_value='a')
92.     declare_respawn_value = DeclareLaunchArgument('respawnValue', default_value='false')
93.     
94.     return LaunchDescription([
95.         declare_lio_type,
96.         declare_robot_prefix,
97.         declare_respawn_value,
98.         OpaqueFunction(function=launch_setup)
99.     ])
100.
```

</details>


---

<sub>⏱️ 121m 4s</sub>

### ✅ `bash`

**Search for multi-robot network config**

$ grep -n "number_of_robots\|ROS_DOMAIN_ID\|DDS\|multicast\|discovery" /home/isaac/DCL-SLAM/src/dcl_slam/config/*.yaml /home/isaac/DCL-SLAM/src/dcl_slam/launch/*.py 2>/dev/null

```
/home/isaac/DCL-SLAM/src/dcl_slam/launch/run.launch.py:49:        parameters=[{'number_of_robots': 3}]
<exited with exit code 0>
```


---

<sub>⏱️ 122m 10s</sub>

### ✅ `bash`

**Check number_of_robots in configs**

$ grep -rn "number_of_robots" /home/isaac/DCL-SLAM/src/dcl_slam/config/dcl_fast_lio_mid360.yaml /home/isaac/DCL-SLAM/src/dcl_slam/config/dcl_lio_sam_vlp16_params*.yaml 2>/dev/null | head -10

```

<exited with exit code 0>
```


---

<sub>⏱️ 121m 25s</sub>

### ✅ `view`

**view the file at /home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp.**

/home/isaac/DCL-SLAM/src/dcl_slam/src/distributedMapping.cpp (lines 609-700)

<details>
<summary>92 lines</summary>

```
609. void distributedMapping::makeDescriptors()
610. {
611. 	while (!store_descriptors.empty())
612. 	{
613. 		auto msg_data = store_descriptors.front().second;
614. 		auto msg_id = store_descriptors.front().first;
615. 		store_descriptors.pop_front();
616. 		keyframe_descriptor->saveDescriptorAndKey(msg_data.values.data(), msg_id, msg_data.index);
617. 	}
618. 
619. 	if(initial_values->empty() || (!intra_robot_loop_closure_enable_ && !inter_robot_loop_closure_enable_))
620. 	{
621. 		return;
622. 	}
623. 
624. 	// downsample keyframe
625. 	cloud_for_decript_ds->clear();
626. 	if(sensor_ == LiDARType::LIVOX)
627. 	{
628. 		// pcl::PointCloud<PointPose3D>::Ptr multiKeyFrameCloud(new pcl::PointCloud<PointPose3D>());
629. 		// loopFindNearKeyframes(multiKeyFrameCloud, initial_values->size() - 1, history_keyframe_search_num_);
630. 		// downsample_filter_for_descriptor.setInputCloud(multiKeyFrameCloud);
631. 		downsample_filter_for_descriptor.setInputCloud(robots[id_].keyframe_cloud);
632. 	}
633. 	else
634. 	{
635. 		downsample_filter_for_descriptor.setInputCloud(robots[id_].keyframe_cloud);
636. 	}
637. 	downsample_filter_for_descriptor.filter(*cloud_for_decript_ds);
638. 
639. 	// make and save global descriptors
640. 	auto descriptor_vec = keyframe_descriptor->makeAndSaveDescriptorAndKey(*cloud_for_decript_ds, id_, initial_values->size()-1);
641. 	LOG(INFO) << "makeDescriptors[" << id_ << "]." << endl;
642. 
643. 	// extract descriptors values
644. 	global_descriptor_msg.values.swap(descriptor_vec);
645. 	// keyfame index
646. 	global_descriptor_msg.index = initial_values->size()-1;
647. 	global_descriptor_msg.header.stamp = robots[id_].time_cloud_input_stamp;
648. 	// publish message
649. 	robots[id_].pub_descriptors->publish(global_descriptor_msg);
650. }
651. 
652. /*
653.  * 函数名：publishPath
654.  * 作用：发布全局路径和局部路径用于可视化。
655.  * 实现机制：检查是否有订阅者，如果有，更新时间戳并发布 nav_msgs::Path 消息。
656.  */
657. void distributedMapping::publishPath()
658. {
659. 	// publish path
660. 	if(pub_global_path->get_subscription_count() != 0)
661. 	{
662. 		global_path.header.stamp = robots[id_].time_cloud_input_stamp;
663. 		global_path.header.frame_id = world_frame_;
664. 		pub_global_path->publish(global_path);
665. 	}
666. 
667. 	if(pub_local_path->get_subscription_count() != 0)
668. 	{
669. 		local_path.header.stamp = robots[id_].time_cloud_input_stamp;
670. 		local_path.header.frame_id = robots[id_].odom_frame_;
671. 		pub_local_path->publish(local_path);
672. 	}
673. }
674. 
675. /*
676.  * 函数名：publishTransformation
677.  * 作用：发布世界坐标系到里程计坐标系的 TF 变换。
678.  * 实现机制：计算优化后的第一帧位姿与初始第一帧位姿的差值，作为 map 到 odom 的校正变换并发布。
679.  */
680. void distributedMapping::publishTransformation(
681. 	const rclcpp::Time& timestamp)
682. {
683. 	static tf2_ros::TransformBroadcaster world_to_odom_tf_broadcaster(this);
684. 	static Symbol first_key((id_+'a'), 0);
685. 
686. 	Pose3 first_pose = initial_values->at<Pose3>(first_key);
687. 	Pose3 old_first_pose = robots[id_].piror_odom;
688. 	Pose3 pose_between = first_pose * old_first_pose.inverse();
689. 
690. 	geometry_msgs::msg::TransformStamped world_to_odom;
691. 	world_to_odom.header.stamp = timestamp;
692. 	world_to_odom.header.frame_id = world_frame_;
693. 	world_to_odom.child_frame_id = robots[id_].odom_frame_;
694. 	
695. 	world_to_odom.transform.translation.x = pose_between.translation().x();
696. 	world_to_odom.transform.translation.y = pose_between.translation().y();
697. 	world_to_odom.transform.translation.z = pose_between.translation().z();
698. 	world_to_odom.transform.rotation.x = pose_between.rotation().toQuaternion().x();
699. 	world_to_odom.transform.rotation.y = pose_between.rotation().toQuaternion().y();
700. 	world_to_odom.transform.rotation.z = pose_between.rotation().toQuaternion().z();
```

</details>


---

<sub>⏱️ 122m 14s</sub>

### 💬 Copilot

现在信息充分了。以下是完整的回答：

---

## 每个机器人通信的前提条件

### 1. 关键配置要求

每个机器人进程必须满足 **3 个条件**：

**① namespace 必须是 `/a`、`/b`、`/c`...（小写字母）**

这是机器人身份的唯一来源（`paramsServer.cpp:17`），namespace 的第二个字符决定 ID：
```
/a → id=0,  /b → id=1,  /c → id=2 ...
```

当前的 `dcl_fast_lio_mid360.launch.py` 里**硬编码了 `namespace='/a'`**，所以只启动了一个机器人。要启动多台需要：
- 要么写多个 launch 文件或用参数控制 namespace
- 要么参考 `run.launch.py` 的方式用 `single_ugv.launch.py` 启动多台

**② `number_of_robots` 参数必须一致且 \> 1**

当前 `dcl_fast_lio_mid360.yaml` **没有配置** `number_of_robots`，默认值是 `1`（见 `paramsServer.cpp:33-34`）。这意味着构造函数中循环只走一轮（`it=0, it==id_`），**不会创建任何 subscriber**，完全没有跨机器人通信能力。

**③ 所有机器人在同一 ROS2 DDS 网络中**

即相同 `ROS_DOMAIN_ID`，且网络互通（同一局域网或配置了 DDS discovery）。

### 2. 通信建立的具体机制

构造函数中的循环（`distributedMappingBasis.cpp:27-122`）做了这件事：

```
假设 number_of_robots=3, 本机 id=0 (/a):

it=0 (自己): 创建 publisher → /a/distributedMapping/globalDescriptors 等
it=1 (/b):   创建 subscriber → /b/distributedMapping/globalDescriptors 等
it=2 (/c):   创建 subscriber → /c/distributedMapping/globalDescriptors 等
```

同时机器人B（id=1）做了：
```
it=0 (/a):   创建 subscriber → /a/distributedMapping/globalDescriptors
it=1 (自己): 创建 publisher → /b/distributedMapping/globalDescriptors
it=2 (/c):   创建 subscriber → /c/distributedMapping/globalDescriptors
```

**A publish → B subscribe 同一个 topic**，ROS2 DDS 自动发现并连接。

### 3. 启动两台机器人通信的最小配置

以当前 FAST-LIO + MID360 配置为例，需修改两处：

**步骤一**：yaml 中添加 `number_of_robots`:
```yaml
number_of_robots: 2
```

**步骤二**：用不同 namespace 启动两个进程：
```bash
# 终端1 - 机器人A
ros2 launch dcl_slam dcl_fast_lio_mid360.launch.py
# (已有 namespace='/a')

# 终端2 - 机器人B（需要修改launch或命令行覆盖namespace）
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file \<config_path\> \
  -r __ns:=/b \
  -r /b/livox/imu:=/livox/imu_b \
  -r /b/livox/lidar:=/livox/lidar_b
```

---

## 验证通信正常的方法

### 方法 1：检查 Topic 是否存在且有发布者/订阅者

```bash
# 查看所有 DCL-SLAM 相关 topic
ros2 topic list | grep distributedMapping

# 预期输出（2台机器人时）:
# /a/distributedMapping/globalDescriptors
# /a/distributedMapping/loopInfo
# /a/distributedMapping/optimizationState
# /b/distributedMapping/globalDescriptors
# /b/distributedMapping/loopInfo
# /b/distributedMapping/optimizationState
# ...

# 检查某个 topic 的发布者和订阅者数量
ros2 topic info /a/distributedMapping/globalDescriptors

# 预期: 1 publisher (机器人A), N-1 subscribers (其他机器人)
```

### 方法 2：监听描述子消息是否在流通

```bash
# 监听A的描述子（B应该能收到）
ros2 topic echo /a/distributedMapping/globalDescriptors --no-arr

# 监听B的描述子（A应该能收到）
ros2 topic echo /b/distributedMapping/globalDescriptors --no-arr
```

有输出说明描述子在正常交换。

### 方法 3：检查优化状态 Topic（验证 DGS 通信）

```bash
# 监听优化状态（global_optmization_enable 必须为 true）
ros2 topic echo /a/distributedMapping/optimizationState
ros2 topic echo /b/distributedMapping/optimizationState

# 数据含义: 0=Idle, 1=Start, 2=Initialization, ...
```

### 方法 4：查看 Topic 频率

```bash
ros2 topic hz /a/distributedMapping/globalDescriptors
# 预期: 与关键帧产生频率一致（约 1-10 Hz，取决于运动速度和阈值）
```

### 方法 5：查看日志

```bash
# 查看构造时的日志确认初始化成功
# 应看到:
#   [INFO] Robot Name: /a, ID: 0
#   [INFO] distributed mapping class initialization finish

# 查看glog日志确认描述子交换
cat ~/log/*.INFO | grep globalDescriptorHandler
# 预期: [globalDescriptorHandler(1)] saveDescriptorAndKey:42.
#        ↑ 收到了机器人1(B)的第42个描述子
```

### 方法 6：用 rqt_graph 可视化通信拓扑

```bash
ros2 run rqt_graph rqt_graph
```
可以直观看到各节点之间 topic 的连接关系。

---

### 常见通信失败排查

| 现象 | 原因 | 排查 |
|------|------|------|
| topic 列表只有 `/a/...` 没有 `/b/...` | 机器人B未启动或namespace错误 | 检查B的进程和namespace |
| topic 存在但 `info` 显示 0 subscribers | `number_of_robots=1` | 检查yaml配置 |
| topic 有发布者但 echo 无数据 | 机器人还没产生关键帧 | 等待机器人运动超过 `keyframe_distance_threshold` |
| 跨机器回环从未触发 | `inter_robot_loop_closure_enable: false` | 检查yaml配置 |
| DGS 优化从未启动 | `global_optmization_enable: false` | 检查yaml配置 |
| 两台机器在不同主机上无法通信 | `ROS_DOMAIN_ID` 不同或网络不通 | `export ROS_DOMAIN_ID=0` 保持一致，检查防火墙 |

---

<sub>⏱️ 125m 52s</sub>

### 👤 User

怎么保存对话记录

---

<sub>⏱️ 125m 59s</sub>

### ✅ `fetch_copilot_cli_documentation`

<details>
<summary>Arguments</summary>

```json
{}
```

</details>

<details>
<summary>207 lines</summary>

````
# GitHub Copilot CLI Documentation

## README
# GitHub Copilot CLI

The power of GitHub Copilot, now in your terminal.

GitHub Copilot CLI brings AI-powered coding assistance directly to your command line, enabling you to build, debug, and understand code through natural language conversations. Powered by the same agentic harness as GitHub's Copilot coding agent, it provides intelligent assistance while staying deeply integrated with your GitHub workflow.

See [our official documentation](https://docs.github.com/copilot/concepts/agents/about-copilot-cli) for more information.

![Image of the splash screen for the Copilot CLI](https://github.com/user-attachments/assets/f40aa23d-09dd-499e-9457-1d57d3368887)


## 🚀 Introduction and Overview

We're bringing the power of GitHub Copilot coding agent directly to your terminal. With GitHub Copilot CLI, you can work locally and synchronously with an AI agent that understands your code and GitHub context.

- **Terminal-native development:** Work with Copilot coding agent directly in your command line — no context switching required.
- **GitHub integration out of the box:** Access your repositories, issues, and pull requests using natural language, all authenticated with your existing GitHub account.
- **Agentic capabilities:** Build, edit, debug, and refactor code with an AI collaborator that can plan and execute complex tasks.
- **MCP-powered extensibility:** Take advantage of the fact that the coding agent ships with GitHub's MCP server by default and supports custom MCP servers to extend capabilities.
- **Full control:** Preview every action before execution — nothing happens without your explicit approval.

We're still early in our journey, but with your feedback, we're rapidly iterating to make the GitHub Copilot CLI the best possible companion in your terminal.

## 📦 Getting Started

### Supported Platforms

- **Linux**
- **macOS**
- **Windows**

### Prerequisites

- (On Windows) **PowerShell** v6 or higher
- An **active Copilot subscription**. See [Copilot plans](https://github.com/features/copilot/plans?ref_cta=Copilot+plans+signup&ref_loc=install-copilot-cli&ref_page=docs).

If you have access to GitHub Copilot via your organization or enterprise, you cannot use GitHub Copilot CLI if your organization owner or enterprise administrator has disabled it in the organization or enterprise settings. See [Managing policies and features for GitHub Copilot in your organization](http://docs.github.com/copilot/managing-copilot/managing-github-copilot-in-your-organization/managing-github-copilot-features-in-your-organization/managing-policies-for-copilot-in-your-organization) for more information.

### Installation

Install with the install script (macOS and Linux):

```bash
curl -fsSL https://gh.io/copilot-install | bash
```

Or

```bash
wget -qO- https://gh.io/copilot-install | bash
```

Use `| sudo bash` to run as root and install to `/usr/local/bin`.

Set `PREFIX` to install to `$PREFIX/bin/` directory. Defaults to `/usr/local`
when run as root or `$HOME/.local` when run as a non-root user.

Set `VERSION` to install a specific version. Defaults to the latest version.

For example, to install version `v0.0.369` to a custom directory:

```bash
curl -fsSL https://gh.io/copilot-install | VERSION="v0.0.369" PREFIX="$HOME/custom" bash
```

Install with [Homebrew](https://formulae.brew.sh/cask/copilot-cli) (macOS and Linux):

```bash
brew install copilot-cli
```

```bash
brew install copilot-cli@prerelease
```


Install with [WinGet](https://github.com/microsoft/winget-cli) (Windows):

```bash
winget install GitHub.Copilot
```

```bash
winget install GitHub.Copilot.Prerelease
```


Install with [npm](https://www.npmjs.com/package/@github/copilot) (macOS, Linux, and Windows):

```bash
npm install -g @github/copilot
```

```bash
npm install -g @github/copilot@prerelease
```


### Launching the CLI

```bash
copilot
```

On first launch, you'll be greeted with our adorable animated banner! If you'd like to see this banner again, launch `copilot` with the `--banner` flag.

If you're not currently logged in to GitHub, you'll be prompted to use the `/login` slash command. Enter this command and follow the on-screen instructions to authenticate.

#### Authenticate with a Personal Access Token (PAT)

You can also authenticate using a fine-grained PAT with the "Copilot Requests" permission enabled.

1. Visit https://github.com/settings/personal-access-tokens/new
2. Under "Permissions," click "add permissions" and select "Copilot Requests"
3. Generate your token
4. Add the token to your environment via the environment variable `GH_TOKEN` or `GITHUB_TOKEN` (in order of precedence)

### Using the CLI

Launch `copilot` in a folder that contains code you want to work with.

By default, `copilot` utilizes Claude Sonnet 4.5. Run the `/model` slash command to choose from other available models, including Claude Sonnet 4 and GPT-5.

### Experimental Mode

Experimental mode enables access to new features that are still in development. You can activate experimental mode by:

- Launching with the `--experimental` flag: `copilot --experimental`
- Using the `/experimental` slash command from within the CLI

Once activated, the setting is persisted in your config, so the `--experimental` flag is no longer needed on subsequent launches.

#### Experimental Features

- **Autopilot mode:** Autopilot is a new mode (press `Shift+Tab` to cycle through modes), which encourages the agent to continue working until a task is completed.

Each time you submit a prompt to GitHub Copilot CLI, your monthly quota of premium requests is reduced by one. For information about premium requests, see [About premium requests](https://docs.github.com/copilot/managing-copilot/monitoring-usage-and-entitlements/about-premium-requests).

For more information about how to use the GitHub Copilot CLI, see [our official documentation](https://docs.github.com/copilot/concepts/agents/about-copilot-cli).

## 🔧 Configuring LSP Servers

GitHub Copilot CLI supports Language Server Protocol (LSP) for enhanced code intelligence. This feature provides intelligent code features like go-to-definition, hover information, and diagnostics.

### Installing Language Servers

Copilot CLI does not bundle LSP servers. You need to install them separately. For example, to set up TypeScript support:

```bash
npm install -g typescript-language-server
```

For other languages, install the corresponding LSP server and configure it following the same pattern shown below.

### Configuring LSP Servers

LSP servers are configured through a dedicated LSP configuration file. You can configure LSP servers at the user level or repository level:

**User-level configuration** (applies to all projects):
Edit `~/.copilot/lsp-config.json`

**Repository-level configuration** (applies to specific project):
Create `.github/lsp.json` in your repository root

Example configuration:

```json
{
  "lspServers": {
    "typescript": {
      "command": "typescript-language-server",
      "args": ["--stdio"],
      "fileExtensions": {
        ".ts": "typescript",
        ".tsx": "typescript"
      }
    }
  }
}
```

### Viewing LSP Server Status

Check configured LSP servers using the `/lsp` command in an interactive session, or view your configuration files directly.

For more information, see the [changelog](./changelog.md).

## 📢 Feedback and Participation

We're excited to have you join us early in the Copilot CLI journey.

We're building quickly. Expect frequent updates--please keep your client up to date for the latest features and fixes!

Your insights are invaluable! Open an issue in this repo, join Discussions, and run `/feedback` from the CLI to submit a confidential feedback survey!


## Help Command Output
Global
  @ - mention files, include contents in context
  ctrl+s - run command while preserving input
  shift+tab - cycle modes (interactive → plan)
  ctrl+t - toggle model reasoning display
  ctrl+o - expand recent timeline (when no input)
  ctrl+e - expand all timeline (when no input)
  ↑ ↓ - navigate command history
  ! - execute command in your local shell (bypass Copilot)
  Esc - cancel the current operation
  ctrl+c - cancel operation / clear input / exit
  ctrl+d - shutdown
  ctrl+l - clear the screen

Editing
  ctrl+a - move to beginning of line (when typing)
  ctrl+e - move to end of line (when typing)
  ctrl+h - delete previous character
  ctrl+w - delete previous word
  ctrl+u - delete from cursor to beginning of line
  ctrl+k - delete from cursor to end of line
  meta+← → - move cursor by word
  ctrl+g - edit prompt in external editor

Agent environment
  /init - Initialize Copilot instructions for this repository, or suppress the init suggestion
  /agent - Browse and select from available agents (if any)
  /skills - Manage skills for enhanced capabilities
  /mcp - Manage MCP server configuration
  /plugin - Manage plugins and plugin marketplaces

Models and subagents
  /model - Select AI model to use
  /fleet - Enable fleet mode for parallel subagent execution
  /tasks - View and manage background tasks (subagents and shell sessions)

Code
  /ide - Connect to an IDE workspace
  /diff - Review the changes made in the current directory
  /review - Run code review agent to analyze changes
  /lsp - Manage language server configuration
  /terminal-setup - Configure terminal for multiline input support (shift+enter)

Permissions
  /allow-all - Enable all permissions (tools, paths, and URLs)
  /add-dir - Add a directory to the allowed list for file access
  /list-dirs - Display all allowed directories for file access
  /cwd - Change working directory or show current directory
  /reset-allowed-tools - Reset the list of allowed tools

Session
  /resume - Switch to a different session (optionally specify session ID)
  /rename - Rename the current session (alias for /session rename)
  /context - Show context window token usage and visualization
  /usage - Display session usage metrics and statistics
  /session - Show session info and workspace summary. Use subcommands for details.
  /compact - Summarize conversation history to reduce context window usage
  /share - Share session or research report to markdown file or GitHub gist

Help and feedback
  /help - Show help for interactive commands
  /changelog - Display changelog for CLI versions
  /feedback - Provide feedback about the CLI
  /theme - View or configure terminal theme
  /update - Update the CLI to the latest version
  /experimental - Show available experimental features, or enable/disable experimental mode
  /clear - Clear the conversation history
  /instructions - View and toggle custom instruction files
  /streamer-mode - Toggle streamer mode (hides preview model names and quota details for streaming)

Other commands
  /exit, /quit - Exit the CLI
  /login - Log in to Copilot
  /logout - Log out of Copilot
  /plan - Create an implementation plan before coding
  /research - Run deep research investigation using GitHub search and web sources
  /user - Manage GitHub user list

Copilot respects instructions from these locations:
  CLAUDE.md
  GEMINI.md
  AGENTS.md (in git root & cwd)
  .github/instructions/**/*.instructions.md (in git root & cwd)
  .github/copilot-instructions.md
  $HOME/.copilot/copilot-instructions.md
  COPILOT_CUSTOM_INSTRUCTIONS_DIRS (additional directories via env var)

To learn about what I can do
  Ask me: "What can you do?"
  Or visit: https://docs.github.com/en/copilot/how-tos/use-copilot-agents/use-copilot-cli

---

**Note**: If the above documentation doesn't answer the user's request, you can fetch additional documentation from the official GitHub Docs for more information.

Use the web_fetch tool to fetch https://docs.github.com/api/article/body?pathname=/en/copilot/how-tos/use-copilot-agents/use-copilot-cli

You can also follow hyperlinks to related docs pages using the same API pattern.
````

</details>


---

<sub>⏱️ 126m 4s</sub>

### 💬 Copilot

你可以用 `/share` 命令保存对话记录：

- **`/share`** — 将对话导出为 Markdown 文件或 GitHub Gist

直接在输入框输入 `/share`，按提示选择保存方式即可。

---

<sub>Generated by [GitHub Copilot CLI](https://github.com/features/copilot/cli)</sub>
