#ifndef __tPVector_h
#define __tPVector_h 1

#ifndef __tX_del_h
#include "tX_del.h"
#endif

#include <vector>
using std::vector;




template <class T>
class tPVector : public std::vector<T*>{
public:
	tPVector() : vector<T*>() {};
	tPVector(typename vector<T*>::size_type n) : vector<T*>(n) {};
	virtual ~tPVector() { clear(); };

private:
	tPVector(const tPVector<T>& x);
	tPVector<T>& operator= (const tPVector<T>& x);

	void assign(typename vector<T*>::iterator first, typename vector<T*>::iterator last);
	void assign(typename vector<T*>::size_type, T *t); 

public:
	void erase(typename vector<T*>::iterator pos) {
	  delete (*pos); this->vector<T*>::erase(pos); };

	void erase(typename vector<T*>::iterator first, typename vector<T*>::iterator last) {
	  __tX_del<T> fx;
	  std::for_each(first, last, fx);
//   for ( std::vector<T*>::iterator i = first; i != last; ++i )  delete (*i);
	  this->vector<T*>::erase(first, last); 
	};

	void clear() { erase(begin(), end()); };

};// class tPVector




#endif // __tPVector

