#ifndef __tUdpSocket_h
#define __tUdpSocket_h 1

#ifndef __tSocket_h
#include "tSocket.h"
#endif




class tUdpSocket : private tSocket {
	//friend class tServerSocket;
	//friend class __tSSocket;

private:
	tUdpSocket(int s);

public:
	tUdpSocket() : tSocket() {};
	tUdpSocket(const string &localAdr, int localPort, bool Broadcast = true) :
		tSocket(false, localAdr, localPort, "", 0) 
	{
		if ( Alive() && Broadcast )  setBroadcast();
	};
	tUdpSocket(const tUdpSocket &s) : tSocket(s) {};
	virtual ~tUdpSocket() {};

	tUdpSocket &operator=(const tUdpSocket &s) { this->tSocket::operator=(s); return *this; }

	bool operator==(const tUdpSocket &s) const { return this->tSocket::operator==(s); };
	bool operator!=(const tUdpSocket &s) const { return this->tSocket::operator!=(s); };

public:
	bool Alive() const { return this->tSocket::Alive(); }
	bool isTcp() const { return false; }
	bool operator!() const { return !Alive(); };

	void close() { this->tSocket::close(); }
	int handle() const { return this->tSocket::handle(); }

	string getLocalAddress() const { return this->tSocket::getLocalAddress(); }
	int getLocalPort() const { return this->tSocket::getLocalPort(); }

	bool setNoBlock() { return this->tSocket::setNoBlock(); }
	bool setKeepAlive() { return this->tSocket::setKeepAlive(); }

	bool availableRead(int timeout = 0) const { return this->tSocket::availableRead(timeout); }
	bool availableWrite(int timeout = 0) const { return this->tSocket::availableWrite(timeout); }

	string readFrom(string &addr, int &port) { return this->tSocket::readFrom(addr, port); }

	int writeTo(const string &b, const string &addr, int port) { return this->tSocket::writeTo(b, addr, port); }
	int writeTo(const string &b, int port) { return this->tSocket::writeTo(b, "", port); }

	string error() const { return this->tSocket::error(); }
};// class tUdpSocket




#endif // __tUdpSocket_h
