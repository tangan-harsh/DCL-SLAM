# DCL-SLAM MID-360 部署指南

在另一台机器上从零部署 DCL-SLAM（FAST-LIO2 + 分布式后端），仅适用于 **Livox MID-360** 雷达。

---

## 1. 环境要求

| 项目 | 要求 |
|------|------|
| 系统 | Ubuntu 22.04 |
| ROS | ROS2 Humble Desktop |
| CPU | ≥ 4 核（影响 OpenMP 并行度） |
| 内存 | ≥ 8 GB |
| 网络 | 与 MID-360 同一网段（默认 192.168.1.x） |

---

## 2. 安装 ROS2 Humble

如果已安装可跳过。

```bash
# 设置 locale
sudo apt update && sudo apt install -y locales
sudo locale-gen en_US en_US.UTF-8
sudo update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
export LANG=en_US.UTF-8

# 添加 ROS2 源
sudo apt install -y software-properties-common
sudo add-apt-repository universe
sudo apt update && sudo apt install -y curl
sudo curl -sSL https://raw.githubusercontent.com/ros/rosdistro/master/ros.key -o /usr/share/keyrings/ros-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/ros-archive-keyring.gpg] http://packages.ros.org/ros2/ubuntu $(. /etc/os-release && echo $UBUNTU_CODENAME) main" | sudo tee /etc/apt/sources.list.d/ros2.list > /dev/null

# 安装
sudo apt update
sudo apt install -y ros-humble-desktop

# 写入 bashrc
echo "source /opt/ros/humble/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

---

## 3. 安装系统依赖

```bash
# GTSAM（因子图优化库）
sudo apt install -y ros-humble-gtsam

# PCL、OpenCV、Eigen（ROS2 Desktop 通常已包含）
sudo apt install -y libpcl-dev libopencv-dev libeigen3-dev

# Google Logging
sudo apt install -y libgoogle-glog-dev

# 编译工具
sudo apt install -y cmake build-essential python3-colcon-common-extensions
```

---

## 4. 编译 Livox ROS Driver 2

MID-360 必须使用 `livox_ros_driver2`。

```bash
mkdir -p ~/ws_livox/src && cd ~/ws_livox/src

git clone https://github.com/Livox-SDK/Livox-SDK2.git
cd Livox-SDK2
mkdir build && cd build
cmake .. && make -j$(nproc)
sudo make install
cd ~/ws_livox/src

git clone https://github.com/Livox-SDK/livox_ros_driver2.git

cd ~/ws_livox
source /opt/ros/humble/setup.bash
colcon build --symlink-install

echo "source ~/ws_livox/install/setup.bash" >> ~/.bashrc
source ~/.bashrc
```

### 配置 MID-360 连接

编辑驱动配置文件中的 IP，确保：
- 本机 IP 设为 `192.168.1.5`（或其他同网段地址）
- MID-360 IP 默认为 `192.168.1.1xx`

```bash
# 设置本机 IP（以 eth0 为例，根据实际网卡名替换）
sudo ip addr add 192.168.1.5/24 dev eth0
```

验证驱动：
```bash
ros2 launch livox_ros_driver2 msg_MID360_launch.py
# 另一个终端检查
ros2 topic hz /livox/lidar   # 应有 ~10Hz
ros2 topic hz /livox/imu     # 应有 ~200Hz
```

---

## 安装libnabo

cd ~
git clone https://github.com/norlab-ulaval/libnabo.git
cd libnabo
# 1. 创建并进入编译目录
mkdir build && cd build

# 2. 配置项目 (生成 Makefile)
cmake ..

# 3. 编译 (使用全部 CPU 核心加快速度)
make -j$(nproc)

# 4. 运行测试 (可选，确保编译没问题)
make test

# 5. 安装到系统路径
sudo make install

## 5. 编译 DCL-SLAM

```bash
cd ~
git clone <DCL-SLAM仓库地址> DCL-SLAM
cd ~/DCL-SLAM

# 创建日志目录
mkdir -p ~/log

# 编译（必须先 source livox 驱动，否则不会启用 Livox CustomMsg 支持）
source /opt/ros/humble/setup.bash
source ~/ws_livox/install/setup.bash
colcon build --symlink-install

source install/setup.bash
```

> ⚠️ **不要用 `sudo colcon build`**，否则会报 `No module named 'ament_package'`。

### 编译成功标志

终端输出中应包含：
```
-- Found livox_ros_driver2, enabling Livox support
```

如果看到 `livox_ros_driver2 not found`，说明编译前没有 source livox 驱动，需要清理后重新编译：
```bash
rm -rf build/ install/ log/
source ~/ws_livox/install/setup.bash
colcon build --symlink-install
```

---

## 6. 单机器人启动

### 一键启动（推荐）

```bash
# 终端 1：启动雷达
ros2 launch livox_ros_driver2 msg_MID360_launch.py

