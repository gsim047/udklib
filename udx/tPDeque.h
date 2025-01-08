#ifndef __tPDeque_h
#define __tPDeque_h 1

#ifndef __tX_del_h
#include "tX_del.h"
#endif

#include <deque>
using std::deque;




template <class T>
class tPDeque : public deque<T*>{
public:
	tPDeque() : deque<T*>() {};
	tPDeque(typename deque<T*>::size_type n) : deque<T*>(n) {};
	virtual ~tPDeque() { clear(); };

private:
	tPDeque(const tPDeque<T>& x);
	tPDeque<T>& operator= (const tPDeque<T>& x);

	void assign(typename deque<T*>::iterator first, typename deque<T*>::iterator last);
	void assign(typename deque<T*>::size_type, T *t); 

public:
	void erase(typename deque<T*>::iterator pos) {
	  delete (*pos); this->deque<T*>::erase(pos); };

	void erase(typename deque<T*>::iterator first, typename deque<T*>::iterator last) {
	  __tX_del<T> fx;
	  std::for_each(first, last, fx);
//   for ( deque<T*>::iterator i = first; i != last; ++i )  delete (*i);
	  this->deque<T*>::erase(first, last); 
	};

	void clear() { erase(begin(), end()); };

};// class tPDeque




#endif // __tPDeque

