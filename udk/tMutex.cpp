#include "tMutex.h"
#include "tTimer.h"
#include "fPid.h"
#include "tDebug.h"
#include "fString.h"



/*
namespace nError {
	LockError::LockError() : Message("can't lock tMutex") 
	{
	}// LockError::LockError

	DeadLock::DeadLock() : Message("second lock in the same thread") 
	{
	}// DeadLock::DeadLock
};// namespace nError
*/



#ifdef __GNUG__

#ifdef __CYGWIN__
#include <sys/errno.h>
#else
#include <asm/errno.h>
#endif

#endif


/*
tMutex::tMutex() :
  _stop(false)
{
	pthread_mutex_init(&_lock, NULL);
}// tMutex::tMutex


tMutex::~tMutex()
{
	invalidate();
	unlock();
	pthread_mutex_destroy(&_lock);
}// tMutex::~tMutex


bool tMutex::lock()
{
	if ( _stop )  return false;
	pthread_mutex_lock(&_lock);
	return true;
}// tMutex::lock


bool tMutex::lock(int timeout)
{
	if ( _stop )  return false;
	if ( timeout < 0 )  return lock();

	for ( tTimer tim; !tim.check(timeout); tTimer::delay(10) ){
		if ( pthread_mutex_trylock(&_lock) != EBUSY )  return true;
	}
	return pthread_mutex_trylock(&_lock) != EBUSY;
}// tMutex::lock


void tMutex::unlock()
{
	pthread_mutex_unlock(&_lock);
}// tMutex::unlock


#endif // __GNUG__


#ifdef _WIN32



tMutex::tMutex() :
	_lock(false),
	_stop(false),
	tids(new unsigned [33])
{
	InitializeCriticalSection(&m_cs);
	//memset(tids, 0, 33 * sizeof(unsigned));
	for ( unsigned i = 0; i < 33; i++ ) tids[i] = 0;
}// tMutex::tMutex


tMutex::~tMutex()
{
	invalidate();
	while ( tids[0] != 0 )  tTimer::delay(10);
	//for ( tTimer tm; !tm.check(2000); tTimer::delay(10) ){
	// if ( tids[0] == 0 ) break;
	//}
	DeleteCriticalSection(&m_cs);
	delete [] tids;
}// tMutex::~tMutex


bool tMutex::lock()
{
	if ( _stop )  return false;

	// --------------------
	tTimer tm;
	bool msg = false;
	// --------------------

	//cout << ToString(getTid()) << " m::lock+" << endl;
	unsigned tid = getTid();

	EnterCriticalSection(&m_cs);
	if ( !_lock ){
		_lock = true;
		tids[0] = tid;
		LeaveCriticalSection(&m_cs);
	//cout << ToString(getTid()) << " m::lock-1" << endl;
		return true;
	}
	for (;;){
		bool ok = setMark(tid);
		LeaveCriticalSection(&m_cs);
		if ( ok ) break;
		tTimer::delay(10);

		// ----------------
		if ( !msg && tm.check(5000) ){
			msg = true;
			errorLog("Can't lock tMutex during 5 sec");
		}
		// ----------------

		EnterCriticalSection(&m_cs);
	}
	while ( tids[0] != tid ){
		tTimer::delay(3);
	}
	//cout << ToString(getTid()) << " m::lock-2" << endl;
	return true;
}// tMutex::lock


bool tMutex::lock(int timeout)
{
	if ( _stop )  return false;
	if ( timeout < 0 )  return lock();
	tTimer tim;
	unsigned tid = getTid();

	EnterCriticalSection(&m_cs);
	if ( !_lock ){
		_lock = true;
		tids[0] = tid;
		LeaveCriticalSection(&m_cs);
		return true;
	}
	for (;;){
		bool ok = setMark(tid);
		LeaveCriticalSection(&m_cs);
		if ( tim.check(timeout) ){
			clearMark(tid);
			return false;
		}
		if ( ok ) break;
		tTimer::delay(10);
		EnterCriticalSection(&m_cs);
	}
	while ( tids[0] != tid ){
		if ( tim.check(timeout) ){
			clearMark(tid);
			return false;
		}
		tTimer::delay(3);
	}
	return true;
}// tMutex::lock


bool tMutex::setMark(unsigned tid)
{
	for ( unsigned i = 0; i < 32; i++ ){
		if ( tids[i] == tid )  nError::Throw(nError::DeadLock());
		if ( tids[i] == 0 ){
			tids[i] = tid;
			return true;
		}
	}
	return false;
}// tMutex::setMark


void tMutex::clearMark(unsigned tid)
{
	EnterCriticalSection(&m_cs);
	for ( unsigned i = 0; i < 32; i++ ){
		if ( tids[i] == tid ){
			for ( unsigned j = i; j < 32; j++ )  tids[j] = tids[j + 1];
			//memmove(tids + i * sizeof(unsigned),
			//        tids + (i + 1) * sizeof(unsigned),
			//        (32 - i) * sizeof(unsigned));
			break;
		}
	}
	LeaveCriticalSection(&m_cs);
}// tMutex::clearMark


void tMutex::unlock()
{
	//cout << ToString(getTid()) << " m::unlock+" << endl;
	EnterCriticalSection(&m_cs);
	//cout << "tMutex::unlock() 1: " << tids[0] << endl;
	//memmove(tids, tids + sizeof(unsigned), 32 * sizeof(unsigned));
	for ( unsigned i = 0; i < 32; i++ )  tids[i] = tids[i + 1];
	//cout << "tMutex::unlock() 2: " << pids[0] << endl;
	if ( tids[0] == 0 ){
		_lock = false;
	//}else{
	// dl("tMutex::unlock -> " + ToString(tids[0]));
	}
	LeaveCriticalSection(&m_cs);
	//cout << ToString(getTid()) << " m::unlock-" << endl;
}// tMutex::unlock



#endif // _WIN32
*/


tSyncString::operator string() const 
{
	std::lock_guard<std::mutex> a(m); 
	return b; 
}// tSyncString::operator string


string tSyncString::operator=(const string &s) 
{
	std::lock_guard<std::mutex> a(m); 
	return (b = s); 
}// tSyncString::operator=




bool tRefCounter::use()
{
	if ( stopped )  return false;
	users++;
	return true;
}// tRefCounter::use


void tRefCounter::wait()
{
	while ( users ){
		if ( stopped )  return;
		tTimer::delay(100);
	}
}// tRefCounter::wait
