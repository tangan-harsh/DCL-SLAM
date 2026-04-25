# Total Launch - 无人机统一启动系统

## 📦 包描述

`total_launch` 是一个 ROS2 功能包，用于统一启动无人机完整系统，包括：

1. **Livox LiDAR 驱动** - Livox MID360 雷达驱动（外部工作空间）
2. **DCL-SLAM** - LiDAR 里程计和建图系统
3. **无人机控制系统**（通过 `demo3.launch.py`）
   - UART 到 STM32 串口通信
   - 位置 PID 控制器
   - 航点导航发布器

## 🚀 快速开始

### 基础用法

```bash
cd /home/intelcup/ws_drone
source install/setup.bash

# 启动完整系统（Livox + DCL-SLAM + 无人机控制）
ros2 launch total_launch total_system.launch.py
```

### 高级用法

```bash
# 不启动 Livox 驱动（假设已单独启动）
ros2 launch total_launch total_system.launch.py enable_livox_driver:=false

# 不启动 RViz（节省资源）
ros2 launch total_launch total_system.launch.py use_rviz:=false

# 只启动 Livox + DCL-SLAM（测试建图）
ros2 launch total_launch total_system.launch.py enable_drone_system:=false

# 只启动 Livox（测试雷达）
ros2 launch total_launch total_system.launch.py \
  enable_drone_system:=false \
  use_rviz:=false

# 使用自定义 Livox launch 文件
ros2 launch total_launch total_system.launch.py \
  livox_launch_path:=/path/to/custom/msg_MID360_launch.py

# 组合参数
ros2 launch total_launch total_system.launch.py \
  enable_livox_driver:=true \
  enable_drone_system:=false \
  use_rviz:=false
```

## 📋 启动参数

| 参数名 | 默认值 | 说明 |
|--------|--------|------|
| `use_rviz` | `true` | 是否启动 RViz 可视化 |
| `enable_drone_system` | `true` | 是否启动无人机控制系统（demo3） |
| `enable_livox_driver` | `true` | 是否启动 Livox 雷达驱动 |
| `livox_launch_path` | `/home/intelcup/ws_livox/src/livox_ros_driver2/launch_ROS2/msg_MID360_launch.py` | Livox 驱动 launch 文件绝对路径 |

## ⏱️ 启动时序

```
T+0.0s: Livox 驱动启动
  └─ livox_ros_driver2_node（雷达驱动）

T+0.5s: DCL-SLAM 启动
  ├─ fastlio_mapping 节点
  └─ RViz2（可选）

T+2.0s: 无人机控制系统启动（demo3.launch.py）
  ├─ uart_to_stm32_node（串口通信）
  ├─ position_pid_controller（PID 控制器）
  └─ route_target_publisher（航点发布器，加载 test_19 预设）
```

**延迟原因：**
- **Livox 驱动（T+0.0s）**：需要最先启动，发布点云数据
- **DCL-SLAM（T+0.5s）**：需要 Livox 的点云数据才能初始化
- **无人机系统（T+2.0s）**：需要 DCL-SLAM 的里程计数据

## 📂 目录结构

```
total_launch/
├── launch/
│   └── total_system.launch.py  # 统一启动文件
├── total_launch/
│   └── __init__.py
├── resource/
├── test/
├── package.xml
├── setup.py
├── setup.cfg
└── README.md
```

## 🔧 依赖包

- `livox_ros_driver2` - Livox 雷达驱动（外部工作空间）
- `dcl_slam` - DCL-SLAM 后端
- `dcl_fast_lio` - FAST-LIO2 前端
- `uart_to_stm32` - 串口通信桥接
- `pid_control_pkg` - PID 控制器
- `activity_control_pkg` - 航点导航
- `my_launch` - 包含 demo3.launch.py

## 📝 使用示例

### 示例 1：完整系统测试

```bash
# 1. 打开终端 1，启动完整系统
cd /home/intelcup/ws_drone
source install/setup.bash
ros2 launch total_launch total_system.launch.py

# 2. 打开终端 2，查看节点
ros2 node list

# 预期输出：
# /livox_lidar_publisher
# /a/laserMapping
# /rviz2
# /uart_to_stm32
# /position_pid_controller
# /route_target_publisher
```

### 示例 2：只测试建图（Livox + DCL-SLAM）

```bash
# 只启动 Livox + DCL-SLAM，不启动无人机系统
ros2 launch total_launch total_system.launch.py enable_drone_system:=false

# 查看点云数据
ros2 topic hz /livox/lidar

# 查看里程计数据
ros2 topic hz /a/Odometry
```

### 示例 3：只测试雷达（Livox）

