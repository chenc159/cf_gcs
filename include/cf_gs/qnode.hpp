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

/*****************************************************************************
** Namespaces
*****************************************************************************/

using Mocap = cf_gs::Mocap;
using PoseStamped = geometry_msgs::PoseStamped;
// using signalRec = cf_gs::signalRec;

namespace cf_gs
{

/*****************************************************************************
** Class
*****************************************************************************/

struct signalRec
{
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

	cf_gs::signalRec Update_cf0_signal();

	void pub_command();
	void takeoff();
	void land();
	void move_cf(float target[3]);
	Mocap GetMocap();
	PoseStamped GetDesPos();

Q_SIGNALS:
	void rosLoopUpdate();
	void rosShutdown();

private:
	int init_argc;
	char **init_argv;

	int DroneNumber;
	bool commandFlag[3];
	// bool mocapReceived[3];
	// bool rosReceived[3];
	// bool Isconnected[3];
	signalRec cf0_Recieved;

	ros::Subscriber mocap_cf0;
	ros::Subscriber goal_subs_cf0;

	ros::Publisher pose_pub_cf0;

	ros::ServiceClient cf0_Takeoff;
	ros::ServiceClient cf0_Land;

	std::vector<std::string> mocap_topics;

	Eigen::Vector3d m_goal;
	Eigen::Vector3d m_dronePositionWorld;
	Eigen::Vector3d m_droneVelocityWorld;

	PoseStamped cf0_goal;
	PoseStamped my_goal;

	Mocap mocap;
	
	void goalChanged(const PoseStamped::ConstPtr &msg);
	void mocap_callback(const Mocap::ConstPtr &msg);
};

} // namespace cf_gs
#endif /* cf_gs_QNODE_HPP_ */
