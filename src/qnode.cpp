/**
 * @file /src/qnode.cpp
 *
 * @brief Ros communication central!
 *
 * @date February 2011
 **/

/*****************************************************************************
** Includes
*****************************************************************************/

#include <ros/ros.h>
#include <ros/network.h>
#include <string>
#include <std_msgs/String.h>
#include <sstream>
#include "../include/cf_gs/qnode.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace cf_gs
{

/*****************************************************************************
** Implementation
*****************************************************************************/

QNode::QNode(int argc, char **argv) : init_argc(argc),
									  init_argv(argv)
{
	DroneNumber = 6;
	commandFlag[0] = false;
	commandFlag[1] = false;
	commandFlag[2] = false;
	commandFlag[3] = false;
	commandFlag[4] = false;
	commandFlag[5] = false;
}

QNode::~QNode()
{
	if (ros::isStarted())
	{
		ros::shutdown(); // explicitly needed since we use ros::start();
		ros::waitForShutdown();
	}
	wait();
}


bool QNode::init(void)
{
	ros::init(init_argc, init_argv, "cf_gs");
	if (!ros::master::check())
	{
		return false;
	}

	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;

	mocap_cf0 		= n.subscribe<Mocap>("mocap/cf0", 1, &QNode::mocap_callback_0, this);
	goal_subs_cf0 	= n.subscribe<PoseStamped>("cf0/goal", 1, &QNode::goalChanged_0, this);
	imu_sub_cf0 	= n.subscribe<sensor_msgs::Imu>("cf0/imu", 1, &QNode::imu_callback_0, this);
	pose_pub_cf0 	= n.advertise<PoseStamped>("cf0/goal", 100);
	cmdv_pub_cf0 	= n.advertise<Twist>("cf0/cmdV", 100);
	takeoff_cf0 	= n.serviceClient<std_srvs::Empty>("cf0/cftakeoff");
	land_cf0 		= n.serviceClient<std_srvs::Empty>("cf0/cfland");

	mocap_cf1 		= n.subscribe<Mocap>("mocap/cf1", 1, &QNode::mocap_callback_1, this);
	goal_subs_cf1 	= n.subscribe<PoseStamped>("cf1/goal", 1, &QNode::goalChanged_1, this);
	imu_sub_cf1 	= n.subscribe<sensor_msgs::Imu>("cf1/imu", 1, &QNode::imu_callback_1, this);
	pose_pub_cf1 	= n.advertise<PoseStamped>("cf1/goal", 100);
	cmdv_pub_cf1 	= n.advertise<Twist>("cf1/cmdV", 100);
	takeoff_cf1 	= n.serviceClient<std_srvs::Empty>("cf1/cftakeoff");
	land_cf1 		= n.serviceClient<std_srvs::Empty>("cf1/cfland");

	mocap_cf2 		= n.subscribe<Mocap>("mocap/cf2", 1, &QNode::mocap_callback_2, this);
	goal_subs_cf2 	= n.subscribe<PoseStamped>("cf2/goal", 1, &QNode::goalChanged_2, this);
	imu_sub_cf2 	= n.subscribe<sensor_msgs::Imu>("cf2/imu", 1, &QNode::imu_callback_2, this);
	pose_pub_cf2 	= n.advertise<PoseStamped>("cf2/goal", 100);
	cmdv_pub_cf2 	= n.advertise<Twist>("cf2/cmdV", 100);
	takeoff_cf2		= n.serviceClient<std_srvs::Empty>("cf2/cftakeoff");
	land_cf2 		= n.serviceClient<std_srvs::Empty>("cf2/cfland");

	mocap_cf3 		= n.subscribe<Mocap>("mocap/cf3", 1, &QNode::mocap_callback_3, this);
	goal_subs_cf3 	= n.subscribe<PoseStamped>("cf3/goal", 1, &QNode::goalChanged_3, this);
	imu_sub_cf3 	= n.subscribe<sensor_msgs::Imu>("cf3/imu", 1, &QNode::imu_callback_3, this);
	pose_pub_cf3 	= n.advertise<PoseStamped>("cf3/goal", 100);
	cmdv_pub_cf3 	= n.advertise<Twist>("cf3/cmdV", 100);
	takeoff_cf3 	= n.serviceClient<std_srvs::Empty>("cf3/cftakeoff");
	land_cf3 		= n.serviceClient<std_srvs::Empty>("cf3/cfland");

	mocap_cf4 		= n.subscribe<Mocap>("mocap/cf4", 1, &QNode::mocap_callback_4, this);
	goal_subs_cf4 	= n.subscribe<PoseStamped>("cf4/goal", 1, &QNode::goalChanged_4, this);
	imu_sub_cf4 	= n.subscribe<sensor_msgs::Imu>("cf4/imu", 1, &QNode::imu_callback_4, this);
	pose_pub_cf4 	= n.advertise<PoseStamped>("cf4/goal", 100);
	cmdv_pub_cf4 	= n.advertise<Twist>("cf4/cmdV", 100);
	takeoff_cf4 	= n.serviceClient<std_srvs::Empty>("cf4/cftakeoff");
	land_cf4 		= n.serviceClient<std_srvs::Empty>("cf4/cfland");

	mocap_cf5 		= n.subscribe<Mocap>("mocap/cf5", 1, &QNode::mocap_callback_5, this);
	goal_subs_cf5 	= n.subscribe<PoseStamped>("cf5/goal", 1, &QNode::goalChanged_5, this);
	imu_sub_cf5 	= n.subscribe<sensor_msgs::Imu>("cf5/imu", 1, &QNode::imu_callback_5, this);
	pose_pub_cf5 	= n.advertise<PoseStamped>("cf5/goal", 100);
	cmdv_pub_cf5 	= n.advertise<Twist>("cf5/cmdV", 100);
	takeoff_cf5 	= n.serviceClient<std_srvs::Empty>("cf5/cftakeoff");
	land_cf5 		= n.serviceClient<std_srvs::Empty>("cf5/cfland");


	start();
	return true;
}

void QNode::run()
{
	ros::Rate loop_rate(1);
	while (ros::ok())
	{
		pub_command();
		ros::spinOnce();

		// Update mocap/ros singal received or not
		for (int i = 0; i <DroneNumber; i++)
		{
			cf_Recieved[i].mocapReceived = false;
			cf_Recieved[i].rosReceived = false;
			cf_Recieved[i].Isconnected = false;
			if (cf_Recieved[i].premocap){
				cf_Recieved[i].mocapReceived = true;
			}
			if (cf_Recieved[i].preros){
				cf_Recieved[i].rosReceived = true;
			}
			if (cf_Recieved[i].premocap && cf_Recieved[i].preros){
				cf_Recieved[i].Isconnected = true;
			}
			cf_Recieved[i].premocap = false;
			cf_Recieved[i].preros = false;
		}

		/* signal a ros loop update  */
		Q_EMIT rosLoopUpdate();
		loop_rate.sleep();
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}


void QNode::mocap_callback_0(const cf_gs::Mocap::ConstPtr &msg){
	cf_mocap[0] = *msg;
	cf_Recieved[0].premocap = true;
}
void QNode::mocap_callback_1(const cf_gs::Mocap::ConstPtr &msg){
	cf_mocap[1] = *msg;
	cf_Recieved[1].premocap = true;
}
void QNode::mocap_callback_2(const cf_gs::Mocap::ConstPtr &msg){
	cf_mocap[2] = *msg;
	cf_Recieved[2].premocap = true;
}
void QNode::mocap_callback_3(const cf_gs::Mocap::ConstPtr &msg){
	cf_mocap[3] = *msg;
	cf_Recieved[3].premocap = true;
}
void QNode::mocap_callback_4(const cf_gs::Mocap::ConstPtr &msg){
	cf_mocap[4] = *msg;
	cf_Recieved[4].premocap = true;
}
void QNode::mocap_callback_5(const cf_gs::Mocap::ConstPtr &msg){
	cf_mocap[5] = *msg;
	cf_Recieved[5].premocap = true;
}


void QNode::goalChanged_0(const geometry_msgs::PoseStamped::ConstPtr &msg){
	cf_goal[0] = *msg;
}
void QNode::goalChanged_1(const geometry_msgs::PoseStamped::ConstPtr &msg){
	cf_goal[1] = *msg;
}
void QNode::goalChanged_2(const geometry_msgs::PoseStamped::ConstPtr &msg){
	cf_goal[2] = *msg;
}
void QNode::goalChanged_3(const geometry_msgs::PoseStamped::ConstPtr &msg){
	cf_goal[3] = *msg;
}
void QNode::goalChanged_4(const geometry_msgs::PoseStamped::ConstPtr &msg){
	cf_goal[4] = *msg;
}
void QNode::goalChanged_5(const geometry_msgs::PoseStamped::ConstPtr &msg){
	cf_goal[5] = *msg;
}


void QNode::imu_callback_0(const sensor_msgs::Imu::ConstPtr &msg){
	// If can get msg from imu, ros connected
	cf_imu[0] = *msg;
	cf_Recieved[0].preros = true;
}
void QNode::imu_callback_1(const sensor_msgs::Imu::ConstPtr &msg){
	cf_imu[1] = *msg;
	cf_Recieved[1].preros = true;
}
void QNode::imu_callback_2(const sensor_msgs::Imu::ConstPtr &msg){
	cf_imu[2] = *msg;
	cf_Recieved[2].preros = true;
}
void QNode::imu_callback_3(const sensor_msgs::Imu::ConstPtr &msg){
	cf_imu[3] = *msg;
	cf_Recieved[3].preros = true;
}
void QNode::imu_callback_4(const sensor_msgs::Imu::ConstPtr &msg){
	cf_imu[4] = *msg;
	cf_Recieved[4].preros = true;
}
void QNode::imu_callback_5(const sensor_msgs::Imu::ConstPtr &msg){
	cf_imu[5] = *msg;
	cf_Recieved[5].preros = true;
}


void QNode::pub_command(){
	if (commandFlag[0]){
		pose_pub_cf0.publish(cf_goal[0]);
		cmdv_pub_cf0.publish(cf_cmdv[0]);
		commandFlag[0] = false;
	}
	if (commandFlag[1]){
		pose_pub_cf1.publish(cf_goal[1]);
		cmdv_pub_cf1.publish(cf_cmdv[1]);
		commandFlag[1] = false;
	}
	if (commandFlag[2]){
		pose_pub_cf2.publish(cf_goal[2]);
		cmdv_pub_cf2.publish(cf_cmdv[2]);
		commandFlag[2] = false;
	}
	if (commandFlag[3]){
		pose_pub_cf3.publish(cf_goal[3]);
		cmdv_pub_cf3.publish(cf_cmdv[3]);
		commandFlag[3] = false;
	}
	if (commandFlag[4]){
		pose_pub_cf4.publish(cf_goal[4]);
		cmdv_pub_cf4.publish(cf_cmdv[4]);
		commandFlag[4] = false;
	}
	if (commandFlag[5]){
		pose_pub_cf5.publish(cf_goal[5]);
		cmdv_pub_cf5.publish(cf_cmdv[5]);
		commandFlag[5] = false;
	}
}

void QNode::takeoff(int id){
	cf_goal[id].header.stamp = ros::Time::now();
	cf_goal[id].pose.position.x = cf_mocap[id].position[0];
	cf_goal[id].pose.position.y = cf_mocap[id].position[1];
	cf_goal[id].pose.position.z = 0.5;
	commandFlag[id] = true;
	switch (id){
	case 0:
		takeoff_cf0.call(serv_empty);
		break;
	case 1:
		takeoff_cf1.call(serv_empty);
		break;
	case 2:
		takeoff_cf2.call(serv_empty);
		break;
	case 3:
		takeoff_cf3.call(serv_empty);
		break;
	case 4:
		takeoff_cf4.call(serv_empty);
		break;
	case 5:
		takeoff_cf5.call(serv_empty);
		break;
	}
}
void QNode::land(int id){
	switch (id){
	case 0:
		land_cf0.call(serv_empty);
		break;
	case 1:
		land_cf1.call(serv_empty);
		break;
	case 2:
		land_cf2.call(serv_empty);
		break;
	case 3:
		land_cf3.call(serv_empty);
		break;
	case 4:
		land_cf4.call(serv_empty);
		break;
	case 5:
		land_cf5.call(serv_empty);
		break;
	}
}

void QNode::move_cf(int id, float target[3]){
	cf_goal[id].header.stamp = ros::Time::now();
	cf_goal[id].pose.position.x = target[0];
	cf_goal[id].pose.position.y = target[1];
	cf_goal[id].pose.position.z = target[2];
	commandFlag[id] = true;
}

cf_gs::signalRec QNode::Update_cf_signal(int id){
	return cf_Recieved[id];
}

PoseStamped QNode::GetDesPos(int id){
	return cf_goal[id];
}

Mocap QNode::GetMocap(int id){
	return cf_mocap[id];
}


