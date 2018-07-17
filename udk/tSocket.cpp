#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "tSocket.h"
#include "tTimer.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

#ifdef __GNUG__
#include <unistd.h>
#include <sys/select.h>
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

#ifdef errno 
#undef errno
#define errno WSAGetLastError()
#endif

#endif

#include <list>
#include <vector>
#include <algorithm>
#include <mutex>

#include "__tSocket.cpp"
#include "fString.h"
//#include "tThread.h"
//#include "tPList.h"
#include "tDebug.h"



//static void debugLog(const string &s)
//{
//}//



static bool __getpeername(int s, string &addr, int &port)
{
	struct sockaddr_in name;
	memset(&name, 0, sizeof(name));
	__tSocket::elmlen plen = sizeof(name);
	if ( ::getpeername(s, (struct sockaddr *)&name, &plen) < 0 )  return false;
	addr = inet_ntoa(name.sin_addr);
	port = (int) ntohs(name.sin_port);
	printf("::peername for socket [%d] - %s:%d [[]]\n", s, addr.c_str(), port);
	return true;
}// __getpeername


static bool __getsockname(int s, string &adr, int &port)
{
	struct sockaddr addr;
	__tSocket::elmlen adrlen = sizeof(addr);
	if ( ::getsockname(s, &addr, &adrlen ) < 0 )  return false;
	struct sockaddr_in *nptr = (struct sockaddr_in *) &addr;
	adr = inet_ntoa(nptr->sin_addr);
	port = (int) ntohs(nptr->sin_port);
	printf("::sockname for socket [%d] - %s:%d [[]]\n", s, adr.c_str(), port);
	return true;
}// __getsockname




static void __init_WSA()
{
#ifdef _WIN32
	static bool inited = false;
	if ( !inited ){
		inited = true;
		WORD wVersionRequested = MAKEWORD(2, 0);

		WSADATA wsaData;
		int err = WSAStartup(wVersionRequested, &wsaData);
		if ( err != 0 ){
			nError::Message e("can't init WSA. error " + ToString(err));
			throw e;
		}
/*
struct WSAData {
    WORD            wVersion;
    WORD            wHighVersion;
    char            szDescription[WSADESCRIPTION_LEN+1];
    char            szSystemStatus[WSASYSSTATUS_LEN+1];
    unsigned short  iMaxSockets;
    unsigned short  iMaxUdpDg; 
    char FAR *      lpVendorInfo;
};*/
		/*
  debugLog("Using WINSOCK ver " +
           ToString(LOBYTE(wsaData.wVersion)) + "." +
           ToString(HIBYTE(wsaData.wVersion )));
  debugLog("wsaData.wHighVersion " + ToString(wsaData.wHighVersion));
  debugLog("wsaData.iMaxSockets " + ToString(wsaData.iMaxSockets));
  debugLog("wsaData.iMaxUdpDg " + ToString(wsaData.iMaxUdpDg)); */
	}
#endif
}// __init_WSA


/*
class __init_tSocket{
public:
	__init_tSocket();
	~__init_tSocket();

	void init();

private:
	//static int cnt;
	static bool inited;
};// class __init_tSocket

bool __init_tSocket::inited = false;

static __init_tSocket __init_tSocket_ex;
*/



//int __init_tSocket::cnt = 0;



static const int __atomic_timeout = 3;

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



namespace nError {

	IOException::IOException() : Message("tSocket::IOException") 
	{
	}// IOException::IOException

	IOException::IOException(const string &s) : Message("tSocket::IOException: " + s) 
	{
	}// IOException::IOException

};// namespace nError


