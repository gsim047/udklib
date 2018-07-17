#ifndef __tDebug_h
#define __tDebug_h 1

#include "nError.h"




class __init_tDebug{
public:
	__init_tDebug();
	~__init_tDebug();

	static int cnt;
};// class __init_tDebug

static __init_tDebug __init_tDebug_ex;




namespace nError{
	class DebugLogError : public Message {
	public:
		DebugLogError();
		DebugLogError(const string &s);
	};// class DebugLogError
};// namespace nError



// Debug Level n:
//   n < 0       - only error messages
//   n = 0       - no debug at all
//   n > 0       - all debug and error messages
void debugSetLevel(int level = -1);

// if fname == 'cerr', then use cerr :-)
// if !strict, create file 'fname.YYMMDD-hhmmss.log'
void debugSetFileName(const string &fname, bool strict = false);

bool isDebugStd();

void debugLog(const string &text);
void errorLog(const string &text);
void infoLog(const string &text);

void debugFmt(const char *fmt, ...);
void errorFmt(const char *fmt, ...);
void infoFmt(const char *fmt, ...);

void debugDump(const void *buf, size_t size);
void errorDump(const void *buf, size_t size);
void infoDump(const void *buf, size_t size);

inline void debugDump(const string &s) { debugDump(s.c_str(), s.size()); }
inline void errorDump(const string &s) { errorDump(s.c_str(), s.size()); }
inline void infoDump(const string &s) { infoDump(s.c_str(), s.size()); }



/*
class ikLogger{  // init/kill logger
public:
	ikLogger(const string &iniMsg, const string &killMsg, bool useCnt);
	virtual ~ikLogger();

	int count() const { return cnt; }

private:
	void log(const string &msg, int cur);

private:
	string msg2;
	bool ucnt;

	static int cnt;
};// class ikLogger
*/


#endif // __tDebug_h
