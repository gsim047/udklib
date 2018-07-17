#include "tDebug.h"

#include "fPid.h"
#include "tFile.h"
#include "fString.h"
#include "nError.h"
#include "tAutoThread.h"
//#include "tFile.h"

#include <stdio.h>
#include <stdarg.h>
//#include <deque>
//#include <sstream>
//#include <thread>
//using std::cout;
//using std::cerr;
//using std::endl;

#ifdef __GNUG__
#include <sys/time.h>
#ifdef __CYGWIN__
#include <unistd.h>
#else
#include <time.h>
#endif // __CYGWIN__
#endif // __GNUG__

#ifdef _WIN32
#include <time.h>
#endif
#include <mutex>




static string longTS()
{
	char buf[30];
	time_t cur_secs;
	time(&cur_secs);
	struct tm *cur_time = localtime(&cur_secs);
	strftime(buf, 29, "%Y-%m-%d %H:%M:%S", cur_time);
	return string(buf);
}// longTS


static string shortTS()
{
	char buf[30];
	time_t cur_secs;
	time(&cur_secs);
	struct tm *cur_time = localtime(&cur_secs);
	strftime(buf, 29, "%y%m%d-%H%M%S", cur_time);
	return string(buf);
}// shortTS




namespace nError {

	DebugLogError::DebugLogError() : Message("debugLog error") 
	{
	}// DebugLogError::DebugLogError

	DebugLogError::DebugLogError(const string &s) : Message("debugLog error: " + s) 
	{
	}// DebugLogError::DebugLogError

};// namespace nError




static string __ts_n_tid()
{
	return string(shortTS() + " [" + ToString(tAutoThread::uid(), 4) + "]: ");
}// __ts_n_tid



static const char * const __tDebug_errMsg1 = { "tDebugFile doesn't inited !!!" };
static const char * const __tDebug_errMsg2 = { "Can't open logfile '" };
static const char * const __tDebug_errMsg3 = { "' : " };
int __init_tDebug::cnt = 0;




class tDebugFile{
public:
	tDebugFile(int Level = -1) : level(Level), _std(true), f(nullptr), oldtype(-111) {};
	tDebugFile(const string &fname, int Level = -1) :
		level(Level), filename(fname), _std(false), f(NULL), oldtype(-111) {};
	~tDebugFile() { if ( f )  fclose(f); };

public:
	void setFileName(const string &FileName, bool strict);
	int  setLevel(int new_level);

	bool log(const string &text, unsigned type, bool decor);

	void close() { if ( f ){ fclose(f); f = NULL; } }
private:
	void flush(const string &s);

private:
	int level;
	std::mutex dflock;
	string oldtext;
	unsigned oldtype;

public:
	bool _std;
	string filename;
	FILE *f;
};// class tDebugFile



void tDebugFile::setFileName(const string &FileName, bool strict)
{
	std::lock_guard<std::mutex> ml(dflock);

	if ( FileName.empty() || FileName == "stderr" || FileName == "cerr" ){
		_std = true;
		//setLevel(1);
	}else{
		_std = false;
		filename = FileName;
		if ( !strict )  filename = ChangeFileExt(filename, shortTS() + ".log");
	}
	//wrlines = 0;
}// tDebugFile::setFileName


void tDebugFile::flush(const string &s)
{
	std::lock_guard<std::mutex> ml(dflock);
	try{
		if ( _std ){
			fprintf(stderr, "%s\n", s.c_str());
		}else{
			bool op = (f == NULL);
			if ( !f ){
//				if ( !_std ){
//					tFILE::DirCreate(ExtractFilePath(filename));
//				}
				f = fopen(filename.c_str(), "at");
			} 
			if ( f ){
				fprintf(f, "%s\n", s.c_str());
				fflush(f);   
				if ( op ){
					fclose(f);
					f = NULL;
				}
			}
		}
	}
	catch ( nError::Message &m ){
		nError::DebugLogError e(__tDebug_errMsg2 + filename + __tDebug_errMsg3 + m);
		throw e;
	}
}// tDebugFile::flush


bool tDebugFile::log(const string &text, unsigned type, bool decor)
{
	if ( level == 0 )  return false;
	if ( type == 0 && level < 0 )  return false;

	if ( type == 0 && text == oldtext && type == oldtype )  return true;
	oldtext = text;
	oldtype = type;

	string s;
	if ( decor ){
		if ( type == 2 )  s = "!!!";
		s += __ts_n_tid();
	} 
	s += text;
	flush(s);
	return true;
}// tDebugFile::log


int tDebugFile::setLevel(int new_level)
{
	int old = level;
	level = new_level;
	return old;
}// tDebugFile::setLevel


static tDebugFile *df = NULL;




void debugSetLevel(int level)
{
	if ( !df )  fprintf(stderr, "%s\n", __tDebug_errMsg1);
	else  df->setLevel(level);
}// debugSetLevel


void debugSetFileName(const string &fname, bool strict)
{
	if ( !df )  fprintf(stderr, "%s\n", __tDebug_errMsg1);
	else  df->setFileName(fname, strict);
}// debugSetFileName


inline void errOut(const string &text)
{
	fprintf(stderr, "%s\n", text.c_str());
}// errOut


