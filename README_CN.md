# DCL-SLAM ROS2

基于 **FAST-LIO2** 前端和 **DCL-SLAM** 分布式后端的多机器人协同激光 SLAM 系统，适配 ROS2 Humble。

## 架构

```
fastlio_mapping 进程（每台机器人运行一个）
├── FAST-LIO2 前端        ← laserMapping 节点：IMU 预积分 + 点云配准 + 里程计输出
└── DCL-SLAM 后端         ← dcl_slam_node 节点：闭环检测 + 分布式位姿图优化
```

> **注意**：前端和后端运行在同一个进程内。启动 `fastlio_mapping` 即同时启动前端和后端。

不同机器人通过 ROS2 命名空间（`/a`、`/b`、`/c`）区分，通过 DDS 自动发现和通信。

---

## 依赖

| 依赖 | 安装方式 |
|------|---------|
| ROS2 Humble | 基础环境 |
| GTSAM | `sudo apt install ros-humble-gtsam` |
| PCL / OpenCV | ROS2 自带 |
| libnabo | 源码已包含（`src/libnabo`） |
| distributed_mapper | 源码已包含（`src/distributed_mapper`） |
| livox_ros_driver2（可选） | Livox 雷达需要，单独编译 |

---

## 编译

```bash
cd ~/DCL-SLAM
source /opt/ros/humble/setup.bash

# 使用 Livox 雷达时，先 source livox 驱动
source ~/ws_livox/install/setup.bash

# 编译（不要用 sudo）
colcon build --symlink-install

source install/setup.bash
```

> ⚠️ 编译时如果环境中有 `livox_ros_driver2`，会自动启用 Livox CustomMsg 支持；否则仅支持 Velodyne/Ouster 的 PointCloud2。

---

## 快速开始：Livox MID-360

### 方式一：使用 Launch 文件（推荐）

```bash
# 终端 1：启动雷达驱动
source ~/ws_livox/install/setup.bash
ros2 launch livox_ros_driver2 msg_MID360_launch.py

# 终端 2：启动 DCL-SLAM（前端 + 后端 + RViz）
source ~/ws_livox/install/setup.bash
source ~/DCL-SLAM/install/setup.bash
ros2 launch dcl_slam dcl_fast_lio_mid360.launch.py
```

Launch 文件会自动完成：
- 以命名空间 `/a` 启动 `fastlio_mapping`（含前端和后端）
- 话题重映射（`/a/livox/*` → `/livox/*`）
- 加载参数文件
- 启动 RViz2（已配置好 Fixed Frame、话题、Decay Time）

不需要 RViz 时：`ros2 launch dcl_slam dcl_fast_lio_mid360.launch.py rviz:=false`

### 方式二：手动分别启动

```bash
# 终端 1：雷达驱动
source ~/ws_livox/install/setup.bash
ros2 launch livox_ros_driver2 msg_MID360_launch.py

# 终端 2：DCL-SLAM
source ~/ws_livox/install/setup.bash
source ~/DCL-SLAM/install/setup.bash
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
     --params-file src/dcl_slam/config/dcl_fast_lio_mid360.yaml \
     -r __ns:=/a \
     -r /a/livox/imu:=/livox/imu \
     -r /a/livox/lidar:=/livox/lidar

# 终端 3：RViz（使用预配置文件）
source ~/DCL-SLAM/install/setup.bash
rviz2 -d install/dcl_slam/share/dcl_slam/config/dcl_fast_lio_mid360.rviz
```

### 启动成功标志

```
Multi thread started
[INFO] Robot Name: /a, ID: 0
[INFO] distributed mapping class initialization finish
p_pre->lidar_type 1 4
~~~~/.../dcl_fast_lio/ file opened
IMU Initial Done        ← IMU 初始化完成，开始建图
```

### 验证

```bash
ros2 topic hz /livox/lidar         # ~10Hz
ros2 topic hz /livox/imu           # ~200Hz
ros2 topic hz /a/cloud_registered  # ~5-8Hz（建图输出）
ros2 topic hz /a/Odometry          # ~30Hz（里程计）
```

---

## 快速开始：Velodyne 数据包回放

```bash
# 终端 1：启动 DCL-SLAM
source ~/DCL-SLAM/install/setup.bash
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml \
  -r __ns:=/a

# 终端 2：播放数据包
ros2 bag play <数据包路径> --remap /velodyne_points:=/a/points_raw /imu/data:=/a/imu_correct

# 终端 3：RViz（Fixed Frame 设为 a/camera_init）
rviz2
```

---

## 多机器人协同

### 同一台电脑模拟（数据包回放）

```bash
# 分别在不同终端启动各机器人
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml -r __ns:=/a

ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml -r __ns:=/b

ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml -r __ns:=/c

# 闭环可视化
ros2 run dcl_slam dcl_slam_loopVisualizationNode --ros-args -p number_of_robots:=3

# 分别播放各机器人数据包
ros2 bag play robot_a.bag --remap /velodyne_points:=/a/points_raw /imu/data:=/a/imu_correct
ros2 bag play robot_b.bag --remap /velodyne_points:=/b/points_raw /imu/data:=/b/imu_correct
ros2 bag play robot_c.bag --remap /velodyne_points:=/c/points_raw /imu/data:=/c/imu_correct
```

