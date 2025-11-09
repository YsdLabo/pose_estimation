#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/imu.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

// 重力による姿勢計算
class  PoseUsingGyro  :  public  rclcpp::Node
{
public:
	// コンストラクタ
	PoseUsingGyro( ) : Node("pose_using_gyro_node")
	{
		// サブスクライバーを作成（/imu_rcfトピックを購読）
		sub_imu_ = this->create_subscription<sensor_msgs::msg::Imu> (
			"/imu_rcf", 
			rclcpp::SensorDataQoS( ),
			std::bind(&PoseUsingGyro::callback_imu, this, std::placeholders::_1)
		);
		// パブリッシャーを作成（/pose_gyroトピックを配信）
		pub_pose_ = this->create_publisher<geometry_msgs::msg::PoseStamped> (
			"/pose_gyro",
			rclcpp::SensorDataQoS( )
		);
	}

private:
	// メンバー変数
	rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr  sub_imu_;
	rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr  pub_pose_;
	double  roll_ = 0.0, pitch_ = 0.0, yaw_ = 0.0;
	double  w_x_i_ = 0.0, w_y_i_ = 0.0, w_z_i_ = 0.0;
	int  count = 0;
	rclcpp::Time last_stamp_;
	bool  first_time = true;

	// 購読用コールバック関数
	void  callback_imu( const  sensor_msgs::msg::Imu::SharedPtr  msg )
	{
		sensor_msgs::msg::Imu::SharedPtr  imu_rcf = msg;   // 受信メッセージをコピー
		rclcpp::Time current_stamp(imu_rcf->header.stamp);
		double  w_x = imu_rcf->angular_velocity.x;    // 角速度X
		double  w_y = imu_rcf->angular_velocity.y;    // 角速度Y
		double  w_z = imu_rcf->angular_velocity.z;    // 角速度Z

		if( !first_time ) {
			double  dt = (current_stamp - last_stamp_).seconds( );
			double  dwx = (w_x - w_x_i) * dt;
			double  dwy = (w_y - w_y_i) * dt;
			double  dwz = (w_z - w_z_i) * dt;
			double  Sr = std::sin( roll_ );
			double  Cr = std::cos( roll_ );
			double  Tp = std::tan( pitch_ );
			double  Cp = std::cos( pitch_ );

			roll_ += // ここに数式を入れましょう
			pitch_ += // ここに数式を入れましょう
			yaw_ += // ここに数式を入れましょう
		}
		else {
			w_x_i_ += w_x;
			w_y_i_ += w_y;
			w_z_i_ += w_z;
			count ++;
			if(count == 1000) {
				w_x_i_ /= (double)count;
				w_y_i_ /= (double)count;
				w_z_i_ /= (double)count;
				first_time = false;
			}
		}

		// オイラー角をクォータニオンに変換して配信
		tf2::Quaternion  quat;
		quat.setRPY( roll_, pitch_, yaw_ );    // オイラー角からクォータニオンを生成
		quat.normalize( );     // 正規化
		geometry_msgs::msg::PoseStamped  pub_msg;  // メッセージ生成
		pub_msg.pose.orientation = tf2::toMsg( quat );    // メッセージ型へ変換
		pub_msg.header.stamp = imu_rcf->header.stamp;
		pub_msg.header.frame_id = "imu_link";
		pub_pose_->publish( pub_msg );
		
		last_stamp_ = current_stamp;
	}
};

int  main( int  argc, char**  argv )
{
	rclcpp::init( argc, argv );
	rclcpp::spin(std::make_shared<PoseUsingGyro>( ));
	rclcpp::shutdown( );
	return 0;
}

