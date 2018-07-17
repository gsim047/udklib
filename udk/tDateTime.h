#ifndef __tDateTime_h
#define __tDateTime_h 1

#pragma warning(disable:4786)

#include <string>
using std::string;

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _INC_MATH
#include <math.h>
#endif




class tDateTime {
public:
	enum {
		DateTime = 0,
		Date,
		Time
	};

public:
	tDateTime() : val(0.), local(loc) {}
	tDateTime(const tDateTime &s) : val(s.val), local(s.local) {};
	tDateTime(double src) : val(src), local(loc) {}
	tDateTime(int src) : val(src), local(loc) {}
	tDateTime(int year, int month, int day);
	tDateTime(int hour, int minute, int second, int msec);
	tDateTime(int year, int month, int day, int hour, int minute, int second, int msec);
	virtual ~tDateTime() {}

	bool empty() const { return fabs(val) < nullDate; }
	bool hasDate() const; //{ return int(val) != 0; }
	bool hasTime() const; //{ return fmod(val, 1.) != 0.; }
	bool isLocal() const { return local; }

	tDateTime &operator=(const tDateTime &s) { val = s.val; local = s.local; return *this; }
	tDateTime ToSystem() const;
	tDateTime ToLocal() const;

	int DateTimeToTime_t() const;
	void Time_tToDateTime(int tm);

	bool operator==(const tDateTime &s) const; // { return val == s.val; }
	bool operator!=(const tDateTime &s) const { return !operator==(s); }
	bool operator< (const tDateTime &s) const; // { return val <  s.val; }
	bool operator> (const tDateTime &s) const; // { return val >  s.val; }
	bool operator<=(const tDateTime &s) const { return !operator>(s); }
	bool operator>=(const tDateTime &s) const { return !operator<(s); }

	tDateTime &operator+=(const tDateTime &s) { val += s.val; return *this; }
	tDateTime &operator+=(double src) { val += src; return *this; }
	tDateTime &operator+=(int src) { val += src; return *this; }

	tDateTime &operator-=(const tDateTime &s) { val -= s.val; return *this; }
	tDateTime &operator-=(double src) { val -= src; return *this; }
	tDateTime &operator-=(int src) { val -= src; return *this; }

	tDateTime &operator++() { val += 1.; return *this; }
	tDateTime &operator--() { val -= 1.; return *this; }
	tDateTime operator++(int); // { tDateTime ret = *this; val += 1.0; return ret; }
	tDateTime operator--(int); // { tDateTime ret = *this; val -= 1.0; return ret; }

	operator double() const { return val; }
	operator string() const;

	string DateString() const;
	string TimeString() const;
	string DateTimeString() const;

	string FormatString(const string &fmt) const;
	/*
+ c      - Date in ShortDateFormat + Time in LongTimeFormat (if DateTime % 1. != 0.)

+ d      - Day as (1-31)
+ dd     - Day as (01-31)

+ ddd    - Weekday as an abbreviation (Sun-Sat) using ShortDayNames global variable.
+ dddd   - Weekday as a full name (Sunday-Saturday) using LongDayNames global variable.

+ ddddd  - Date using the format given by the ShortDateFormat global variable.
+ dddddd - Date using the format given by the LongDateFormat global variable.

+ m      - Month as (1-12). If the 'm' immediately follows 'h/hh' - mean minute
+ mm     - Month as (01-12). If the 'mm' immediately follows 'h/hh' - mean minute

+ mmm    - Month as an abbreviation (Jan-Dec) using ShortMonthNames global variable.
+ mmmm   - Month as a full name (January-December) using LongMonthNames global variable.

+ yy     - Year as (00-99).
+ yyyy   - Year as (0000-9999).

+ h      - Hour as (0-23).
+ hh     - Hour as (00-23).
+ n      - Minute as (0-59).
+ nn     - Minute as (00-59).
+ s      - Second as (0-59).
+ ss     - Second as (00-59).

+ t      - Time using ShortTimeFormat global variable.
+ tt     - Time using LongTimeFormat global variable.
+ am/pm  - Uses the 12-hour clock for the preceding h or hh specifier, and displays 'am' for 
      any hour before noon, and 'pm' for any hour after noon. The am/pm specifier can use 
      lower, upper, or mixed case, and the result is displayed accordingly.
+ a/p    - Uses the 12-hour clock for the preceding h or hh specifier, and displays 'a' for 
      any hour before noon, and 'p' for any hour after noon. The a/p specifier can use lower, 
      upper, or mixed case, and the result is displayed accordingly.
+ ampm   - Uses the 12-hour clock for the preceding h or hh specifier, and displays the 
      contents of the TimeAMString global variable for any hour before noon, and the contents 
      of the TimePMString global variable for any hour after noon.
+ /      - Date separator character given by the DateSeparator global variable.

+ :      - Time separator character given by the TimeSeparator global variable.

  'xx'/"xx" Characters enclosed in '' or "" are displayed as is, and do not affect formatting. 
	*/


