#include "motor_gui.h"
#include <QtWidgets/QApplication>
#include <iostream> 
#include <windows.h>
using namespace std;


bool checkOneRunning()
{
	//  ����������
	HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"motor_gui");
	//  ���������
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		//  ������л������������ͷž������λ������
		CloseHandle(m_hMutex);
		m_hMutex = NULL;
		//  �����˳�
		return  false;
	}
	else
		return true;
}

int main(int argc, char *argv[])
{


	QApplication a(argc, argv);

	if (!checkOneRunning()) {
		return 0;
	}
	motor_gui w;
	//QString s1 = QStringLiteral("���������");
	//w.setWindowTitle(s1);

	//w.setWindowFlags(w.windowFlags()&~Qt::WindowMaximizeButtonHint);
	w.show();
	
	//Sleep(15000);
	w.time_counter();
	
	
	return a.exec();



}

