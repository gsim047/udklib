#ifndef ____tSocket_cpp
#define ____tSocket_cpp 1

#include <list>
#include "tMutex.h"   // for tRefCounter




class __tSocket : public tRefCounter
{
	//friend class tServerSocket;
	friend class tSocket;
	friend class __tSSocket;
	friend class __init_tSocket;

protected:
	//__tSocket(const string &adr, int port, bool stream = true);
	__tSocket(bool tcp = true);
	__tSocket(int i);

public:
	~__tSocket();

private:
	__tSocket(const __tSocket &);
	__tSocket &operator=(const __tSocket &);

	//static void add(__tSocket *);
	//static bool erase(__tSocket *);
	//static void eraseAll();

	void _Throw() const;
	void _Throw(const string &info) const;

	int _read();
	string _read(unsigned n);
	int _write(char b);
	int _write(const string &b);
	int _writetm(char b, int timeout = 0);

	static int _socket(bool tcp);

	bool setOption(int opt, int val);
	bool setOption(int proto, int opt, int val);
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
	bool setNoDelay(int v = 1);

	bool getpeername(string &addr, int &port) const;
	bool getsockname(string &addr, int &port) const;
	bool bind(const string &adr, int p);
	bool connect(const string &adr, int p);

	bool listen();
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
	std::mutex m1;
	std::mutex m2;
	std::mutex m3;
	int s;
	bool tcp;
	mutable string lasterror;

	static bool usezip;
};// class __tSocket




#endif // ____tSocket_cpp
