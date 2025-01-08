#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

#include "tIPC.h"
#include "tIPCConn.h"
#include "fPid.h"
#include "fString.h"
//#include "fTime.h"
#include "tTimer.h"
//#include "tThread.h"
#include "tDebug.h"

#include <time.h>
#include <algorithm>




namespace nError {

	IPCException::IPCException() : Message("IPCException") 
	{
	}// IPCException::IPCException

	IPCException::IPCException(const string &s) : Message("IPCException: " + s) 
	{
	}// IPCException::IPCException

};// namespace nError



/*
class __rwIPCThread : public tThread{
public:
	__rwIPCThread(const string &s) : tThread(s) {};

	void run();

	static void init();
	static bool inited;
};// class __rwIPCThread
*/

static tSyncInt __ipc_counter = 0;


struct __ipc {
	enum{
		QUESIZE           =  128,
		BUFSLOTS          =  128,
		BUFSIZE           = 1024
	};

	bool lock(tSemaph &sem, unsigned pid);
	bool lock(tSemaph &sem, unsigned pid, int timeout);
	bool query(tSemaph &sem, unsigned pid);
	void clear(tSemaph &sem, unsigned pid);
	void unlock();

	string get(unsigned &_from);
	bool put(string &s, unsigned adr, unsigned _from, unsigned timeout = 15); // in sec

	void free(unsigned adr);
	void freeAllOld();

	unsigned pids[QUESIZE];           // очередь доступа 
	unsigned addr[BUFSLOTS];          // адресат
	unsigned from[BUFSLOTS];          // автор
	unsigned size[BUFSLOTS];          // размер посылки
	unsigned timo[BUFSLOTS];          // таймаут в секундах
	unsigned numb[BUFSLOTS];          // абс. номер посылки

	char block[BUFSLOTS][BUFSIZE];    // данные посылки
		// посылка целиком == (addr[i] + size[i] + timo[i] + numb[i] + block[i])
};//  __ipc


bool __ipc::lock(tSemaph &sem, unsigned pid)
{
	while ( !query(sem, pid) )  tTimer::delay(10);
	while ( pid != pids[0] )  tTimer::delay(10);
	return true;
}// __ipc::lock


bool __ipc::lock(tSemaph &sem, unsigned pid, int timeout)
{
	if ( timeout <= 0 )  return lock(sem, pid);
	tTimer tm;
	while ( !query(sem, pid) ){
		if ( tm.check(timeout) ) return false;
		tTimer::delay(10);
	}
	while ( pid != pids[0] ){
		if ( tm.check(timeout) ){
			clear(sem, pid);
			return false;
		}
		tTimer::delay(10);
	}
	return true;
}// __ipc::lock


bool __ipc::query(tSemaph &sem, unsigned pid)
{
	tSemLock sl(sem);
	for ( int i = 0; i < QUESIZE; i++ ){
		if ( pids[i] == 0 || pids[i] == pid ){
			pids[i] = pid;
			return true;
		}
	}
	return false;
}// __ipc::query


void __ipc::clear(tSemaph &sem, unsigned pid)
{
	tSemLock sl(sem);
	for ( int i = 0; i < QUESIZE; i++ ){
		if ( pids[i] == pid ){
			for ( int j = i + 1; j < QUESIZE; j++ ) pids[j - 1] = pids[j];
			pids[QUESIZE - 1] = 0;
			break;
		}
	}
}// __ipc::clear


void __ipc::unlock()
{
	unsigned pid0 = pids[1];
	for ( unsigned i = 1; i < QUESIZE - 1; i++ ) pids[i] = pids[i + 1];
	pids[QUESIZE - 1] = 0;
	pids[0] = pid0;
}// __ipc::unlock


void __ipc::freeAllOld()
{
	unsigned tm = time(NULL);
	for ( int i = 0; i < BUFSLOTS; i++ ){
		if ( addr[i] != 0 && tm > timo[i] ){
			errorLog("__ipc::freeAllOld: free buf " + ToString(numb[i]) + ", " +
			         ToStringHex(from[i]) + "->" + ToStringHex(addr[i]) + "  " +
			         ToString(size[i]) + " byte(s)");
			addr[i] = 0;
		}
	}
}// __ipc::freeAllOld


