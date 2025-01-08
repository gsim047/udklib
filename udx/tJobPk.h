#ifndef __tJobPk_h
#define __tJobPk_h 1

#ifndef __tJob_h
#include "tJob.h"
#endif // __tJob_h

#ifndef __tPk_h
#include "tPk.h"
#endif // __tPk_h




class tJobPk : public tJob {
public:
	tJobPk(tPk *P, const string &Login) : tJob(), p(P), login(Login) {};
	virtual ~tJobPk() { if ( p ) delete p; };

	virtual void exec();

	static void addJob(tPk *p, const string &login);

private:
	tPk *p;
	string login;
};// class tJobPk




#endif // __tJobPk_h
