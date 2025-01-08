#include "tCfgKey.h"
#include "fString.h"
#include "fBase64.h"
#include <limits.h>
#include <float.h>




tCfgString::tCfgString(const string &val) :
	p(NULL),
	Val(val)
{
}// tCfgString::tCfgString


tCfgString::tCfgString(tCfgKey *k) :
	p(k ? k->link() : NULL)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_String ) 
			nError::Throw("tCfgString: Invalid type : " + ToString(p->type()));
		Val = p->get();
	}
}// tCfgString::tCfgString


tCfgString::tCfgString(tCfg &f, const string &Path, const string &Name) :
	p(f.makeKey(tCfgKey::T_String, Path, Name, ""))
{
	if ( p ){ 
		if ( p->type() != tCfgKey::T_String ) 
			nError::Throw("tCfgString: Invalid type : " + ToString(p->type()));
		Val = p->get();
	}
}// tCfgString::tCfgString


tCfgString::tCfgString(tCfg &f, const string &Path, const string &Name, const string &val) :
	p(f.makeKey(tCfgKey::T_String, Path, Name, val)),
	Val(val)
{
	if ( p ){ 
		if ( p->type() != tCfgKey::T_String ) 
			nError::Throw("tCfgString: Invalid type : " + ToString(p->type()));
		Val = p->get();
	}
}// tCfgString::tCfgString


tCfgString::~tCfgString()
{
	if ( p )  p->unlink();
}// tCfgString::~tCfgString


void tCfgString::set(const string &val)
{
	//tMutexLock l(m);
	Val = val;
	if ( p )  p->set(Val);
}// tCfgString::set


tCfgString::operator string() const 
{
	tMutexLock l(m); 
	return Val; 
}// tCfgString::operator string


void tCfgString::setInfo(const string &Inf)
{
	tMutexLock l(m);
	if ( p )  p->setInfo(Inf);
}// tCfgString::setInfo


void tCfgString::setRO(bool RO)
{
	tMutexLock l(m);
	if ( p )  p->setRO(RO);
}// tCfgString::setRO




string tCfgBin::d2v(const void *val, unsigned size)
{
	string ret;
	if ( val && size )  ret = string((const char *)val, size);
	return ret;
}// tCfgBin::d2v


tCfgBin::tCfgBin(const string &val) :
	p(NULL),
	Val(val)
{
}// tCfgBin::tCfgBin


tCfgBin::tCfgBin(const void *val, unsigned size) :
	p(NULL),
	Val(d2v(val, size))
{
}// tCfgBin::tCfgBin


tCfgBin::tCfgBin(tCfgKey *k) :
	p(k ? k->link() : NULL)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Bin ) 
			nError::Throw("tCfgBin: Invalid type : " + ToString(p->type()));
		Val = decode_base64(p->get());
	}
}// tCfgBin::tCfgBin


tCfgBin::tCfgBin(tCfg &f, const string &Path, const string &Name) :
	p(f.makeKey(tCfgKey::T_Bin, Path, Name, ""))
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Bin ) 
			nError::Throw("tCfgBin: Invalid type : " + ToString(p->type()));
		Val = decode_base64(p->get());
	}
}// tCfgBin::tCfgBin


tCfgBin::tCfgBin(tCfg &f, const string &Path, const string &Name, 
                 const void *val, unsigned size) :
	p(f.makeKey(tCfgKey::T_Bin, Path, Name, 
	            encode_base64(d2v(val, size), false))),
													//kill_all_of(encode_base64(d2v(val, size), false), "\r\n"))),
	Val(d2v(val, size))
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Bin ) 
			nError::Throw("tCfgBin: Invalid type : " + ToString(p->type()));
		Val = decode_base64(p->get());
	}
}// tCfgBin::tCfgBin


tCfgBin::~tCfgBin()
{
	if ( p )  p->unlink();
}// tCfgBin::~tCfgBin


void tCfgBin::set(const string &val)
{
	//tMutexLock l(m);
	Val = val;
	//if ( p )  p->set(kill_all_of(encode_base64(Val, false), "\r\n"));
	if ( p )  p->set(encode_base64(Val, false));
}// tCfgBin::set


tCfgBin::operator string() const 
{
	tMutexLock l(m); 
	return Val; 
}// tCfgBin::operator string


void tCfgBin::setInfo(const string &Inf)
{
	tMutexLock l(m);
	if ( p )  p->setInfo(Inf);
}// tCfgBin::setInfo


void tCfgBin::setRO(bool RO)
{
	tMutexLock l(m);
	if ( p )  p->setRO(RO);
}// tCfgBin::setRO




