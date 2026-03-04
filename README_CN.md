# DCL-SLAM ROS2 使用说明

本项目是 **DCL-SLAM (Distributed Collaborative SLAM)** 的 ROS2 Humble 移植版本。它提供了一个分布式多机器人协同激光SLAM系统，包含：

- **dcl_slam** — 分布式后端（闭环检测 + 位姿图优化）
- **dcl_fast_lio** — FAST-LIO2 前端（激光惯性里程计）

---

## 1. 系统要求

| 依赖 | 说明 |
|------|------|
| **ROS2 Humble** | 基础运行环境 |
| **GTSAM** | `sudo apt install ros-humble-gtsam` |
| **PCL** | ROS2 自带 |
| **OpenCV** | ROS2 自带 |
| **libnabo** | 源码已包含在 `src/libnabo` |
| **distributed_mapper** | 源码已包含在 `src/distributed_mapper` |
| **livox_ros_driver2**（可选） | Livox 雷达（如 MID-360）需要，单独编译在其他工作空间即可 |

---

## 2. 编译

```bash
cd ~/DCL-SLAM

# 确保已 source ROS2 环境
source /opt/ros/humble/setup.bash

# （可选）如果使用 Livox 雷达，先 source livox 驱动
source ~/ws_livox/install/setup.bash

# 编译全部（不要用 sudo！）
colcon build --symlink-install

# source 编译后的环境
source install/setup.bash
```

编译完成后会生成 4 个包：`distributed_mapper`、`libnabo`、`dcl_slam`、`dcl_fast_lio`

> ⚠️ 如果编译时环境中有 `livox_ros_driver2`，会自动启用 Livox 支持；否则仅支持 Velodyne/Ouster。

---

## 3. 快速测试运行（MID-360 实物雷达）

以下是使用 Livox MID-360 雷达进行单机测试的完整步骤：

### 步骤 1：启动 Livox 雷达驱动

```bash
# 终端 1
source ~/ws_livox/install/setup.bash
ros2 launch livox_ros_driver2 msg_MID360_launch.py
```

> 确保 MID-360 的 IP 配置正确（检查 `~/ws_livox/src/livox_ros_driver2/config/MID360_config.json`），电脑有线网口和雷达在同一网段。

### 步骤 2：启动 DCL-SLAM + FAST-LIO

```bash
# 终端 2
source ~/ws_livox/install/setup.bash
source ~/DCL-SLAM/install/setup.bash
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_mid360.yaml \
  -r __ns:=/a
```

看到以下输出说明启动成功：
```
Multi thread started
[INFO] Robot Name: /a, ID: 0
[INFO] distributed mapping class initialization finish
p_pre->lidar_type 1 4
~~~~/.../dcl_fast_lio/ file opened
IMU Initial Done        ← 等待几秒，IMU 初始化完成后开始建图
```

### 步骤 3：启动 RViz2 可视化

```bash
# 终端 3
source ~/DCL-SLAM/install/setup.bash
rviz2
```

在 RViz2 中配置：
1. 左侧 **Global Options** → **Fixed Frame** → 输入 **`a/camera_init`**（注意带 `a/` 前缀）
2. 点左下角 **Add** → **By topic** → 展开并添加：
   - `/a/cloud_registered` (PointCloud2) — 实时配准点云
   - `/a/path` (Path) — 运动轨迹
   - `/a/Odometry` (Odometry) — 里程计
3. 可选：给点云设置颜色（选中 PointCloud2 → Color Transformer → `FlatColor`）
4. 可选：**File → Save Config As** 保存配置，下次直接 `rviz2 -d 你的配置.rviz` 加载

### 步骤 4：验证数据

```bash
# 终端 4：确认各话题有数据
ros2 topic hz /livox/lidar         # 雷达数据 ~10Hz
ros2 topic hz /livox/imu           # IMU 数据 ~200Hz
ros2 topic hz /a/cloud_registered  # 建图输出 ~5-8Hz
ros2 topic hz /a/Odometry          # 里程计输出
```

拿着雷达缓慢移动，RViz2 中就能看到实时三维点云地图。

---

## 4. 快速测试运行（Velodyne + 数据包回放）

### 步骤 1：启动 FAST-LIO

```bash
# 终端 1
source ~/DCL-SLAM/install/setup.bash
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml \
  -r __ns:=/a
```

### 步骤 2：播放 ROS2 数据包

```bash
# 终端 2
ros2 bag play <你的数据包路径> --remap /velodyne_points:=/a/points_raw /imu/data:=/a/imu_correct
```

