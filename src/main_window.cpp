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

// #include <QtGui>
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
    // ui.tab_manager->setCurrentIndex(0); // ensure the first tab is showing - qt-designer should have this already hardwired, but often loses it (settings?).
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
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf0()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf1()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf2()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf3()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf4()));
    QObject::connect(&qnode, SIGNAL(rosLoopUpdate()), this, SLOT(updatecf5()));


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

/*****************************************************************************
** All cfs
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_allcf_takoff_clicked(bool check){
    for (int i = 0; i <DroneNumber; i++){
        cf_gs::signalRec signal = qnode.Update_cf_signal(i);
        if (signal.Isconnected){
            qnode.takeoff(i);
        }
    }
    ui.allcf_takoff->setEnabled(false);
    ui.allcf_land->setEnabled(true);
}

void MainWindow::on_allcf_land_clicked(bool check){
    for (int i = 0; i <DroneNumber; i++){
        cf_gs::signalRec signal = qnode.Update_cf_signal(i);
        if (signal.rosReceived){
            qnode.land(i);
        }
    }
    ui.allcf_takoff->setEnabled(true);
    ui.allcf_land->setEnabled(false);
}

/*****************************************************************************
** cf0
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_cf0_Button_Takeoff_clicked(bool check){
    qnode.takeoff(0);
    ui.cf0_Button_Takeoff->setEnabled(false);
    ui.cf0_Button_Land->setEnabled(true);
}

void MainWindow::on_cf0_Button_Land_clicked(bool check){
    qnode.land(0);
    ui.cf0_Button_Takeoff->setEnabled(true);
    ui.cf0_Button_Land->setEnabled(false);
}

void MainWindow::on_cf0_Button_Set_clicked(bool check){
    /* read values from line edit */
    float target_state[3];

    target_state[0] =  ui.cf0_x_input->text().toFloat();
    target_state[1] =  ui.cf0_y_input->text().toFloat();
    target_state[2] =  ui.cf0_z_input->text().toFloat();
    /*----------------determine whether the input is in safe range ------------------*/
    bool input_is_valid = true;

    if(target_state[0] < -1.5 || target_state[0] > 1.3) {
        input_is_valid = false;
    }

    if(target_state[1] < -1 || target_state[1] > 1) {
        input_is_valid = false;
    }

    if(target_state[2] < 0|| target_state[2] > 1.8) {
        input_is_valid = false;
    }

    /*----------------send input ------------------*/

    if(input_is_valid){
        /*  update the ENU target label */
        ui.cf0_des_x->setText(QString::number(target_state[0], 'f', 2));
        ui.cf0_des_y->setText(QString::number(target_state[1], 'f', 2));
        ui.cf0_des_z->setText(QString::number(target_state[2], 'f', 2));

        qnode.move_cf(0, target_state);

    } else {
        QMessageBox msgBox;
        msgBox.setText("Input position is out of range!!");
        msgBox.exec();
    };
}

void MainWindow::on_cf0_Button_Get_clicked(bool check){
    cf_gs::Mocap temp_mocap = qnode.GetMocap(0);
    ui.cf0_x_input->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf0_y_input->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf0_z_input->setText(QString::number(temp_mocap.position[2], 'f', 2));
}


////////////////////////// Update signals /////////////////////////

