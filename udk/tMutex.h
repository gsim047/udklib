#ifndef __tMutex_h
#define __tMutex_h 1

#include "nError.h"

#include <mutex>


/*
class tMutex{
	friend class tMutexLock;
public:
	tMutex();
	virtual ~tMutex();

private:
	tMutex(const tMutex &);
	tMutex &operator=(const tMutex &);

	bool lock();
	bool lock(int timeout);
	void unlock();

public:
	void invalidate() { _stop = true; };

#ifdef __GNUG__
protected:
	pthread_mutex_t _lock;
	bool _stop;
#endif // __GNUG__

#ifdef _WIN32
private:
	bool setMark(unsigned tid);
	void clearMark(unsigned tid);

protected:
	bool _lock;
	CRITICAL_SECTION m_cs;
	bool _stop;
	unsigned *tids; //[33];
#endif // _WIN32
};// class tMutex
*/



namespace nError
{
	class LockError : public Message {
	public:
		LockError();
	};// class LockError

	class DeadLock : public Message {
	public:
		DeadLock();
	};// class DeadLock
}// namespace nError


/*
class tMutexLock{
public:
	tMutexLock(tMutex &l) : _lock(l) { locked = _lock.lock(); }
	tMutexLock(tMutex &l, int timeout) : _lock(l) { locked = _lock.lock(timeout); }
	virtual ~tMutexLock() { if ( locked )  _lock.unlock(); }

private:
	tMutexLock(const tMutexLock &);
	tMutexLock &operator=(const tMutexLock &);

public:
	bool operator!() const { return !locked; };

protected:
	tMutex &_lock;
	bool locked;
};// class tMutexLock
*/


/*
class tSyncBool {
public:
	tSyncBool() : b(false) {};
	tSyncBool(bool init) : b(init) {};
	tSyncBool(const tSyncBool &s) : b(bool(s)) {};

public:
	operator bool() const { std::lock_guard<std::mutex> a(m); return b; }
	bool operator=(bool s) { tMutexLock a(m); return (b = s); }
	bool operator!() const { tMutexLock a(m); return !b; }

protected:
	std::mutex m;
	bool b;
};// class tSyncBool
*/



class tSyncInt {
public:
	tSyncInt() : i(0) {};
	tSyncInt(int init) : i(init) {};
	tSyncInt(const tSyncInt &s) : i(int(s)) {};

public:
	operator int() const { return i; }
	int operator=(int src) { return (i = src); }
	int operator++() { return ++i; }
	int operator--() { return --i; }
	int operator++(int) { return i++; }
	int operator--(int) { return i--; }
	int operator+=(int s) { std::lock_guard<std::mutex> a(m); return (i += s); }
	int operator-=(int s) { std::lock_guard<std::mutex> a(m); return (i -= s); }
	int operator*=(int s) { std::lock_guard<std::mutex> a(m); return (i *= s); }
	int operator/=(int s) { std::lock_guard<std::mutex> a(m); return (i /= s); }
	int operator%=(int s) { std::lock_guard<std::mutex> a(m); return (i %= s); }

	bool operator==(int s) const { return (i == s); }
	bool operator!=(int s) const { return !operator==(s); };

protected:
	mutable std::mutex m;
	int i;
};// class tSyncInt




class tSyncString {
public:
	tSyncString() {};
	tSyncString(const string &init) : b(init) {};
	tSyncString(const tSyncString &s) : b(string(s)) {};

public:
	operator string() const;
	string operator=(const string &s);

	size_t size() const { std::lock_guard<std::mutex> a(m); return b.size(); }
	bool empty() const { std::lock_guard<std::mutex> a(m); return b.empty(); }

	bool operator==(const string &s) const { std::lock_guard<std::mutex> a(m); return (b == s); }
	bool operator!=(const string &s) const { return !operator==(s); };

protected:
	mutable std::mutex m;
	string b;
};// class tSyncString




class tRefCounter{                       // multy-thread access object
public:
	tRefCounter() {};
	virtual ~tRefCounter() { wait(); };

private:
	tRefCounter(const tRefCounter &);
	tRefCounter &operator=(const tRefCounter &);

public:
	bool used() const { return (users != 0); };
	bool use();                              // a procedure wants to use the object
	void unuse() { if ( users )  users--; }; // a procedure ends using the object
	void stop() { stopped = true; };         // signals need to stop
	void wait();                             // waits for all components exit
	bool stopping() const { return stopped; };
	int count() const { return users; };

protected:
	tSyncInt users;
	bool stopped;
};// class tRefCounter




template <class T> class tRefPtr{
public:
	tRefPtr(T *link) : t(link) { if ( t && !t->use() )  t = NULL; };
	tRefPtr(const tRefPtr &Ptr) : t(Ptr.t) { if ( t && !t->use() )  t = NULL; };
	virtual ~tRefPtr() { if ( t ){ t->unuse(); if ( !t->used() )  delete t; } };

	tRefPtr &operator=(T *l) {
		if ( t != l ){
			if ( t ){ t->unuse(); if ( !t->used() )  delete t; }
			t = l;
			if ( t ){ if ( !t->use() )  t = NULL; }
		}
		return *this;
	}

	tRefPtr &operator=(const tRefPtr &src) {
		if ( this != &src ){
			if ( t ){ t->unuse(); if ( !t->used() )  delete t; }
			t = src.t;
			if ( t ){ if ( !t->use() )  t = NULL; }
		}
		return *this;
	}

public:
	operator T*() const { return t; };
	T *operator->() const { return t; };

	bool operator==(const tRefPtr &s) const {
		bool ret = false;
		if ( !t && !s.t ){
			ret = true;
		}else{
			if ( t && s.t ){
				ret = (*t == *s.t);
			}
		}
		return ret;
	}
	bool operator!=(const tRefPtr &s) const { return !operator==(s); }

	bool operator!() const { return t == NULL; };

private:
	T *t;
};// class tRefPtr




#endif // __tMutex_h
