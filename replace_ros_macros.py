import os

files = [
    "src/dcl_slam/src/distributedLoopClosure.cpp",
    "src/dcl_slam/src/distributedMapping.cpp"
]
base_path = "/home/isaac/DCL-SLAM"

replacements = [
    ("ROS_DEBUG(", "RCLCPP_DEBUG(this->get_logger(), "),
    ("ROS_INFO(", "RCLCPP_INFO(this->get_logger(), "),
    ("ROS_WARN(", "RCLCPP_WARN(this->get_logger(), "),
    ("ROS_ERROR(", "RCLCPP_ERROR(this->get_logger(), "),
    ("ROS_FATAL(", "RCLCPP_FATAL(this->get_logger(), ")
]

for rel_path in files:
    path = os.path.join(base_path, rel_path)
    if not os.path.exists(path):
        print(f"Skipping {rel_path}: file not found")
        continue

    with open(path, "r") as f:
        content = f.read()

    new_content = content
    for old, new in replacements:
        new_content = new_content.replace(old, new)

    if new_content != content:
        with open(path, "w") as f:
            f.write(new_content)
        print(f"Updated {rel_path}")
    else:
        print(f"No changes in {rel_path}")
