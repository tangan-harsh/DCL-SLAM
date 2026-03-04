#include "distributedMapping.h"
#include <rclcpp/rclcpp.hpp>

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<distributedMapping>();
    
    // distributedMapping has internal timers/threads, so we just spin
    rclcpp::spin(node);
    
    rclcpp::shutdown();
    return 0;
}
