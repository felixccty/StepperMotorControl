#pragma once

#include <QtWidgets/QWidget>
#include "ui_motor_gui.h"

#include"ui_closeBox.h"
#include "vsmdlib.h"
#include "stdafx.h"
#include <QtGui/QCloseEvent>
#include <QSystemTrayIcon>
class motor_gui : public QWidget
{
	Q_OBJECT

public:
	motor_gui(QWidget *parent = Q_NULLPTR);
	QString perst="";
	QString process = "";
	QString persent_str = "";
	QThread* thread1;
	QString ico_path = "";
	bool running = false;
	void sleep_timer(int ms);
	QSystemTrayIcon* trayIcon;
	Vsmd* vsmd;
	VsmdDevice* devices[1];
	float acc_cur = 2.2;
	float run_cur = 2.2;
	float ava_cur = 2.2;
	bool check_open = false;


public slots:
	void time_counter();
	void scan();
	void search_north();
	void stop_motor();
	void left_move();
	void right_move();
	void display_bar(int bar_process);
	void run_thread();
	void closeEvent(QCloseEvent *event);
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void read_voltage();
	void load_speed();
	void write_speed();
	

	float gear_ratio_switch(int gear_ratio_int);
private:
	Ui::motor_guiClass ui;
	Ui::Dialog box;
};

class thread_obj : public QObject
{
	Q_OBJECT
private slots:
	void process_bar_move();
	
signals:
	void bar_signal(int);

};
