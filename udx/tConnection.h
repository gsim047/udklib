#ifndef __tConnection_h
#define __tConnection_h 1



class tPk;



class tConnection {
public:
	tConnection() {};
	virtual ~tConnection() {};

	virtual bool send(tPk *p) const = 0;
	tConnection &operator<<(tPk *p);

private:
	tConnection(const tConnection &);
	tConnection &operator=(const tConnection &);
};// class tConnection




#endif // __tConnection_h