/*
class __tSocket : public tRefCounter
{
	//friend class tServerSocket;
	friend class tSocket;
	friend class __tSSocket;

	static tMutex _listMutex;
	static std::list<__tSocket *> _list;

private:
	//__tSocket(const string &adr, int port, bool stream = true);
	__tSocket(bool tcp = true);
	__tSocket(int i);

public:
	~__tSocket();

private:
	__tSocket(const __tSocket &);
	__tSocket &operator=(const __tSocket &);

	static bool erase(__tSocket *);
	static void add(__tSocket *);

	void _Throw() const;
	void _Throw(const string &info) const;

	int _read();
	string _read(unsigned n);
	int _write(char b);
	int _write(const string &b);
	int _writetm(char b, int timeout = 0);

	int _socket(bool tcp) const;

	bool setOption(int opt, int val = 1);
	bool getOption(int opt, int &val) const;

public:
#ifdef __GNUG__
#ifdef __CYGWIN__
	typedef int elmlen;
#else
	typedef unsigned elmlen;
#endif
#endif
#ifdef _WIN32
	typedef int elmlen;
#endif // _WIN32

	void close(); 
	int handle() const { return s; }
	string getInetAddress() const;
	string getLocalAddress() const;
	int getLocalPort() const;
	int getPort() const;
	int getReceiveBufferSize() const;
	int getSendBufferSize() const;
	int getMaxMsgSize() const;

	int setReceiveBufferSize(int size);
	int setSendBufferSize(int size);

	bool setReuseAddress(int v = 1);
	bool setNoBlock(int v = 1);
	bool setKeepAlive(int v = 1);
	bool setBroadcast(int v = 1);

	bool getpeername(string &addr, int &port) const;
	bool getsockname(string &addr, int &port) const;
	bool bind(const string &adr, int p);
	bool connect(const string &adr, int p);

	int accept(int tm);

	bool availableRead(int timeout = 0) const;
	bool availableWrite(int timeout = 0) const;

	int read();
	string read(unsigned size);
	int readtm(int timeout = 0);          
	string readtm(unsigned size, int timeout = 0);
	string readFrom(string &addr, int &port);

	int write(char b);
	int write(const string &b);
	int writetm(char b, int timeout = 0);
	int writetm(const string &b, int timeout = 0);
	int writeTo(const string &b, const string &addr, int port);

	bool Alive() const { return alive; }
	string error() const { return lasterror; };

private:
	mutable bool alive;
	mutable tMutex m1;
	mutable tMutex m2;
	mutable tMutex m3;
	int s;
	bool tcp;
	mutable string lasterror;
};// class __tSocket
*/

bool __tSocket::usezip = true;



__tSocket::__tSocket(bool Tcp) :
	s(_socket(Tcp)),
	tcp(Tcp),
	alive(false)
{
	if ( s <= 0 ){
		lasterror = "Can't open socket";  return;
	}

	alive = true;
	//debugLog("open socket [" + ToString(s) + "] - " +
	//         getInetAddress() + ":" + ToString(getPort()) + " / " +
	//         getLocalAddress() + ":" + ToString(getLocalPort()));
}// __tSocket::__tSocket


__tSocket::__tSocket(int i) :
	s(i),
	tcp(true),
	alive(false)
{
	if ( s <= 0 ){
		lasterror = "Can't open socket";  return;
	}

	alive = true;
	//debugLog("open socket [" + ToString(s) + "] - " +
	//         getInetAddress() + ":" + ToString(getPort()) + " / " +
	//         getLocalAddress() + ":" + ToString(getLocalPort()));
}// __tSocket::__tSocket


int __tSocket::_socket(bool tcp)
{
	//__init_tSocket_ex.init();
	__init_WSA();
	return ::socket(AF_INET, tcp ? SOCK_STREAM : SOCK_DGRAM, 0); 
}// __tSocket::_socket


string __tSocket::getInetAddress() const 
{
	string ar;
	int pt;
	if ( !getpeername(ar, pt) )  return "?.?.?.?";
	return ar; 
}// __tSocket::getInetAddress


int __tSocket::getPort() const 
{
	string ar;
	int pt;
	if ( !getpeername(ar, pt) )  return -1;
	return pt; 
}// __tSocket::getPort


string __tSocket::getLocalAddress() const 
{
	string la;
	int lp;
	if ( !getsockname(la, lp) )  return "?.?.?.?";
	return la; 
}// __tSocket::getLocalAddress


int __tSocket::getLocalPort() const 
{
	string la;
	int lp;
	if ( !getsockname(la, lp) )  return -1;
	return lp; 
}// __tSocket::getLocalPort


bool __tSocket::bind(const string &_adr, int p)
{
	string adr = trim(_adr);
	struct sockaddr_in myname;
	memset(&myname, 0, sizeof(myname));
	myname.sin_family = AF_INET;
	if ( adr.empty() ){
		myname.sin_addr.s_addr = INADDR_ANY;
	}else{
		myname.sin_addr.s_addr = inet_addr(adr.c_str());
	}
	myname.sin_port = htons(p);
	if ( ::bind(s, (struct sockaddr *) &myname, sizeof(myname)) < 0 ){
		alive = false;
		lasterror = "bind: 'bind' error";  return false;
	}
	return true;
}// __tSocket::bind


