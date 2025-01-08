#ifndef __tTcpSocket_h
#define __tTcpSocket_h 1

#ifndef __tSocket_h
#include "tSocket.h"
#endif




class tTcpSocket : private tSocket {
	friend class tServerSocket;
	//friend class __tSSocket;

private:
	tTcpSocket(int s) : tSocket(s) {};

public:
	tTcpSocket() : tSocket() {};
	tTcpSocket(const string &localAdr, int localPort,
	           const string &remoteAdr, int remotePort, bool NoBlock = true) :
		tSocket(true, localAdr, localPort, remoteAdr, remotePort) 
	{ 
		if ( Alive() ){
			if ( NoBlock )  setNoBlock();
			setKeepAlive();
		}
	};
	tTcpSocket(const string &remoteAdr, int remotePort, bool NoBlock = true) :
		tSocket(true, "", 0, remoteAdr, remotePort) 
	{
		if ( Alive() ){
			if ( NoBlock )  setNoBlock();
			setKeepAlive();
		}
	};
	tTcpSocket(const tTcpSocket &s) : tSocket(s) {};
	virtual ~tTcpSocket() {};

	tTcpSocket &operator=(const tTcpSocket &s) { this->tSocket::operator=(s); return *this; }

	bool operator==(const tTcpSocket &s) const { return this->tSocket::operator==(s); };
	bool operator!=(const tTcpSocket &s) const { return this->tSocket::operator!=(s); };

public:
	bool Alive() const { return this->tSocket::Alive(); }
	bool isTcp() const { return true; }
	bool operator!() const { return !Alive(); };

	void close() { this->tSocket::close(); }
	int handle() const { return this->tSocket::handle(); }

	string getInetAddress() const { return this->tSocket::getInetAddress(); }
	string getLocalAddress() const { return this->tSocket::getLocalAddress(); }
	int getLocalPort() const { return this->tSocket::getLocalPort(); }
	int getPort() const { return this->tSocket::getPort(); }
	int getReceiveBufferSize() const { return this->tSocket::getReceiveBufferSize(); }
	int getSendBufferSize() const { return this->tSocket::getSendBufferSize(); }
	int setReceiveBufferSize(int size) { return this->tSocket::setReceiveBufferSize(size); }
	int setSendBufferSize(int size) { return this->tSocket::setSendBufferSize(size); }

	bool setNoBlock(int v = 1) { return this->tSocket::setNoBlock(v); }
	bool setKeepAlive(int v = 1) { return this->tSocket::setKeepAlive(v); }
	bool setNoDelay(int v = 1) { return this->tSocket::setNoDelay(v); }

	bool availableRead(int timeout = 0) const { return this->tSocket::availableRead(timeout); }
	bool availableWrite(int timeout = 0) const { return this->tSocket::availableWrite(timeout); }

	int read() { return this->tSocket::read(); }
	string read(unsigned size) { return this->tSocket::read(size); }
	int readtm(int timeout = 0) { return this->tSocket::readtm(timeout); }
	string readtm(unsigned size, int timeout = 0) { return this->tSocket::readtm(size, timeout); }

	int write(char b) { return this->tSocket::write(b); }
	int write(const string &b) { return this->tSocket::write(b); }
	int writetm(char b, int timeout = 0) { return this->tSocket::writetm(b, timeout); }
	int writetm(const string &b, int timeout = 0) { return this->tSocket::writetm(b, timeout); }

	string error() const { return this->tSocket::error(); }
};// class tTcpSocket




#endif // __tTcpSocket_h
