# DCL-SLAM ROS2 迁移指南

## 状态
- **接口 (Interfaces)**: 已迁移 (`msg` 文件)。
- **描述符 (Descriptors)**: 已迁移 (`scanContext`, `lidarIris`, `m2dp`)。
- **核心逻辑 (Core Logic)**: 已移植到 `rclcpp`, `tf2`。
- **启动文件 (Launch)**: 已转换为 Python 启动文件。
- **依赖项 (Dependencies)**: `distributed_mapper` 和 `libnabo` 已包含在 `src/` 中并配置为 `colcon` 构建。

## 依赖项
您只需要手动安装 **GTSAM**。

1.  **GTSAM** (4.0+):
    ```bash
    sudo add-apt-repository ppa:borglab/gtsam-release-4.0
    sudo apt install libgtsam-dev libgtsam-unstable-dev
    # 或者从源码编译
    # git clone https://github.com/borglab/gtsam.git ...
    ```

2.  **libnabo** & **distributed_mapper**:
    这些已经包含在工作区的 `src/` 目录中。 `colcon` 会自动编译它们。

## 编译说明
1.  如上所示安装 GTSAM。
2.  编译工作区：
    ```bash
    colcon build --symlink-install
    ```

## 运行
```bash
source install/setup.bash
ros2 launch dcl_slam run.launch.py
```
