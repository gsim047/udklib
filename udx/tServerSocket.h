#ifndef __tServerSocket_h
#define __tServerSocket_h 1

#ifndef __tTcpSocket_h
#include "tTcpSocket.h"
#endif




class tServerSocket : private tSocket {
public:
	tServerSocket() : tSocket() {};
	tServerSocket(const string &localAdr, int localPort) :
		tSocket(true, localAdr, localPort, "", 0) { _listen(); };
	tServerSocket(int localPort) :
		tSocket(true, "", localPort, "", 0) { _listen(); };
	tServerSocket(const tServerSocket &s) : tSocket(s) {};
	virtual ~tServerSocket() {};

	tServerSocket &operator=(const tServerSocket &s) { this->tSocket::operator=(s); return *this; }

	bool operator==(const tServerSocket &s) const { return this->tSocket::operator==(s); };
	bool operator!=(const tServerSocket &s) const { return this->tSocket::operator!=(s); };

public:
	bool Alive() const { return this->tSocket::Alive(); }
	//bool isTcp() const { return true; }
	bool operator!() const { return !Alive(); };

	void close() { this->tSocket::close(); }
	int handle() const { return this->tSocket::handle(); }

	string getLocalAddress() const { return this->tSocket::getLocalAddress(); }
	int getLocalPort() const { return this->tSocket::getLocalPort(); }

	tTcpSocket accept(int tm);
	tSocket accept0(int tm);

	string error() const { return this->tSocket::error(); }

private:
	int _accept(int tm) { return this->tSocket::accept(tm); }
	bool _listen() { return this->tSocket::listen(); }
};// class tServerSocket




#endif // __tServerSocket_h
