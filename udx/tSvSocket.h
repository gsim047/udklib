#ifndef __tSvSocket_h
#define __tSvSocket_h 1

#ifndef __tSocket_h
#include "tSocket.h"
#endif

#ifndef __tByteArray_h
#include "tByteArray.h"
#endif

#ifndef __tPk_h
#include "tPk.h"
#endif




class __tSSocket;




class tSSocket{
	friend class tServerSocket;
	friend class __rwThread;

public:
	tSSocket() : p(NULL) {};
	//tSSocket(__tSocket *);
	tSSocket(const string &adr, int port, bool stream = true);
	tSSocket(tSocket &);
	tSSocket(const tSSocket &);
	virtual ~tSSocket();

	tSSocket &operator=(const tSSocket &);

private:
	tSSocket(__tSSocket *);

	void use();

public:
	void close();
	void closeWait(int timeout = 10000);
	static void closeAll();
	static void closeAllWait(int timeout = 10000);

	bool Alive() const;
	bool operator!() const { return !Alive(); };

	bool operator==(const tSSocket &s) const { return p == s.p; };
	bool operator!=(const tSSocket &s) const { return p != s.p; };

	void setOutput(const tByteArray &ba);
	tByteArray getInput(int timeout);

	void cryptoOn(const string &/*key*/) {};
	void cryptoOff() {};

private:
	__tSSocket *p;
};// class tSSocket




tSSocket &operator<<(tSSocket &, tPk *);

tPk *getPk(tSSocket &s, int timeout = 0);


#endif // __tSvSocket_h