bool __tSocket::connect(const string &Adr, int p)
{
	string adr = trim(Adr);
	struct sockaddr_in myname;
	memset(&myname, 0, sizeof(myname));
	myname.sin_family = AF_INET;
	myname.sin_port = htons(p);

	struct hostent *entry = gethostbyname(adr.c_str());
	if ( entry == NULL ){
		alive = false;
		lasterror = "connect: 'gethostbyname' error";  return false;
	}

	string _adr = inet_ntoa(*((in_addr *)entry->h_addr));
	myname.sin_addr.s_addr = inet_addr(_adr.c_str());
	if ( ::connect(s, (struct sockaddr *)&myname, sizeof(myname) ) < 0 ){
		alive = false;
		lasterror = "connect: 'connect' error";  return false;
	}
	return true;
}// __tSocket::connect


bool __tSocket::getpeername(string &addr, int &port) const
{
	struct sockaddr_in name;
	memset(&name, 0, sizeof(name));
	elmlen plen = sizeof(name);
	if ( ::getpeername(s, (struct sockaddr *)&name, &plen) < 0 )  return false;
	addr = inet_ntoa(name.sin_addr);
	port = (int) ntohs(name.sin_port);
	return true;
}// __tSocket::getpeername


bool __tSocket::getsockname(string &adr, int &port) const
{
	struct sockaddr addr;
	elmlen adrlen = sizeof(addr);
	if ( ::getsockname(s, &addr, &adrlen ) < 0 )  return false;
	struct sockaddr_in *nptr = (struct sockaddr_in *) &addr;
	adr = inet_ntoa(nptr->sin_addr);
	port = (int) ntohs(nptr->sin_port);
	return true;
}// __tSocket::getsockname


bool __tSocket::listen()
{
	if ( ::listen(s, 5) == -1 ){
		alive = false;
		lasterror = "listen: 'listen' error";  return false;
	}
	return true;
}// __tSocket::listen


int __tSocket::accept(int tm)
{
	std::lock_guard<std::mutex> mr(m2);
	if ( !Alive() )  return -1;
	if ( tm < 0 )  return -1;

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = (unsigned)tm * 1000;
	fd_set tSet;
	fd_set tExc;
	FD_ZERO(&tSet);
	FD_SET(((unsigned)s), &tSet);
	FD_ZERO(&tExc);
	FD_SET(((unsigned)s), &tExc);
	int q;
	if ( tm == 0 ){
		q = select(s + 1, &tSet, (fd_set *)NULL, &tExc, NULL);
	}else{
		q = select(s + 1, &tSet, (fd_set *)NULL, &tExc, &timeout);
	}
	if ( q == -1 )  _Throw("select error");
	if ( FD_ISSET(s, &tExc) )  _Throw("socket exception");
	if ( FD_ISSET(s, &tSet) ){
		struct sockaddr_in c_addr;
		elmlen sa_size = sizeof(c_addr);
		int rc = ::accept(s, (struct sockaddr*) &c_addr, &sa_size);
		if ( rc < 0 )  _Throw("accept error");
		printf("::accept %d [[[]]]\n", rc);
		string addr; int port;
		__getpeername(rc, addr, port);
		__getsockname(rc, addr, port);
		return rc;
	}
	return -1;
}// __tSocket::accept


void __tSocket::_Throw() const
{
	alive = false;
	lasterror = "IOException";
	debugLog("_Throw: " + lasterror);
	nError::IOException e;
	throw e;
}// __tSocket::_Throw


void __tSocket::_Throw(const string &s) const
{
	alive = false;
	lasterror = s;
	debugLog("_Throw: " + lasterror);
	nError::IOException e(s);
	throw e;
}// __tSocket::_Throw


int __tSocket::_read()                // read 1 byte, else throw
{
	char r;
	int err = ::recv(s, &r, 1, 0);
	if ( err != 1 ){
		_Throw("_read errno: " + ToString(errno) + ", err = " + ToString(err));
		//debugLog("__tSocket::_read() error");
	}
	int i = (unsigned char) r;
	//debugLog("__tSocket::_read() [" + ToString(i) + "]");
	return i;
}// __tSocket::_read


string __tSocket::_read(unsigned n)   // read n bytes
{
	char *r = new char[n];
	unsigned i = 0;
	for ( ; i < n; ){
		bool ar;
		try{
			ar = availableRead(0);
		}
		catch( nError::Message & ){
			delete [] r;
			throw;
		}
		if ( ar ){
			int err = ::recv(s, r + i, n - i, 0);
			//if ( err < 1 && errno != EAGAIN && errno != 0 ){
			if ( err < 1 /*&& errno != EAGAIN && errno != 0*/ ){
				delete [] r;
				_Throw("_read errno: " + ToString(errno) + ", err = " + ToString(err));
			}
			if ( err == 0 ){
				delete [] r;
				_Throw("_read errno: " + ToString(errno) + ", err = " + ToString(err));
			}
			if ( err > 0 ){
				i += err;
				continue;
			}
		}
		tTimer::delay(__atomic_timeout);
	}
	string ret(r, i);
	delete [] r;
	return ret;
}// __tSocket::_read


