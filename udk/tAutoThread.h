#ifndef __tAutoThread_h
#define __tAutoThread_h 1

#include <thread>
#include <string>
using std::string;


// Абстрактный класс для служебных потоков
// Служебные потоки должны оформляться классами-наследниками tAutoThread
// В них необходимо определить процедуру run() - она вызывается в цикле выполнения потока, при желании можно определить begin() и end()
// Поток допускает многократный запуск/остановку через вызовы start/stop+join

class tAutoThread {
	friend void __thread_at(tAutoThread *th);

public:
	tAutoThread();
	tAutoThread(const string& Name);
	virtual ~tAutoThread();

	bool operator==(const tAutoThread &s) const;
	bool operator!=(const tAutoThread &s) const { return !operator==(s); }

	virtual bool begin() { return true; };    // начальный код выполнения потока (может быть пустым). возврат false останавливает поток
	virtual bool run() = 0;                   // основной выполняемый код потока. вызывается из цикла в классе tAutoThread, потому сам не должен организовывать циклы. возврат false останавливает поток
	virtual bool end() { return true; };      // конечный код выполнения потока (может быть пустым)

	bool alive() const;         // поток жив (уже запущен и ещё не остановлен)
	void start();               // запустить поток (при этом создается std::thread). если поток уже запущен, не делает ничего

	string tid() const { return name; }    // thread id

	void stop();                // послать потоку сигнал остановки
	void join();                // дождаться окончания потока / автоматически вызывается в деструкторе / удаляет std::thread

	static void stopAll();      // послать всем потокам сигнал остановки
	static void joinAll();      // дождаться окончания всех потоков

	static void wait(int ms = 5);        // пауза/стандартная пауза
	static int uid();

private:
	tAutoThread(const tAutoThread &s);                // запрещено создание копии класса tAutoThread
	tAutoThread &operator=(const tAutoThread &s);     // запрещено присвоение класса tAutoThread

protected:
	string name;

	std::thread* thr = nullptr;

	bool th_start = false;       // флаг разрешения выполняться потоку. должен устанавливаться явным вызовом start, автоматически не стартует
	bool th_stop = false;        // флаг остановки потока. устанавливается вызовом stop. поток остановится после ближайшего выхода из run()
	static bool th_stop_all;     // флаг общей остановки всех служебных потоков. устанавливается вызовом stopAll(). 
};// class tAutoThread




#endif // __tAutoThread_h
