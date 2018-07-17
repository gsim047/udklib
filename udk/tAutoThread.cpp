#include "tAutoThread.h"
#include <mutex>
#include <sstream>
#include <map>
#include "tDebug.h"


static std::mutex th_mu;
static int th_cnt = 0;         // счётчик запущенных потоков
static std::map<string, int> th_names;


static int __cnt_inc()
{
	std::lock_guard<std::mutex> l(th_mu);
	th_cnt++;
	return th_cnt;
}

static int __cnt_dec()
{
	std::lock_guard<std::mutex> l(th_mu);
	th_cnt--;
	return th_cnt;
}


bool tAutoThread::th_stop_all = false;      // флаг "останавливать все потоки"


bool tAutoThread::operator==(const tAutoThread &s) const
{
  if ( &s == this )  return true;
  return false;
}// tAutoThread::operator==


int tAutoThread::uid()
{
	std::thread::id this_id = std::this_thread::get_id();
	std::ostringstream out; 
	out << this_id;
	string nm = out.str();
	std::lock_guard<std::mutex> l(th_mu);
	std::map<string, int>::const_iterator it = th_names.find(nm);
	if ( it != th_names.end() ) return it->second;
	int ret = th_names.size()+1;
	th_names[nm] = ret;
	return ret;
}// tAutoThread::uid



void __thread_at(tAutoThread *th)
{
	if ( th ){
//		std::thread::id this_id = std::this_thread::get_id();
//		std::ostringstream out; 
//		out << this_id;
		th->name += std::to_string(tAutoThread::uid());
		string name = "__thread_at[" + th->name + "]: ";

		__cnt_inc();
		while ( !(th->th_start) ){
			th->wait(10);     // ждём, пока поток не "запустят"
		}

		try{
			debugLog(name + "begin");
			if ( !th->begin() ){                                                     // выполняем начальный код потока
				debugLog(name + "begin returns false, exiting..");
			}else
			for ( ; !th->th_stop && !th->th_stop_all; th->wait() ){          // цикл, пока нет сигнала stop для этого потока либо для всех потоков
				if ( !th->run() ){                                                     // выполняем основной цикл потока
					debugLog(name + "run returns false, exiting..");
					break;
				}
				std::this_thread::yield();
			}
			debugLog(name + "end");
			if ( !th->end() ){                                                       // выполняем завершающий код потока
				debugLog(name + "end returns false");
			}
		}
		catch (...){
			debugLog(name + "exception...");
		}
		__cnt_dec();
	}else{
		debugLog("__thread_at: th is NULL!");
	}
}// __thread_at


tAutoThread::tAutoThread()
{
}// tAutoThread::~tAutoThread


tAutoThread::tAutoThread(const string& Name) :
	name(Name)
{
}// tAutoThread::~tAutoThread


tAutoThread::~tAutoThread()
{
	if ( thr ){                                // если поток был успешно запущен
		if ( !th_stop ){
			th_stop = true;                        // выставляем флаг stop для него
		}
		join();                                  // ожидаем присоединения
	}
}// tAutoThread::~tAutoThread


bool tAutoThread::alive() const
{
	return (thr != nullptr) && !th_stop;
}

void tAutoThread::start()
{
	if ( !thr ){
		thr = new std::thread(__thread_at, this);
	}
	th_start = true;
	th_stop = false;
}


void tAutoThread::stop()
{
	th_stop = true;
	th_start = false;
}


void tAutoThread::stopAll()
{
	th_stop_all = true;
}// 


void tAutoThread::join()
{
	if ( thr ){
		thr->join();
		delete thr;                              // удаляем
		thr = nullptr;
	}
}// tAutoThread::join


void tAutoThread::joinAll()
{
	while ( th_cnt > 0 )  wait();
}// tAutoThread::join


void tAutoThread::wait(int ms)
{
	std::chrono::milliseconds dura( ms );
	std::this_thread::sleep_for( dura );
}// tAutoThread::wait

