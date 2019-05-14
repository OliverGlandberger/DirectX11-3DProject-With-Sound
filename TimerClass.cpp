#include "TimerClass.hpp"

TimerClass::TimerClass()
{
	this->LastTime = 0.0;
	this->dt = 0.0;
	this->TotalTime = 0.0;

	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		MessageBox(NULL, L"The 'QueryPerformanceFrequency' failed!", L"ERROR: 'Update() - TimerClass'", MB_OK);

	this->PCFreq = double(li.QuadPart);// / 1000.0;

	QueryPerformanceCounter(&li);
	PreviousCounter = li.QuadPart;
}

TimerClass::~TimerClass()
{

}





//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//																					//
//									PUBLIC FUNCTIONS								//
//																					//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void TimerClass::Update()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);	// Checks the 'counter' since the QueryPerformanceFrequency

	this->dt += double(li.QuadPart - PreviousCounter) / this->PCFreq;	// Calculates the delta time

	this->TotalTime += this->dt;	// Updating the total time elapsed

	PreviousCounter = li.QuadPart;	// This current Query becomes the new PreviousCounter
}

void TimerClass::Reset()
{
	this->dt = 0.0;
}




// GET FUNCTIONS

double TimerClass::get_dt()
{
	return this->dt;
}