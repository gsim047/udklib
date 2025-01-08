#ifndef __tJob_h
#define __tJob_h 1




class tJob {
public:
	enum Status {
		waiting         = 0,
		executing       = 1,
		done            = 2
	};

public:
	tJob() : status(waiting), result(0) {};
	virtual ~tJob() {};

	virtual void exec() = 0;
	void Exec();

	int getStatus() const { return status; }
	int getResult() const { return result; }

	static void addJob(tJob *j);

private:
	tJob(const tJob &);
	tJob &operator=(const tJob &);

protected:
	void setStatus(int Status) { status = Status; }
	void setResult(int Result) { result = Result; }

private:
	int status;
	int result;
};// class tJob




#endif // __tJob_h
