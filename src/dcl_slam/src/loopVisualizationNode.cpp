#include "dcl_slam/msg/loop_info.hpp"

#include <sensor_msgs/msg/point_cloud2.hpp>
#include <pcl_conversions/pcl_conversions.h>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>

#include <gtsam/geometry/Rot3.h>
#include <gtsam/geometry/Pose3.h>
#include <gtsam/inference/Symbol.h>

#include <rclcpp/rclcpp.hpp>

#include <thread>
#include <mutex>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

class LoopVisualizationNode : public rclcpp::Node
{
public:
    LoopVisualizationNode() : Node("loop_visualization_node")
    {
        this->declare_parameter("number_of_robots", 3);
        number_of_robots_ = this->get_parameter("number_of_robots").as_int();

        pub_loop_closure_constraints = this->create_publisher<visualization_msgs::msg::MarkerArray>(
            "distributedMapping/loopClosureConstraints", 1);

        for(int i = 0; i < number_of_robots_; i++)
        {
            std::string name = "a";
            name[0] += i;

            auto sub_keypose_cloud = this->create_subscription<sensor_msgs::msg::PointCloud2>(
                name+"/distributedMapping/keyposeCloud", 1, 
                [this, i](const sensor_msgs::msg::PointCloud2::SharedPtr msg) {
                    this->keyposeCloudHandler(msg, i);
                });
            sub_keypose_cloud_vec.push_back(sub_keypose_cloud);

            auto sub_loop_info = this->create_subscription<dcl_slam::msg::LoopInfo>(
                name+"/distributedMapping/loopInfo", 1, 
                [this, i](const dcl_slam::msg::LoopInfo::SharedPtr msg) {
                    this->loopInfoHandler(msg, i);
                });
            sub_loop_info_vec.push_back(sub_loop_info);
        }

        // Use timer for loop closure thread logic (0.1Hz = 10s)
        timer_ = this->create_wall_timer(
            10s, std::bind(&LoopVisualizationNode::loopClosureTimerCallback, this));
    }

private:
    void keyposeCloudHandler(
        const sensor_msgs::msg::PointCloud2::SharedPtr msg,
        int id)
    {
        pcl::PointCloud<pcl::PointXYZI>::Ptr keyposes(new pcl::PointCloud<pcl::PointXYZI>());
        pcl::fromROSMsg(*msg, *keyposes);
        cloud_queue_vec[id] = *keyposes;
    }

    void loopInfoHandler(
        const dcl_slam::msg::LoopInfo::SharedPtr msg,
        int id)
    {
        (void)id; // unused
        if((int)msg->noise != 999 && (int)msg->noise != 888)
        {
            gtsam::Symbol symbol0((msg->robot0 + 'a'), msg->index0);
            gtsam::Symbol symbol1((msg->robot1 + 'a'), msg->index1);
            auto it = loop_indexs.find(symbol0);
            if(it == loop_indexs.end() || (it != loop_indexs.end() && it->second != symbol1))
            {
                loop_indexs[symbol0] = symbol1;
            }
        }
    }

    void loopClosureTimerCallback()
    {
        if(loop_indexs.empty())
        {
            return;
        }

        // loop nodes
        visualization_msgs::msg::Marker nodes;
        nodes.header.frame_id = "world";
        nodes.header.stamp = this->now();
        nodes.action = visualization_msgs::msg::Marker::ADD;
        nodes.type = visualization_msgs::msg::Marker::SPHERE_LIST;
        nodes.ns = "loop_nodes";
        nodes.id = 2;
        nodes.pose.orientation.w = 1;
        nodes.scale.x = 0.3; nodes.scale.y = 0.3; nodes.scale.z = 0.3; 
        nodes.color.r = 0.93; nodes.color.g = 0.83; nodes.color.b = 0.0;
        nodes.color.a = 1;

        // loop edges
        visualization_msgs::msg::Marker constraints;
        constraints.header.frame_id = "world";
        constraints.header.stamp = this->now();
        constraints.action = visualization_msgs::msg::Marker::ADD;
        constraints.type = visualization_msgs::msg::Marker::LINE_LIST;
        constraints.ns = "loop_constraints";
        constraints.id = 3;
        constraints.pose.orientation.w = 1;
        constraints.scale.x = 0.1;
        constraints.color.r = 1.0; constraints.color.g = 0.91; constraints.color.b = 0.31;
        constraints.color.a = 1;
        
        pcl::PointXYZI pose_3d;
        int robot0, robot1, index0, index1;
        gtsam::Symbol key0, key1;
        for(auto it = loop_indexs.begin(); it != loop_indexs.end(); ++it)
        {
            key0 = it->first;
            key1 = it->second;
            robot0 = key0.chr() - 'a';
            robot1 = key1.chr() - 'a';
            index0 = key0.index();
            index1 = key1.index();

            if(robot0 >= 0 && robot1 >= 0 && 
               cloud_queue_vec.count(robot0) && cloud_queue_vec.count(robot1) &&
               index0 < (int)cloud_queue_vec[robot0].size() && index1 < (int)cloud_queue_vec[robot1].size())
            {
                geometry_msgs::msg::Point p;
                pose_3d = cloud_queue_vec[robot0].points[index0];
                p.x = pose_3d.x;
                p.y = pose_3d.y;
                p.z = pose_3d.z;
                nodes.points.push_back(p);
                constraints.points.push_back(p);
                pose_3d = cloud_queue_vec[robot1].points[index1];
                p.x = pose_3d.x;
                p.y = pose_3d.y;
                p.z = pose_3d.z;
                nodes.points.push_back(p);
                constraints.points.push_back(p);
            }
        }

        // publish loop closure markers
        visualization_msgs::msg::MarkerArray markers_array;
        markers_array.markers.push_back(nodes);
        markers_array.markers.push_back(constraints);
        pub_loop_closure_constraints->publish(markers_array);
    }

    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr pub_loop_closure_constraints;
    std::vector<rclcpp::Subscription<sensor_msgs::msg::PointCloud2>::SharedPtr> sub_keypose_cloud_vec;
    std::vector<rclcpp::Subscription<dcl_slam::msg::LoopInfo>::SharedPtr> sub_loop_info_vec;
    int number_of_robots_;
    std::map<int, pcl::PointCloud<pcl::PointXYZI>> cloud_queue_vec;
    std::map<gtsam::Symbol, gtsam::Symbol> loop_indexs;
    rclcpp::TimerBase::SharedPtr timer_;
};

int main(
    int argc,
    char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<LoopVisualizationNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