void MainWindow::updatecf0(){
    cf_gs::signalRec signal = qnode.Update_cf_signal(0);
    if (signal.Isconnected){
        cf_gs::Mocap temp_mocap = qnode.GetMocap(0);
        ui.cf0_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
        ui.cf0_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
        ui.cf0_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

        geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos(0);
        ui.cf0_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
        ui.cf0_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
        ui.cf0_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));

        ui.cf0_con->setText("<font color='green'>cf0</font>");
        ui.cf0_Button_Set->setEnabled(true);
        ui.cf0_Button_Takeoff->setEnabled(true);
        ui.cf0_Button_Land->setEnabled(true);
        ui.cf0_title->setPalette(Qt::green);
        ui.cf0_title->setStyleSheet("color: green;");

        ui.cf0_mocap->setText("<font color='green'>cf0</font>");
        ui.cf0_rosc->setText("<font color='green'>cf0</font>");

    } else {
        ui.cf0_x->setText("<font color='red'>---</font>");
        ui.cf0_y->setText("<font color='red'>---</font>");
        ui.cf0_z->setText("<font color='red'>---</font>");

        ui.cf0_des_x->setText("<font color='red'>---</font>");
        ui.cf0_des_y->setText("<font color='red'>---</font>");
        ui.cf0_des_z->setText("<font color='red'>---</font>");

        ui.cf0_con->setText("<font color='red'>cf0</font>");
        ui.cf0_Button_Set->setEnabled(false);
        ui.cf0_Button_Get->setEnabled(false);
        ui.cf0_Button_Takeoff->setEnabled(false);
        ui.cf0_Button_Land->setEnabled(false);
        ui.cf0_title->setPalette(Qt::red);
        ui.cf0_title->setStyleSheet("color: red;");

        ui.cf0_x_input->setText("N/A");
        ui.cf0_y_input->setText("N/A");
        ui.cf0_z_input->setText("N/A");
        ui.cf0_x_input->setReadOnly(true);
        ui.cf0_y_input->setReadOnly(true);
        ui.cf0_z_input->setReadOnly(true);
    
        if (!signal.mocapReceived){
            ui.cf0_mocap->setText("<font color='red'>cf0</font>");
        } 
        if (!signal.rosReceived) {
            ui.cf0_rosc->setText("<font color='red'>cf0</font>");
        }
    }
}

/*****************************************************************************
** cf1
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_cf1_Button_Takeoff_clicked(bool check){
    qnode.takeoff(1);
    ui.cf1_Button_Takeoff->setEnabled(false);
    ui.cf1_Button_Land->setEnabled(true);
}

void MainWindow::on_cf1_Button_Land_clicked(bool check){
    qnode.land(1);
    ui.cf1_Button_Takeoff->setEnabled(true);
    ui.cf1_Button_Land->setEnabled(false);
}

void MainWindow::on_cf1_Button_Set_clicked(bool check){
    /* read values from line edit */
    float target_state[3];

    target_state[0] =  ui.cf1_x_input->text().toFloat();
    target_state[1] =  ui.cf1_y_input->text().toFloat();
    target_state[2] =  ui.cf1_z_input->text().toFloat();
    /*----------------determine whether the input is in safe range ------------------*/
    bool input_is_valid = true;

    if(target_state[0] < -1.5 || target_state[0] > 1.3) {
        input_is_valid = false;
    }

    if(target_state[1] < -1 || target_state[1] > 1) {
        input_is_valid = false;
    }

    if(target_state[2] < 0|| target_state[2] > 1.8) {
        input_is_valid = false;
    }

    /*----------------send input ------------------*/

    if(input_is_valid){
        /*  update the ENU target label */
        ui.cf1_des_x->setText(QString::number(target_state[0], 'f', 2));
        ui.cf1_des_y->setText(QString::number(target_state[1], 'f', 2));
        ui.cf1_des_z->setText(QString::number(target_state[2], 'f', 2));

        qnode.move_cf(1, target_state);

    } else {
        QMessageBox msgBox;
        msgBox.setText("Input position is out of range!!");
        msgBox.exec();
    };
}

void MainWindow::on_cf1_Button_Get_clicked(bool check){
    cf_gs::Mocap temp_mocap = qnode.GetMocap(1);
    ui.cf1_x_input->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf1_y_input->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf1_z_input->setText(QString::number(temp_mocap.position[2], 'f', 2));
}


////////////////////////// Update signals /////////////////////////