int __tSocket::read()
{
	std::lock_guard<std::mutex> mr(m2);
	return _read();
}// __tSocket::read


string __tSocket::read(unsigned i)
{
	std::lock_guard<std::mutex> mr(m2);
	return _read(i);
}// __tSocket::read


int __tSocket::readtm(int tm)         // read 1 byte with timeout
{
	std::lock_guard<std::mutex> mr(m2);
	if ( availableRead(tm) )  return _read();
	return -1;
}// __tSocket::readtm


string __tSocket::readtm(unsigned n, int tm)  // read n bytes with timeout
{
	if ( n < 1 || tm < 0 )  return "";
	char *r = new char[n];
	std::lock_guard<std::mutex> mr(m2);

	unsigned i = 0;
	for ( tTimer t; i < n; ){
		bool ar;
		try{
			ar = availableRead(0);
		}
		catch( nError::Message & ){
			delete [] r;
			throw;
		}
		if ( ar ){
			int err = ::recv(s, r + i, n - i, 0);
			//if ( err <= 0 ){
			// debugLog("_read sleep, errno: " + ToString(errno) + ", err = " + ToString(err));
			//}
			//if ( err < 0 && errno != EAGAIN && errno != 0 ){
			if ( err < 0 /*&& errno != EAGAIN && errno != 0*/ ){
				delete [] r;
				_Throw("readtm errno: " + ToString(errno) + ", err = " + ToString(err));
			}
			if ( err == 0 ){
				delete [] r;
				_Throw("readtm2 errno: " + ToString(errno) + ", err= " + ToString(err) +
				       ", n-i=" + ToString(n - i));
			}
			if ( err > 0 ){
				i += err;
				continue;
			}
		}
		if ( tm < __atomic_timeout ) break;
		tTimer::delay(__atomic_timeout);
		if ( t.check(tm) ) break;
	}
	string ret(r, i);
	delete [] r;
	return ret;
}// __tSocket::readtm


string __tSocket::readFrom(string &addr, int &port)
{
	unsigned n = getMaxMsgSize();
	if ( n < 1 )  return "";
	char *r = new char[n];
	std::lock_guard<std::mutex> mr(m2);

	unsigned i = 0;
	bool ar;
	try{
		ar = availableRead(0);
	}
	catch( nError::Message & ){
		delete [] r;
		throw;
	}
	if ( ar ){
		struct sockaddr_in from;
		memset(&from, 0, sizeof(from));
		elmlen fromlen = sizeof(from);

		int err = ::recvfrom(s, r, n, 0, (struct sockaddr *)&from, &fromlen);
		//if ( err <= 0 ){
		// debugLog("_read sleep, errno: " + ToString(errno) + ", err = " + ToString(err));
		//}
		//if ( err < 0 && errno != EAGAIN && errno != 0 ){
		if ( err < 0 && errno != 0 ){
			delete [] r;
			_Throw("readFrom errno: " + ToString(errno) + ", err = " + ToString(err));
		}
		if ( err == 0 ){
			delete [] r;
			_Throw("readFrom2 errno: " + ToString(errno) + ", err= " + ToString(err));
		}
		if ( err > 0 ){
			i = err;
			addr = inet_ntoa(from.sin_addr);
			port = (int) ntohs(from.sin_port);
		}
	}
	string ret(r, i);
	delete [] r;
	return ret;
}// __tSocket::readFrom


int __tSocket::_write(char r)
{
	int err = ::send(s, &r, 1, 0);
	if ( err != 1 ){
		_Throw("_write error: " + ToString(errno));
		//debugLog("__tSocket::_write() error");
	}
	//debugLog(string("__tSocket::_write() [") + r + "]");
	return 1;
}// __tSocket::_write


int __tSocket::_write(const string &q)
{
	string w = q;
	while ( !w.empty() ){
		if ( availableWrite(0) ){
			int err = ::send(s, w.c_str(), w.size(), 0);
			if ( err < 0 && errno != EAGAIN ){
				_Throw("_write error: " + ToString(errno));
			}
			if ( err > 0 ){
				w.erase(0, err);
				continue;
			}
		}
		tTimer::delay(__atomic_timeout);
	}
	return q.size();
}// __tSocket::_write


