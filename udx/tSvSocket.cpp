//#include "fTime.h"

#include <errno.h>
#include <stdio.h>

#ifdef __GNUG__
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#endif // __GNUG__

#ifdef _WIN32
#include <io.h>
#define NOGDI
#define FD_SETSIZE    (256)
#include <winsock2.h>
#include <windows.h>
#endif

#include "tSvSocket.h"

#include <list>
#include <vector>
#include <algorithm>
#include <sys/select.h>

#include "__tSocket.cpp"
#include "fString.h"
#include "tThread.h"
#include "tDebug.h"
#include "tSockConn.h"
#include "tTimer.h"
#include "tMutex.h"





class __rwThread : public tThread{
public:
	__rwThread(const string &s) : tThread(s) {};

	void run();

	static void init();
};// class __rwThread




static void tm2timeval(int tm, struct timeval &timeout)
{
	if ( tm < 0 ) tm = 0;
	timeout.tv_sec = 0;
	timeout.tv_usec = (unsigned)tm * 1000;
}// tm2timeval

static void fdClr(fd_set *f1, fd_set *f2, fd_set *f3)
{
	if ( f1 )  FD_ZERO(f1);
	if ( f2 )  FD_ZERO(f2);
	if ( f3 )  FD_ZERO(f3);
}// fdClr

static void fdAdd(int s, fd_set *f1, fd_set *f2, fd_set *f3)
{
	if ( f1 ){
		FD_SET(((unsigned)s), f1);
	}
	if ( f2 ){
		FD_SET(((unsigned)s), f2);
	}
	if ( f3 ){
		FD_SET(((unsigned)s), f3);
	}
}// fdAdd





class __tSSocket : public tRefCounter{
	friend class tSSocket;
	friend class __rwThread;

private:
  __tSSocket() : p(NULL) {};
  __tSSocket(__tSocket *s);
  __tSSocket(tSocket *zs);

public:
  ~__tSSocket();

private:
  __tSSocket(const __tSSocket &);
  __tSSocket &operator=(const __tSSocket &);

	static bool erase(__tSSocket *);
	static void add(__tSSocket *);

public:
	operator __tSocket*() const { return p; };
	__tSocket *operator->() const { return p; };

	void close() { readDisabled = true; if ( p ) p->close(); };
	void closeWait(int timeout);
	static void closeAll();
	static void closeAllWait(int timeout);
	bool Alive() const { return p != NULL && p->Alive(); };
	bool operator!() const { return !Alive(); };
	int handle() const { return Alive() ? p->handle() : -1; }

	void setOutput(tByteArray, int packMethod = 4);
	bool tryToOut(int Timeout);

	bool tryToGet(int Timeout);
	tByteArray getInput();

	static bool pkOk(tByteArray &ba, unsigned *size = NULL);
	static tByteArray getPk(tByteArray &ba);

private:
	__tSocket *p;

	std::mutex vlock;
	std::vector<tByteArray> poolOut;
	bool readDisabled;
	std::mutex ilock;
	tByteArray inBuffer;

	static std::mutex _listMutex;
	//static tPList<__tSSocket> _list;
	static std::list<__tSSocket *> _list;
};// class __tSSocket


std::mutex __tSSocket::_listMutex;
//tPList<__tSSocket> __tSSocket::_list;
std::list<__tSSocket *> __tSSocket::_list;


__tSSocket::__tSSocket(__tSocket *s) :
  p(s),
  readDisabled(false)
{
	if ( Alive() ){
		p->use();
	}else{
		p = NULL;
	}
}// __tSSocket::__tSSocket


__tSSocket::__tSSocket(tSocket *zs) :
  p(zs ? zs->p : NULL),
  readDisabled(false)
{
	if ( Alive() ){
		p->use();
	}else{
		p = NULL;
	}
}// __tSSocket::__tSSocket


