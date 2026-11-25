#include "motor_gui.h"
#include "motor_gui_ui_theme.h"

#include "stdafx.h"
#include "windows.h"
#include <cstdlib>
#include <string>
#include <list>
#include <direct.h>
#include <iostream>  
#include <cstring>
#include <QThread>
using namespace std;
#include "vsmdlib.h"
#pragma comment(lib, "vsmdlib.lib")
#include <conio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <QSystemTrayIcon>
#include <QAction>
#include <Qmenu>
#include<QSettings>
#include <QTextCodec>
static HANDLE s_hEnventAquire = INVALID_HANDLE_VALUE;
LPCTSTR lpPath = L"D://motor_control.ini";
QString setting_Path_str = "D://motor_control.ini";
#ifdef WIN32

#pragma execution_character_set("utf-8")

#endif

bool stop = false;
bool stop_scan = false;
bool close_window = true;
bool only_search_north = true;
bool search_N_stop = false;
static int run_speed = 100000;//???????120000?????100000

int noraml_speed= 400000 * 10;//???????120000 ??1200000??
int low_speed = 120000 * 10;
int fast_speed = 120000 * 10;
static int accelerate = 120000; //???????120000
int sleep_15 = 960000 * 5;
float scan_time = 3.0;
float gear_ratio_float = -6.0;
bool start_process_bar = false;
#include <windows.h>
typedef enum enLogLevel {
	LOG_DEBUG = 1,
	LOG_INFO,
	LOG_ERROR,
	LOG_WARN,
	LOG_FATAL
}LogLevel;
const char LogTip[][8] = { "","Debug","Info","Error","Warn","Fatal" };
void getLogFileName(string &logFileName)
{

	time_t t = time(0);
	char tmp[32] = { NULL };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d", localtime(&t));
	strcat(tmp, ".log");

	string folderPath = "D:/LogFile";

	if (0 != folderPath.c_str())
	{
		mkdir(folderPath.c_str());   // ???? 0 ????????????-1 ??????
	}

	char path[50] = "D:/LogFile/";
	strcat(path, tmp);
	logFileName = path;
}
void writeLog(int logLevel, const char *pLogFormat)
{
	string logFileName;
	getLogFileName(logFileName);
	std::ofstream    OsWrite(logFileName, std::ofstream::app);
	char logTxt[2048];
	memset(logTxt, 0, sizeof(logTxt));
	time_t t = time(0);
	char date[32] = { NULL };
	strftime(date, sizeof(date), "%Y-%m-%d %H:%M:%S", localtime(&t));
	sprintf(logTxt, "%s : [%s] %s", date, LogTip[logLevel], pLogFormat);
	OsWrite << logTxt;
	OsWrite << std::endl;
	OsWrite.close();
}

//string?QString
QString str2qstr(const string str)
{
	return QString::fromLocal8Bit(str.data());
}
//??????????????
QString read_ico_path() {
	string strPath;
	char buffer[MAX_PATH];
	TCHAR exeFullPath[MAX_PATH]; // MAX_PATH??WINDEF.h?????????????260  
	memset(exeFullPath, 0, MAX_PATH);
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	int iLength;
	//?????????   
	iLength = WideCharToMultiByte(CP_ACP, 0, exeFullPath, -1, NULL, 0, NULL, NULL);
	//??tchar?????_char    
	WideCharToMultiByte(CP_ACP, 0, exeFullPath, -1, buffer, iLength, NULL, NULL);
	strPath = (string)buffer;
	int pos = strPath.find_last_of('\\', strPath.length());
	strPath = strPath.substr(0, pos);
	QString ico_path_Qstr = str2qstr(strPath);
	return ico_path_Qstr;
}

//???????float???
float readSettingFloat(LPCTSTR lpcName, int defalut_v,int decimal) {
	float saved_value;
	int saved_value_int;
	saved_value_int = GetPrivateProfileInt(L"Setting", lpcName, defalut_v, lpPath);
	saved_value = float(saved_value_int) / float(decimal);
	return saved_value;
}
//???????float???
float readSettingFloat_new(LPCTSTR lpcName, string defalut_v) {
	float saved_value;
	//string value_str;
	//char value[16];
	//value_str = GetPrivateProfileString("Setting", lpcName, value, sizeof(value), lpPath);
	
	return saved_value;
}

