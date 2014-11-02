//-----------------------------------------//
//	Timer.cpp							   //
//	Timer Class							   //
//	uses QueryPerformanceFrequency/counter //
//										   //
//	RJ Fox 11/09					       //
//-----------------------------------------//

#ifndef TIMER_H
#define TIMER_H

#include <windows.h>

class Timer
{
public:
	Timer();
	void Start();
	void Stop();
	void Pause();
	void Resume();
	void Update();
	enum timerState{ STARTED = 1, STOPPED = 2, PAUSED = 3 }; 

	int getTimerState() { return m_timerState; }
	float GetFPS() { return m_fps; }
	float GetRunningTime() { return m_runningTime; }
	float GetElapsedTime();
	
private:
	INT64		m_ticksPerSecond;	//frequency of the counter per second.
	INT64		m_currentTime;		//current counter time.
	INT64		m_lastTime;			//last time it updated.
	INT64		m_lastFPSUpdate;	//internal time the last fps update was
	INT64		m_FPSUpdateInterval;//interval to update the fps 
	INT64		m_FPSUpdate;		//
	INT64		m_stopTime;			//
	UINT		m_numFrames;		//stores frame ctr between updates.
	float		m_runningTime;		//total elapsed time of the counter
	float		m_timeElapsed;		//how long the counter has been running since last update (in seconds)
	float		m_fps;				//frames per second
	int			m_timerState;		//counter time it stopped
};

#endif