__tSSocket::~__tSSocket()
{
	if ( p ){
		p->unuse();
		//if ( !p->used() )  __tSocket::erase(p);
		if ( !p->used() )  delete p;
	}
}// __tSSocket::~__tSSocket


bool __tSSocket::erase(__tSSocket *p)
{
	if ( p ){
		std::lock_guard<std::mutex> ml(_listMutex);
		std::list<__tSSocket *>::iterator i = std::find(__tSSocket::_list.begin(),
		                                                __tSSocket::_list.end(), p);
		if ( i != __tSSocket::_list.end() ){
			//__tSSocket::_list.erase(i);
			debugLog("__tSSocket::_list - socket erased.  __tSSocket::_list.size()=" + 
			         ToString(__tSSocket::_list.size()));
			delete p;
			return true;
		}
	}
	return false;
}// __tSSocket::erase


void __tSSocket::add(__tSSocket *p)
{
	if ( p && p->Alive() ){
		__rwThread::init();
		std::lock_guard<std::mutex> ml(_listMutex);
		__tSSocket::_list.push_back(p);
		debugLog("__tSSocket::_list - socket added.  __tSSocket::_list.size()=" +
		         ToString(__tSSocket::_list.size()));
	}
}// __tSSocket::add


void __tSSocket::closeWait(int timeout)
{
	readDisabled = true;

	for ( tTimer tm; !tm.check(timeout) && !poolOut.empty(); tTimer::delay(100) );
	close();
}// __tSSocket::closeWait


void __tSSocket::closeAll()                   // static
{
	std::lock_guard<std::mutex> ml(_listMutex);
	for ( std::list<__tSSocket *>::iterator i = _list.begin();
	      i != _list.end(); ++i ){
		//debugLog("closeAll: close");
		(*i)->close();
	}
}// __tSSocket::closeAll


void __tSSocket::closeAllWait(int timeout)    // static
{
	tTimer tm;
	bool alive;
	do{
		alive = false;
		std::lock_guard<std::mutex> ml(_listMutex);
		for ( std::list<__tSSocket *>::iterator i = _list.begin();
		      i != _list.end(); ++i ){
			if ( (*i)->Alive() ){
				alive = true;
				if ( !tm.check(timeout) && !(*i)->poolOut.empty() ) continue;
			//debugLog("closeAll: close");
				(*i)->close();
			}
		}
		if ( alive )  tTimer::delay(100);
	}while( alive );
}// __tSSocket::closeAllWait


void __tSSocket::setOutput(tByteArray a, int packMethod)
{
	char pack = '\0';
	if ( p->usezip )
		if ( a.size() >= 1024 && a.zip(packMethod) )  pack = '\x01';
	tByteArray ba('\0');
	ba << unsigned(a.size() + 10) << pack << a << (unsigned)a.calcCrc32();
	std::lock_guard<std::mutex> ll(vlock);
	poolOut.push_back(ba);
	//debugLog("setOutput: " + BufDump(ba.data(), ba.size()));
	debugLog("setOutput: +" + ToString(ba.size()) + " bytes");
}// __tSSocket::setOutput


bool __tSSocket::tryToOut(int Timeout)
{
	if ( poolOut.empty() ) return false;

	try{
		std::lock_guard<std::mutex> ll(vlock);
		unsigned i = p->writetm(poolOut[0], Timeout);
		if ( i > 0 ){
			//debugLog("tryToOut: " + ToString(i) + " bytes");
			poolOut[0].erase(0, i);
			if ( poolOut[0].empty() ){
				poolOut.erase(poolOut.begin());
				//pknumber++;
				return true;
			}
		}
		return true;
	}
	catch ( nError::IOException &e ){
		debugLog("__tSSocket::tryToOut: " + e);
		close();
		return false;
	}
}// __tSSocket::tryToOut