string tCfgBool::b2s(bool v)
{
	return v ? "true" : "false";
}// tCfgBool::b2s


bool tCfgBool::s2b(const string &s)
{
	return s == "true";
}// tCfgBool::s2b


tCfgBool::tCfgBool(bool val) :
	p(NULL),
	Val(val)
{
}// tCfgBool::tCfgBool


tCfgBool::tCfgBool(tCfgKey *k) :
	p(k ? k->link() : NULL),
	Val(false)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Bool ) 
			nError::Throw("tCfgBool: Invalid type : " + ToString(p->type()));
		Val = s2b(p->get());
	}
}// tCfgBool::tCfgBool


tCfgBool::tCfgBool(tCfg &f, const string &Path, const string &Name) :
	p(f.makeKey(tCfgKey::T_Bool, Path, Name, "false")),
	Val(false)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Bool ) 
			nError::Throw("tCfgBool: Invalid type : " + ToString(p->type()));
		Val = s2b(p->get());
	}
}// tCfgBool::tCfgBool


tCfgBool::tCfgBool(tCfg &f, const string &Path, const string &Name, bool val) :
	p(f.makeKey(tCfgKey::T_Bool, Path, Name, b2s(val))),
	Val(val)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Bool ) 
			nError::Throw("tCfgBool: Invalid type : " + ToString(p->type()));
		Val = s2b(p->get());
	}
}// tCfgBool::tCfgBool


tCfgBool::~tCfgBool()
{
	if ( p )  p->unlink();
}// tCfgBool::~tCfgBool


tCfgBool &tCfgBool::operator=(const tCfgBool &s)
{
	tMutexLock l(m);
	if ( this != &s ){
		if ( Val != s.Val ){
			Val = s.Val;
			if ( p )  p->set(b2s(Val));
		}
	}
	return *this;
}// tCfgBool::operator=


bool tCfgBool::set(bool val)
{
	//tMutexLock l(m);
	Val = val;
	if ( p )  p->set(b2s(Val));
	return Val;
}// tCfgBool::set


string tCfgBool::getInfo() const
{
	tMutexLock l(m);
	string ret;
	if ( p )  ret = p->getInfo();
	return ret;
}// tCfgBool::getInfo


bool tCfgBool::getRO() const
{
	tMutexLock l(m);
	bool ret = false;
	if ( p )  ret = p->getRO();
	return ret;
}// tCfgBool::getRO


void tCfgBool::setInfo(const string &Inf)
{
	tMutexLock l(m);
	if ( p )  p->setInfo(Inf);
}// tCfgBool::setInfo


void tCfgBool::setRO(bool RO)
{
	tMutexLock l(m);
	if ( p )  p->setRO(RO);
}// tCfgBool::setRO




int tCfgInt::s2i(const string &s, int def)
{
	return ToInt(s, def);
}// tCfgInt::s2i


string tCfgInt::i2s(int v)
{
	return ToString(v);
}// tCfgInt::i2s


tCfgInt::tCfgInt(int val) :
	p(NULL),
	Val(val),
	Min(INT_MIN),
	Max(INT_MAX)
{
}// tCfgInt::tCfgInt


tCfgInt::tCfgInt(tCfgKey *k) :
	p(k ? k->link() : NULL),
	Val(0),
	Min(INT_MIN),
	Max(INT_MAX)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Int ) 
			nError::Throw("tCfgInt: Invalid type : " + ToString(p->type()));
		Val = s2i(p->get(), 0);
		Min = s2i(p->getMin(), INT_MIN);
		Max = s2i(p->getMax(), INT_MAX);
	}
}// tCfgInt::tCfgInt


tCfgInt::tCfgInt(tCfg &f, const string &Path, const string &Name) :
	p(f.makeKey(tCfgKey::T_Int, Path, Name, "0")),
	Val(0),
	Min(INT_MIN),
	Max(INT_MAX)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Int ) 
			nError::Throw("tCfgInt: Invalid type : " + ToString(p->type()));
		Val = s2i(p->get(), 0);
		Min = s2i(p->getMin(), INT_MIN);
		Max = s2i(p->getMax(), INT_MAX);
	}
}// tCfgInt::tCfgInt


tCfgInt::tCfgInt(tCfg &f, const string &Path, const string &Name, int val) :
	p(f.makeKey(tCfgKey::T_Int, Path, Name, i2s(val))),
	Val(val),
	Min(INT_MIN),
	Max(INT_MAX)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Int ) 
			nError::Throw("tCfgInt: Invalid type : " + ToString(p->type()));
		Val = s2i(p->get(), Val);
		Min = s2i(p->getMin(), INT_MIN);
		Max = s2i(p->getMax(), INT_MAX);
	}
}// tCfgInt::tCfgInt


