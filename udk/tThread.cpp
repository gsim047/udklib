#include "tThread.h"

#include <stdio.h>
#include <atomic>

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

/*
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
*/

static std::atomic<int> tThreadCounter(0);

/*
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
*/


void run_thread_item(tThread *thr)
{
	//printf("run_...+\n");

	try{
//		unsigned tid = getTid();
//		thr->tid = tid;
//		{
//			std::lock_guard<std::mutex> ll(tThread::listLock);
//			__list_set(tid, thr);
//		}
		//printf("run_... to run\n");
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
//			std::lock_guard<std::mutex> ll(tThread::listLock);
//			__list_erase(tid);
		}
		//debugLog("tThread '" + thread->getName() + "' exiting");
		if ( thr->_detach ){
			//printf("run_... delete this\n");
			delete thr;
		}
	}
	catch ( nError::Message &m ){
		errorLog("run_thread_item (3): " + m);
	}
	//printf("run_...-\n");
}// run_thread_item




tThread::tThread() :
	_ok(false),
	tid(0),
	name("Thread-" + ToString(++tThreadCounter)),
	stopFlag(false),
	suspendFlag(true),
//	_heap(getHeap()),
	_detach(false)
{
}// tThread::tThread


tThread::tThread(const string &Name) :
	_ok(false),
	tid(0),
	name(Name),
	stopFlag(false),
	suspendFlag(true),
//	_heap(getHeap()),
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
	--tThreadCounter;
}// tThread::~tThread


void tThread::start()
{
	//printf("- start 1\n");
	if ( tid != 0 )  return;
	//printf("- start 2\n");
	suspendFlag = true;
	//printf("- start 3\n");
	//printf("- start 4\n");
	th = new std::thread(run_thread_item, this);
	//printf("- start 5\n");
	_ok = true;
//	if ( _ok ){
//		while ( suspendFlag )  __sleep(10);
//	}
}// tThread::start

/*
tThread *tThread::getThread(unsigned tid)  // static 
{
	std::lock_guard<std::mutex> ll(listLock);
	return __list_find(tid);
}// tThread::getThread
*/

void tThread::destroy()
{
	_ok = false;
	suspendFlag = false;
//	std::lock_guard<std::mutex> ll(listLock);
	//list.erase(tid);
//	__list_erase(tid);
}// tThread::destroy


int tThread::getPriority() const
{
	return 5;
}// tThread::getPriority


bool tThread::isInterrupted() const
{
	return !_ok || suspendFlag;
}// tThread::isInterrupted


void tThread::join()
{
	if ( _ok ){
		th->join();
	}
}// tThread::join

/*
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
*/

void tThread::setPriority(int new_priority)
{
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
	while ( tThreadCounter ){
		__sleep(100);
	}
}// tThread::joinAll


void tThread::resume()
{
	if ( suspendFlag ){
		suspendFlag = false;
	}
}// tThread::resume


void tThread::suspend()
{
	if ( !suspendFlag ){
		suspendFlag = true;
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