	// if ( sundayBeginsWeek ) :  1-Sunday, ... 7-Saturday
	// else                    :  1-Monday, ... 7-Sunday 
	int DayOfWeek() const;
	int DayOfYear() const;        // 1-365/366
	int WeekOfYear() const;       // 1-53/54
	int WeekOfMonth() const;      // 1-6

	void DecodeDate(int *year, int *month, int *day) const;
	void DecodeTime(int *hour, int *minute, int *sec, int *msec) const;
	void DecodeDateTime(int *year, int *month, int *day, int *hour, int *minute, int *sec, int *msec) const;

	void roundUpDay();
	void roundDownDay();
	void roundUpWeek();
	void roundDownWeek();
	void roundUpMonth();
	void roundDownMonth();
	void roundUpYear();
	void roundDownYear();

	void addDays(int add);
	void addWeeks(int add);
	void addMonths(int add);
	void addYears(int add);

	int FileDate() const;

#ifdef _WIN32
	void ToSystemTime(SYSTEMTIME *st) const;
	void ToLocalTime(SYSTEMTIME *st) const;
	void ToFileTime(FILETIME *ft) const;
#endif

public:
	static bool isLeapYear(int year);

	static tDateTime CurrentDate();
	static tDateTime CurrentTime();
	static tDateTime CurrentDateTime();
	static tDateTime now();
	static int now_t();

	// Date : "yyyy-mm-dd"
	// Time : "hh:nn:ss"
	static string CurrentDateTimeFormat(const string &fmt);

	static tDateTime EncodeDate(int year, int month, int day);
	static tDateTime EncodeTime(int hour, int minute, int second, int msec = 0);
	static tDateTime EncodeDateTime(int year, int month, int day, int hour, int minute, int second, int msec = 0);
	static tDateTime EncodeDateTime(const string &Src);

	static tDateTime FileDateToDateTime(int fileDate);

	static bool localeUsed() { return loc; }
	static void useLocale(bool Loc = true) { loc = Loc; }

	static int getBias() { return bias; }

#ifdef _WIN32
	static tDateTime SystemTimeToDateTime(const SYSTEMTIME *st);
	static tDateTime LocalTimeToDateTime(const SYSTEMTIME *st);
	static tDateTime FileTimeToDateTime(const FILETIME *ft);
#endif

	static bool sundayBeginsWeek;  // true

	static double nullDate;

private:
	static void readLocale();
	string FormatSysString(const string &fmt) const;

	//static void _DecodeDateTime(double val, int *year, int *month, int *day, int *hour, int *minute, int *sec, int *msec);
	//static double _EncodeDateTime(int year, int month, int day, int hour, int minute, int second, int msec);

#ifdef _WIN32
	static bool getDstDates(int year, double &sD, double &dD); // <= dates for given year
#endif


	double localShift() const;  // current bias - : UTC(val) = local + bias

private:
	double val;     // GMT !!!, Y-M-D,h-m-s-ms since 1899-12-30 00:00:00.000
	bool local;
	static bool loc;
	static int bias;
	static int dst;
};// class tDateTime




#endif // __tDateTime_h