### 步骤 3：启动 RViz2

```bash
# 终端 3
rviz2
# Fixed Frame 设为 a/camera_init，添加 /a/cloud_registered 话题
```

---

## 5. 多机器人协同运行

### 方式一：同一台电脑模拟多机器人（数据包回放）

```bash
# 终端 1：启动机器人 a
source ~/DCL-SLAM/install/setup.bash
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml \
  -r __ns:=/a

# 终端 2：启动机器人 b
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml \
  -r __ns:=/b

# 终端 3：启动机器人 c
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_velodyne.yaml \
  -r __ns:=/c

# 终端 4：闭环可视化节点
ros2 run dcl_slam dcl_slam_loopVisualizationNode --ros-args -p number_of_robots:=3

# 终端 5：RViz2 可视化（Fixed Frame 设为 a/camera_init）
rviz2

# 终端 6/7/8：分别播放每台机器人的数据包
ros2 bag play robot_a.bag --remap /velodyne_points:=/a/points_raw /imu/data:=/a/imu_correct
ros2 bag play robot_b.bag --remap /velodyne_points:=/b/points_raw /imu/data:=/b/imu_correct
ros2 bag play robot_c.bag --remap /velodyne_points:=/c/points_raw /imu/data:=/c/imu_correct
```

### 方式二：多台物理机器人（实际部署）

每台机器人上编译并运行各自的 `fastlio_mapping`，用不同命名空间（`/a`、`/b`、`/c`）区分。
**ROS2 的 DDS 通信会自动发现同一局域网内的节点**，无需配置中心服务器。

```
机器人A (192.168.1.10)          机器人B (192.168.1.11)          监控站 (192.168.1.20)
┌──────────────────┐           ┌──────────────────┐           ┌──────────────┐
│ fastlio_mapping  │           │ fastlio_mapping  │           │ rviz2        │
│ namespace: /a    │◄─ DDS ──►│ namespace: /b    │◄─ DDS ──►│ 只看不算     │
│ 雷达 + IMU       │           │ 雷达 + IMU       │           │ 无需编译本项目│
└──────────────────┘           └──────────────────┘           └──────────────┘
```

多机通信要求：
- 所有机器人在同一局域网
- `export ROS_DOMAIN_ID=0`（所有机器人相同，默认就是 0）
- 多台物理机需要 NTP/chrony 时钟同步

### 方式三：使用 Launch 文件（需要 dcl_lio_sam 前端）

```bash
ros2 launch dcl_slam run.launch.py set_lio_type:=1
```

---

## 6. 参数配置

配置文件位于 `src/dcl_slam/config/` 目录：

| 文件 | 适用场景 |
|------|---------|
| `dcl_fast_lio_mid360.yaml` | Livox MID-360 |
| `dcl_fast_lio_velodyne.yaml` | Velodyne VLP-64 |
| `dcl_fast_lio_velodyne16.yaml` | Velodyne VLP-16 |

### 关键参数说明

```yaml
/**:
  ros__parameters:
    # ===== 传感器配置 =====
    common:
      lid_topic: "livox/lidar"     # 激光雷达话题名
      imu_topic: "livox/imu"       # IMU 话题名

    preprocess:
      lidar_type: 1                # 1=Livox, 2=Velodyne, 3=Ouster
      scan_line: 4                 # 激光雷达线数（MID-360=4, VLP-16=16, VLP-64=64）
      blind: 0.5                   # 最小检测距离（米）

    # ===== IMU-雷达外参 =====
    mapping:
      extrinsic_T: [x, y, z]      # 平移
      extrinsic_R: [r00,...,r22]   # 旋转矩阵（行优先 3x3）
      acc_cov: 0.1                 # 加速度计噪声协方差
      gyr_cov: 0.1                 # 陀螺仪噪声协方差

    # ===== DCL-SLAM 后端配置（顶层参数）=====
    descriptor_type: "LidarIris"           # 闭环检测描述子: ScanContext/LidarIris/M2DP
    inter_robot_loop_closure_enable: true  # 开启多机闭环
    intra_robot_loop_closure_enable: false # 开启单机闭环
    global_optmization_enable: true        # 开启分布式优化
    keyframe_distance_threshold: 1.0       # 关键帧距离阈值（米）
```

> **注意**：ROS2 参数文件必须包含 `/**:` → `ros__parameters:` 结构。DCL-SLAM 后端参数（如 `descriptor_type`）直接放在 `ros__parameters:` 下（不嵌套），FAST-LIO 前端参数（如 `mapping`、`preprocess`）使用嵌套结构。