bool __tSSocket::tryToGet(int Timeout)
{
	try{
		if ( !readDisabled ){
			string s = p->readtm(0x8000, Timeout);
			if ( !s.empty() ){
				//debugLog("tryToGet: " + ToString(s.size()) + " bytes");
				std::lock_guard<std::mutex> ll(ilock);
				inBuffer += s;
			}
		}
		std::lock_guard<std::mutex> ll(ilock);
		return pkOk(inBuffer);
	}
	catch ( nError::IOException &e ){
		debugLog("__tSSocket::tryToGet: " + e);
		close();
		return false;
	}
}// __tSSocket::tryToGet


tByteArray __tSSocket::getInput()
{
	std::lock_guard<std::mutex> ll(ilock);
	return getPk(inBuffer);
}// __tSSocket::getInput


bool __tSSocket::pkOk(tByteArray &ba, unsigned *size)
{
	//debugLog("pkOk: " + BufDump(ba.data(), ba.size()));
	unsigned i = 0;
	char c;
	if ( ba.get(i, c) ){
		if ( c != '\0' ){
			ba.clear();
			return false;
		}
		unsigned pkSize;
		if ( ba.get(i, pkSize) ){
			if ( pkSize < 10 || pkSize > 0x40000 ){
				ba.clear();
				return false;
			}
			if ( pkSize <= ba.size() ){
				i = pkSize - 4;
				unsigned crc32;
				if ( ba.get(i, crc32) && crc32 == (unsigned)ba.calcCrc32(6, pkSize - 10) ){
					if ( size != NULL ) *size = pkSize;
					//debugLog("pkOk - ok size = " + ToString(pkSize));
					return true;
				}
				ba.clear();
			}
		}
	}
	return false;
}// __tSSocket::pkOk


tByteArray __tSSocket::getPk(tByteArray &ba)
{
	tByteArray ret;
	unsigned pkSize;
	if ( pkOk(ba, &pkSize) ){
		//debugLog("getPk: " + BufDump(ba.data(), ba.size()));
		ret = tByteArray(ba.data() + 6, pkSize - 10);          // del envelope
		if ( ba[5] != '\0' )  ret.unzip();
		ba.erase(0, pkSize);
	}
	//debugLog("getPk size = " + ToString(ret.size()));
	return ret;
}// __tSSocket::getPk



/*
tSSocket::tSSocket(__tSocket *s)
{
	p = new __tSSocket(s);
	if ( !p->Alive() ){
		delete p;
		p = NULL;
	}else{
		__tSSocket::add(p);
		p->use();
	}
}// tSSocket::tSSocket
*/

tSSocket::tSSocket(const string &adr, int port, bool stream) :
  p(NULL)
{
	tSocket ts(stream, "", 0, adr, port);
	if ( ts.Alive() ){
		p = new __tSSocket(&ts);
		if ( p->Alive() ){
			__tSSocket::add(p);
			p->use();
		}else{
			delete p;
			p = NULL;
		}
	}
}// tSSocket::tSSocket


tSSocket::tSSocket(tSocket &zs) :
  p(new __tSSocket(&zs))
{
	if ( p->Alive() ){
		__tSSocket::add(p);
		p->use();
	}else{
		delete p;
		p = NULL;
	}
}// tSSocket::tSSocket


tSSocket::tSSocket(__tSSocket *zs) :
  p(zs)
{
	use();
}// tSSocket::tSSocket


static std::mutex __y;


tSSocket::~tSSocket()
{
	std::lock_guard<std::mutex> l(__y);
	if ( p ){
		p->unuse();
		if ( !p->used() )  __tSSocket::erase(p);
	}
}// tSSocket::~tSSocket


tSSocket::tSSocket(const tSSocket &src) :
  p(src.p)
{
	use();
}// tSSocket::tSSocket


tSSocket &tSSocket::operator=(const tSSocket &src)
{
	if ( this != &src ){
		if ( p ){
			p->unuse();
			if ( !p->used() )  __tSSocket::erase(p);
		}
		p = src.p;
		use();
	}
	return *this;
}// tSSocket::operator=


void tSSocket::use()
{
	if ( p && p->Alive() ){
		p->use();
	}else{
		p = NULL;
	}
}// tSSocket::use


