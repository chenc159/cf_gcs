/**
 * @file /include/cf_gs/main_window.hpp
 *
 * @brief Qt based gui for cf_gs.
 *
 * @date November 2010
 **/
#ifndef cf_gs_MAIN_WINDOW_H
#define cf_gs_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QMainWindow>
#include <QSettings>
#include "ui_main_window.h"
#include "qnode.hpp"

/*****************************************************************************
** Namespace
*****************************************************************************/

namespace cf_gs {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();

	void ReadSettings(); // Load up qt program settings at startup
	void WriteSettings(); // Save qt program settings when closing

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();

public Q_SLOTS:
	/******************************************
	** Auto-connections (connectSlotsByName())
	*******************************************/

	void on_allcf_takoff_clicked(bool check);
	void on_allcf_land_clicked(bool check);

	void on_cf0_Button_Takeoff_clicked(bool check);
	void on_cf0_Button_Land_clicked(bool check);
	void on_cf0_Button_Set_clicked(bool check);
	void on_cf0_Button_Get_clicked(bool check);

	void on_cf1_Button_Takeoff_clicked(bool check);
	void on_cf1_Button_Land_clicked(bool check);
	void on_cf1_Button_Set_clicked(bool check);
	void on_cf1_Button_Get_clicked(bool check);

	void on_cf2_Button_Takeoff_clicked(bool check);
	void on_cf2_Button_Land_clicked(bool check);
	void on_cf2_Button_Set_clicked(bool check);
	void on_cf2_Button_Get_clicked(bool check);

	void on_cf3_Button_Takeoff_clicked(bool check);
	void on_cf3_Button_Land_clicked(bool check);
	void on_cf3_Button_Set_clicked(bool check);
	void on_cf3_Button_Get_clicked(bool check);

	void on_cf4_Button_Takeoff_clicked(bool check);
	void on_cf4_Button_Land_clicked(bool check);
	void on_cf4_Button_Set_clicked(bool check);
	void on_cf4_Button_Get_clicked(bool check);

	void on_cf5_Button_Takeoff_clicked(bool check);
	void on_cf5_Button_Land_clicked(bool check);
	void on_cf5_Button_Set_clicked(bool check);
	void on_cf5_Button_Get_clicked(bool check);


    /******************************************
    ** Manual connections
    *******************************************/
//    void updateLoggingView(); // no idea why this can't connect automatically
	void updatecf0();
	void updatecf1();
	void updatecf2();
	void updatecf3();
	void updatecf4();
	void updatecf5();

Q_SIGNALS:
    // void send_Takeoffcf0();


private:
	int DroneNumber = 6;

	Ui::MainWindowDesign ui;
	QNode qnode;
};

}  // namespace cf_gs

#endif // cf_gs_MAIN_WINDOW_H
