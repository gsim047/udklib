#ifndef __tResult_h
#define __tResult_h 1

#include "nError.h"




class tResult {
public:
	tResult() : code(0) {};
	tResult(int Code) : code(Code) {};
	tResult(int Code, const string &Msg) : code(Code), msg(Msg) {};
	tResult(const string &Msg) : code(-1), msg(Msg) {};

protected:
	tResult(const tResult &s) : code(s.code), msg(s.msg) {};
	tResult &operator=(const tResult &s);

	void setResult(int Code, const string &Msg = "") const { code = Code; msg = Msg; }
	void setResult(const string &Msg) const { code = -1; msg = Msg; }

	void checkResult() const;

public:
	void cR() const { checkResult(); }
	bool badCode() const { return code != 0; }
	int getCode() const { return code; }
	void clearResult() { code = 0; msg = ""; }

private:
	mutable int code;
	mutable string msg;
};// class tResult




#endif // __tResult_h
