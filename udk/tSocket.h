#ifndef __tSocket_h
#define __tSocket_h 1

#include "nError.h"



/*
class __init_tSocket{
public:
	__init_tSocket();
	~__init_tSocket();

private:
	static int cnt;
};// class __init_tSocket

static __init_tSocket __init_tSocket_ex;
*/



namespace nError{
	class IOException : public Message {
	public:
		IOException(); // : Message("tSocket::IOException") {};
		IOException(const string &s); // : Message("tSocket::IOException: " + s) {};
	};
};// namespace nError



class __tSocket;




class tSocket{
	friend class tServerSocket;
	friend class __tSSocket;

protected:
	tSocket(int s);

public:
	tSocket() : p(NULL) {};
	tSocket(bool Tcp, 
	        const string &localAdr, int localPort,
	        const string &remoteAdr, int remotePort);
	tSocket(const tSocket &);
	virtual ~tSocket();

	tSocket &operator=(const tSocket &);

	bool operator==(const tSocket &s) const { return p == s.p; };
	bool operator!=(const tSocket &s) const { return p != s.p; };

public:
	bool Alive() const;
	bool isTcp() const;
	bool operator!() const { return !Alive(); };

	void close();
	int handle() const;

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
	string readFrom(string &addr, int &port);         // for UDP

	int write(char b);
	int write(const string &b);
	int writetm(char b, int timeout = 0);
	int writetm(const string &b, int timeout = 0);
	int writeTo(const string &b, const string &addr, int port);      // for UDP

	string error() const;

	static string getHostName();
	static bool zip();
	static bool zip(bool newzip);

private:
	__tSocket *p;
};// class tSocket




#endif // __tSocket_h