---

## 7. 话题 (Topics)

### 输入话题

| 话题 | 类型 | 说明 |
|------|------|------|
| `/livox/lidar` 或 `/<ns>/points_raw` | `CustomMsg` 或 `PointCloud2` | 激光雷达点云（Livox 用 CustomMsg） |
| `/livox/imu` 或 `/<ns>/imu_correct` | `sensor_msgs/msg/Imu` | IMU 数据 |

> 如果雷达话题没有命名空间前缀，需要在启动时 remap：
> `-r /a/livox/lidar:=/livox/lidar -r /a/livox/imu:=/livox/imu`

### 输出话题

| 话题 | 类型 | 说明 |
|------|------|------|
| `/<ns>/cloud_registered` | `PointCloud2` | 配准后的点云（世界坐标系） |
| `/<ns>/Odometry` | `Odometry` | 里程计输出 |
| `/<ns>/path` | `Path` | 轨迹路径 |
| `/<ns>/cloud_registered_body` | `PointCloud2` | 体坐标系点云 |
| `/<ns>/Laser_map` | `PointCloud2` | 全局激光地图 |

其中 `<ns>` 是机器人命名空间，如 `a`、`b`、`c`。

---

## 8. 架构说明

```
┌─────────────────────────────────────────────────┐
│                 dcl_fast_lio                     │
│  (fastlio_mapping 可执行文件)                     │
│                                                  │
│  ┌──────────────┐  ┌─────────────────────────┐  │
│  │ FAST-LIO2    │  │ distributedMapping       │  │
│  │ 激光惯性里程计│→│ (dcl_slam 后端, 内置)     │  │
│  │              │  │ - 闭环检测               │  │
│  │ IMU预积分    │  │ - 分布式位姿图优化       │  │
│  │ 点云配准     │  │ - 跨机器人通信           │  │
│  └──────────────┘  └─────────────────────────┘  │
└─────────────────────────────────────────────────┘
        ↓                      ↓
   里程计输出            通过 ROS2 话题
                      与其他机器人通信
```

每台机器人运行一个 `fastlio_mapping` 进程，不同机器人通过**不同的命名空间**（`/a`、`/b`、`/c`）区分。机器人之间通过 ROS2 话题自动交换闭环检测信息和位姿估计。

---

## 9. 常见问题

### Q: 编译报错 "ModuleNotFoundError: No module named 'ament_package'"
**A**: 不要用 `sudo colcon build`，删除 `build/` 和 `install/` 后用普通用户重新编译。

### Q: 运行时报错 "Invalid robot prefix"
**A**: 必须设置命名空间为单个字母，如 `-r __ns:=/a`。

### Q: 数据包话题名和配置不一致怎么办？
**A**: 使用 `-r` 在启动时重映射话题：
```bash
# 雷达驱动话题没有命名空间前缀时
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file src/dcl_slam/config/dcl_fast_lio_mid360.yaml \
  -r __ns:=/a \
  -r /a/livox/lidar:=/livox/lidar \
  -r /a/livox/imu:=/livox/imu
```

### Q: RViz2 看不到点云？
**A**: 检查以下几点：
1. **Fixed Frame** 必须设为 `a/camera_init`（带命名空间前缀，不是 `camera_init`）
2. 确认 `ros2 topic hz /a/cloud_registered` 有频率输出（约 5-8Hz）
3. 通过 **Add → By topic** 添加 `/a/cloud_registered`

### Q: Ctrl+C 退不出程序？
**A**: 在另一个终端执行 `ps aux | grep fastlio`，然后 `kill -9 <PID>` 强制退出。

### Q: 如何只运行单机 FAST-LIO（不要分布式功能）？
**A**: 目前 dcl_fast_lio 内置了 dcl_slam 后端，无法完全关闭。但可以在配置文件中设置 `inter_robot_loop_closure_enable: false` 和 `global_optmization_enable: false` 来禁用多机协作功能。

### Q: 支持哪些激光雷达？
**A**: 
- **Velodyne**: VLP-16 / VLP-32 / VLP-64（直接支持）
- **Ouster**: OS0 / OS1 / OS2-64（直接支持）
- **Livox**: MID-360 等（需要安装 `livox_ros_driver2`，编译时自动检测启用）

### Q: "Could not create logging file" 报错？
**A**: 创建日志目录即可：`mkdir -p ~/DCL-SLAM/src/dcl_fast_lio/Log`
