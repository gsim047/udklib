#ifndef __tPkSv_h
#define __tPkSv_h 1

#ifndef __tPk_h
#include "tPk.h"
#endif



// must be present somewhere !!!
bool doLogin(const string &Login, const string &Password);



class tPk_Login : public tPk{
public:
	tPk_Login() {};
	tPk_Login(const string &Login, const string &Password) : 
		login(Login), password(Password) {};

public:
	void get(tByteArray &ib) { ib >> login >> password; };
	void put(tByteArray &ob) { ob << login << password; };
	tPk *exec(const string &) const;

public:
	string login;
	string password;
};// class tPk_Login




class tPk_Logged : public tPk{
public:
	tPk_Logged() {};

public:
	void get(tByteArray &) { };
	void put(tByteArray &) { };
	tPk *exec(const string &) const;

};// class tPk_Logged




class tPk_Ping : public tPk{
public:
	tPk_Ping();
	tPk_Ping(const tPk_Ping &s) : n(s.n) {};

public:
	void get(tByteArray &ib) { ib >> n; };
	void put(tByteArray &ob) { ob << n; };
	tPk *exec(const string &) const;

private:
	static int N;

public:
	int n;
};// class tPk_Ping




class tPk_Message : public tPk{
public:
	tPk_Message() : code(0) {};
	tPk_Message(const string &Msg) : code(0), msg(Msg) {};
	tPk_Message(int Code, const string &Msg) : code(Code), msg(Msg) {};

public:
	void get(tByteArray &ib) { ib >> code >> msg; };
	void put(tByteArray &ob) { ob << code << msg; };
	tPk *exec(const string &) const;

public:
	int code;
	string msg;
};// class tPk_Ping




#endif // __tPkSv_h