void __ipc::free(unsigned adr)
{
	for ( int i = 0; i < BUFSLOTS; i++ ){
		if ( addr[i] == adr || (addr[i] != 0 && from[i] == adr) ){
			errorLog("__ipc::free[" + ToStringHex(adr) + "]: free buf " +
			         ToString(numb[i]) + ", " + ToStringHex(from[i]) + "->" +
			         ToStringHex(addr[i]) + "  " + ToString(size[i]) + " byte(s)");
			addr[i] = 0;
		}
	}
}// __ipc::free


string __ipc::get(unsigned &_from)
{
	unsigned pid = pids[0];
	unsigned num = (unsigned)-1;
	string ret;
	for (;;){
		int i0 = -1;
		for ( int i = 0; i < BUFSLOTS; i++ ){
			if ( pid == addr[i] ){
//    debugLog("__ipc::get found My addr " + ToStringHex(pid) + " at " + ToString(i));
				if ( num > numb[i] ){
					num = numb[i];
					i0 = i;
				}
			}
		}
		if ( i0 == -1 )  break;
		ret += string(block[i0], size[i0]);
		_from = from[i0];
		addr[i0] = 0;
	}
	return ret;
}// __ipc::get


bool __ipc::put(string &s, unsigned adr, unsigned _from, unsigned timeout)
{
	for ( int i = 0; i < BUFSLOTS; i++ ){
		if ( addr[i] == 0 ){
			addr[i] = adr;
			from[i] = _from;
			size[i] = s.size();
			numb[i] = ++__ipc_counter;
			if ( size[i] > BUFSIZE )  size[i] = BUFSIZE;
//   debugLog("__ipc::put s.size=" + ToString(s.size()));
//   debugLog("__ipc::put result" + BufDump(s.data(), s.size()));
			memmove(block[i], s.c_str(), size[i]);
			s.erase(0, size[i]);
			timo[i] = time(NULL) + timeout;
//   debugLog("__ipc::put addr " + ToStringHex(adr) + " at " + ToString(i));
			return true;
		}
	}
// debugLog("__ipc::put return false; ");
	return false;
}// __ipc::put




class __ipcLock{
public:
  __ipcLock(__ipc *ipc, tSemaph &sem, unsigned pid) :
    p(ipc), _valid(false) { if ( p )  _valid = p->lock(sem, pid); };
  __ipcLock(__ipc *ipc, tSemaph &sem, unsigned pid, int timeout) :
    p(ipc), _valid(false) { if ( p )  _valid = p->lock(sem, pid, timeout); };
  ~__ipcLock() { if ( valid() )  p->unlock(); };

	bool valid() const { return p != NULL && _valid; };

private:
	__ipc *p;
	bool _valid;
};// class __ipcLock




//tMutex tIPC::_ml;
//std::list<tIPC *> tIPC::_list;

/*
void tIPC::_add(tIPC *ip)
{
	if ( ip ){
		tMutexLock ml(_ml);
		_list.push_back(ip);
	}
}// tIPC::_add


void tIPC::add(tIPC *ip)
{
	if ( ip ){
		tMutexLock ml(_ml);
		std::list<tIPC *>::iterator i = std::find(_list.begin(), _list.end(), ip);
		if ( i == _list.end() )  _list.push_back(ip);
	}
}// tIPC::add


void tIPC::erase(tIPC *ip)
{
	if ( ip ){
		tMutexLock ml(_ml);
		std::list<tIPC *>::iterator i = std::find(_list.begin(), _list.end(), ip);
		if ( i != _list.end() )  _list.erase(i);
	}
}// tIPC::erase
*/
/*
tIPC *tIPC::find(unsigned Quenumber, bool server)
{
	tMutexLock ml(_ml);
	for ( std::list<tIPC *>::iterator i = _list.begin(); i != _list.end(); ++i ){
		if ( (*i)->getNumber() == Quenumber &&
		     ( ((*i)->getServer() && server) || (!(*i)->getServer() && !server) ) ){
			return *i;
		}
	}
	return NULL;
}// tIPC::find
*/

tIPC::tIPC(unsigned Quenumber, bool Server) :
  name("IPC-ShM-" + ToString(Quenumber)),
  server(Server),
  p(NULL),
  pid(Server ? 1 : getPid()),
  sem(Quenumber + 1, Server),
  mem(Quenumber, sizeof(__ipc), Server),
  _number(Quenumber),
  _server(Server)
{
	//debugLog("tIPC() " + ToString(mem.Alive()) + ", " + ToString(sem.Alive()));
	if ( mem.Alive() && sem.Alive() ){
	//debugLog("tIPC() 2");
		p = (__ipc *)mem.getbase();
	//debugLog("tIPC() 3 p=" + ToStringHex((unsigned)p));
		__ipcLock il(p, sem, pid, 200);
	//debugLog("tIPC() 4");
		if ( il.valid() ) {
			p->free(pid);
			//debugLog("tIPC() p->free(pid)");
		}
	}
	//debugLog("tIPC() 5");
}// tIPC::tIPC


