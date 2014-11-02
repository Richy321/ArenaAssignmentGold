//-----------------------------------------//
//	Timer.cpp							   //
//	Timer Class							   //
//	uses QueryPerformanceFrequency/counter //
//										   //
//	RJ Fox 11/09					       //
//-----------------------------------------//

#include "Timer.h"
/*----------------------------------------------------
		CTimer::CTimer
				Default Constructor
-----------------------------------------------------*/
Timer::Timer()
{
	//get times a second the counter will increment
	QueryPerformanceFrequency( (LARGE_INTEGER *)&m_ticksPerSecond );
	m_currentTime = m_lastTime = m_lastFPSUpdate = m_stopTime = m_numFrames = 0;
	m_runningTime = m_timeElapsed = m_fps  = 0.0f;
	m_FPSUpdateInterval = m_ticksPerSecond >> 1;	//bitshift right (div by 2)
	m_timerState = STOPPED;
}



/*----------------------------------------------------
				CTimer::Start
				Starts the timer
-----------------------------------------------------*/
void Timer::Start()
{
	if( m_timerState == STARTED )
	{
		//already running
		return;
	}
	//Get counter value when timer was started and put in m_lastTime.
	QueryPerformanceCounter( (LARGE_INTEGER *) &m_lastTime );
	m_timerState = STARTED;
}

/*----------------------------------------------------
				CTimer::Stop
				Stops the Timer
-----------------------------------------------------*/
void Timer::Stop()
{
	if ( m_timerState != STARTED )
	{
		//already stopped
		return;
	}
	m_stopTime = 0;
	//Get counter value when timer was stopped and put in stopTime.
	QueryPerformanceCounter( (LARGE_INTEGER *)&m_stopTime);

	//calc running time.
	m_runningTime += (float)(m_stopTime - m_lastTime) / (float)m_ticksPerSecond;
	m_timerState = STOPPED;
}

void Timer::Pause()
{
	if ( m_timerState == STOPPED || m_timerState == PAUSED )
	{
		//already stopped / paused
		return;
	}
	m_stopTime = 0;
	//Get counter value when timer was stopped and put in stopTime.
	QueryPerformanceCounter( (LARGE_INTEGER *)&m_stopTime);

	//calc running time.
	m_runningTime += (float)(m_stopTime - m_lastTime) / (float)m_ticksPerSecond;
	m_timerState = PAUSED;
}
void Timer::Resume()
{
	if( m_timerState != PAUSED)
	{
		//already running or wasn't running in the first place
		return;
	}
	 
	
	m_lastTime = m_stopTime;
	m_timerState = STARTED;
}

/*----------------------------------------------------
				CTimer::Update
	Updates the frames per second + total running time
-----------------------------------------------------*/

void Timer::Update()
{
	if( m_timerState != STARTED )
	{
		return;	//not running
	}

	//Get the current time
	QueryPerformanceCounter( (LARGE_INTEGER *) &m_currentTime );


	m_timeElapsed = (float)(m_currentTime - m_lastTime) / (float)m_ticksPerSecond;
	m_runningTime += m_timeElapsed;

	//Update FPS
	m_numFrames++;
	if ( m_currentTime - m_lastFPSUpdate >= m_FPSUpdateInterval )
	{
		float currentTime  = (float)m_currentTime / (float)m_ticksPerSecond;
		float lastTime = (float)m_lastFPSUpdate / (float)m_ticksPerSecond;
		m_fps = (float)m_numFrames / (currentTime - lastTime);
		m_lastFPSUpdate = m_currentTime;
		m_numFrames = 0;
	}

	m_lastTime = m_currentTime;
}

float Timer::GetElapsedTime()
{
	if (m_timerState != STARTED)
		return 0.0f;
	else
		return m_timeElapsed;
}
