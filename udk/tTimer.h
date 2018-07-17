#ifndef __tTimer_h
#define __tTimer_h 1

#include <chrono>



class tTimer {
public:
	tTimer() { reset(); }
	tTimer(const tTimer &s) : init(s.init) {};

	tTimer &operator=(const tTimer &s) { init = s.init; return *this; }

public:
	double getms() const;                                           // in milliseconds
	double gets() const;                                            // in seconds
	bool check(int timeout) const { return (double)timeout <= getms(); }; // in milliseconds

	void reset();

	static void delay(int timeout);                                 // in milliseconds

	static int tick();

//	static ulonglong freq();
	//static ulonglong cpu_ratio;
private:
//	ulonglong init;
//	static double s( ticks);                               // converts CPU ticks to secs
//	static double ms(std::chrono::high_resolution_clock::time_point ticks) { return 1000. * s(ticks); }  // converts CPU ticks to ms

	static std::chrono::high_resolution_clock::time_point _tick();

	std::chrono::high_resolution_clock::time_point init;
};// class tTimer




#endif // __tTimer_h