int __tSocket::write(char r)
{
	std::lock_guard<std::mutex> mw(m3);
	return _write(r);
}// __tSocket::write


int __tSocket::write(const string &b)
{
	std::lock_guard<std::mutex> mw(m3);
	return _write(b);
}// __tSocket::write


int __tSocket::_writetm(char r, int tm)
{
	if( tm < 0 ) return 0;
	if ( availableWrite(tm) ) return _write(r);
	return 0;
}// __tSocket::_writetm


int __tSocket::writetm(char r, int tm)
{
	if( tm < 0 ) return 0;
	std::lock_guard<std::mutex> mw(m3);
	return _writetm(r, tm);
}// __tSocket::writetm


int __tSocket::writetm(const string &q, int tm)
{
	if ( tm < 0 || q.empty() ) return 0;

	std::lock_guard<std::mutex> mw(m3);
	string w = q;
	for ( tTimer t; !w.empty(); ){
		if ( availableWrite(0) ){
			int err = ::send(s, w.c_str(), w.size(), 0);
			if ( err < 0 && errno != EAGAIN && errno != 0 ){
				_Throw("writetm errno: " + ToString(errno) + ", err = " + ToString(err));
			}
			if ( err == 0 ){
				_Throw("writetm errno: " + ToString(errno) + ", err= " + ToString(err));
			}
			if ( err > 0 ){
				w.erase(0, err);
			}
		}
		if ( tm < __atomic_timeout ) break;
		tTimer::delay(__atomic_timeout);
		if ( t.check(tm) ) break;
	}
	return q.size() - w.size();
}// __tSocket::writetm


int __tSocket::writeTo(const string &q, const string &addr, int port)
{
	if ( q.empty() ) return 0;
	int maxmsg = getMaxMsgSize();
	if ( (int)q.size() > maxmsg ){
		_Throw("writeTo error: buffer size=" + ToString(q.size()) + " > maxmsg=" + ToString(maxmsg)); 
	}

	std::lock_guard<std::mutex> mw(m3);
	string w = q;

	int err = 0;
	if ( availableWrite(0) ){
		string adr = trim(addr);

		struct sockaddr_in name;
		memset(&name, 0, sizeof(name));
		name.sin_family = AF_INET;
		name.sin_port = htons(port);

		if ( !adr.empty() ){
			struct hostent *entry = gethostbyname(adr.c_str());
			if ( entry == NULL ){
				_Throw("writeTo error: gethostbyname returns NULL");
			}
			adr = inet_ntoa(*((in_addr *)entry->h_addr));
			name.sin_addr.s_addr = inet_addr(adr.c_str());
		}else{
			name.sin_addr.s_addr = INADDR_BROADCAST;
		}
		elmlen namelen = sizeof(name);

		err = ::sendto(s, w.c_str(), w.size(), 0, (const struct sockaddr *)&name, namelen);
		if ( err < 0 && errno != EAGAIN && errno != 0 ){
			_Throw("writeTo errno: " + ToString(errno) + ", err = " + ToString(err));
		}
		if ( err == 0 ){
			_Throw("writeTo errno: " + ToString(errno) + ", err= " + ToString(err));
		}
	}
	return err;
}// __tSocket::writeTo


bool __tSocket::availableRead(int tm) const
{
	struct timeval timeout;
	tm2timeval(tm, timeout);
	fd_set tSet;
	fd_set tExc;
	fdClr(&tSet, NULL, &tExc);
	fdAdd(s, &tSet, NULL, &tExc);
	if ( select(s + 1, &tSet, (fd_set *)NULL, &tExc, &timeout) == -1 )
		_Throw("availableRead:: select error");
	if ( FD_ISSET(s, &tExc) )  _Throw("availableRead:: FD_ISSET error");
	if ( FD_ISSET(s, &tSet) )  return true;
	return false;
}// __tSocket::availableRead


bool __tSocket::availableWrite(int tm) const
{
	struct timeval timeout;
	tm2timeval(tm, timeout);
	fd_set tSet;
	fd_set tExc;
	fdClr(NULL, &tSet, &tExc);
	fdAdd(s, NULL, &tSet, &tExc);
	if ( select(s + 1, (fd_set *)NULL, &tSet, &tExc, &timeout) == -1 )
	  _Throw("availableWrite:: select error");
	if ( FD_ISSET(s, &tExc) )  _Throw("availableWrite:: FD_ISSET");
	if ( FD_ISSET(s, &tSet) )  return true;
	return false;
}// __tSocket::availableWrite


__tSocket::~__tSocket()
{
	close();
}// __tSocket::~__tSocket


