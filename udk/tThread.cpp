#include "tThread.h"

#include <stdio.h>

#ifdef __GNUG__
#include <signal.h>
#include <sys/resource.h>
#endif // __GNUG__

#include "nError.h"
#include "fString.h"
#include "tTimer.h"
#include "tDebug.h"



static int getDelta(int timeout)
{
	int delta = timeout / 10;
	if ( delta < 2 )  delta = 2;
	if ( delta > 100 )  delta = 100;
	return delta;
}// getDelta




bool tThread::stopAllFlag = false;

std::mutex tThread::listLock;
//std::map<unsigned, tThread *> tThread::list;
static unsigned __list_id[200];
static tThread * __list_ptr[200];
static unsigned __list_idx = 0;

static void __list_set(unsigned id, tThread *ptr)
{
	if ( __list_idx >= 200 )  nError::Throw("tThread::__list_id overflow !");
	__list_id[__list_idx] = id;
	__list_ptr[__list_idx] = ptr;
	__list_idx++;
}// __list_set

static tThread *__list_find(unsigned id)
{
	for ( unsigned i = 0; i < __list_idx; i++ ){
		if ( __list_id[i] == id )		return __list_ptr[i];
	}
	return NULL;
}// __list_find

static void __list_erase(unsigned id)
{
	for ( unsigned i = 0; i < __list_idx; i++ ){
		if ( __list_id[i] == id ){
			for ( unsigned j = i + 1; j < __list_idx; j++ ){
				__list_id[j - 1] = __list_id[j];
				__list_ptr[j - 1] = __list_ptr[j];
			}
			__list_idx--;
			return;
		}
	}
}// __list_erase

/*
#ifdef _WIN32

static HANDLE __h2_close[200];
static unsigned __h2_idx = 0;

static void __h2_set(HANDLE h)
{
	if ( __h2_idx >= 200 )  nError::Throw("tThread::__h2_close overflow !");
	__h2_close[__h2_idx] = h;
	__h2_idx++;
}// __h2_set

static void __h2_erase()
{
	for ( unsigned i = 0; i < __h2_idx; i++ ){
		CloseHandle(__h2_close[i]);
		__h2_close[i] = NULL;
	}
	__h2_idx = 0;
}// __h2_erase

#endif // _WIN32
*/

//std::map<unsigned, bool> tThread::in_heap;

static tSyncInt tThreadCounter = 0;

static std::mutex in_heap_m;
static unsigned in_heap = 0;

void tThread::setHeap()
{
	std::lock_guard<std::mutex> ml(in_heap_m);
	while ( in_heap )  __sleep(10);
	in_heap = getTid();
}// tThread::setHeap

static bool getHeap()
{
	bool ret = (in_heap == getTid());
	if ( ret )  in_heap = 0;
	return ret;
}// setHeap



/*
#ifdef __GNUG__
void *run_thread_item(void *attr)
{
	tThread *thread = (tThread *)attr;
#endif // __GNUG__
#ifdef _WIN32
DWORD run_thread_item(LPDWORD attr)
{
	tThread *thread = (tThread *)*attr;
#endif // _WIN32
*/
void run_thread_item(tThread *thr)
{
//	tThread *thread = (tThread *)*attr;
	if ( thr->suspendFlag ) return;

	try{
		unsigned tid = getTid();
		thr->tid = tid;
		{
			std::lock_guard<std::mutex> ll(tThread::listLock);
			//tThread::list[tid] = thread;
			__list_set(tid, thr);
//#ifdef _WIN32
//			__h2_erase();
//#endif // _WIN32
		}
		thr->suspendFlag = false;
		try{
			thr->run();
		}
		catch ( nError::Message &m ){
			errorLog("run_thread_item (1): " + m);
		}
		catch ( ... ){
			errorLog("run_thread_item (2): Error !!!");
		}
		thr->_ok = false;
		{
			std::lock_guard<std::mutex> ll(tThread::listLock);
			//tThread::list.erase(tid);
			__list_erase(tid);

//#ifdef _WIN32
//			__h2_erase();
//			__h2_set(thread->hThread);
//#endif // _WIN32
		}
		//debugLog("tThread '" + thread->getName() + "' exiting");
		if ( thr->_detach )  delete thr;
//		tThread::exit();
	}
	catch ( nError::Message &m ){
		errorLog("run_thread_item (3): " + m);
	}
//	return NULL;
}// run_thread_item




tThread::tThread() :
	_ok(false),
	tid(0),
	name("Thread-" + ToString(++tThreadCounter)),
	stopFlag(false),
	suspendFlag(true),
	_heap(getHeap()),
	_detach(false)
{
}// tThread::tThread


tThread::tThread(const string &Name) :
	_ok(false),
	tid(0),
	name(Name),
	stopFlag(false),
	suspendFlag(true),
	_heap(getHeap()),
	_detach(false)
{
	++tThreadCounter;
}// tThread::tThread


tThread::~tThread()
{
	if ( _ok && tid != getTid() ){
		stopFlag = true;
		resume();
		join();
	}
}// tThread::~tThread