void debugLog(const string &text)
{
	if ( !df ){
		fprintf(stderr, "%s\n", __tDebug_errMsg1);
		errOut(text);
	}else{
		df->log(text, 0, true);
	}
}// debugLog


void errorLog(const string &text)
{
	if ( !df ){
		fprintf(stderr, "%s\n", __tDebug_errMsg1);
		errOut(text);
	}else{
		df->log(text, 2, true);
	}
}// errorLog


void infoLog(const string &text)
{
	if ( !df ){
		fprintf(stderr, "%s\n", __tDebug_errMsg1);
		errOut(text);
	}else{
		df->log(text, 1, true);
	}
}// infoLog


static char __str[0x8001];

void debugFmt(const char *fmt, ...)
{
	va_list argptr;

	va_start(argptr, fmt);
	//int cnt = 
#ifdef __GNUG__
	vsnprintf(__str, 0x8000, fmt, argptr);
#else
	_vsnprintf(__str, 0x8000, fmt, argptr);
#endif
	va_end(argptr);

	debugLog(__str);
}// debugFmt


void errorFmt(const char *fmt, ...)
{
	va_list argptr;

	va_start(argptr, fmt);
	//int cnt = 
#ifdef __GNUG__
	vsnprintf(__str, 0x8000, fmt, argptr);
#else
	_vsnprintf(__str, 0x8000, fmt, argptr);
#endif
	va_end(argptr);

	errorLog(__str);
}// errorFmt


void infoFmt(const char *fmt, ...)
{
	va_list argptr;

	va_start(argptr, fmt);
	//int cnt = 
#ifdef __GNUG__
	vsnprintf(__str, 0x8000, fmt, argptr);
#else
	_vsnprintf(__str, 0x8000, fmt, argptr);
#endif
	va_end(argptr);

	infoLog(__str);
}// infoFmt


static string bufDump(const unsigned char *c, size_t &is, size_t size)
{
	string str = ToStringHex(is, 5) + ": ";
	string cc;
	for ( size_t i = 0; i < 16 && (i + is) < size; i++ ){
		if ( i == 8 )  str += " -";
		str += ' ';
		unsigned char ch = c[i + is];
		str += ch_hexValue[ch];
		cc += ( ch >= ' ' ) ? ch : '.';
	}
	is += 16;
	if ( !cc.empty() ){
		size_t ccsize = cc.size();
		cc += string(16 - ccsize, ' ');
		str += string(3 * (16 - ccsize) + ((ccsize <= 8) ? 2 : 0), ' ') + "   [" + cc + "]";
	}
	return str;
}// bufDump


void debugDump(const void *buf, size_t size)
{
	if ( !buf || !size )  return;

	if ( !df ){
		fprintf(stderr, "%s\n", __tDebug_errMsg1);
		//errOut(text);
		return;
	}

	if ( !df->f ){
		df->f = fopen(df->filename.c_str(), "at");
	} 
	df->log("", 0, true);

	const unsigned char *cbuf = (const unsigned char *)buf;
	for ( size_t i = 0; i < size; ){
		string s = bufDump(cbuf, i, size);
		df->log(s, 0, false);
	}
	if ( df->f )  df->close();
}// debugDump


void errorDump(const void *buf, size_t size)
{
	if ( !buf || !size )  return;

	if ( !df ){
		fprintf(stderr, "%s\n", __tDebug_errMsg1);
		return;
	}

	if ( !df->f ){
		df->f = fopen(df->filename.c_str(), "at");
	} 
	df->log("", 2, true);

	const unsigned char *cbuf = (const unsigned char *)buf;
	for ( size_t i = 0; i < size; ){
		string s = bufDump(cbuf, i, size);
		df->log(s, 2, false);
	}
	if ( df->f )  df->close();
}// errorDump


void infoDump(const void *buf, size_t size)
{
	if ( !buf || !size )  return;

	if ( !df ){
		fprintf(stderr, "%s\n", __tDebug_errMsg1);
		return;
	}

	if ( !df->f ){
		df->f = fopen(df->filename.c_str(), "at");
	} 
	df->log("", 1, true);

	const unsigned char *cbuf = (const unsigned char *)buf;
	for ( size_t i = 0; i < size; ){
		string s = bufDump(cbuf, i, size);
		df->log(s, 1, false);
	}
	if ( df->f )  df->close();
}// infoDump





__init_tDebug::__init_tDebug()
{
	if ( !(cnt++) )  df = new tDebugFile();
}// __init_tDebug::__init_tDebug

__init_tDebug::~__init_tDebug()
{
	if ( !(--cnt) )  delete df;
}// __init_tDebug::~__init_tDebug



bool isDebugStd()
{
	if ( !df )  return false;
	return df->_std;
}// isDebugStd



/*
int ikLogger::cnt = 0;


ikLogger::ikLogger(const string &iniMsg, const string &killMsg, bool useCnt) :
	msg2(killMsg),
	ucnt(useCnt)
{
	log(iniMsg, ++cnt);
}// ikLogger::ikLogger


ikLogger::~ikLogger()
{
	log(msg2, --cnt);
}// ikLogger::~ikLogger


void ikLogger::log(const string &msg, int cur)
{
	if ( !msg.empty() ){
		if ( ucnt ){
			debugLog(msg2 + " " + std::to_string(cur));
		}else{
			debugLog(msg2);
		}
	}
}// ikLogger::log
*/