void MainWindow::updatecf1(){
    cf_gs::signalRec signal = qnode.Update_cf_signal(1);
    if (signal.Isconnected){
        cf_gs::Mocap temp_mocap = qnode.GetMocap(1);
        ui.cf1_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
        ui.cf1_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
        ui.cf1_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

        geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos(1);
        ui.cf1_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
        ui.cf1_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
        ui.cf1_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));

        ui.cf1_con->setText("<font color='green'>cf1</font>");
        ui.cf1_Button_Set->setEnabled(true);
        ui.cf1_Button_Takeoff->setEnabled(true);
        ui.cf1_Button_Land->setEnabled(true);
        ui.cf1_title->setPalette(Qt::green);
        ui.cf1_title->setStyleSheet("color: green;");

        ui.cf1_mocap->setText("<font color='green'>cf1</font>");
        ui.cf1_rosc->setText("<font color='green'>cf1</font>");

    } else {
        ui.cf1_x->setText("<font color='red'>---</font>");
        ui.cf1_y->setText("<font color='red'>---</font>");
        ui.cf1_z->setText("<font color='red'>---</font>");

        ui.cf1_des_x->setText("<font color='red'>---</font>");
        ui.cf1_des_y->setText("<font color='red'>---</font>");
        ui.cf1_des_z->setText("<font color='red'>---</font>");

        ui.cf1_con->setText("<font color='red'>cf1</font>");
        ui.cf1_Button_Set->setEnabled(false);
        ui.cf1_Button_Get->setEnabled(false);
        ui.cf1_Button_Takeoff->setEnabled(false);
        ui.cf1_Button_Land->setEnabled(false);
        ui.cf1_title->setPalette(Qt::red);
        ui.cf1_title->setStyleSheet("color: red;");

        ui.cf1_x_input->setText("N/A");
        ui.cf1_y_input->setText("N/A");
        ui.cf1_z_input->setText("N/A");
        ui.cf1_x_input->setReadOnly(true);
        ui.cf1_y_input->setReadOnly(true);
        ui.cf1_z_input->setReadOnly(true);
    
        if (!signal.mocapReceived){
            ui.cf1_mocap->setText("<font color='red'>cf1</font>");
        } 
        if (!signal.rosReceived) {
            ui.cf1_rosc->setText("<font color='red'>cf1</font>");
        }
    }
}

/*****************************************************************************
** cf2
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_cf2_Button_Takeoff_clicked(bool check){
    qnode.takeoff(2);
    ui.cf2_Button_Takeoff->setEnabled(false);
    ui.cf2_Button_Land->setEnabled(true);
}

void MainWindow::on_cf2_Button_Land_clicked(bool check){
    qnode.land(2);
    ui.cf2_Button_Takeoff->setEnabled(true);
    ui.cf2_Button_Land->setEnabled(false);
}

void MainWindow::on_cf2_Button_Set_clicked(bool check){
    /* read values from line edit */
    float target_state[3];

    target_state[0] =  ui.cf2_x_input->text().toFloat();
    target_state[1] =  ui.cf2_y_input->text().toFloat();
    target_state[2] =  ui.cf2_z_input->text().toFloat();
    /*----------------determine whether the input is in safe range ------------------*/
    bool input_is_valid = true;

    if(target_state[0] < -1.5 || target_state[0] > 1.3) {
        input_is_valid = false;
    }

    if(target_state[1] < -1 || target_state[1] > 1) {
        input_is_valid = false;
    }

    if(target_state[2] < 0|| target_state[2] > 1.8) {
        input_is_valid = false;
    }

    /*----------------send input ------------------*/

    if(input_is_valid){
        /*  update the ENU target label */
        ui.cf2_des_x->setText(QString::number(target_state[0], 'f', 2));
        ui.cf2_des_y->setText(QString::number(target_state[1], 'f', 2));
        ui.cf2_des_z->setText(QString::number(target_state[2], 'f', 2));

        qnode.move_cf(2, target_state);

    } else {
        QMessageBox msgBox;
        msgBox.setText("Input position is out of range!!");
        msgBox.exec();
    };
}

void MainWindow::on_cf2_Button_Get_clicked(bool check){
    cf_gs::Mocap temp_mocap = qnode.GetMocap(2);
    ui.cf2_x_input->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf2_y_input->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf2_z_input->setText(QString::number(temp_mocap.position[2], 'f', 2));
}


////////////////////////// Update signals /////////////////////////