tIPC::~tIPC()
{
	//debugLog("~tIPC");
	if ( Alive() ){
		__ipcLock il(p, sem, pid, 200);
		if ( il.valid() )  p->free(pid);
	}
	//erase(this);
}// tIPC::~tIPC


void tIPC::close()
{
	if ( Alive() ){
		__ipcLock il(p, sem, pid, 200);
		if ( il.valid() )  p->free(pid);
		p = NULL;
	}
	//erase(this);
}// tIPC::close


string tIPC::get(unsigned &from)
{
	if ( !Alive() )  return "";
	__ipcLock il(p, sem, pid);

	if ( pid == 1 )  p->freeAllOld();
	return p->get(from);
}// tIPC::get


string tIPC::get(unsigned &from, int timeout)
{
	//debugLog("tIPC::get 0");
	if ( !Alive() )  return "";
	string ret;

	for ( tTimer tim; ret.empty(); tTimer::delay(10) ){
		//debugLog("tIPC::get 1");
		__ipcLock il(p, sem, pid, timeout);
		if ( il.valid() ){
			//debugLog("tIPC::get 2");
			if ( pid == 1 )  p->freeAllOld();
			ret = p->get(from);
		}
		if ( tim.check(timeout) ) break;
	}
	return ret;
}// tIPC::get


bool tIPC::put(string &buf, unsigned addr)
{
	//debugLog("tIPC::put size " + ToString(buf.size()));
	if ( !Alive() )  return false;
// debugLog("tIPC::put Alive!");
	__ipcLock il(p, sem, pid);
// debugLog("tIPC::put size: " + ToString(buf.size()));
	for (;;){
		if ( p->put(buf, addr, pid) )  return true;
		//if ( pid == 1 ) p->freeAllOld();
	}
}// tIPC::put


bool tIPC::put(string &buf, unsigned addr, int timeout)
{
	//debugLog("tIPC::put 1");
	if ( !Alive() )  return false;
	//debugLog("tIPC::put 2");

	for ( tTimer tim;; tTimer::delay(10) ){
		__ipcLock il(p, sem, pid, timeout);
		if ( il.valid() ){
			if ( p->put(buf, addr, pid) )  return true;
		}
		if ( tim.check(timeout) ) break;
	}
	return false;
}// tIPC::put




tSIPC::tSIPC(unsigned queuenumber, bool server) :
  p(NULL)
{
	//debugLog("tSIPC::tSIPC");
	p = new tIPC(queuenumber, server);
	if ( !p->Alive() ){
		delete p;
		p = NULL;
	}
}// tSIPC::tSIPC


tSIPC::~tSIPC()
{
	//debugLog("~tSIPC a " + ToStringHex((unsigned)this, 8));
	if ( p ){
		delete p;
	}
	//debugLog("~tSIPC y");
}// tSIPC::~tSIPC


void tSIPC::setOutput(tByteArray a, unsigned addr, int packMethod)
{
	char pack = '\0';
	if ( a.size() >= 1024 && a.zip(packMethod) ) pack = '\x01';
	tByteArray ba('\0');
	ba << (a.size() + 10) << pack << a << (unsigned)a.calcCrc32();
	tMutexLock ll(vlock);
	std::deque<unsigned>::iterator i = std::find(poolOutAddr.begin(),
	                                             poolOutAddr.end(), addr);
	if ( i == poolOutAddr.end() ){
		poolOutAddr.push_back(addr);
		poolOutData.push_back(ba);
	}else{
		poolOutData[i - poolOutAddr.begin()] += ba;
	}
// debugLog("setOutput: " + BufDump(ba.data(), ba.size()));
	//debugLog("setOutput: +" + ToString(ba.size()) + " bytes");
}// tSIPC::setOutput