### 多台物理机器人

每台机器人运行各自的 `fastlio_mapping`，用不同命名空间区分。ROS2 DDS 会自动发现同一局域网的节点。

```
机器人A (192.168.1.10)          机器人B (192.168.1.11)          监控站 (192.168.1.20)
┌──────────────────┐           ┌──────────────────┐           ┌──────────────┐
│ fastlio_mapping  │           │ fastlio_mapping  │           │ rviz2        │
│ namespace: /a    │◄─ DDS ──►│ namespace: /b    │◄─ DDS ──►│              │
│ 雷达 + IMU       │           │ 雷达 + IMU       │           │              │
└──────────────────┘           └──────────────────┘           └──────────────┘
```

要求：同一局域网、相同 `ROS_DOMAIN_ID`（默认 0）、NTP 时钟同步。

---

## 参数配置

配置文件位于 `src/dcl_slam/config/`：

| 文件 | 适用场景 |
|------|---------|
| `dcl_fast_lio_mid360.yaml` | Livox MID-360 |
| `dcl_fast_lio_velodyne.yaml` | Velodyne VLP-64 |
| `dcl_fast_lio_velodyne16.yaml` | Velodyne VLP-16 |

### 关键参数

```yaml
/**:
  ros__parameters:
    # --- FAST-LIO 前端 ---
    common:
      lid_topic: "/livox/lidar"    # 雷达话题（绝对路径避免命名空间前缀）
      imu_topic: "/livox/imu"      # IMU 话题
    preprocess:
      lidar_type: 1                # 1=Livox, 2=Velodyne, 3=Ouster
      scan_line: 4                 # 雷达线数
      blind: 0.5                   # 最小检测距离（米）
    mapping:
      det_range: 100.0             # 最大检测距离（米）
      extrinsic_T: [x, y, z]      # IMU-雷达外参平移
      extrinsic_R: [...]           # 旋转矩阵 3x3（行优先）
    cube_side_length: 1000.0       # 地图立方体边长（必须 > 3 × det_range）

    # --- DCL-SLAM 后端 ---
    descriptor_type: "LidarIris"   # 闭环描述子: ScanContext / LidarIris / M2DP
    inter_robot_loop_closure_enable: true   # 多机闭环
    intra_robot_loop_closure_enable: false  # 单机闭环
    global_optmization_enable: true         # 分布式优化
    keyframe_distance_threshold: 1.0        # 关键帧距离阈值（米）
```

---

## 话题

### 输入

| 话题 | 类型 | 说明 |
|------|------|------|
| `/livox/lidar` 或 `/<ns>/points_raw` | CustomMsg / PointCloud2 | 点云 |
| `/livox/imu` 或 `/<ns>/imu_correct` | sensor_msgs/Imu | IMU |

### 输出

| 话题 | 类型 | 说明 |
|------|------|------|
| `/<ns>/cloud_registered` | PointCloud2 | 配准点云（世界坐标系） |
| `/<ns>/Odometry` | Odometry | 里程计 |
| `/<ns>/path` | Path | 轨迹 |
| `/<ns>/Laser_map` | PointCloud2 | 全局地图 |

`<ns>` 为机器人命名空间（`a`、`b`、`c`）。

---

## RViz 可视化说明

使用 Launch 文件启动时会自动加载预配置的 RViz。手动配置时注意：

| 设置项 | 值 | 说明 |
|--------|-----|------|
| Fixed Frame | `a/camera_init` | 必须带命名空间前缀 |
| PointCloud2 Topic | `/a/cloud_registered` | 配准后的点云 |
| **Decay Time** | **30** | **关键！使点云累积显示，看起来像在建图** |

> Decay Time = 0 时只显示当前帧点云，看起来像原始数据跟着旋转。设为 30 秒后点云会持续累积。

---

## 支持的雷达

| 雷达 | 型号 | 说明 |
|------|------|------|
| Livox | MID-360 等 | 需要 `livox_ros_driver2`，编译时自动检测 |
| Velodyne | VLP-16 / VLP-32 / VLP-64 | 直接支持 |
| Ouster | OS0 / OS1 / OS2-64 | 直接支持 |

---

## 常见问题

**Q: 编译报错 "No module named 'ament_package'"**
不要用 `sudo colcon build`，删除 `build/` `install/` 后普通用户重新编译。

**Q: 运行时报错 "Invalid robot prefix"**
必须设置单字母命名空间：`-r __ns:=/a`

**Q: "No Effective Points!" 刷屏**
检查 `cube_side_length` 是否 > 3 × `det_range`。如果 `det_range=100` 则 `cube_side_length` 至少 `400`（推荐 `1000`）。

**Q: "Could not create logging file"**
创建日志目录：`mkdir -p ~/log`

**Q: RViz 看不到点云 / 点云跟着旋转**
1. Fixed Frame 设为 `a/camera_init`（不是 `camera_init`）
2. PointCloud2 的 **Decay Time** 设为 30
3. 确认 `ros2 topic hz /a/cloud_registered` 有输出

**Q: Ctrl+C 退不出？**
`ps aux | grep fastlio` 找到 PID，`kill -9 <PID>`

**Q: 如何禁用分布式功能只跑单机？**
配置文件中设置 `inter_robot_loop_closure_enable: false` 和 `global_optmization_enable: false`