void MainWindow::updatecf2(){
    cf_gs::signalRec signal = qnode.Update_cf_signal(2);
    if (signal.Isconnected){
        cf_gs::Mocap temp_mocap = qnode.GetMocap(2);
        ui.cf2_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
        ui.cf2_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
        ui.cf2_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

        geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos(2);
        ui.cf2_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
        ui.cf2_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
        ui.cf2_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));

        ui.cf2_con->setText("<font color='green'>cf2</font>");
        ui.cf2_Button_Set->setEnabled(true);
        ui.cf2_Button_Takeoff->setEnabled(true);
        ui.cf2_Button_Land->setEnabled(true);
        ui.cf2_title->setPalette(Qt::green);
        ui.cf2_title->setStyleSheet("color: green;");

        ui.cf2_mocap->setText("<font color='green'>cf2</font>");
        ui.cf2_rosc->setText("<font color='green'>cf2</font>");

    } else {
        ui.cf2_x->setText("<font color='red'>---</font>");
        ui.cf2_y->setText("<font color='red'>---</font>");
        ui.cf2_z->setText("<font color='red'>---</font>");

        ui.cf2_des_x->setText("<font color='red'>---</font>");
        ui.cf2_des_y->setText("<font color='red'>---</font>");
        ui.cf2_des_z->setText("<font color='red'>---</font>");

        ui.cf2_con->setText("<font color='red'>cf2</font>");
        ui.cf2_Button_Set->setEnabled(false);
        ui.cf2_Button_Get->setEnabled(false);
        ui.cf2_Button_Takeoff->setEnabled(false);
        ui.cf2_Button_Land->setEnabled(false);
        ui.cf2_title->setPalette(Qt::red);
        ui.cf2_title->setStyleSheet("color: red;");

        ui.cf2_x_input->setText("N/A");
        ui.cf2_y_input->setText("N/A");
        ui.cf2_z_input->setText("N/A");
        ui.cf2_x_input->setReadOnly(true);
        ui.cf2_y_input->setReadOnly(true);
        ui.cf2_z_input->setReadOnly(true);
    
        if (!signal.mocapReceived){
            ui.cf2_mocap->setText("<font color='red'>cf2</font>");
        } 
        if (!signal.rosReceived) {
            ui.cf2_rosc->setText("<font color='red'>cf2</font>");
        }
    }
}


/*****************************************************************************
** cf3
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_cf3_Button_Takeoff_clicked(bool check){
    qnode.takeoff(3);
    ui.cf3_Button_Takeoff->setEnabled(false);
    ui.cf3_Button_Land->setEnabled(true);
}

void MainWindow::on_cf3_Button_Land_clicked(bool check){
    qnode.land(3);
    ui.cf3_Button_Takeoff->setEnabled(true);
    ui.cf3_Button_Land->setEnabled(false);
}

void MainWindow::on_cf3_Button_Set_clicked(bool check){
    /* read values from line edit */
    float target_state[3];

    target_state[0] =  ui.cf3_x_input->text().toFloat();
    target_state[1] =  ui.cf3_y_input->text().toFloat();
    target_state[2] =  ui.cf3_z_input->text().toFloat();
    /*----------------determine whether the input is in safe range ------------------*/
    bool input_is_valid = true;

    if(target_state[0] < -1.5 || target_state[0] > 1.3) {
        input_is_valid = false;
    }

    if(target_state[1] < -1 || target_state[1] > 1) {
        input_is_valid = false;
    }

    if(target_state[2] < 0|| target_state[2] > 1.8) {
        input_is_valid = false;
    }

    /*----------------send input ------------------*/

    if(input_is_valid){
        /*  update the ENU target label */
        ui.cf3_des_x->setText(QString::number(target_state[0], 'f', 2));
        ui.cf3_des_y->setText(QString::number(target_state[1], 'f', 2));
        ui.cf3_des_z->setText(QString::number(target_state[2], 'f', 2));

        qnode.move_cf(3, target_state);

    } else {
        QMessageBox msgBox;
        msgBox.setText("Input position is out of range!!");
        msgBox.exec();
    };
}

