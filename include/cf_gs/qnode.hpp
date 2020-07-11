/**
 * @file /include/cf_gs/qnode.hpp
 *
 * @brief Communications central!
 *
 * @date February 2011
 **/
/*****************************************************************************
** Ifdefs
*****************************************************************************/

#ifndef cf_gs_QNODE_HPP_
#define cf_gs_QNODE_HPP_

/*****************************************************************************
** Includes
*****************************************************************************/

// To workaround boost/qt4 problems that won't be bugfixed. Refer to
//    https://bugreports.qt.io/browse/QTBUG-22829
#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif
#include <string>
#include <QThread>
#include <QStringListModel>

#include <Eigen/Eigen>

#include <cf_gs/Mocap.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/PoseStamped.h>
#include "sensor_msgs/Imu.h"
#include <std_srvs/Empty.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

using Mocap = cf_gs::Mocap;
using PoseStamped = geometry_msgs::PoseStamped;
using Twist = geometry_msgs::Twist;
	// using signalRec = cf_gs::signalRec;

	namespace cf_gs
{

	/*****************************************************************************
** Class
*****************************************************************************/

	struct signalRec
	{
		bool premocap = false;
		bool preros = false;
		bool mocapReceived = false;
		bool rosReceived = false;
		bool Isconnected = false;
	};

	class QNode : public QThread
	{
		Q_OBJECT

	public:
		QNode(int argc, char **argv);
		virtual ~QNode();
		// bool init();
		bool init();
		void run();

		std::map<std::string, ros::Subscriber> sub_;

		void pub_command();
		void takeoff(int id);
		void land(int id);
		void move_cf(int id, float target[3]);
		cf_gs::signalRec Update_cf_signal(int id);
		Mocap GetMocap(int id);
		PoseStamped GetDesPos(int id);

	Q_SIGNALS:
		void rosLoopUpdate();
		void rosShutdown();

	private:
		int init_argc;
		char **init_argv;

		int DroneNumber;
		
		
		bool commandFlag[6];
		signalRec cf_Recieved[6];
		Mocap cf_mocap[6];
		PoseStamped cf_goal[6];
		Twist cf_cmdv[6];
		sensor_msgs::Imu cf_imu[6];

		ros::Subscriber mocap_cf0;
		ros::Subscriber goal_subs_cf0;
		ros::Subscriber imu_sub_cf0;
		ros::Publisher pose_pub_cf0;
		ros::Publisher cmdv_pub_cf0;
		ros::ServiceClient takeoff_cf0;
		ros::ServiceClient land_cf0;

		ros::Subscriber mocap_cf1;
		ros::Subscriber goal_subs_cf1;
		ros::Subscriber imu_sub_cf1;
		ros::Publisher pose_pub_cf1;
		ros::Publisher cmdv_pub_cf1;
		ros::ServiceClient takeoff_cf1;
		ros::ServiceClient land_cf1;

		ros::Subscriber mocap_cf2;
		ros::Subscriber goal_subs_cf2;
		ros::Subscriber imu_sub_cf2;
		ros::Publisher pose_pub_cf2;
		ros::Publisher cmdv_pub_cf2;
		ros::ServiceClient takeoff_cf2;
		ros::ServiceClient land_cf2;

		ros::Subscriber mocap_cf3;
		ros::Subscriber goal_subs_cf3;
		ros::Subscriber imu_sub_cf3;
		ros::Publisher pose_pub_cf3;
		ros::Publisher cmdv_pub_cf3;
		ros::ServiceClient takeoff_cf3;
		ros::ServiceClient land_cf3;

		ros::Subscriber mocap_cf4;
		ros::Subscriber goal_subs_cf4;
		ros::Subscriber imu_sub_cf4;
		ros::Publisher pose_pub_cf4;
		ros::Publisher cmdv_pub_cf4;
		ros::ServiceClient takeoff_cf4;
		ros::ServiceClient land_cf4;

		ros::Subscriber mocap_cf5;
		ros::Subscriber goal_subs_cf5;
		ros::Subscriber imu_sub_cf5;
		ros::Publisher pose_pub_cf5;
		ros::Publisher cmdv_pub_cf5;
		ros::ServiceClient takeoff_cf5;
		ros::ServiceClient land_cf5;

		std_srvs::Empty serv_empty;

		void mocap_callback_0(const Mocap::ConstPtr &msg);
		void mocap_callback_1(const Mocap::ConstPtr &msg);
		void mocap_callback_2(const Mocap::ConstPtr &msg);
		void mocap_callback_3(const Mocap::ConstPtr &msg);
		void mocap_callback_4(const Mocap::ConstPtr &msg);
		void mocap_callback_5(const Mocap::ConstPtr &msg);
		void goalChanged_0(const PoseStamped::ConstPtr &msg);
		void goalChanged_1(const PoseStamped::ConstPtr &msg);
		void goalChanged_2(const PoseStamped::ConstPtr &msg);
		void goalChanged_3(const PoseStamped::ConstPtr &msg);
		void goalChanged_4(const PoseStamped::ConstPtr &msg);
		void goalChanged_5(const PoseStamped::ConstPtr &msg);
		void imu_callback_0(const sensor_msgs::Imu::ConstPtr &msg);
		void imu_callback_1(const sensor_msgs::Imu::ConstPtr &msg);
		void imu_callback_2(const sensor_msgs::Imu::ConstPtr &msg);
		void imu_callback_3(const sensor_msgs::Imu::ConstPtr &msg);
		void imu_callback_4(const sensor_msgs::Imu::ConstPtr &msg);
		void imu_callback_5(const sensor_msgs::Imu::ConstPtr &msg);



		

		// // std::vector<std::string> mocap_topics;

		// // Eigen::Vector3d m_goal;
		// // Eigen::Vector3d m_dronePositionWorld;
		// // Eigen::Vector3d m_droneVelocityWorld;

		// signalRec cf0_Recieved;
		// PoseStamped cf0_goal;
		// // PoseStamped my_goal;
		// Twist cf0_cmdv;
		// sensor_msgs::Imu cf0_imu;

		// Mocap mocap;

		// void mocap_callback(const Mocap::ConstPtr &msg);
		// void goalChanged(const PoseStamped::ConstPtr &msg);
		// void imu_callback(const sensor_msgs::Imu::ConstPtr &msg);
	};

} // namespace cf_gs
#endif /* cf_gs_QNODE_HPP_ */
