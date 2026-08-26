#include <algorithm>
#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

class MecanumController : public rclcpp::Node
{
 public:
  MecanumController()
      : Node("fwr_controller",
             rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true))
  {
    // Declare and load parameters from YAML (ROS 2 Best Practice)
    double wheel_radius = 0.04;
    double wheel_base_x = 0.125;
    double wheel_base_y = 0.150;

    this->get_parameter_or("wheel_radius", wheel_radius, 0.04);
    this->get_parameter_or("wheel_base_x", wheel_base_x, 0.125);
    this->get_parameter_or("wheel_base_y", wheel_base_y, 0.150);
    this->get_parameter_or("max_wheel_rpm", max_wheel_rpm_, 300.0);

    std::vector<double> wheel_dir;
    this->get_parameter_or("wheel_direction", wheel_dir, std::vector<double>{1.0, 1.0, 1.0, 1.0});

    std::string cmd_vel_topic, wheel_speed_topic;
    this->get_parameter_or("cmd_vel_topic", cmd_vel_topic, std::string("/cmd_vel"));
    this->get_parameter_or("wheel_speed_topic", wheel_speed_topic, std::string("/wheel_speed_cmd"));
    // Exception handling and validation
    if (wheel_radius <= 0.0)
    {
      RCLCPP_ERROR(this->get_logger(), "wheel_radius must be positive! Resetting to default 0.04m");
      wheel_radius = 0.04;
    }

    if (wheel_dir.size() != NUM_WHEELS)
    {
      RCLCPP_WARN(this->get_logger(),
                  "wheel_direction size mismatch! Expected %zu, got %zu. Resetting to [1, 1, 1, 1]",
                  NUM_WHEELS, wheel_dir.size());
      wheel_dir = std::vector<double>(NUM_WHEELS, 1.0);
    }

    // Pre-compute constants to reduce overhead in Callback
    wheel_base_sum_ = wheel_base_x + wheel_base_y;
    double scale = (60.0 / (2.0 * M_PI)) / wheel_radius;

    // Pre-multiply scale with wheel directions (OPTIMIZED)
    for (size_t i = 0; i < NUM_WHEELS; ++i)
    {
      scaled_wheel_dir_[i] = scale * wheel_dir[i];
    }

    // Pre-allocate memory for the message (Avoids malloc during callback)
    speed_msg_.data.resize(NUM_WHEELS, 0.0);

    RCLCPP_INFO(this->get_logger(),
                "Controller Initialized | R=%.3f, Lx=%.3f, Ly=%.3f, Max RPM=%.1f", wheel_radius,
                wheel_base_x, wheel_base_y, max_wheel_rpm_);

    // Initialize Publisher & Subscriber
    wheel_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(wheel_speed_topic, 10);
    cmd_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
        cmd_vel_topic, 10,
        std::bind(&MecanumController::cmdVelCallback, this, std::placeholders::_1));
  }

 private:
  static constexpr size_t NUM_WHEELS = 4;

  void cmdVelCallback(const geometry_msgs::msg::Twist::SharedPtr msg)
  {
    const double vx = msg->linear.x;
    const double vy = msg->linear.y;
    const double rot = wheel_base_sum_ * msg->angular.z;

    // Kinematic sign matrix
    static constexpr double signs[NUM_WHEELS][2] = {
        {-1.0, -1.0},  // FL
        {1.0, 1.0},    // FR
        {1.0, -1.0},   // RL
        {-1.0, 1.0}    // RR
    };

    for (size_t i = 0; i < NUM_WHEELS; ++i)
    {
      const double rpm = (vx + signs[i][0] * vy + signs[i][1] * rot) * scaled_wheel_dir_[i];
      speed_msg_.data[i] = std::clamp(rpm, -max_wheel_rpm_, max_wheel_rpm_);
    }

    wheel_pub_->publish(speed_msg_);
  }

  double wheel_base_sum_;
  double max_wheel_rpm_;
  std::array<double, NUM_WHEELS> scaled_wheel_dir_;

  std_msgs::msg::Float64MultiArray speed_msg_;

  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr wheel_pub_;
};

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<MecanumController>());
  rclcpp::shutdown();
  return 0;
}