void __tSocket::close()
{
	if ( s != -1 ){
//		shutdown(s, 2);
#ifdef __GNUG__
		shutdown(s, SHUT_RDWR);
		::close(s);
#endif
#ifdef _WIN32
		shutdown(s, SD_BOTH);
		::closesocket(s);
#endif
		s = -1;
		debugLog("close socket [" + ToString(s) + "]");
	}else{
		debugLog("close socket (INVALID_SOCKET)");
	}
	alive = false;
}// __tSocket::close


bool __tSocket::setOption(int opt, int val)
{
	int ret;
	{
		std::lock_guard<std::mutex> m(m1);
		int optval = val;
		ret = setsockopt(s, SOL_SOCKET, opt, (char *)&optval, sizeof(optval));
	}
	return (ret == 0);
}// __tSocket::setOption


bool __tSocket::setOption(int proto, int opt, int val)
{
	int ret;
	{
		std::lock_guard<std::mutex> m(m1);
		int optval = val;
		ret = setsockopt(s, proto, opt, (char *)&optval, sizeof(optval));
	}
	return (ret == 0);
}// __tSocket::setOption


bool __tSocket::getOption(int opt, int &val) const
{
	int optval;
	elmlen optlen = sizeof(optval);
	if ( getsockopt(s, SOL_SOCKET, opt, (char *)&optval, &optlen) < 0 )  return false;
	val = optval;
	return true;
}// __tSocket::getOption


int __tSocket::getReceiveBufferSize() const
{
	int n;
	if ( !getOption(SO_RCVBUF, n) )  _Throw("getReceiveBufferSize");
	return n;
}// __tSocket::getReceiveBufferSize


int __tSocket::setReceiveBufferSize(int i)
{
	if ( !setOption(SO_RCVBUF, i) )  return -1;
	return getReceiveBufferSize();
}// __tSocket::setReceiveBufferSize


int __tSocket::getSendBufferSize() const
{
	int n;
	if ( !getOption(SO_SNDBUF, n) )  _Throw("getSendBufferSize");
	return n;
}// __tSocket::getSendBufferSize


int __tSocket::setSendBufferSize(int i)
{
	if ( !setOption(SO_SNDBUF, i) )  return -1;
	return getSendBufferSize();
}// __tSocket::setSendBufferSize


bool __tSocket::setReuseAddress(int v)
{
	return setOption(SO_REUSEADDR, v);
}// __tSocket::setReuseAddress


bool __tSocket::setKeepAlive(int v)
{
	return setOption(SO_KEEPALIVE, v);
}// __tSocket::setKeepAlive


bool __tSocket::setNoDelay(int v)
{
	return ( tcp ) ? setOption(IPPROTO_TCP, 1 /*TCP_NODELAY*/, v) : false;
}// __tSocket::setNoDelay


bool __tSocket::setBroadcast(int v)
{
	return setOption(SO_BROADCAST, v);
}// setBroadcast


bool __tSocket::setNoBlock(int v)
{
	int ret;
	{
		std::lock_guard<std::mutex> m(m1);
#ifdef __GNUG__
		int dontblock = v;
		ret = ioctl(s, FIONBIO, (char *) &dontblock);
#endif
#ifdef _WIN32
		unsigned long dontblock = v;
		ret = ioctlsocket(s, FIONBIO, &dontblock);
#endif
	}
	return (ret == 0);
}// __tSocket::setNoBlock


int __tSocket::getMaxMsgSize() const
{
#ifdef _WIN32
	static int maxmsg = -1;
	if ( maxmsg == -1 ){
		int n;
		if ( !getOption(SO_MAX_MSG_SIZE, n) )  _Throw("setSendBufferSize");
		maxmsg = n;
	}
	return maxmsg;
#endif
#ifdef __GNUG__
	return 0x10000;
#endif
}// __tSocket::getMaxMsgSize

/*
bool __tSocket::erase(__tSocket *p)
{
	if ( p != NULL ){
		std::lock_guard<std::mutex> ml(_listMutex);
		std::list<__tSocket *>::iterator i = std::find(__tSocket::_list.begin(),
		                                               __tSocket::_list.end(), p);
		if ( i != __tSocket::_list.end() ){
			__tSocket::_list.erase(i);
			debugLog("__tSocket::_list socket erased.  __tSocket::_list.size()=" +  
			         ToString(__tSocket::_list.size())); 
			delete p;
			return true;
		}
	}
	return false;
}// __tSocket::erase


void __tSocket::eraseAll()
{
	//std::lock_guard<std::mutex> ml(_listMutex);
	//printf("__tSocket::_list socket eraseAll.  __tSocket::_list.size()=%d\n", __tSocket::_list.size());

	for ( std::list<__tSocket *>::iterator i = __tSocket::_list.begin();
	      i != __tSocket::_list.end(); ++i ){
		__tSocket *p = (*i);
		delete p;
	} 
	//__tSocket::_list.clear();
}// __tSocket::erase


void __tSocket::add(__tSocket *p)
{
	if ( p != NULL ){

		std::lock_guard<std::mutex> ml(_listMutex);
		__tSocket::_list.push_back(p);
		debugLog("__tSocket::_list socket added.  __tSocket::_list.size()=" +  
		         ToString(__tSocket::_list.size())); 
	}
}// __tSocket::erase
*/