void MainWindow::on_cf3_Button_Get_clicked(bool check){
    cf_gs::Mocap temp_mocap = qnode.GetMocap(3);
    ui.cf3_x_input->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf3_y_input->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf3_z_input->setText(QString::number(temp_mocap.position[2], 'f', 2));
}


////////////////////////// Update signals /////////////////////////

void MainWindow::updatecf3(){
    cf_gs::signalRec signal = qnode.Update_cf_signal(3);
    if (signal.Isconnected){
        cf_gs::Mocap temp_mocap = qnode.GetMocap(3);
        ui.cf3_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
        ui.cf3_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
        ui.cf3_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

        geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos(3);
        ui.cf3_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
        ui.cf3_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
        ui.cf3_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));

        ui.cf3_con->setText("<font color='green'>cf3</font>");
        ui.cf3_Button_Set->setEnabled(true);
        ui.cf3_Button_Takeoff->setEnabled(true);
        ui.cf3_Button_Land->setEnabled(true);
        ui.cf3_title->setPalette(Qt::green);
        ui.cf3_title->setStyleSheet("color: green;");

        ui.cf3_mocap->setText("<font color='green'>cf3</font>");
        ui.cf3_rosc->setText("<font color='green'>cf3</font>");

    } else {
        ui.cf3_x->setText("<font color='red'>---</font>");
        ui.cf3_y->setText("<font color='red'>---</font>");
        ui.cf3_z->setText("<font color='red'>---</font>");

        ui.cf3_des_x->setText("<font color='red'>---</font>");
        ui.cf3_des_y->setText("<font color='red'>---</font>");
        ui.cf3_des_z->setText("<font color='red'>---</font>");

        ui.cf3_con->setText("<font color='red'>cf3</font>");
        ui.cf3_Button_Set->setEnabled(false);
        ui.cf3_Button_Get->setEnabled(false);
        ui.cf3_Button_Takeoff->setEnabled(false);
        ui.cf3_Button_Land->setEnabled(false);
        ui.cf3_title->setPalette(Qt::red);
        ui.cf3_title->setStyleSheet("color: red;");

        ui.cf3_x_input->setText("N/A");
        ui.cf3_y_input->setText("N/A");
        ui.cf3_z_input->setText("N/A");
        ui.cf3_x_input->setReadOnly(true);
        ui.cf3_y_input->setReadOnly(true);
        ui.cf3_z_input->setReadOnly(true);
    
        if (!signal.mocapReceived){
            ui.cf3_mocap->setText("<font color='red'>cf3</font>");
        } 
        if (!signal.rosReceived) {
            ui.cf3_rosc->setText("<font color='red'>cf3</font>");
        }
    }
}

/*****************************************************************************
** cf4
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_cf4_Button_Takeoff_clicked(bool check){
    qnode.takeoff(4);
    ui.cf4_Button_Takeoff->setEnabled(false);
    ui.cf4_Button_Land->setEnabled(true);
}

void MainWindow::on_cf4_Button_Land_clicked(bool check){
    qnode.land(4);
    ui.cf4_Button_Takeoff->setEnabled(true);
    ui.cf4_Button_Land->setEnabled(false);
}

void MainWindow::on_cf4_Button_Set_clicked(bool check){
    /* read values from line edit */
    float target_state[3];

    target_state[0] =  ui.cf4_x_input->text().toFloat();
    target_state[1] =  ui.cf4_y_input->text().toFloat();
    target_state[2] =  ui.cf4_z_input->text().toFloat();
    /*----------------determine whether the input is in safe range ------------------*/
    bool input_is_valid = true;

    if(target_state[0] < -1.5 || target_state[0] > 1.3) {
        input_is_valid = false;
    }

    if(target_state[1] < -1 || target_state[1] > 1) {
        input_is_valid = false;
    }

    if(target_state[2] < 0|| target_state[2] > 1.8) {
        input_is_valid = false;
    }

    /*----------------send input ------------------*/

    if(input_is_valid){
        /*  update the ENU target label */
        ui.cf4_des_x->setText(QString::number(target_state[0], 'f', 2));
        ui.cf4_des_y->setText(QString::number(target_state[1], 'f', 2));
        ui.cf4_des_z->setText(QString::number(target_state[2], 'f', 2));

        qnode.move_cf(4, target_state);

    } else {
        QMessageBox msgBox;
        msgBox.setText("Input position is out of range!!");
        msgBox.exec();
    };
}

