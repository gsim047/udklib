#pragma once
#ifndef __tThread_h
#define __tThread_h 1

#include <thread>
//#include <cstddef>
#include "fPid.h"
#include "tMutex.h"





class tThread{
public:
	tThread();
	tThread(const string &name);
	virtual ~tThread();

/*
#ifdef __GNUG__
	friend void *run_thread_item(void *attr);
#endif // __GNUG__
#ifdef _WIN32
	friend DWORD run_thread_item(LPDWORD attr);
#endif // _WIN32
*/
	friend void run_thread_item(tThread *thr);

private:
	tThread(const tThread &);
	tThread &operator=(const tThread &);

public:
	void destroy();

	unsigned gettid() const { return tid; }

	string getName() const { return name; };
	void setName(const string &Name) { name = Name; };

	int getPriority() const;
	void setPriority(int new_priority);

	//void interrupt();                                    // ???
	//static bool interrupted();                           // ???
	bool isAlive() const { return _ok; };
	bool isDaemon() const { return _detach; };
	bool isInterrupted() const;
	void join();
	bool join(int timeout);
	void setDaemon(bool on = true) { _detach = (on && _heap); };
	void start();
	void stop() { stopFlag = true; };                    // ---
	void suspend();                                      // ---
	void resume();                                       // ---


	static tThread *currentThread() { return getThread(getTid()); }
	//static tThread *getThread(const string &name);
	static tThread *getThread(unsigned threadId);
	static void __sleep(int timeout);

	static void stopAll() { stopAllFlag = true; };       // ---
	static void joinAll();

	void *operator new(size_t size) { setHeap(); return ::operator new(size); };

protected:
	bool testStop() const { return stopAllFlag || stopFlag; };

	static void yield();
	virtual void run() = 0;

//	static void exit();
	bool sleep(int timeout) const;   // in milliseconds, ret false if thread stopping

private:
	bool _ok;
	bool _heap;
	bool _detach;

/*
#ifdef __GNUG__
	pthread_t thread;
#endif // __GNUG__
#ifdef _WIN32
	DWORD ThreadId;
	HANDLE hThread;
#endif // _WIN32
*/
	std::thread *th = nullptr;

	unsigned tid;
	string name;
	bool stopFlag;
	bool suspendFlag;

protected:
	static bool stopAllFlag;
	static std::mutex listLock;

private:
	static void setHeap();
};// class tThread 


/*
// use example :

class localThread : public tThread {
public:
	localThread() : tThread() {}

	void run() {
		string name = "localThread " + getName();

		cout << name << " starting !" << endl;
		while ( sleep(1000) ){
			cout << name << " polling !" << endl;
		}
		cout << name << " stopping !" << endl;
	} 
};// class localThread

*/


#endif // __tThread_h