```bash
# 只启动 Livox 雷达
ros2 launch total_launch total_system.launch.py \
  enable_drone_system:=false \
  use_rviz:=false

# 查看点云数据
ros2 topic echo /livox/lidar --no-arr
```

### 示例 4：无 RViz 模式（嵌入式/远程运行）

```bash
# 不启动 RViz，节省资源
ros2 launch total_launch total_system.launch.py use_rviz:=false

# 在另一台机器上查看 RViz
rviz2 -d $(ros2 pkg prefix dcl_slam)/share/dcl_slam/config/dcl_fast_lio_mid360.rviz
```

## 🆚 与其他启动方式的对比

### 方式 1：使用 total_launch（推荐）
```bash
ros2 launch total_launch total_system.launch.py
```
✅ 一个命令启动所有  
✅ 自动处理启动延迟  
✅ 参数配置集中管理  
✅ 支持 Livox 驱动  

### 方式 2：手动分步启动
```bash
# 终端 1
ros2 launch livox_ros_driver2 msg_MID360_launch.py

# 终端 2
cd ws_drone/
source install/setup.bash
ros2 launch dcl_slam dcl_fast_lio_mid360.launch.py
ros2 launch my_launch demo3.launch.py
```
⚠️ 需要多个终端  
⚠️ 需要手动控制启动时间  
⚠️ 参数分散在多个文件  

### 方式 3：仅使用 demo3
```bash
ros2 launch my_launch demo3.launch.py
```
ℹ️ 仅启动无人机系统  
ℹ️ 适用于 DCL-SLAM 和 Livox 已运行的场景  

## 🛠️ 故障排查

### 问题 1：Livox 驱动启动失败

**可能原因：**
- Livox 工作空间未编译
- 路径配置错误
- 雷达未连接或网络配置错误

**解决方案：**
```bash
# 1. 检查 Livox 工作空间是否已编译
cd /home/intelcup/ws_livox
colcon build
source install/setup.bash

# 2. 检查 launch 文件路径
ls /home/intelcup/ws_livox/src/livox_ros_driver2/launch_ROS2/msg_MID360_launch.py

# 3. 单独测试 Livox 驱动
ros2 launch livox_ros_driver2 msg_MID360_launch.py
```

### 问题 2：DCL-SLAM 无法获取点云数据

**可能原因：**
- Livox 驱动未启动
- Livox 话题名称不匹配

**解决方案：**
```bash
# 检查 Livox 话题
ros2 topic list | grep livox

# 检查点云数据
ros2 topic hz /livox/lidar
```

### 问题 3：无人机系统启动后无法获取里程计数据

**解决方案：**
```bash
# 增加 DCL-SLAM 启动延迟
# 修改 total_system.launch.py 中的延迟时间
TimerAction(period=1.0, actions=[dcl_slam_launch]),  # 从 0.5s 改为 1.0s
```

### 问题 4：RViz 启动失败

**解决方案：**
```bash
# 检查 RViz 配置文件路径
ls $(ros2 pkg prefix dcl_slam)/share/dcl_slam/config/dcl_fast_lio_mid360.rviz
```

### 问题 5：航点导航未启动

**解决方案：**
```bash
# 检查 enable_drone_system 参数
ros2 launch total_launch total_system.launch.py --show-args

# 显式启用无人机系统
ros2 launch total_launch total_system.launch.py enable_drone_system:=true
```

## 🔗 前置要求

### 1. Livox 工作空间

确保 Livox 工作空间已正确编译和配置：

```bash
# 编译 Livox 工作空间
cd /home/intelcup/ws_livox
colcon build
source install/setup.bash

# 测试 Livox 驱动
ros2 launch livox_ros_driver2 msg_MID360_launch.py
```

### 2. DCL-SLAM 工作空间

确保 DCL-SLAM 工作空间已编译：

```bash
cd /home/intelcup/ws_drone
colcon build
source install/setup.bash
```

### 3. 网络配置

配置 Livox MID360 的网络：

```bash
# 设置本机 IP（根据实际网卡调整）
sudo ip addr add 192.168.1.5/24 dev eth0

# 测试雷达连接
ping 192.168.1.100  # Livox MID360 默认 IP
```

## 📞 维护信息

- **维护者：** ubuntu
- **邮箱：** ubuntu@todo.todo
- **许可证：** Apache-2.0
- **版本：** 1.0.0

## 🔗 相关文档

- [Livox ROS Driver 2 文档](https://github.com/Livox-SDK/livox_ros_driver2)
- [DCL-SLAM 文档](../../dcl_slam/README.md)
- [Drone_ROS2 文档](../Drone_ROS2/README.md)
- [ROS2 Launch 文档](https://docs.ros.org/en/humble/Tutorials/Intermediate/Launch/Launch-Main.html)
