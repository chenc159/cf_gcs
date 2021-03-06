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
	// commandFlag = true;
	commandFlag[0] = false;
	// commandFlag[1] = false;
	// commandFlag[2] = false;
	// commandPayloadFlag = false;
	DroneNumber = 3;
	// mocapReceived[0] = false;
	// rosReceived[0] = false;
	// Isconnected[0] = false;
	// ispayloaddetected = false;
	// ispayloadmocaprecieved = false;
	// ispayloadcontrolactivated = false;
	// comid = 1;
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

// bool QNode::init()
// {
// 	// ros::init(init_argc, init_argv, "cf_gs");
// 	// if (!ros::master::check()){
// 	// 	return false;
// 	// }
// 	ros::start(); // explicitly needed since our nodehandle is going out of scope.
// 	ros::NodeHandle n;
// 	// Add your ros communications here.

// 	start();
// 	return true;
// }

bool QNode::init(void)
{
	ros::init(init_argc, init_argv, "cf_gs");
	if (!ros::master::check())
	{
		return false;
	}

	// std::map<std::string, std::string> remappings;
	// remappings["__master"] = master_url;
	// remappings["__hostname"] = host_url;
	// ros::init(remappings, "cf_gs");

	ros::start(); // explicitly needed since our nodehandle is going out of scope.
	ros::NodeHandle n;

	mocap_topics = {"/mocap/cf0",
					"/mocap/cf1",
					"/mocap/cf2"};

	// cf0_pubNav = n.advertise<geometry_msgs::Twist>("cf0/cmd_vel", 1);
	// cf0_pubCmdVtemp = n.advertise<geometry_msgs::Twist>("cf0/cmdVtemp", 1);
	mocap_cf0 = n.subscribe<Mocap>("mocap/cf0", 1, &QNode::mocap_callback, this);
	goal_subs_cf0 = n.subscribe<PoseStamped>("cf0/goal", 1, &QNode::goalChanged, this);

	pose_pub_cf0 = n.advertise<PoseStamped>("cf0/goal", 100);
	// cf0_serviceTakeoff = n.advertiseService("cf0/cftakeoff", &QNode::takeoff, this);
	// m_serviceAuto = n.advertiseService(drone_ID + "/cfauto", &Controller::automatic, this);
	// m_serviceGame = n.advertiseService(drone_ID + "/cfplay", &Controller::play, this);
	// cf0_serviceLand = n.advertiseService("cf0/cfland", &QNode::land, this);

	// Add your ros communications here.
	// chatter_publisher = n.advertise<std_msgs::String>("chatter", 1000);
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
		if (cf0_Recieved.mocapReceived && cf0_Recieved.rosReceived)
		{
			cf0_Recieved.Isconnected = true;
		}
		cf0_Recieved.Isconnected = false;

		// for (int i = 0; i < DroneNumber; i++)
		// {
		// 	if (UavLogList[i].islogreceived)
		// 	{
		// 		UavLogList[i].isconnected = true;
		// 	}
		// 	else
		// 	{
		// 		UavLogList[i].isconnected = false;
		// 	}
		// 	UavLogList[i].islogreceived = false;
		// }

		// /*----------------detect whether payload mocap is published --------------------------*/
		// if (ispayloadmocaprecieved)
		// {
		// 	ispayloaddetected = true;
		// }
		// else
		// {
		// 	ispayloaddetected = false;
		// }
		// ispayloadmocaprecieved = false;

		/* signal a ros loop update  */
		Q_EMIT rosLoopUpdate();
		loop_rate.sleep();
	}
	std::cout << "Ros shutdown, proceeding to close the gui." << std::endl;
	Q_EMIT rosShutdown(); // used to signal the gui for a shutdown (useful to roslaunch)
}

void QNode::goalChanged(const geometry_msgs::PoseStamped::ConstPtr &msg)
{
	cf0_goal = *msg;
	// m_goal[0] = msg.pose.position.x;
	// m_goal[1] = msg.pose.position.y;
	// m_goal[2] = msg.pose.position.z;
	cf0_Recieved.rosReceived = true;
}

void QNode::mocap_callback(const cf_gs::Mocap::ConstPtr &msg)
{
	mocap = *msg;
	// m_dronePositionWorld[0] = *msg.position[0];
	// m_dronePositionWorld[1] = *msg.position[1];
	// m_dronePositionWorld[2] = *msg.position[2];
	// m_droneVelocityWorld[0] = *msg.velocity[0];
	// m_droneVelocityWorld[1] = *msg.velocity[1];
	// m_droneVelocityWorld[2] = *msg.velocity[2];
	cf0_Recieved.mocapReceived = true;
	//
}

void QNode::pub_command()
{
	if (commandFlag[0])
	{
		pose_pub_cf0.publish(cf0_goal);
		commandFlag[0] = false;
	}
	// if (commandFlag[1])
	// {
	//     moveUAV1.publish(Command_List[1]);
	//     commandFlag[1] = false;
	// }
	// if (commandFlag[2])
	// {
	//     moveUAV2.publish(Command_List[2]);
	//     commandFlag[2] = false;
	// }
}

void QNode::takeoff()
{
	cf0_goal.pose.position.x = mocap.position[0]; //m_dronePositionWorld[0];
	cf0_goal.pose.position.y = mocap.position[1]; //m_dronePositionWorld[1];
	cf0_goal.pose.position.z = 0.5;
	commandFlag[0] = true;
}
void QNode::land()
{
	cf0_goal.pose.position.x = mocap.position[0]; //m_dronePositionWorld[0];
	cf0_goal.pose.position.y = mocap.position[1]; //m_dronePositionWorld[1];
	cf0_goal.pose.position.z = 0;
	commandFlag[0] = true;
}

void QNode::move_cf(float target[3])
{
	cf0_goal.pose.position.x = target[0];
	cf0_goal.pose.position.y = target[1];
	cf0_goal.pose.position.z = target[2];
	commandFlag[0] = true;
}

cf_gs::signalRec QNode::Update_cf0_signal()
{
	return cf0_Recieved;
}

PoseStamped QNode::GetDesPos()
{
	return cf0_goal;
}

Mocap QNode::GetMocap() // int ID
{
	return mocap; //mocap[ID]
}

} // namespace cf_gs
