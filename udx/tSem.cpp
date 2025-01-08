#include "tSem.h"
#include "tTimer.h"
//#include "tDebug.h"
#include "fString.h"


#ifdef __GNUG__

#include <fcntl.h>
#include <sys/stat.h>




namespace nError {

	SemLockError::SemLockError() : Message("can't lock tSemaph") 
	{
	}// SemLockError::SemLockError

};// namespace nError




#ifdef _SEM_SEMUN_UNDEFINED

union semun
{
	int val;                              // <= value for SETVAL
	struct semid_ds *buf;                 // <= buffer for IPC_STAT & IPC_SET
	unsigned short int *array;            // <= array for GETALL & SETALL
	struct seminfo *__buf;                // <= buffer for IPC_INFO
};

#endif // _SEM_SEMUN_UNDEFINED




tSemaph::tSemaph(unsigned semnumber, bool server) :
	_stop(false),
	_lock(false),
	_server(server),
	name("Sem-" + ToString(semnumber)),
	//mem(semnumber, sizeof(sem_t), server),
	s(0)
{
	//debugLog("sem 1 name=[" + name + "]");
	if ( server ){
		s = semget(semnumber, 1, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG | S_IRWXO);
	}else{
		s = semget(semnumber, 0, S_IRWXU | S_IRWXG | S_IRWXO);
	}
	//debugLog("sem 2 " + ToStringHex((unsigned)s));
	//if ( sem_init(s, 1, 1) == -1 )  s = NULL;
	// debugLog("sem 3 " + ToStringHex((unsigned)s));
	//}
	if ( s == -1 ){
		_stop = true;
		s = 0;
	}else{
		//debugLog("sem 3 " + ToString(semctl(s, 0, GETVAL, NULL)));
		if ( server ){
			struct sembuf sb;
			sb.sem_num = 0;
			sb.sem_op = 1;
			sb.sem_flg = 0;
			semop(s, &sb, 1);
			//debugLog("sem 4 " + ToString(semctl(s, 0, GETVAL, NULL)));
		}
	}
}// tSemaph::tSemaph


tSemaph::~tSemaph()
{
	//debugLog("~sem");
	invalidate();
	unlock();
	if ( s && _server ){
		int ret = semctl(s, 0, IPC_RMID, 0);
		//debugLog("~sem ret=" + ToString(ret));
	}
}// tSemaph::~tSemaph


bool tSemaph::lock()
{
	if ( _stop )  return false;
	if ( _lock )  return true;

	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	//debugLog("sem.lock 1");
	int res = semop(s, &sb, 1);
	//debugLog("sem.lock 2");
	_lock = (res == 0);
	return _lock;
}// tSemaph::lock


bool tSemaph::lock(int timeout)
{
	if ( _stop )  return false;
	if ( _lock )  return true;

	struct sembuf sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = /*SEM_UNDO |*/ IPC_NOWAIT;
	for ( tTimer tm;; Delay(3) ){
		int res = semop(s, &sb, 1);
		if ( res == 0 ){
			_lock = true;
			break;
		}
		if ( tm.check(timeout) ) break;
	}
	return _lock;
}// tSemaph::lock


void tSemaph::unlock()
{
	if ( _lock ){
		struct sembuf sb;
		sb.sem_num = 0;
		sb.sem_op = 1;
		sb.sem_flg = 0;
		semop(s, &sb, 1);
		_lock = false;
	}
}// tSemaph::unlock



#endif // __GNUG__


#ifdef _WIN32



tSemaph::tSemaph(unsigned semnumber, bool server) :
	_stop(false),
	_lock(false),
	_server(server),
	name("Sem-" + ToString(semnumber))
{
	if ( _server ){
		s = CreateSemaphore(NULL, 1, 1, name.c_str());
	}else{
		s = OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, name.c_str());
	}
	int err = GetLastError();
	if ( err == ERROR_ALREADY_EXISTS ){
		CloseHandle(s);
		s = NULL;
	}
	if ( s == NULL )  _stop = true;
}// tSemaph::tSemaph


tSemaph::~tSemaph()
{
	invalidate();
	unlock();
	if ( s != NULL )  CloseHandle(s);
}// tSemaph::~tSemaph


bool tSemaph::lock()
{
	if ( _stop )  return false;
	if ( _lock )  return true;

	DWORD res = WaitForSingleObject(s, INFINITE);
	_lock = (res == WAIT_OBJECT_0);
	return _lock;
}// tSemaph::lock


bool tSemaph::lock(int timeout)
{
	if ( _stop )  return false;
	if ( _lock )  return true;

	DWORD res = WaitForSingleObject(s, timeout);
	_lock = (res == WAIT_OBJECT_0);
	return _lock;
}// tSemaph::lock


void tSemaph::unlock()
{
	if ( _lock ){
		ReleaseSemaphore(s, 1, NULL);
		_lock = false;
	}
}// tSemaph::unlock



#endif // _WIN32
