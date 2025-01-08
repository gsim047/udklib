#ifndef __tSem_h
#define __tSem_h 1

#ifndef __nError_h
#include "nError.h"
#endif

#ifdef __GNUG__
#ifndef _SYS_SEM_H
#include <sys/sem.h>
#endif
#endif // __GNUG__

#ifdef _WIN32
#ifndef _WINDOWS_
#define NOGDI
#include <windows.h>
#endif
#endif // _WIN32




class tSemaph{
public:
	tSemaph(unsigned semnumber, bool server = false);
	virtual ~tSemaph();

private:
	tSemaph(const tSemaph &);
	tSemaph &operator=(const tSemaph &);

public:
	bool Alive() const { return !_stop; };
	bool lock();
	bool lock(int timeout);
	void unlock();
	void invalidate() { _stop = true; };

protected:
	bool _server;
	bool _lock;
	bool _stop;
	string name;
#ifdef __GNUG__
	int s;
#endif
#ifdef _WIN32
	HANDLE s;
#endif
};// class tSemaph




namespace nError
{
	class SemLockError : public Message
	{
	public:
		SemLockError();
	};// class SemLockError

}// namespace nError




class tSemLock{
public:
	tSemLock(tSemaph &l) : _lock(l) {
	  result = _lock.lock(); if ( !result ) nError::Throw(nError::SemLockError()); };
	tSemLock(tSemaph &l, int timeout) : _lock(l) {
	  result = _lock.lock(timeout); };
	virtual ~tSemLock() { if ( result ) _lock.unlock(); };

private:
	tSemLock(const tSemLock &);
	tSemLock &operator=(const tSemLock &);

public:
	bool operator!() const { return !result; };

protected:
	tSemaph &_lock;
	bool result;
};// class tSemLock




#endif // __tSem_h