//????????float???
void writeSettingFloat(LPCTSTR lpcName, float sValue, int decimal) {
	int sValue_f;
	sValue_f = int(sValue * float(decimal));
	WCHAR strVal[128] = { 0 };
	swprintf(strVal, L"%d", sValue_f);
	WritePrivateProfileString(L"Setting", lpcName, strVal, lpPath);
}

//????????String???
void writeSettingString(QString lpcName, QString save_string) {

	QSettings* settings = new QSettings(setting_Path_str, QSettings::IniFormat);
	settings->setIniCodec(QTextCodec::codecForName("UTF8"));
	settings->beginGroup("Setting");
	settings->setValue(lpcName, save_string);
	settings->endGroup();
}
QString readSettingString(QString lpcName) {
	QSettings* settings = new QSettings(setting_Path_str, QSettings::IniFormat);
	settings->setIniCodec(QTextCodec::codecForName("UTF8"));
	settings->beginGroup("Setting");
	QString read_str = settings->value(lpcName).toString();
	settings->endGroup();
	return read_str;
}
//???????
void SetFloat(LPCTSTR lpszName, float fValue)
{
	WCHAR strVal[128] = { 0 };
	swprintf(strVal, L"%.1f", fValue);

	WritePrivateProfileString(L"WindSetting", lpszName, strVal, L"C:\\WindSetting.ini");
}
//????????
void SetInt(LPCTSTR lpszName, int nValue)
{
	WCHAR strVal[128] = { 0 };
	swprintf(strVal, L"%d", nValue);

	WritePrivateProfileString(L"WindSetting", lpszName, strVal, L"C:\\WindSetting.ini");
}
//??????????
void motor_gui::read_voltage() {
	float votage_set;
	votage_set = readSettingFloat(L"????(22??2.2A)",23, 10);
	writeSettingFloat(L"????(22??2.2A)", votage_set, 10);
	if (votage_set >= 2.5) {
		votage_set = 2.5;
		writeSettingFloat(L"????(22??2.2A)", votage_set, 10);
	}
	if (votage_set <= 1.7) {
		votage_set = 1.7;
		writeSettingFloat(L"????(22??2.2A)", votage_set, 10);
	}
	acc_cur = votage_set;
	run_cur = votage_set;
	ava_cur = votage_set-0.5;
}


//???????????????
void motor_gui::load_speed() {
	float speed_set,acc_set;
	speed_set=readSettingFloat(L"??????",100000 ,10);
	acc_set = readSettingFloat(L"?????????", 120000,10);
	writeSettingFloat(L"??????", speed_set, 10);
	writeSettingFloat(L"?????????", acc_set, 10);

	ui.doubleSpinBox_speed->setValue(speed_set);
	ui.doubleSpinBox_acc->setValue(acc_set);
	run_speed = int(speed_set);
	accelerate = int(acc_set);
}
void motor_gui::write_speed() {
	
	float speed_set, acc_set;
	speed_set = ui.doubleSpinBox_speed->value();
	acc_set = ui.doubleSpinBox_acc->value();
	writeSettingFloat(L"??????", speed_set, 10);
	writeSettingFloat(L"?????????", acc_set, 10);

	//SetFloat(L"VADASPEED", speed_set);
	//run_speed = int(speed_set * 89600*5 / 360);//int(speed_set * 89600 / 360);
	//accelerate = int(acc_set * 89600 * 5 / 360);// int(acc_set * 89600 / 360);

}