tCfgInt::tCfgInt(tCfg &f, const string &Path, const string &Name, int val, int _Min, int _Max) :
	p(f.makeKey(tCfgKey::T_Int, Path, Name, i2s(val), i2s(_Min), i2s(_Max))),
	Val(val),
	Min(_Min),
	Max(_Max)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Int ) 
			nError::Throw("tCfgInt: Invalid type : " + ToString(p->type()));
		Val = s2i(p->get(), Val);
		Min = s2i(p->getMin(), Min);
		Max = s2i(p->getMax(), Max);
	}
}// tCfgInt::tCfgInt


tCfgInt::~tCfgInt()
{
	if ( p )  p->unlink();
}// tCfgInt::~tCfgInt


tCfgInt &tCfgInt::operator=(const tCfgInt &s)
{
	tMutexLock l(m);
	if ( this != &s ){
		if ( Val != s.Val ){
			Val = s.Val;
			if ( p )  p->set(i2s(Val));
		}
		if ( Min != s.Min ){
			Min = s.Min;
			if ( p )  p->setMin(i2s(Min));
		}
		if ( Max != s.Max ){
			Max = s.Max;
			if ( p )  p->setMax(i2s(Max));
		}
	}
	return *this;
}// tCfgInt::operator=


int tCfgInt::set(int val)
{
	//tMutexLock l(m);
	Val = val;
	if ( Val < Min )  Val = Min;
	if ( Val > Max )  Val = Max;
	if ( p )  p->set(i2s(Val));
	return Val;
}// tCfgInt::set


string tCfgInt::getInfo() const
{
	tMutexLock l(m);
	string ret;
	if ( p )  ret = p->getInfo();
	return ret;
}// tCfgInt::getInfo


bool tCfgInt::getRO() const
{
	tMutexLock l(m);
	bool ret = false;
	if ( p )  ret = p->getRO();
	return ret;
}// tCfgInt::getRO


void tCfgInt::setMin(int _Min)
{
	if ( _Min == INT_MIN ){
		delMin();
		return;
	}  
	tMutexLock l(m);
	if ( Min != _Min ){
		Min = _Min;
		if ( p )  p->setMin(i2s(Min));
		if ( Val < Min ){
			Val = Min;
			if ( p )  p->set(i2s(Val));
		}
		if ( Max < Min ){
			Max = Min;
			if ( p )  p->setMax(i2s(Max));
		}
	}
}// tCfgInt::setMin


void tCfgInt::setMax(int _Max)
{
	if ( _Max == INT_MAX ){
		delMax();
		return;
	}  
	tMutexLock l(m);
	if ( Max != _Max ){
		Max = _Max;
		if ( p )  p->setMax(i2s(Max));
		if ( Val > Max ){
			Val = Max;
			if ( p )  p->set(i2s(Val));
		}
		if ( Min > Max ){
			Min = Max;
			if ( p )  p->setMin(i2s(Min));
		}
	}
}// tCfgInt::setMax


void tCfgInt::delMin()
{
	tMutexLock l(m);
	if ( Min != INT_MIN ){
		Min = INT_MIN;
		if ( p )  p->setMin("");
	}
}// tCfgInt::delMin


void tCfgInt::delMax()
{
	tMutexLock l(m);
	if ( Max != INT_MAX ){
		Max = INT_MAX;
		if ( p )  p->setMax("");
	}
}// tCfgInt::delMax


void tCfgInt::setInfo(const string &Inf)
{
	tMutexLock l(m);
	if ( p )  p->setInfo(Inf);
}// tCfgInt::setInfo


void tCfgInt::setRO(bool RO)
{
	tMutexLock l(m);
	if ( p )  p->setRO(RO);
}// tCfgInt::setRO




string tCfgFloat::d2s(double v)
{
	return ToStringD(v, -1);
}// tCfgFloat::d2s


double tCfgFloat::s2d(const string &v, double def)
{
	return ToDouble(v, def);
}// tCfgFloat::s2d


tCfgFloat::tCfgFloat(double val) :
	p(NULL),
	Val(val)
{
}// tCfgFloat::tCfgFloat


tCfgFloat::tCfgFloat(tCfgKey *k) :
	p(k ? k->link() : NULL),
	Val(0),
	Min(DBL_MIN),
	Max(DBL_MAX)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Float ) 
			nError::Throw("tCfgFloat: Invalid type : " + ToString(p->type()));
		Val = s2d(p->get(), 0.0);
		Min = s2d(p->getMin(), Min);
		Max = s2d(p->getMax(), Max);
	}
}// tCfgFloat::tCfgFloat


