/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/cf_gs/main_window.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

using namespace cf_gs;
using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char **argv, QWidget *parent)
    : QMainWindow(parent), qnode(argc, argv)
{
    ui.setupUi(this);                                                                    // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt())); // qApp is a global variable for the application

    ReadSettings();
    setWindowIcon(QIcon(":/images/icon.png"));
    ui.tab_manager->setCurrentIndex(0); // ensure the first tab is showing - qt-designer should have this already hardwired, but often loses it (settings?).
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));
    /*------------init ros node -----------*/
    bool init_ros_ok = qnode.init();
    if (!init_ros_ok)
    {
        showNoMasterMessage();
    }
    else
    {
        //ui.button_connect->setEnabled(false);
    }

    /*********************
	** Logging
	**********************/
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf0mocap()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf0despos()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf0signal()));


    /*********************
    ** Auto Start
    **********************/
}

MainWindow::~MainWindow() {}

/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage()
{
    QMessageBox msgBox;
    msgBox.setText("Couldn't find the ros master.");
    msgBox.exec();
    close();
}

/*
 * These triggers whenever the button is clicked, regardless of whether it
 * is already checked or not.
 */

void MainWindow::on_cf0_Button_Takeoff_clicked(bool check)
{
    qnode.takeoff();
}

void MainWindow::on_cf0_Button_Land_clicked(bool check)
{
    qnode.land();
}

////////////////////////// Update signals

void MainWindow::updatecf0mocap()
{
    cf_gs::Mocap temp_mocap = qnode.GetMocap();
    ui.cf0_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf0_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf0_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

    // ui.UAV0_vx->setText(QString::number(temp_mocap.velocity[0], 'f', 2));
    // ui.UAV0_vy->setText(QString::number(temp_mocap.velocity[1], 'f', 2));
    // ui.UAV0_vz->setText(QString::number(temp_mocap.velocity[2], 'f', 2));
}

void MainWindow::updatecf0despos()
{
    geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos();
    ui.cf0_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
    ui.cf0_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
    ui.cf0_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));
}

void MainWindow::updatecf0signal()
{
    cf_gs::signalRec signal = qnode.Update_cf0_signal();

    if (signal.mocapReceived)
    {
        ui.cf0_mocap->setText("<font color='green'>cf0</font>");
    }
    else
    {
        ui.cf0_mocap->setText("<font color='red'>cf0</font>");
    }

    if (signal.rosReceived)
    {
        ui.cf0_rosc->setText("<font color='green'>cf0</font>");
    }
    else
    {
        ui.cf0_rosc->setText("<font color='red'>cf0</font>");
    }

    if (signal.Isconnected)
    {
        ui.cf0_con->setText("<font color='green'>cf0</font>");
        ui.cf0_Button_Set->setEnabled(true);
        ui.cf0_Button_Takeoff->setEnabled(true);
        ui.cf0_Button_Land->setEnabled(true);
    }
    else
    {
        ui.cf0_con->setText("<font color='red'>cf0</font>");
        ui.cf0_Button_Set->setEnabled(false);
        ui.cf0_Button_Takeoff->setEnabled(false);
        ui.cf0_Button_Land->setEnabled(false);
    }
}

/*****************************************************************************
** Implemenation [Slots][manually connected]
*****************************************************************************/

/**
 * This function is signalled by the underlying model. When the model changes,
 * this will drop the cursor down to the last line in the QListview to ensure
 * the user can always see the latest log message.
 */
// void MainWindow::updateLoggingView() {
//         ui.view_logging->scrollToBottom();
// }

/*****************************************************************************
** Implementation [Menu]
*****************************************************************************/

// void MainWindow::on_actionAbout_triggered() {
//     QMessageBox::about(this, tr("About ..."),tr("<h2>PACKAGE_NAME Test Program 0.10</h2><p>Copyright Yujin Robot</p><p>This package needs an about description.</p>"));
// }

/*****************************************************************************
** Implementation [Configuration]
*****************************************************************************/

void MainWindow::ReadSettings()
{
    QSettings settings("Qt-Ros Package", "cf_gs");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    // QString master_url = settings.value("master_url",QString("http://192.168.1.2:11311/")).toString();
    // QString host_url = settings.value("host_url", QString("192.168.1.3")).toString();
    // //QString topic_name = settings.value("topic_name", QString("/chatter")).toString();
    // ui.line_edit_master->setText(master_url);
    // ui.line_edit_host->setText(host_url);
    // //ui.line_edit_topic->setText(topic_name);
    // bool remember = settings.value("remember_settings", false).toBool();
    // ui.checkbox_remember_settings->setChecked(remember);
    // bool checked = settings.value("use_environment_variables", false).toBool();
    // ui.checkbox_use_environment->setChecked(checked);
    // if ( checked ) {
    // 	ui.line_edit_master->setEnabled(false);
    // 	ui.line_edit_host->setEnabled(false);
    // 	//ui.line_edit_topic->setEnabled(false);
    // }
}

void MainWindow::WriteSettings()
{
    QSettings settings("Qt-Ros Package", "cf_gs");
    // settings.setValue("master_url",ui.line_edit_master->text());
    // settings.setValue("host_url",ui.line_edit_host->text());
    //settings.setValue("topic_name",ui.line_edit_topic->text());
    // settings.setValue("use_environment_variables",QVariant(ui.checkbox_use_environment->isChecked()));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    // settings.setValue("remember_settings",QVariant(ui.checkbox_remember_settings->isChecked()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    WriteSettings();
    QMainWindow::closeEvent(event);
}