//???????????????
//???????????????????
void motor_gui::time_counter() {
	int com_waite;
	com_waite = GetPrivateProfileInt(L"Setting", L"com_waite", 1, lpPath);
	WCHAR strVal[5] = { 0 };
	swprintf(strVal, L"%d", com_waite);
	WritePrivateProfileString(L"Setting", L"com_waite", strVal, lpPath);
	ui.pushButton1->setStyleSheet("");
	ui.pushButton2->setStyleSheet("");
	ui.pushButton1->setEnabled(false);
	ui.pushButton2->setEnabled(false);
	ui.pushButton3->setStyleSheet("background-color: rgb(255, 70, 73)");
	ui.pushButton3->setEnabled(true);
	ui.label_5->setText(tr("?????????"));
	for (int i = 0; i < com_waite *500; i++) {
		Sleep(1);
		if (i% 500) {
			QString second_str = QString::number(com_waite-int(i / 500));
			ui.label_5->setText(tr("???  ")+ second_str);
		}
		qApp->processEvents();
	}
	int reconnect_counter = 1;
	while (1) {
		QString add_ft = "\\\\.\\";
		QString com_number = ui.comboBox->currentText();
		add_ft.append(com_number);
		QByteArray com_number_b = add_ft.toLatin1();
		char *com_number_c = com_number_b.data();
		//????????
		//Vsmd* vsmd;
		VsmdDevice* devices_t[1];
		vsmd = Vsmd::createVsmd(com_number_c, 9600);
		devices_t[0] = vsmd->createDevice(0);
		check_open = vsmd->open();
		if (stop_scan) {
			vsmd->close();
			stop_scan = false;
			ui.label_5->setText(tr("?????"));
			qApp->processEvents();
			break;
		}
		//sleep_timer(150);
		Sleep(100);
		if (check_open) {
			vsmd->close();
			writeLog(LOG_INFO, "??????");
			ui.label_5->setText(tr("??????"));
			qApp->processEvents();
			Sleep(300);
			scan(); 
			//run_thread();
			reconnect_counter = 0;
		}
		vsmd->close();
		if (stop_scan) {
			ui.label_5->setText(tr("?????"));
			qApp->processEvents();
			stop_scan = false;
			break;
		}
		ui.pushButton1->setStyleSheet("");
		ui.pushButton2->setStyleSheet("");
		ui.pushButton1->setEnabled(false);
		ui.pushButton2->setEnabled(false);
		//sleep_timer(100);
		Sleep(100);
		QString counter_str = QString::number(reconnect_counter);
		ui.label_5->setText(tr("????????...") + counter_str);
		reconnect_counter++;
		qApp->processEvents();
	}
	ui.pushButton1->setEnabled(true);
	ui.pushButton1->setStyleSheet("background-color: rgb(0, 170, 255)");
	ui.pushButton2->setEnabled(true);
	ui.pushButton2->setStyleSheet("background-color: rgb(170, 255, 127)");
	ui.pushButton3->setStyleSheet("");
	ui.pushButton3->setEnabled(false);
	qApp->processEvents();
}
void motor_gui::sleep_timer(int ms) {
	for (int i = 0; i < ms; i++) {
		Sleep(1);
		qApp->processEvents();
	}
}
//???????????
void AutoPowerOn(){
	HKEY hKey;
	//std::string strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	//1???????????????  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) ///????????       
{
		//2??????????????????????
		TCHAR strExeFullDir[MAX_PATH];
		GetModuleFileName(NULL, strExeFullDir, MAX_PATH);
		//3??????????????????????
		TCHAR strDir[MAX_PATH] = {};
		DWORD nLength = MAX_PATH;
		long result = RegGetValue(hKey, nullptr, L"Motor_start", RRF_RT_REG_SZ, 0, strDir, &nLength);
		//4?????????
		if (result != ERROR_SUCCESS || wcscmp(strExeFullDir, strDir) != 0)
		{

			//5???????????Key,?????????"GISRestart"????????????????????.exe??
			RegSetValueEx(hKey, L"Motor_start", 0, REG_SZ, (LPBYTE)strExeFullDir, (lstrlen(strExeFullDir) + 1) * sizeof(TCHAR));

			//6?????????

			RegCloseKey(hKey);

		}

	}

}
//???????????????
void CanclePowerOn(){
	HKEY hKey;

	//std::string strRegPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	//1???????????????  
	if (RegOpenKeyEx(HKEY_CURRENT_USER, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS){
		//2??????
		RegDeleteValue(hKey, L"Motor_start");
		//3?????????
		RegCloseKey(hKey);
	}
}


//????????????
void send_direction(int curr_angle)
{
	char dir_char[20];
	sprintf(dir_char, "??????:%d", curr_angle);
	writeLog(LOG_INFO, dir_char);
	if (s_hEnventAquire == INVALID_HANDLE_VALUE) {
		s_hEnventAquire = ::CreateEvent(NULL, FALSE, FALSE, L"FELIX_WIND_TRACER_20180818");
	}
	SetInt(L"WHAngle", curr_angle);
	if (s_hEnventAquire != INVALID_HANDLE_VALUE)
		::SetEvent(s_hEnventAquire);

}

//?????????id?????????
//model 1.ppi 2.cappi 3.rhi 4.vad 5.dbs 6.LOS
void send_model_id(int model) {
	SetInt(L"WorkMode", model);
	
}
//???????

//??????
void motor_gui::scan()
{
	running = true;
	int runTimeCounter = 0;
	bool clear_org = false;
	
	AutoPowerOn();
	ui.label_5->setText(tr("????????????"));
	stop = false;
	stop_scan = false;
	only_search_north = false;
	//write_speed();
	
	//????????
	search_north();
	load_speed();
	
	close_window = false;
	qApp->processEvents();
	Sleep(200);
	ui.pushButton3->setStyleSheet("background-color: rgb(255, 70, 73)");
	ui.pushButton3->setEnabled(true);
	while (1) {
	send_model_id(4);
	//?????????????
	if (search_N_stop == false) {
		ui.label_5->setText(tr("??????????"));
		ui.pushButton2->setStyleSheet("");
		ui.pushButton1->setEnabled(false);
		ui.pushButton2->setEnabled(false);
		//??????
		read_voltage();
		QString add_ft = "\\\\.\\";
		QString com_number = ui.comboBox->currentText();
		add_ft.append(com_number);
		QByteArray com_number_b = add_ft.toLatin1();
		char *com_number_c = com_number_b.data();
		//????????
		//Vsmd* vsmd;
		vsmd = Vsmd::createVsmd(com_number_c, 9600);
		devices[0] = vsmd->createDevice(0);
		devices[0]->autoStsOn();
		vsmd->open();
		devices[0]->cfgMcs(8);
		//devices[0]->cfgMdir(1);
		devices[0]->cfgCurrent(acc_cur, run_cur, ava_cur);//3A, 2.7A, 2.7A  ???????
		devices[0]->enable();
		devices[0]->cfgSpd(run_speed);//???????????
		devices[0]->cfgAcc(accelerate);//?????????????
		devices[0]->cfgDec(accelerate);//???????????

		//devices[0]->moveto(6400);
		int position = 0;
		//int n_quarter = 1.75;//??????????????1:2???????1:1.75
		//int position_move = 12800 * n_quarter;//90??????
		//90??????=51200*1.75/4=22400?????   ???51200??????  ??89600??????
		//51200*1.75=89600
		//51200*6=307200 
		////1??10=51200*10/4=128000????
		//1??3=51200*3/4=38400????
		//1??6=51200*6/4=76800????
		// 
		//12800*6=76800
		int position_move = 12800 *gear_ratio_float;

		int curr_postition;
		int motor_dial_turn;
		int radar_dial_turn = 180;
		int move_deg = 0;
		int show_deg = 0;
		int north = 0;
		int east = 90;
		int south = 180;
		int west = 270;
		devices[0]->org();
		char hbc[20];
		sprintf(hbc, "??????????:%d", ui.doubleSpinBox_wait->value());
		writeLog(LOG_INFO, hbc);
		while (1) {
			Sleep(40);//10
			 //???????????
			if (devices[0]->isOnline() == false) {
				vsmd->close();
				//writeLog(LOG_ERROR, "??????????");
				ui.label_5->setText(tr("???????"));
				qApp->processEvents();
				break;
			}
			//??????????????
			unsigned int motor_sts = devices[0]->getStatus();
			if ((motor_sts & 0x00001)) {
				ui.pushButton_sensor->setStyleSheet("background-color: rgb(255, 85, 0)");
			}
			else {
				ui.pushButton_sensor->setStyleSheet("background-color: rgb(170, 255, 127)");
			}
			//qApp->processEvents();
			//????????????????
			if (((show_deg == north) || (show_deg == east) || (show_deg == south) || (show_deg == west))&& move_deg>30) {
				move_deg = 0;
				//??? ??????????????
				if (show_deg == north) {
					runTimeCounter = runTimeCounter + 1;
					ui.pushButton_North->setStyleSheet("background-color: rgb(175,238,238)");
					ui.pushButton_West->setStyleSheet("");
					ui.pushButton_East->setStyleSheet("");
					ui.pushButton_South->setStyleSheet("");
					send_direction(0);}
				//???
				else if (show_deg == east) {
					ui.pushButton_East->setStyleSheet("background-color: rgb(175,238,238)");
					ui.pushButton_West->setStyleSheet("");
					ui.pushButton_South->setStyleSheet("");
					ui.pushButton_North->setStyleSheet("");
					send_direction(90);}
				//?????
				else if (show_deg == south) {
					ui.pushButton_South->setStyleSheet("background-color: rgb(175,238,238)");
					ui.pushButton_West->setStyleSheet("");
					ui.pushButton_East->setStyleSheet("");
					ui.pushButton_North->setStyleSheet("");
					send_direction(180);}
				//?????
				else if (show_deg == west) {
					ui.pushButton_West->setStyleSheet("background-color: rgb(175,238,238)");
					ui.pushButton_East->setStyleSheet("");
					ui.pushButton_South->setStyleSheet("");
					ui.pushButton_North->setStyleSheet("");
					send_direction(270);}
				qApp->processEvents();
				scan_time = ui.doubleSpinBox_wait->value();
				start_process_bar = true;
				while (1) {
					if (start_process_bar == false) {
						break;
					}
					qApp->processEvents();
					Sleep(10);
				}
				//???????????????
				if (runTimeCounter >= 3500) {
					writeLog(LOG_INFO, "3500?????");//3500????? ,?????3153?,
					runTimeCounter = 0;
					clear_org = true;
					devices[0]->org();
					devices[0]->disable();
					vsmd->close();
					break;
				}
				//?????????????????
				devices[0]->moveto(position + position_move);
				position = position + position_move;
				qApp->processEvents();
				//?????
				if (stop) {
					ui.pushButton2->setEnabled(true);
					//devices[0]->disable();
					devices[0]->org();
					stop = false;
					stop_scan = true;
					close_window = true;
					only_search_north = true;
					ui.pushButton_North->setStyleSheet("");
					ui.pushButton_East->setStyleSheet("");
					ui.pushButton_South->setStyleSheet("");
					ui.pushButton_West->setStyleSheet("");
					break;
				}
			}
			//??????????????????????
			curr_postition = devices[0]->getCurPos();
			QString str_motor_move = QString::number(curr_postition);
			ui.label_11->setText(str_motor_move);
			//motor_dial_turn = ((curr_postition % 102400) * 9) / 1280;
			//radar_dial_turn = (motor_dial_turn / 2) + 180;
			//51200*1.75=89600
			//51200*6=307200 
			motor_dial_turn = ((curr_postition % (int(51200*gear_ratio_float))) * 9) / 1280;//89600*5
			radar_dial_turn = (motor_dial_turn / gear_ratio_float) + 180;//radar_dial_turn = (motor_dial_turn / 1.75) + 180;
			ui.dial->setValue(radar_dial_turn);
			ui.dial_2->setValue(motor_dial_turn);
			//qApp->processEvents();
			if (radar_dial_turn >= 180) {
				show_deg = radar_dial_turn - 180;
			}
			else if (radar_dial_turn <180) {
				show_deg = radar_dial_turn + 180;
			}
			if (show_deg > 88 && show_deg < 92) {
				show_deg = 90;
			}
			if (show_deg > 268 && show_deg < 272) {
				show_deg = 270;
			}
			if (show_deg > 178 && show_deg < 182) {
				show_deg = 180;
			}
			if (show_deg > 350 && show_deg <= 360) {
				show_deg = 0;
			}
			QString degree_sb = "??";
			QString str_move = QString::number(show_deg);
			str_move.append(degree_sb);
			ui.label_3->setText(str_move);
			move_deg= move_deg+1;
			qApp->processEvents();
			if (radar_dial_turn >= 360) {
				radar_dial_turn = 0;
			}
		}
		ui.label_5->setText(tr("????????"));
		Sleep(200);
		devices[0]->org();
		devices[0]->disable();
		Sleep(500);
		vsmd->close();
		running = false;
		ui.pushButton1->setEnabled(true);
		ui.pushButton1->setStyleSheet("background-color: rgb(0, 170, 255)");
		ui.pushButton2->setEnabled(true);
		ui.pushButton2->setStyleSheet("background-color: rgb(170, 255, 127)");
		//ui.pushButton1->setText(tr("????????"));
	}
	else {
		close_window = true;
		only_search_north = true;
		ui.pushButton1->setEnabled(true);
		ui.pushButton1->setStyleSheet("background-color: rgb(0, 170, 255)");
		ui.pushButton2->setEnabled(true);
		ui.pushButton2->setStyleSheet("background-color: rgb(170, 255, 127)");
		ui.pushButton3->setStyleSheet("");
		ui.pushButton3->setEnabled(false);
}
	if (clear_org) {
		clear_org = false;
		only_search_north = false;
		search_north();
		ui.pushButton3->setStyleSheet("background-color: rgb(255, 70, 73)");
		ui.pushButton3->setEnabled(true);
		qApp->processEvents();
	}
	else {
		break;
	}
}
}

void motor_gui::stop_motor()
{
	ui.label_5->setText(tr("?????"));
	stop = true;
	stop_scan =true;
	ui.pushButton3->setStyleSheet("");
	ui.pushButton3->setEnabled(false);
	//thread1->quit();
	running = false;
}

void motor_gui::closeEvent(QCloseEvent *event) {

	if (close_window == false) {
		ui.label_5->setText(tr("?????????"));
		qApp->processEvents();
		Sleep(200);
		event->ignore();
	}
	else {
		delete trayIcon;
		//delete vsmd;
		//delete devices;
		event->accept();
	}
}

float motor_gui::gear_ratio_switch(int gear_ratio_int) {

	if (gear_ratio_int == 0) {
		return -6.0;
	}
	else if (gear_ratio_int == 1) {
		return 2.0;
	}
	else if (gear_ratio_int == 2) {
		return 1.75;
	}
	else {
		return 2.0;
	}

}

void motor_gui::search_north()
{
	running = true;
	send_model_id(0);
	close_window = false;
	int com_int,scan_time_int,gear_ratio_id;
	
	ui.pushButton3->setStyleSheet("background-color: rgb(255, 70, 73)");
	ui.pushButton3->setEnabled(true);

	gear_ratio_id = ui.comboBox_gear->currentIndex();
	WCHAR strVal_gear[3] = { 0 };
	swprintf(strVal_gear, L"%d", gear_ratio_id);
	WritePrivateProfileString(L"Setting", L"gear_ratio", strVal_gear, lpPath);
	gear_ratio_float = gear_ratio_switch(gear_ratio_id);

	com_int = ui.comboBox->currentIndex();
	com_int = com_int + 1;
	//???????????
	WCHAR strVal[3] = { 0 };
	swprintf(strVal, L"%d", com_int);
	WritePrivateProfileString(L"Setting", L"Com_Port", strVal, lpPath);
	scan_time = ui.doubleSpinBox_wait->value();
	scan_time_int = int(scan_time * 10);
	WCHAR strtime[8] = { 0 };
	swprintf(strtime, L"%d", scan_time_int);
	WritePrivateProfileString(L"Setting", L"Scan_time", strtime, lpPath);
	int manual_move;
	manual_move = GetPrivateProfileInt(L"Setting", L"manual_turn", 0, lpPath);


	ui.label_5->setText(tr("???????"));
	ui.pushButton1->setStyleSheet("");
	ui.pushButton1->setEnabled(false);
	ui.pushButton2->setStyleSheet("");
	ui.pushButton2->setEnabled(false);
	qApp->processEvents();
	//??????????
	QString add_ft = "\\\\.\\";
	QString com_number = ui.comboBox->currentText();
	add_ft.append(com_number);
	QByteArray com_number_b = add_ft.toLatin1();
	char *com_number_c = com_number_b.data();
	
	VsmdDevice* devices[1];//\\\\.\\'
	vsmd = Vsmd::createVsmd(com_number_c, 9600);
	//???????????????
	devices[0] = vsmd->createDevice(0);
	devices[0]->autoStsOn();
	vsmd->open();
	devices[0]->cfgMcs(8);
	//devices[0]->cfgMdir(1);
	devices[0]->cfgCurrent(acc_cur, run_cur, ava_cur);
	devices[0]->enable();
	int run_speed_n = 100000;// int run_speed_n = 20000;
	int accelerate_n = 100000 * 10;//int accelerate_n = 100000
	devices[0]->cfgSpd(run_speed_n);//???????????
	devices[0]->cfgAcc(accelerate_n);//?????????????
	devices[0]->cfgDec(accelerate_n);//???????????
	if (ui.Slider->value()==0) {
		devices[0]->cfgZero(1, 0, 0, 60000, 0);
	}
	else {
		devices[0]->cfgZero(1, 0, 1, 60000, 0);
	}
	
	devices[0]->zeroStart();
	Sleep(800);
	int point_counter = 0;
	int over_time = 0;
	search_N_stop = false;
	//???? ?????????????
	while (1) {
		//close_ornot = qApp->closingDown();
		if (stop) {
			search_N_stop = true;
			devices[0]->org();
			devices[0]->disable();
			Sleep(500);
			vsmd->close();
			stop = false;
			ui.label_5->setText("???????");
			ui.pushButton2->setEnabled(true);
			ui.pushButton2->setStyleSheet("background-color: rgb(170, 255, 127)");
			ui.pushButton_North->setStyleSheet("");
			ui.pushButton_East->setStyleSheet("");
			ui.pushButton_South->setStyleSheet("");
			ui.pushButton_West->setStyleSheet("");
			ui.pushButton1->setEnabled(true);
			ui.pushButton1->setStyleSheet("background-color: rgb(0, 170, 255)");
			break;
		}
		Sleep(100);
		point_counter++;
		over_time++;
		unsigned int motor_sts = devices[0]->getStatus();
		if (point_counter == 5) {
			ui.label_5->setText("???????");
		}
		if (point_counter == 10) {
			ui.label_5->setText("?????????");
		}
		if (point_counter == 15) {
			ui.label_5->setText("???????????");
			point_counter = 0;
		}
		qApp->processEvents();
		//??????100=10s  ?????
		if (over_time == 200) {
			ui.label_5->setText(tr("?????"));
			devices[0]->zeroStop();
			devices[0]->disable();
			Sleep(500);
			vsmd->close();
			ui.pushButton2->setEnabled(true);
			ui.pushButton2->setStyleSheet("background-color: rgb(170, 255, 127)");
			ui.pushButton_North->setStyleSheet("");
			ui.pushButton_East->setStyleSheet("");
			ui.pushButton_South->setStyleSheet("");
			ui.pushButton_West->setStyleSheet("");
			ui.pushButton1->setEnabled(true);
			ui.pushButton1->setStyleSheet("background-color: rgb(0, 170, 255)");
			ui.pushButton3->setStyleSheet("background-color: rgb(255, 70, 73)");
			ui.pushButton3->setEnabled(true);
			search_N_stop = true;
			break;
		}
		//???????????????
		if ((motor_sts & 0x00180)) {
			ui.label_5->setText(tr("???????"));
			ui.pushButton1->setStyleSheet("background-color: rgb(0, 170, 255)");
			ui.pushButton1->setEnabled(true);
			devices[0]->zeroStop();
			devices[0]->org();
			devices[0]->moveto(manual_move * 142);
			//???????????????????
			while (1) {
				Sleep(500);
				//??????????
				//motor_sts = 0x00080;
				if (motor_sts & 0x00080) {
					devices[0]->org();
					devices[0]->disable();
					Sleep(500);
					vsmd->close();
					break;
				}
			}
			break;
		}
		qApp->processEvents();
	}

	if (only_search_north == true) {
		close_window = true;
		running = false;
	}

	ui.pushButton3->setStyleSheet("");
	ui.pushButton3->setEnabled(false);
	qApp->processEvents();
	return ;
}
//??????-???
QString degree_of_move(int curr_angle) {
	QString show_degree;
	if (curr_angle == 0) {
		show_degree = "????0??";
	}
	else if (curr_angle>0) {
		QString degree = "???";
		QString symbol = "??";
		QString persent_str = QString::number(float(curr_angle*0.5));
		degree.append(persent_str);
		degree.append(symbol);
		show_degree = degree;
	}
	else if (curr_angle<0) {
		QString degree = "???";
		QString symbol = "??";
		QString persent_str = QString::number(float(curr_angle * 0.5));
		degree.append(persent_str);
		degree.append(symbol);
		show_degree = degree;
	}
	return show_degree;
}
//??????-????
void motor_gui::left_move()
{
	int curr_angle;
	curr_angle = GetPrivateProfileInt(L"Setting", L"manual_turn", 0, lpPath);
	curr_angle--;
	ui.label_9->setText(degree_of_move(curr_angle));
	WCHAR strVal[5] = { 0 };
	swprintf(strVal, L"%d", curr_angle);
	WritePrivateProfileString(L"Setting", L"manual_turn", strVal, lpPath);
	}
//??????-????
void motor_gui::right_move() {
	int curr_angle;
	curr_angle = GetPrivateProfileInt(L"Setting", L"manual_turn", 0, lpPath);
	curr_angle++;
	ui.label_9->setText(degree_of_move(curr_angle));
	WCHAR strVal[5] = { 0 };
	swprintf(strVal, L"%d", curr_angle);
	WritePrivateProfileString(L"Setting", L"manual_turn", strVal, lpPath);
}

void thread_obj::process_bar_move() {
	//????????????????
	
	while (1) {
		if (start_process_bar) {
			for (int i = 0; i < 101; i++) {
				emit bar_signal(i);
				Sleep(int(scan_time*10));
			}
			start_process_bar = false;
		}
		Sleep(100);
	}
}

//????????????
void motor_gui::display_bar(int bar_process) {
	ui.progressBar->setValue(bar_process);
	perst = "%";
	process = "???????";
	persent_str = QString::number(bar_process);
	persent_str.append(perst);
	process.append(persent_str);
	ui.label_5->setText(process);
	//Sleep(100);
	//qApp->processEvents();
	//Sleep(100);
}
//?????????
void motor_gui::run_thread() {
	if (!running) {
		running = true;
		thread1 = new QThread;
		connect(thread1, SIGNAL(started()), SLOT(time_counter()));
		thread1->start();//??????
	}
}
//???????
void motor_gui::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch (reason) {
	case QSystemTrayIcon::Trigger:
		showNormal();
		break;
	default:
		;
	}
}

