#pragma once


#ifdef VSMD_DLL_EXPORTS
#define VSMD_DLL_API __declspec(dllexport)
#else
#define VSMD_DLL_API __declspec(dllimport)
#endif


class VSMD_DLL_API VsmdDevice
{
protected:

public:
	VsmdDevice(int cid);
	virtual ~VsmdDevice();

	//
	virtual void autoStsOn();
	virtual void autoStsOff();

	//
	virtual void enable();
	virtual void disable();
	virtual void move();
	virtual void moveto(int pos);
	virtual void rmove(int distance);
	virtual void stop(int mode);
	virtual void sts();
	virtual void dev();
	virtual void org();
	virtual void save();
	virtual void cfgSpd(float spd);
	virtual void cfgAcc(float acc);
	virtual void cfgDec(float dec);
	virtual void cfgCurrent(float cra, float crn, float crh);
	virtual void cfgMcs(int mcs);
	
	virtual void cfgZero(int zmd, int snr, int osv, float zsd, int zsp);
	virtual void zeroStart();
	virtual void zeroStop();

	virtual bool isOnline();
	virtual void online();
	virtual void offline();
	virtual float getCurSpd();
	virtual int getCurPos();
	virtual unsigned int getStatus();

};


class VSMD_DLL_API Vsmd
{
protected:

public:

	static Vsmd* createVsmd(char* comPort, int baudrate);
	virtual VsmdDevice* createDevice(int cid);
	virtual bool open();
	virtual bool close();

};