tCfgFloat::tCfgFloat(tCfg &f, const string &Path, const string &Name) :
	p(f.makeKey(tCfgKey::T_Float, Path, Name, "0.0")),
	Val(0.0),
	Min(DBL_MIN),
	Max(DBL_MAX)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Float ) 
			nError::Throw("tCfgFloat: Invalid type : " + ToString(p->type()));
		Val = s2d(p->get(), 0.0);
		Min = s2d(p->getMin(), Min);
		Max = s2d(p->getMax(), Max);
	}
}// tCfgFloat::tCfgFloat


tCfgFloat::tCfgFloat(tCfg &f, const string &Path, const string &Name, double val) :
	p(f.makeKey(tCfgKey::T_Float, Path, Name, d2s(val))),
	Val(val),
	Min(DBL_MIN),
	Max(DBL_MAX)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Float ) 
			nError::Throw("tCfgFloat: Invalid type : " + ToString(p->type()));
		Val = s2d(p->get(), Val);
		Min = s2d(p->getMin(), Min);
		Max = s2d(p->getMax(), Max);
	}
}// tCfgFloat::tCfgFloat


tCfgFloat::tCfgFloat(tCfg &f, const string &Path, const string &Name, 
                     double val, double _Min, double _Max) :
	p(f.makeKey(tCfgKey::T_Float, Path, Name, d2s(val), d2s(_Min), d2s(_Max))),
	Val(val),
	Min(_Min),
	Max(_Max)
{
	if ( p ){
		if ( p->type() != tCfgKey::T_Float ) 
			nError::Throw("tCfgFloat: Invalid type : " + ToString(p->type()));
		Val = s2d(p->get(), Val);
		Min = s2d(p->getMin(), Min);
		Max = s2d(p->getMax(), Max);
	}
}// tCfgFloat::tCfgFloat


tCfgFloat::~tCfgFloat()
{
	if ( p )  p->unlink();
}// tCfgFloat::~tCfgFloat


tCfgFloat &tCfgFloat::operator=(const tCfgFloat &s)
{
	tMutexLock l(m);
	if ( this != &s ){
		if ( Val != s.Val ){
			Val = s.Val;
			if ( p )  p->set(d2s(Val));
		}
		if ( Min != s.Min ){
			Min = s.Min;
			if ( p )  p->setMin(d2s(Min));
		}
		if ( Max != s.Max ){
			Max = s.Max;
			if ( p )  p->setMax(d2s(Max));
		}
	} 
	return *this;
}// tCfgFloat::operator=


double tCfgFloat::set(double val)
{
	//tMutexLock l(m);
	Val = val;
	if ( Val < Min )  Val = Min;
	if ( Val > Max )  Val = Max;
	if ( p )  p->set(d2s(Val));
	return Val;
}// tCfgFloat::set


void tCfgFloat::setMin(double _Min)
{
	if ( _Min == DBL_MIN ){
		delMin();
		return;
	}
	tMutexLock l(m);
	if ( Min != _Min ){
		Min = _Min;
		if ( p )  p->setMin(d2s(Min));
		if ( Val < Min ){
			Val = Min;
			if ( p )  p->set(d2s(Val));
		}
		if ( Max < Min ){
			Max = Min;
			if ( p )  p->setMax(d2s(Max));
		}
	}
}// tCfgFloat::setMin


void tCfgFloat::setMax(double _Max)
{
	if ( _Max == DBL_MAX ){
		delMax();
		return;
	}
	tMutexLock l(m);
	if ( Max != _Max ){
		Max = _Max;
		if ( p )  p->setMax(d2s(Max));
		if ( Val > Max ){
			Val = Max;
			if ( p )  p->set(d2s(Val));
		}
		if ( Min > Max ){
			Min = Max;
			if ( p )  p->setMin(d2s(Min));
		}
	}
}// tCfgFloat::setMax


void tCfgFloat::delMin()
{
	tMutexLock l(m);
	if ( Min != DBL_MIN ){
		Min = DBL_MIN;
		if ( p )  p->setMin("");
	}
}// tCfgFloat::delMin


void tCfgFloat::delMax()
{
	tMutexLock l(m);
	if ( Max != DBL_MAX ){
		Max = DBL_MAX;
		if ( p )  p->setMax("");
	}
}// tCfgFloat::delMax


void tCfgFloat::setInfo(const string &Inf)
{
	tMutexLock l(m);
	if ( p )  p->setInfo(Inf);
}// tCfgFloat::setInfo


void tCfgFloat::setRO(bool RO)
{
	tMutexLock l(m);
	if ( p )  p->setRO(RO);
}// tCfgFloat::setRO