# 终端 2：启动 DCL-SLAM + RViz
source ~/ws_livox/install/setup.bash
source ~/DCL-SLAM/install/setup.bash
ros2 launch dcl_slam dcl_fast_lio_mid360.launch.py
```

### 启动成功标志

```
[INFO] Robot Name: /a, ID: 0
[INFO] distributed mapping class initialization finish
p_pre->lidar_type 1 4
IMU Initial Done        ← 看到这行说明建图已开始
```

### 验证

```bash
ros2 topic hz /a/cloud_registered  # ~5-8Hz（配准点云）
ros2 topic hz /a/Odometry          # ~30Hz（里程计）
```

---

## 7. 双机器人协同部署

假设两台机器人：**机器人A**（192.168.1.10）和 **机器人B**（192.168.1.11），通过交换机或路由器组成局域网。

### 7.1 前提条件

| 条件 | 说明 |
|------|------|
| 同一局域网 | 两台机器能互相 ping 通 |
| 相同 `ROS_DOMAIN_ID` | 默认都是 0，不需要改；如果改了需保持一致 |
| NTP 时钟同步 | `sudo apt install -y chrony`，确保两台机器时间差 < 100ms |
| 各自有 MID-360 | 每台机器连接自己的雷达 |

### 7.2 修改配置文件

**关键**：配置文件中必须添加 `number_of_robots` 参数，默认值是 1（只有自己，不通信）。

编辑 `src/dcl_slam/config/dcl_fast_lio_mid360.yaml`，在 `ros__parameters:` 下添加：

```yaml
/**:
  ros__parameters:
    number_of_robots: 2          # ← 添加这行，两台机器人
    # ... 其余参数不变
```

两台机器上的这个值**必须相同**。修改后重新编译：
```bash
colcon build --symlink-install --packages-select dcl_slam
```

### 7.3 启动

**机器人 A**（namespace `/a`）：
```bash
# 终端 1：雷达
ros2 launch livox_ros_driver2 msg_MID360_launch.py

# 终端 2：DCL-SLAM（当前 launch 文件默认 namespace=/a，直接启动即可）
source ~/ws_livox/install/setup.bash
source ~/DCL-SLAM/install/setup.bash
ros2 launch dcl_slam dcl_fast_lio_mid360.launch.py rviz:=false
```

**机器人 B**（namespace `/b`）：
```bash
# 终端 1：雷达
ros2 launch livox_ros_driver2 msg_MID360_launch.py

# 终端 2：DCL-SLAM（需要修改 namespace 为 /b）
source ~/ws_livox/install/setup.bash
source ~/DCL-SLAM/install/setup.bash
ros2 run dcl_fast_lio fastlio_mapping --ros-args \
  --params-file $(ros2 pkg prefix dcl_slam)/share/dcl_slam/config/dcl_fast_lio_mid360.yaml \
  -r __ns:=/b \
  -r /b/livox/imu:=/livox/imu \
  -r /b/livox/lidar:=/livox/lidar
```

**监控站**（可选，任意一台电脑）：
```bash
source ~/DCL-SLAM/install/setup.bash
rviz2 -d $(ros2 pkg prefix dcl_slam)/share/dcl_slam/config/dcl_fast_lio_mid360.rviz
```

### 7.4 验证通信正常

```bash
# 1. 检查两台机器人的 topic 是否都能看到
ros2 topic list | grep distributedMapping
# 预期输出：
#   /a/distributedMapping/globalDescriptors
#   /a/distributedMapping/loopInfo
#   /a/distributedMapping/optimizationState
#   /b/distributedMapping/globalDescriptors
#   /b/distributedMapping/loopInfo
#   /b/distributedMapping/optimizationState
#   ...

# 2. 检查 topic 有发布者和订阅者
ros2 topic info /a/distributedMapping/globalDescriptors
# 预期：Publisher count: 1, Subscription count: 1（机器人B在订阅）

# 3. 监听描述子是否在流通
ros2 topic echo /a/distributedMapping/globalDescriptors --no-arr --once
# 有数据输出说明 A 正在广播描述子

# 4. 检查频率
ros2 topic hz /a/distributedMapping/globalDescriptors
# 预期：与关键帧产生频率一致（约 1-10Hz）

# 5. 查看日志确认跨机器人描述子已收到
cat ~/log/*.INFO | grep globalDescriptorHandler
# 预期：[globalDescriptorHandler(1)] saveDescriptorAndKey:5.
#        ↑ 机器人A收到了机器人B(id=1)的第5个描述子
```

### 7.5 通信拓扑图

```
机器人A (192.168.1.10, ns=/a)          机器人B (192.168.1.11, ns=/b)
┌───────────────────────────┐          ┌───────────────────────────┐
│ fastlio_mapping           │          │ fastlio_mapping           │
│                           │          │                           │
│ PUB: /a/.../globalDesc    │──DDS──→  │ SUB: /a/.../globalDesc    │
│ SUB: /b/.../globalDesc    │  ←──DDS──│ PUB: /b/.../globalDesc    │
│ PUB: /a/.../loopInfo      │──DDS──→  │ SUB: /a/.../loopInfo      │
│ SUB: /b/.../loopInfo      │  ←──DDS──│ PUB: /b/.../loopInfo      │
│ PUB: /a/.../optState      │──DDS──→  │ SUB: /a/.../optState      │
│ SUB: /b/.../optState      │  ←──DDS──│ PUB: /b/.../optState      │
│ ... (旋转/位姿估计同理)    │          │ ... (旋转/位姿估计同理)    │
│                           │          │                           │
│ MID-360 (/livox/lidar)    │          │ MID-360 (/livox/lidar)    │
└───────────────────────────┘          └───────────────────────────┘
         ↑ 局域网交换机 / 路由器 ↑
```

---

## 8. 参数速查

`src/dcl_slam/config/dcl_fast_lio_mid360.yaml` 中的关键参数：

| 参数 | 默认值 | 说明 |
|------|--------|------|
| `number_of_robots` | 1 | 机器人总数（**多机必改**） |
| `inter_robot_loop_closure_enable` | true | 跨机器人回环检测 |
| `global_optmization_enable` | true | 分布式位姿图优化 |
| `descriptor_type` | LidarIris | 描述子类型 |
| `keyframe_distance_threshold` | 1.0 | 关键帧距离阈值（米） |
| `fitness_score_threshold` | 0.2 | ICP 匹配阈值（越小越严格） |
| `mapping.det_range` | 40.0 | MID-360 最大检测距离（米） |

---

## 9. 常见问题

**Q: 编译报 `No module named 'ament_package'`**
不要用 `sudo colcon build`。删除 `build/` `install/` `log/` 后重新编译。

**Q: 编译报 `livox_ros_driver2 not found`**
编译前忘了 `source ~/ws_livox/install/setup.bash`。清理后重新编译。

**Q: 启动报 `Invalid robot prefix`**
namespace 必须是单个小写字母，如 `/a`、`/b`。

**Q: `IMU Initial Done` 一直不出现**
保持雷达静止 2-3 秒让 IMU 完成初始化。如果 `/livox/imu` 没数据，检查雷达驱动。

**Q: 看不到其他机器人的 topic**
1. 两台机器能互相 `ping` 通？
2. `echo $ROS_DOMAIN_ID` 值相同？（默认 0）
3. 防火墙是否阻止了 DDS 端口？尝试 `sudo ufw disable` 临时关闭测试。

**Q: topic 存在但 `ros2 topic info` 显示 0 subscribers**
`number_of_robots` 仍为 1。修改 yaml 后重新编译。

**Q: 两台机器人都在跑但从不触发跨机器人回环**
1. 确认 `inter_robot_loop_closure_enable: true`
2. 确认 `number_of_robots: 2`
3. 两台机器人需要经过**重叠区域**才能触发回环
4. 查看 `~/log/*.INFO` 中的 `globalDescriptorHandler` 日志

**Q: `Could not create logging file`**
```bash
mkdir -p ~/log
```

**Q: Ctrl+C 退不出进程**
```bash
ps aux | grep fastlio
kill -9 <PID>
```

---

## 10. 目录结构

```
DCL-SLAM/
├── src/
│   ├── dcl_fast_lio/          # FAST-LIO2 前端（含 DCL-SLAM 集成）
│   │   ├── src/laserMapping.cpp  # 主程序入口
│   │   └── include/              # IMU处理、ikd-Tree 等
│   ├── dcl_slam/              # DCL-SLAM 分布式后端
│   │   ├── src/               # 分布式建图、回环检测、可视化
│   │   ├── include/           # 头文件
│   │   ├── config/            # 参数配置文件
│   │   ├── launch/            # 启动文件
│   │   └── msg/               # 自定义消息（LoopInfo、GlobalDescriptor 等）
│   ├── distributed_mapper/    # 分布式位姿图优化器（DGS）
│   └── libnabo/               # 近邻搜索库
├── build/                     # 编译产物（不要提交）
├── install/                   # 安装产物（不要提交）
└── log/                       # 编译日志（不要提交）
```