tSocket::tSocket(int s) :
  p(new __tSocket(s))
{
	if ( p->Alive() ){
		//__tSocket::add(p);
		p->use();
	}else{
		delete p;
		p = NULL;
	}
}// tSocket::tSocket


tSocket::tSocket(bool Tcp, const string &ladr, int lport, const string &radr, int rport) :
  p(new __tSocket(Tcp))
{
	if ( p->Alive() ){
		//__tSocket::add(p);
		p->use();
		if ( !ladr.empty() || lport != 0 )  p->bind(ladr, lport);
		if ( !radr.empty() || rport != 0 )  p->connect(radr, rport);
	}else{
		delete p;
		p = NULL;
	}
}// tSocket::tSocket


//static tMutex __x;


tSocket::~tSocket()
{
	//tMutexLock l(__x);
	if ( p != NULL ){
		p->unuse();
		//if ( !p->used() )  __tSocket::erase(p);
		if ( !p->used() )  delete p;
	}
}// tSocket::~tSocket


tSocket::tSocket(const tSocket &src) :
  p(src.p)
{
	if ( p )  p->use();
}// tSocket::tSocket


tSocket &tSocket::operator=(const tSocket &src)
{
	if ( this != &src ){
		if ( p != NULL ){
			p->unuse();
			//if ( !p->used() )  __tSocket::erase(p);
			if ( !p->used() )  delete p;
		}
		p = src.p;
		if ( p != NULL )  p->use();
	}
	return *this;
}// tSocket::operator=


string tSocket::getInetAddress() const
{
	return ( p ) ? p->getInetAddress() : string();
}// tSocket::getInetAddress


string tSocket::getLocalAddress() const
{
	return ( p ) ? p->getLocalAddress() : string();
}// tSocket::getLocalAddress


bool tSocket::availableRead(int tim) const
{
	return ( p ) ? p->availableRead(tim) : false;
}// tSocket::availableRead


bool tSocket::availableWrite(int tim) const
{
	return ( p ) ? p->availableWrite(tim) : false;
}// tSocket::availableWrite


string tSocket::read(unsigned size)
{
	return ( p ) ? p->read(size) : string();
}// tSocket::read


string tSocket::readtm(unsigned size, int timeout)
{
	//debugLog("tS::readtm(n, tm)");
	return ( p ) ? p->readtm(size, timeout) : string();
}// tSocket::readtm


bool tSocket::Alive() const
{
	return p != NULL && p->Alive();
}// tSocket::Alive


bool tSocket::isTcp() const
{
	return Alive() && p->tcp;
}// tSocket::isTcp


bool tSocket::listen()
{
	return ( Alive() ) ? p->listen() : false;
}// tSocket::listen


int tSocket::accept(int tm)
{
	return ( Alive() ) ? p->accept(tm) : -1;
}// tSocket::accept


void tSocket::close()
{
	if ( p ) p->close();
}// tSocket::close


int tSocket::handle() const
{
	return ( p ) ? p->handle() : -1;
}// tSocket::handle


int tSocket::getLocalPort() const
{
	return ( p ) ? p->getLocalPort() : 0;
}// tSocket::getLocalPort


int tSocket::getPort() const
{
	return ( p ) ? p->getPort() : 0;
}// tSocket::getPort


int tSocket::getReceiveBufferSize() const
{
	return ( p ) ? p->getReceiveBufferSize() : 0;
}// tSocket::getReceiveBufferSize


int tSocket::getSendBufferSize() const
{
	return ( p ) ? p->getSendBufferSize() : 0;
}// tSocket::getSendBufferSize


int tSocket::getMaxMsgSize() const
{
	return ( p ) ? p->getMaxMsgSize() : 0;
}// tSocket::getMaxMsgSize


bool tSocket::setReuseAddress(int v)
{
	return ( p ) ? p->setReuseAddress(v) : false;
}// tSocket::getReuseAddress