void MainWindow::on_cf4_Button_Get_clicked(bool check){
    cf_gs::Mocap temp_mocap = qnode.GetMocap(4);
    ui.cf4_x_input->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf4_y_input->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf4_z_input->setText(QString::number(temp_mocap.position[2], 'f', 2));
}


////////////////////////// Update signals /////////////////////////

void MainWindow::updatecf4(){
    cf_gs::signalRec signal = qnode.Update_cf_signal(4);
    if (signal.Isconnected){
        cf_gs::Mocap temp_mocap = qnode.GetMocap(4);
        ui.cf4_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
        ui.cf4_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
        ui.cf4_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

        geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos(4);
        ui.cf4_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
        ui.cf4_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
        ui.cf4_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));

        ui.cf4_con->setText("<font color='green'>cf4</font>");
        ui.cf4_Button_Set->setEnabled(true);
        ui.cf4_Button_Takeoff->setEnabled(true);
        ui.cf4_Button_Land->setEnabled(true);
        ui.cf4_title->setPalette(Qt::green);
        ui.cf4_title->setStyleSheet("color: green;");

        ui.cf4_mocap->setText("<font color='green'>cf4</font>");
        ui.cf4_rosc->setText("<font color='green'>cf4</font>");

    } else {
        ui.cf4_x->setText("<font color='red'>---</font>");
        ui.cf4_y->setText("<font color='red'>---</font>");
        ui.cf4_z->setText("<font color='red'>---</font>");

        ui.cf4_des_x->setText("<font color='red'>---</font>");
        ui.cf4_des_y->setText("<font color='red'>---</font>");
        ui.cf4_des_z->setText("<font color='red'>---</font>");

        ui.cf4_con->setText("<font color='red'>cf4</font>");
        ui.cf4_Button_Set->setEnabled(false);
        ui.cf4_Button_Get->setEnabled(false);
        ui.cf4_Button_Takeoff->setEnabled(false);
        ui.cf4_Button_Land->setEnabled(false);
        ui.cf4_title->setPalette(Qt::red);
        ui.cf4_title->setStyleSheet("color: red;");

        ui.cf4_x_input->setText("N/A");
        ui.cf4_y_input->setText("N/A");
        ui.cf4_z_input->setText("N/A");
        ui.cf4_x_input->setReadOnly(true);
        ui.cf4_y_input->setReadOnly(true);
        ui.cf4_z_input->setReadOnly(true);
    
        if (!signal.mocapReceived){
            ui.cf4_mocap->setText("<font color='red'>cf4</font>");
        } 
        if (!signal.rosReceived) {
            ui.cf4_rosc->setText("<font color='red'>cf4</font>");
        }
    }
}

/*****************************************************************************
** cf5
*****************************************************************************/

////////////////////////// Buttons /////////////////////////

void MainWindow::on_cf5_Button_Takeoff_clicked(bool check){
    qnode.takeoff(5);
    ui.cf5_Button_Takeoff->setEnabled(false);
    ui.cf5_Button_Land->setEnabled(true);
}

void MainWindow::on_cf5_Button_Land_clicked(bool check){
    qnode.land(5);
    ui.cf5_Button_Takeoff->setEnabled(true);
    ui.cf5_Button_Land->setEnabled(false);
}