bool tSIPC::tryToOut(int Timeout)
{
	//debugLog("tryToOut: poolOutAddr1 " + ToString(poolOutAddr.size()));
	if ( poolOutAddr.empty() ) return false;
	//debugLog("tryToOut: poolOutAddr2 " + ToString(poolOutAddr.size()));

	try{
		//debugLog("tryToOut size " + ToString(poolOutData[0].size()) +
		//         " addr " + ToStringHex(poolOutAddr[0]));
		tMutexLock ll(vlock);
		if ( p->put(poolOutData[0], poolOutAddr[0], Timeout) ){
			if ( poolOutData[0].empty() ){
				poolOutAddr.pop_front();
				poolOutData.pop_front();
				//pknumber++;
				return true;
			}
		}
		return true;
	}
	catch ( nError::IPCException &e ){
		debugLog("catch tSIPC::tryToOut: " + e);
		//close();
		return false;
	}
}// tSIPC::tryToOut


bool tSIPC::tryToGet(int Timeout)
{
	try{
		//debugLog("tSIPC::tryToGet");
		//if ( !readDisabled ){
	  unsigned addr;
	  string s = p->get(addr, Timeout);
	  if ( !s.empty() ){
//    debugLog("tryToGet: " + ToString(s.size()) + " bytes");
	   tMutexLock ll(ilock);
	   std::deque<unsigned>::iterator i = std::find(poolInAddr.begin(),
	                                                poolInAddr.end(), addr);
	   if ( i == poolInAddr.end() ){
	    poolInAddr.push_back(addr);
	    poolInData.push_back(tByteArray(s.data(), s.size()));
	   }else{
	    poolInData[i - poolInAddr.begin()] += s;
	   }
//    debugLog("tryToGet: poolInData " + ToString(poolInData.size()) +
//             " poolInData[0] " + ToString(poolInData[0].size()) + " bytes");
	  }
		//}
		tMutexLock ll(ilock);
		if ( poolInData.empty() ) return false;
		return pkOk(poolInData[0]);                // ???
	}
	catch ( nError::IPCException &e ){
		debugLog("tSIPC::tryToGet: " + e);
		//close();
		return false;
	}
	catch ( nError::Message &e ){
		debugLog("tSIPC::tryToGet: " + e);
		//close();
		return false;
	}
}// tSIPC::tryToGet


tByteArray tSIPC::getInput(unsigned &addr, int timeout)
{
	tByteArray ba;
	for ( tTimer ti;; tTimer::delay(10) ){
		if ( !Alive() )  return ba;
		tryToGet(0);               // 10
		tryToOut(0);               // 10
		if ( !poolInData.empty() ){
			ba = getPk(poolInData[0]);
			if ( !ba.empty() ) {
				addr = poolInAddr[0];
				if ( poolInData[0].empty() ){
					poolInAddr.pop_front();
					poolInData.pop_front();
				}
				return ba;
			}
		}
		if ( ti.check(timeout) )  break;
	}
	//debugLog("tSIPC::getInput(tm) ti=" + ToString(ti.get()));
	return ba;
}// tSIPC::getInput


bool tSIPC::pkOk(tByteArray &ba, unsigned *size)
{
// debugLog("pkOk: " + BufDump(ba.data(), ba.size()));
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
//     debugLog("pkOk - ok size = " + ToString(pkSize));
					return true;
				}
				ba.clear();
			}
		}
	}
	return false;
}// tSIPC::pkOk


tByteArray tSIPC::getPk(tByteArray &ba)
{
	tByteArray ret;
	unsigned pkSize;
	if ( pkOk(ba, &pkSize) ){
		//debugLog("getPk: " + BufDump(ba.data(), ba.size()));
		ret = tByteArray(ba.data() + 6, pkSize - 10);          // del envelope
		if ( ba[5] != '\0' ) ret.unzip();
		ba.erase(0, pkSize);
	}
	//debugLog("getPk size = " + ToString(ret.size()));
	return ret;
}// tSIPC::getPk




tSIPC &operator<<(tSIPC &so, tPk *p)
{
	if ( p ){
		if ( so.Alive() ){
			if ( !p->conn )  nError::Throw("tIPC << tPk *: in tPk conn is NULL !");
			p->conn->send(p);
		}
		delete p;
	}
	return so;
}// operator<<


tSIPC &send(tSIPC &so, tPk *p, unsigned addr)
{
	if ( p ){
		if ( so.Alive() ){
			so.setOutput(p->put(), addr);
		}
		delete p;
	}
	return so;
}// send


tPk *getPk(tSIPC &s, int timeout)
//inline tPk *getPk(tSIPC &s, int timeout = 0)
{
	unsigned addr;
	tByteArray ba = s.getInput(addr, timeout);
	return pkReceive(ba, new tIPCConnection(s, addr));
}// getPk