bool tSocket::setNoBlock(int v)
{
	return ( p ) ? p->setNoBlock(v) : false;
}// tSocket::getNoBlock


bool tSocket::setKeepAlive(int v)
{
	return ( p ) ? p->setKeepAlive(v) : false;
}// tSocket::getKeepAlive


bool tSocket::setNoDelay(int v)
{
	return ( p ) ? p->setNoDelay(v) : false;
}// tSocket::getNoDelay


bool tSocket::setBroadcast(int v)
{
	return ( p ) ? p->setBroadcast(v) : false;
}// tSocket::getBroadcast


int tSocket::setReceiveBufferSize(int size)
{
	return ( p ) ? p->setReceiveBufferSize(size) : 0;
}// tSocket::setReceiveBufferSize


int tSocket::setSendBufferSize(int size)
{
	return ( p ) ? p->setSendBufferSize(size) : 0;
}// tSocket::setSendBufferSize


int tSocket::read()
{
	return ( p ) ? p->read() : -1;
}// tSocket::read


int tSocket::readtm(int timeout)
{
	return ( p ) ? p->readtm(timeout) : -1;
}// tSocket::readtm


string tSocket::readFrom(string &addr, int &port)
{
	return ( p ) ? p->readFrom(addr, port) : string("");
}// tSocket::readFrom


int tSocket::write(char b)
{
	return ( p ) ? p->write(b) : 0;
}// tSocket::write


int tSocket::write(const string &b)
{
	return ( p ) ? p->write(b) : 0;
}// tSocket::write


int tSocket::writetm(char b, int timeout)
{
	return ( p ) ? p->writetm(b, timeout) : 0;
}// tSocket::writetm


int tSocket::writetm(const string &b, int timeout)
{
	return ( p ) ? p->writetm(b, timeout) : 0;
}// tSocket::writetm


int tSocket::writeTo(const string &b, const string &addr, int port)
{
	return ( p ) ? p->writeTo(b, addr, port) : 0;
}// tSocket::writeTo


bool tSocket::zip()
{
	return __tSocket::usezip;
}// tSocket::zip


bool tSocket::zip(bool newzip)
{
	bool ret = __tSocket::usezip;
	__tSocket::usezip = newzip;
	return ret;
}// tSocket::zip


string tSocket::error() const
{
	return ( p ) ? p->error() : string("");
}// tSocket::error


string tSocket::getHostName()
{
	//__init_tSocket_ex.init();
	__init_WSA();

	string ret;
	char buf[256];
	if ( gethostname(buf, 255) == 0 )  ret = buf;
	return ret;
}// tSocket::getHostName





#ifdef _WIN32

//__init_tSocket::__init_tSocket()
//{
	/*
	printf("+\n");
	if ( !(cnt++) ){
		WORD wVersionRequested = MAKEWORD(2, 0);

		WSADATA wsaData;
		int err = WSAStartup(wVersionRequested, &wsaData);
		if ( err != 0 ){
			nError::Message e("can't init WSA");
			throw e;
		}
		//debugLog("Using WINSOCK ver " +
		//         ToString(LOBYTE(wsaData.wVersion)) + "." +
		//         ToString(HIBYTE(wsaData.wVersion )));
		__WSA_inited = true;
	} */
//}// __init_tSocket::__init_tSocket

/*
void __init_tSocket::init()
{
	if ( !inited ){
		inited = true;
		WORD wVersionRequested = MAKEWORD(2, 0);

		WSADATA wsaData;
		int err = WSAStartup(wVersionRequested, &wsaData);
		if ( err != 0 ){
			nError::Message e("can't init WSA");
			throw e;
		}
		//debugLog("Using WINSOCK ver " +
		//         ToString(LOBYTE(wsaData.wVersion)) + "." +
		//         ToString(HIBYTE(wsaData.wVersion )));
		//__WSA_inited = true;
	}
}// __init_tSocket::init


__init_tSocket::~__init_tSocket()
{
	if ( inited ){
		inited = false;
		//__tSocket::eraseAll();
		//WSACleanup();
	}
}// __init_tSocket::__init_tSocket
*/

#endif // _WIN32

#ifdef __GNUG__

/*
__init_tSocket::__init_tSocket()
{
	if ( !inited ){
		inited = true;
		// <--
	}
}// __init_tSocket::__init_tSocket


__init_tSocket::~__init_tSocket()
{
	if ( inited ){
		inited = false;
		//__tSocket::eraseAll();
	}
}// __init_tSocket::__init_tSocket
*/

#endif // __GNUG__