void MainWindow::on_cf5_Button_Set_clicked(bool check){
    /* read values from line edit */
    float target_state[3];

    target_state[0] =  ui.cf5_x_input->text().toFloat();
    target_state[1] =  ui.cf5_y_input->text().toFloat();
    target_state[2] =  ui.cf5_z_input->text().toFloat();
    /*----------------determine whether the input is in safe range ------------------*/
    bool input_is_valid = true;

    if(target_state[0] < -1.5 || target_state[0] > 1.3) {
        input_is_valid = false;
    }

    if(target_state[1] < -1 || target_state[1] > 1) {
        input_is_valid = false;
    }

    if(target_state[2] < 0|| target_state[2] > 1.8) {
        input_is_valid = false;
    }

    /*----------------send input ------------------*/

    if(input_is_valid){
        /*  update the ENU target label */
        ui.cf5_des_x->setText(QString::number(target_state[0], 'f', 2));
        ui.cf5_des_y->setText(QString::number(target_state[1], 'f', 2));
        ui.cf5_des_z->setText(QString::number(target_state[2], 'f', 2));

        qnode.move_cf(5, target_state);

    } else {
        QMessageBox msgBox;
        msgBox.setText("Input position is out of range!!");
        msgBox.exec();
    };
}

void MainWindow::on_cf5_Button_Get_clicked(bool check){
    cf_gs::Mocap temp_mocap = qnode.GetMocap(5);
    ui.cf5_x_input->setText(QString::number(temp_mocap.position[0], 'f', 2));
    ui.cf5_y_input->setText(QString::number(temp_mocap.position[1], 'f', 2));
    ui.cf5_z_input->setText(QString::number(temp_mocap.position[2], 'f', 2));
}


////////////////////////// Update signals /////////////////////////

void MainWindow::updatecf5(){
    cf_gs::signalRec signal = qnode.Update_cf_signal(5);
    if (signal.Isconnected){
        cf_gs::Mocap temp_mocap = qnode.GetMocap(5);
        ui.cf5_x->setText(QString::number(temp_mocap.position[0], 'f', 2));
        ui.cf5_y->setText(QString::number(temp_mocap.position[1], 'f', 2));
        ui.cf5_z->setText(QString::number(temp_mocap.position[2], 'f', 2));

        geometry_msgs::PoseStamped temp_despos = qnode.GetDesPos(5);
        ui.cf5_des_x->setText(QString::number(temp_despos.pose.position.x, 'f', 2));
        ui.cf5_des_y->setText(QString::number(temp_despos.pose.position.y, 'f', 2));
        ui.cf5_des_z->setText(QString::number(temp_despos.pose.position.z, 'f', 2));

        ui.cf5_con->setText("<font color='green'>cf5</font>");
        ui.cf5_Button_Set->setEnabled(true);
        ui.cf5_Button_Takeoff->setEnabled(true);
        ui.cf5_Button_Land->setEnabled(true);
        ui.cf5_title->setPalette(Qt::green);
        ui.cf5_title->setStyleSheet("color: green;");

        ui.cf5_mocap->setText("<font color='green'>cf5</font>");
        ui.cf5_rosc->setText("<font color='green'>cf5</font>");

    } else {
        ui.cf5_x->setText("<font color='red'>---</font>");
        ui.cf5_y->setText("<font color='red'>---</font>");
        ui.cf5_z->setText("<font color='red'>---</font>");

        ui.cf5_des_x->setText("<font color='red'>---</font>");
        ui.cf5_des_y->setText("<font color='red'>---</font>");
        ui.cf5_des_z->setText("<font color='red'>---</font>");

        ui.cf5_con->setText("<font color='red'>cf5</font>");
        ui.cf5_Button_Set->setEnabled(false);
        ui.cf5_Button_Get->setEnabled(false);
        ui.cf5_Button_Takeoff->setEnabled(false);
        ui.cf5_Button_Land->setEnabled(false);
        ui.cf5_title->setPalette(Qt::red);
        ui.cf5_title->setStyleSheet("color: red;");

        ui.cf5_x_input->setText("N/A");
        ui.cf5_y_input->setText("N/A");
        ui.cf5_z_input->setText("N/A");
        ui.cf5_x_input->setReadOnly(true);
        ui.cf5_y_input->setReadOnly(true);
        ui.cf5_z_input->setReadOnly(true);
    
        if (!signal.mocapReceived){
            ui.cf5_mocap->setText("<font color='red'>cf5</font>");
        } 
        if (!signal.rosReceived) {
            ui.cf5_rosc->setText("<font color='red'>cf5</font>");
        }
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