void tThread::start()
{
	if ( tid != 0 )  return;
	suspendFlag = true;
/*
#ifdef __GNUG__
	_ok = ( pthread_create(&thread, NULL, run_thread_item, this) == 0 );
#endif // __GNUG__
#ifdef _WIN32
	DWORD dwThrdParam = (DWORD)this;
	hThread = CreateThread(NULL,
	                       0,
	                       (LPTHREAD_START_ROUTINE) run_thread_item,
	                       &dwThrdParam,
	                       THREAD_SUSPEND_RESUME,
	                       &ThreadId);
	_ok = ( hThread != NULL );
#endif // _WIN32
*/
	th = new std::thread(run_thread_item, this);
	_ok = true;
	if ( _ok ){
		while ( suspendFlag )  __sleep(10);
	}
}// tThread::start


tThread *tThread::getThread(unsigned tid)  // static 
{
	std::lock_guard<std::mutex> ll(listLock);
	return __list_find(tid);
}// tThread::getThread

/*
tThread *tThread::getThread(const string &name)  // static
{
	std::lock_guard<std::mutex> ll(listLock);
	for ( std::map<unsigned, tThread *>::const_iterator i = list.begin();
	      i != list.end(); ++i ){
		if ( i->second && i->second->name == name )  return i->second;
	}
	return NULL;
}// tThread::getThread
*/

void tThread::destroy()
{
/*
#ifdef __GNUG__
	pthread_cancel(thread);
#endif // __GNUG__
#ifdef _WIN32
	TerminateThread(hThread, 0);
#endif // _WIN32
*/
	_ok = false;
	suspendFlag = false;
	std::lock_guard<std::mutex> ll(listLock);
	//list.erase(tid);
	__list_erase(tid);
}// tThread::destroy


int tThread::getPriority() const
{
/*
#ifdef __GNUG__
#ifndef __CYGWIN__
	return getpriority(PRIO_PROCESS, tid);      // ???  tid -> getPid() ?
#endif
#endif // __GNUG__
#ifdef _WIN32
	return GetThreadPriority(hThread);
#endif // _WIN32
*/
	return 5;
}// tThread::getPriority

/*
void tThread::interrupt()
{
}// tThread::interrupt


bool tThread::interrupted()  // static 
{
	return false;
}// tThread::interrupted
*/

bool tThread::isInterrupted() const
{
	return !_ok || suspendFlag;
}// tThread::isInterrupted


void tThread::join()
{
	if ( _ok ){
		th->join();
	}
	/*
	for (;;){
		{
			std::lock_guard<std::mutex> ll(listLock);
			//if ( list.find(tid) == list.end() ) break;
			if ( !__list_find(tid) )  break;
		}
		__sleep(100);
	} */
}// tThread::join


bool tThread::join(int timeout)
{
	int delta = getDelta(timeout);
	for ( tTimer tm;; ){
		{
			std::lock_guard<std::mutex> ll(listLock);
			//if ( list.find(tid) == list.end() )  return true;
			if ( !__list_find(tid) )  return true;
		}
		if ( tm.check(timeout) )  break;
		__sleep(delta);
	}
	return false;
}// tThread::join


void tThread::setPriority(int new_priority)
{
/*
#ifdef __GNUG__
#ifndef __CYGWIN__
	setpriority(PRIO_PROCESS, tid, new_priority);     // ???   tid -> getPid() ?
#endif
#endif // __GNUG__
#ifdef _WIN32
	SetThreadPriority(hThread, new_priority);
#endif // _WIN32
*/
}// tThread::setPriority


void tThread::__sleep(int timeout)  // static 
{
	tTimer::delay(timeout);
}// tThread::__sleep


void tThread::yield()
{
	std::this_thread::yield();
//	__sleep(10);
}// tThread::yield


void tThread::joinAll()
{
	unsigned size = 0;
	{
		std::lock_guard<std::mutex> ll(listLock);
		unsigned tid = getTid();
		//if ( tThread::list.find(tid) != tThread::list.end() )  size = 1;
		if ( __list_find(tid) != NULL )  size = 1;
	}
	for (;;){
		{
			std::lock_guard<std::mutex> ll(listLock);
			//if ( tThread::list.size() <= size ) break;
			if ( __list_idx <= size )  break;
		}
		__sleep(100);
	}
}// tThread::joinAll


void tThread::resume()
{
	if ( suspendFlag ){
		suspendFlag = false;
/*
#ifdef __GNUG__
		pthread_kill(thread, SIGCONT);
#endif // __GNUG__
#ifdef _WIN32
		ResumeThread(hThread);
#endif // _WIN32
*/
	}
}// tThread::resume


void tThread::suspend()
{
	if ( !suspendFlag ){
		suspendFlag = true;
/*
#ifdef __GNUG__
		pthread_kill(thread, SIGSTOP);
#endif // __GNUG__
#ifdef _WIN32
		SuspendThread(hThread);
#endif // _WIN32
*/
	}
}// tThread::suspend


bool tThread::sleep(int timeout) const  // in milliseconds, ret false if thread stopping
{
	int delta = getDelta(timeout);
	for ( tTimer tm;; ){
		if ( testStop() )  return false;
		if ( tm.check(timeout) )  break;
		__sleep(delta);
	}
	return true;
}// tThread::sleep
