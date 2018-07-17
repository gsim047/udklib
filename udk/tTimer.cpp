#include "tTimer.h"
//#include <time.h>
#include <thread>




int tTimer::tick()
{
	std::chrono::high_resolution_clock::time_point now = _tick();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}


std::chrono::high_resolution_clock::time_point tTimer::_tick()
{
	return std::chrono::high_resolution_clock::now();
}// tick


void tTimer::delay(int timeout)
{
	std::chrono::milliseconds dura( timeout );
	std::this_thread::sleep_for( dura );
}// tTime::delay

/*
ulonglong tTimer::freq()
{
	if ( !cpu_ratio )  init_cpu_ratio();
	return cpu_ratio;
}// tTimer::freq
*/

void tTimer::reset()
{
	init = std::chrono::high_resolution_clock::now();
}// tTimer::reset


double tTimer::gets() const
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(_tick() - init).count() / 1000.;
}// tTimer::gets


double tTimer::getms() const
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(_tick() - init).count() * 1.;
}// tTimer::getms


