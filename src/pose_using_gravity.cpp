#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <tf2/LinearMath/Quaternion.h>   // tf2::Quaternion
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>  // tf2::toMsg
#include <geometry_msgs/msg/pose_stamped.hpp>

class PoseUsingGravity : public rclcpp::Node
{
public:
	PoseUsingGravity() : Node("pose_using_gravity_node")
	{
		// サブスクライバーを作成（/imuトピックを購読）
		sub_imu_ = this->create_subscription<sensor_msgs::msg::Imu> (
			"/imu", 
			rclcpp::SensorDataQoS(),
			std::bind(&PoseUsingGravity::callback_imu, this, std::placeholders::_1)
		);
		// パブリッシャーを作成（/pose_gravトピックを配信）
		pub_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped> (
			"/pose_grav",
			rclcpp::SensorDataQoS()
		);
	}

private:
	// 購読用コールバック関数
	void  callback_imu( const sensor_msgs::msg::Imu::SharedPtr msg )
	{
		const auto& imu_rcf = *msg;	// 受信メッセージをコピー
		double acc_x = imu_rcf.linear_acceleration.x;    // 加速度X
		double acc_y = imu_rcf.linear_acceleration.y;    // 加速度Y
		double acc_z = imu_rcf.linear_acceleration.z;    // 加速度Z

		double roll = // ここに数式を入れましょう
		double pitch = // ここに数式を入れましょう
		double yaw = 0.0;

		// オイラー角をクォータニオンに変換して配信
		tf2::Quaternion  quat;
		quat.setRPY( roll, pitch, yaw );    // クォータニオンの生成
		quat.normalize();     // 正規化
		geometry_msgs::msg::PoseStamped  pub_msg;  // メッセージ生成
		pub_msg.pose.orientation = tf2::toMsg( quat );    // メッセージ型へ変換
		pub_msg.header.stamp = imu_rcf.header.stamp;
		pub_msg.header.frame_id = "imu_link";
		pub_pose_->publish( pub_msg );

	}

	rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr sub_imu_;
	rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pub_pose_;
};

int  main(int  argc, char**  argv)
{
	rclcpp::init(argc, argv);
	rclcpp::spin(std::make_shared<PoseUsingGravity>());
	rclcpp::shutdown();
	return  0;
}