void tSSocket::close()
{
	if ( Alive() ) p->close();
}// tSSocket::close


void tSSocket::closeWait(int timeout)
{
	if ( Alive() ) p->closeWait(timeout);
}// tSSocket::closeWait


void tSSocket::closeAll()                     // static
{
	__tSSocket::closeAll();
}// tSSocket::closeAll


void tSSocket::closeAllWait(int timeout)      // static
{
	__tSSocket::closeAllWait(timeout);
}// tSSocket::closeAllWait


bool tSSocket::Alive() const
{
	return p != NULL && p->Alive();
}// tSSocket::Alive


void tSSocket::setOutput(const tByteArray &ba)
{
	if ( Alive() ) p->setOutput(ba);
}// tSSocket::setOutput


tByteArray tSSocket::getInput(int timeout)
{
	tByteArray ba;
	if ( !Alive() )  return ba;
	tTimer ti;
	while ( !ti.check(timeout) ){
		if ( !Alive() )  return ba;
		ba = p->getInput();
		if ( !ba.empty() )  return ba;
		tTimer::delay(10);
	}
	//debugLog("tSSocket::getInput(tm) ti=" + ToString(ti.get()));
	return p->getInput();
}// tSSocket::getInput




void __rwThread::run()
{
	debugLog("__rwThread::run");
	while ( !testStop() ){
		bool em;
		{
			std::vector<tSSocket> rwlist;
			try{
				fd_set fdR;
				fd_set fdW;
				fd_set fdE;
				fdClr(&fdR, &fdW, &fdE);
				{
					std::lock_guard<std::mutex> ll(__tSSocket::_listMutex);
					//for ( tPList<__tSSocket>::iterator it = __tSSocket::_list.begin();
					for ( std::list<__tSSocket *>::iterator it = __tSSocket::_list.begin();
					      it != __tSSocket::_list.end(); ++it ){
						if ( (*it)->Alive() ){
							fdAdd((*it)->handle(), &fdR, &fdW, &fdE);
							rwlist.push_back(tSSocket(*it));
						}
					}
				}

				if ( !rwlist.empty() ){
					struct timeval timeout;
					tm2timeval(10, timeout);
					if ( select(65, &fdR, &fdW, &fdE, &timeout) == -1 ){      // 65 ???
						nError::IOException e("__rwThread::run: select error");
						throw e;
					}

					for ( unsigned i = 0; i < rwlist.size(); i++ ){
						if ( !rwlist[i].Alive() ) continue;
						int s = rwlist[i].p->handle();
						if ( FD_ISSET(s, &fdE) )  rwlist[i].close();
						if ( FD_ISSET(s, &fdR) )  rwlist[i].p->tryToGet(0);               // 10
						if ( FD_ISSET(s, &fdW) )  rwlist[i].p->tryToOut(0);               // 10
					}// for  rqw
				}
			}
			catch ( nError::Message &e ){
				debugLog("__rwThread::run error: " + e);
			}
			catch ( ... ){
				debugLog("__rwThread::run Error !!!");
			}
			em = rwlist.empty();
		}
		//if ( __tSSocket::_list.empty() ) break;
		tTimer::delay(em ? 150 : 10);
	}
	debugLog("__rwThread::run exiting...");
	tSSocket::closeAll();
}// __rwThread::run


void __rwThread::init()
{
	static bool inited = false;
	if ( !inited ){
		inited = true;
		__rwThread *th = new __rwThread("__rwThread");
		th->setDaemon();
		th->start();
	}
}// __rwThread::init




tSSocket &operator<<(tSSocket &so, tPk *p)
{
	if ( p ){
		if ( so.Alive() )  so.setOutput(p->put());
		delete p;
	}
	return so;
}// operator<<


tPk *getPk(tSSocket &s, int timeout)
{
	tByteArray ba = s.getInput(timeout);
	return pkReceive(ba, new tSocketConnection(s));
}// getPk
