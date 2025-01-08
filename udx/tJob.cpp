#include "tJob.h"
#include "tSyncDeque.h"
#include "tThread.h"
//#include "fTime.h"
#include "tTimer.h"

#include <vector>




void tJob::Exec()
{
	setStatus(executing);
	exec();
	setStatus(done);
}// tJob::Exec




static tSyncDeque<tJob> jque;



class jobThread : public tThread {
public:
  jobThread() : j(NULL) {};

	void run();

	bool testJob() const { tMutexLock l(jm); return j != NULL; };
	bool addJob(tJob *J);

private:
	void clearJob() { tMutexLock l(jm); if ( j ){ delete j; j = NULL; } };

	mutable tMutex jm;
	tJob *j;
};// class jobThread


bool jobThread::addJob(tJob *J)
{
	tMutexLock l(jm);
	if ( j ) return false;
	j = J;
	return true;
}// jobThread::addJob


void jobThread::run()
{
	while ( !testStop() ){
		if ( testJob() ){
			j->Exec();
			clearJob();
		}else{
			sleep(10);
		}
	}
	clearJob();
}// jobThread::run

static std::vector<jobThread *> ThrPool;


static void addThread()
{
	jobThread *j = new jobThread();
	j->setDaemon();
	j->start();
	ThrPool.push_back(j);
}// addThread



class jobMan : public tThread {
public:
	jobMan(unsigned nmin, unsigned nmax) : nMin(nmin), nMax(nmax) {};

	void run();

private:
	unsigned nMin;
	unsigned nMax;
};// class jobMan



void jobMan::run()
{
	for ( unsigned i = 0; i < nMin; i++ )  addThread();
	while ( !testStop() ){
		tJob *j = jque.pop();
		if ( j ){
			tTimer tm;
			while ( j ){
				if ( tm.check(4000) )  addThread();
				for ( unsigned i = 0; i < ThrPool.size(); i++ ){
					if ( ThrPool[i]->addJob(j) ){
						j = NULL;
						break;
					}
				}
				if ( j ) tTimer::delay(10);
			}
		}else{
			tTimer::delay(10);
		}
	}
}// jobMan::run




static jobMan *ThrPoolMan = NULL;


static void init()
{
	if ( !ThrPoolMan ){
		ThrPoolMan = new jobMan(5, 10);
		ThrPoolMan->setDaemon();
		ThrPoolMan->start();
	}
}// init


void tJob::addJob(tJob *j)
{
	if ( j ){
		init();
		jque.push(j);
	}
}// tJob::addJob
