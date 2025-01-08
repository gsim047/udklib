#ifndef __tIPC_h_
#define __tIPC_h_

#ifndef __tMutex_h
#include "tMutex.h"
#endif

#ifndef __tSem_h
#include "tSem.h"
#endif

#ifndef __tShMem_h
#include "tShMem.h"
#endif

#ifndef __tPk_h
#include "tPk.h"
#endif




namespace nError{
	class IPCException : public Message{
	public:
		IPCException();
		IPCException(const string &s);
	};
};// namespace nError


struct __ipc;




class tIPC
{
public:
	tIPC(unsigned queuenumber, bool server = false);
	virtual ~tIPC();

private:
	tIPC(const tIPC &);
	tIPC &operator=(const tIPC &);

public:
	bool Alive() const { return p && mem.Alive() && sem.Alive(); }
	void close();

	string get(unsigned &from);
	string get(unsigned &from, int timeout);
	bool put(string &buf, unsigned addr = 1);
	bool put(string &buf, unsigned addr, int timeout);

	unsigned getNumber() const { return _number; };
	bool getServer() const { return _server; };

protected:
	bool server;
	string name;
	unsigned pid;
	tShMem mem;
	tSemaph sem;
	__ipc *p;

	unsigned _number;
	bool _server;

};// class tIPC




class tSIPC
{
public:
		//tSIPC() : p(NULL) {};
  tSIPC(unsigned queuenumber, bool server = false);
  virtual ~tSIPC();

		//bool operator==(const tSIPC &s) const { return p == s.p; };
		//bool operator!=(const tSIPC &s) const { return p != s.p; };

private:
  tSIPC &operator=(const tSIPC &);
  tSIPC(tIPC *src);
  tSIPC(const tSIPC &);

public:
	//operator tIPC*() const { return p; };
	//tIPC *operator->() const { return p; };

	bool Alive() const { return p != NULL && p->Alive(); };
	bool operator!() const { return !Alive(); };

	void setOutput(tByteArray ba, unsigned addr, int packMethod = 4);
	//tByteArray getInput(unsigned &addr);
	tByteArray getInput(unsigned &addr, int timeout);

	bool tryToOut(int Timeout);
	bool tryToGet(int Timeout);

	//string error() const;
	//void close() { if ( p ) p->close(); };
	//static void closeAll();

private:
	static bool pkOk(tByteArray &ba, unsigned *size = NULL);
	static tByteArray getPk(tByteArray &ba);

private:
	tIPC *p;

	tMutex vlock;
	std::deque<unsigned> poolOutAddr;
	std::deque<tByteArray> poolOutData;
	//bool readDisabled;
	tMutex ilock;
	std::deque<unsigned> poolInAddr;
	std::deque<tByteArray> poolInData;

	//static void add(tSIPC *p);
	//static bool erase(tSIPC *p);

	//static tMutex _listMutex;
	//static std::list<tSIPC *> _list;
};// class tSIPC




tSIPC &operator<<(tSIPC &, tPk *);

tPk *getPk(tSIPC &s, int timeout = 0);
//inline tPk *getPk(tSIPC &s, int timeout = 0)
//{
// unsigned addr;
// tByteArray ba = s.getInput(addr, timeout);
// return pkReceive(ba, new tIPCConnection(s, addr));
//}// getPk

tSIPC &send(tSIPC &, tPk *, unsigned addr);


#endif // __tIPC_h_