motor_gui::motor_gui(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ico_path = read_ico_path();
	QIcon icon = QIcon(ico_path + "/ico/logo.ico");
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(icon);
	trayIcon->setToolTip("??????");
	//create_actions();
	//create_tray_icon();
	read_voltage();
	load_speed();
	trayIcon->show();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	//????logo
	setWindowIcon(QIcon(ico_path + "/ico/logo.ico"));
	//?????????
	int curr_angle,com_port,gear_ratio;

	gear_ratio= GetPrivateProfileInt(L"Setting", L"gear_ratio", 0, lpPath);
	ui.comboBox_gear->setCurrentIndex(gear_ratio);
	curr_angle = GetPrivateProfileInt(L"Setting", L"manual_turn", 0, lpPath);
	ui.label_9->setText(degree_of_move(curr_angle));
	com_port= GetPrivateProfileInt(L"Setting", L"Com_Port", 1, lpPath);
	ui.comboBox->setCurrentIndex(com_port-1);
	int scan_time_int;
	scan_time_int = GetPrivateProfileInt(L"Setting", L"Scan_time",30, lpPath);
	scan_time= float(scan_time_int) / float(10);
	ui.doubleSpinBox_wait->setValue(scan_time);
	SetInt(L"WVAngle", 75);
	thread_obj* thread_show;
	QThread* thread;
	thread_show = new thread_obj();
	thread = new QThread;
	connect(thread, SIGNAL(started()), thread_show, SLOT(process_bar_move()));
	connect(thread_show, SIGNAL(bar_signal(int)), this, SLOT(display_bar(int)));
	thread_show->moveToThread(thread);
	thread->start();//??????
	//???????
	connect(ui.pushButton1, SIGNAL(clicked()), this, SLOT(time_counter()));
	connect(ui.pushButton2, SIGNAL(clicked()), this, SLOT(search_north()));
	connect(ui.pushButton3, SIGNAL(clicked()), this, SLOT(stop_motor()));
	connect(ui.left, SIGNAL(clicked()), this, SLOT(left_move()));
	connect(ui.right, SIGNAL(clicked()), this, SLOT(right_move()));
	connect(ui.pushButton_hide, SIGNAL(clicked()), this, SLOT(hide()));
	
	connect(ui.doubleSpinBox_speed, SIGNAL(valueChanged(float)), ui.doubleSpinBox_speed, SLOT(write_speed()));

	//connect(ui.slow, SIGNAL(clicked()), this, SLOT(slow()));
	//connect(ui.normal, SIGNAL(clicked()), this, SLOT(normal()));
	//connect(ui.fast, SIGNAL(clicked()), this, SLOT(fast()));

	MotorGuiUiTheme::apply(this);
}
