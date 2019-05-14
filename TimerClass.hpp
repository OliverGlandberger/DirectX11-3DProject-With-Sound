#ifndef TIMERCLASS_HPP
#define TIMERCLASS_HPP

#include <time.h>
#include <Windows.h>

class TimerClass
{
private:
	double LastTime;
	double dt;
	double TotalTime;

	double PCFreq;
	_int64 PreviousCounter;

public:
	TimerClass();
	~TimerClass();

	void Update();
	/* ------------- COMMENTS -------------
	Resets the 'dt' after a certain threshold of
	time has been reached. This threshold is determined
	outside of the TimerClass, hence why this public
	function exists.
	*/
	void Reset();



	// GET FUNCTIONS

	double get_dt();
};

#endif