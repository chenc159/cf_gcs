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

#include <cf_gs/Mocap.h>

/*****************************************************************************
** Namespaces
*****************************************************************************/

using Mocap = cf_gs::Mocap;

namespace cf_gs
{

/*****************************************************************************
** Class
*****************************************************************************/

class QNode : public QThread
{
	Q_OBJECT
public:
	QNode(int argc, char **argv);
	virtual ~QNode();
	bool init();
	bool init(const std::string &master_url, const std::string &host_url);
	void run();

	/*********************
	** Logging
	**********************/
	enum LogLevel
	{
		Debug,
		Info,
		Warn,
		Error,
		Fatal
	};

	std::map<std::string, ros::Subscriber> sub_;

	QStringListModel *loggingModel() { return &logging_model; }
	void log(const LogLevel &level, const std::string &msg);

Q_SIGNALS:
	void loggingUpdated();
	void rosShutdown();

private:
	int init_argc;
	char **init_argv;
	ros::Publisher chatter_publisher;
	QStringListModel logging_model;

	std::vector<std::string> mocap_topics;
	Mocap mocap;

	void mocap_callback(const Mocap::ConstPtr &msg);
};

} // namespace cf_gs

#endif /* cf_gs_QNODE_HPP_ */
