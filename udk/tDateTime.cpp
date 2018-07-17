#include "tDateTime.h"

#include "fString.h"
#include "fVector.h"

#include <stdio.h>
#include <time.h>

#ifdef __GNUG__
#define _COMPILING_NEWLIB
#include <sys/time.h>
#ifdef __CYGWIN__
//#include <unistd.h>
#endif // __CYGWIN__
#endif // __GNUG__

#ifdef _WIN32
#include <io.h>
#endif // _WIN32

#include <math.h>



static const double c19700101 = 25569.;   // [1899-12-30 00:00:00] - [1970-01-01 00:00:00] in days
static const double c19800101 = 29221.;   // [1899-12-30 00:00:00] - [1980-01-01 00:00:00] in days
static const double cHM = 1440.;          // 24 * 60       - minutes per day
static const double cHMS = 86400.;        // 24 * 60 * 60  - seconds per day

static const double delta = 1. / 86400000.;
static const double delta2 = 1. / 86400000. / 2.;

static const int _monthDays[12][2] = {                // [month (0-11)][isLeapYear (false-true)]
	{  31,  31 }, {  28,  29 }, {  31,  31 }, {  30,  30 },
	{  31,  31 }, {  30,  30 }, {  31,  31 }, {  31,  31 },
	{  30,  30 }, {  31,  31 }, {  30,  30 }, {  31,  31 }
};

static const int _startDay[12][2] = {                 // [month (0-11)][isLeapYear (false-true)]
	{   0,   0 }, {  31,  31 }, {  59,  60 }, {  90,  91 },
	{ 120, 121 }, { 151, 152 }, { 181, 182 }, { 212, 213 },
	{ 243, 244 }, { 273, 274 }, { 304, 305 }, { 334, 335 }
};

static bool localeSet = false;
static char DateSeparator[8] = { "/" };
static char TimeSeparator[8] = { ":" };
static char ShortDateFormat[32] = { "dd/MM/yyyy" };
static char LongDateFormat[64] = { "dd MMMM yyyy" };
static char ShortTimeFormat[32] = { "H:mm:ss" };
static char LongTimeFormat[64] = { "H:mm:ss" };
static char ShortDayNames[7][16];
static char LongDayNames[7][32];
static char ShortMonthNames[12][16];
static char LongMonthNames[12][32];
static char TimeAMString[16] = { "" };
static char TimePMString[16] = { "" };

static char DayBeginsWeek[3] = { "" };


static int daysBefore(int _year)
{
	_year--; //= 1970;
	return 365 * _year + _year / 4 - _year / 100 + _year / 400 - 693594;
}// daysBefore


static int daysB(int _year)
{
	_year--;
	return 365 * _year + _year / 4 - _year / 100 + _year / 400;
}// daysB


static int toDays(int year, int month, int day)
{
	int days = daysBefore(year);
	days += _startDay[month - 1][tDateTime::isLeapYear(year) ? 1 : 0];;
	days += day;
	return days;
}// toDays


static void fromDays(int days, int &year, int &month, int &day)
{
	days += 693593;

	year = (unsigned)(days / 365.2425 + 1.0);
	if ( daysB(year) <= days ){
		do{
			year++;
		}while ( daysB(year) <= days );
		year--;
	}else{
		do{
			year--;
		}while ( daysB(year) > days );
	}
	days++;

	int _days = days;
	_days -= daysB(year);

	int i;
	for ( i = 12; i > 1; i-- ){
		if ( _days > _startDay[i - 1][tDateTime::isLeapYear(year) ? 1 : 0] ) break;
	}
	month = i;
	day = _days - _startDay[i - 1][tDateTime::isLeapYear(year) ? 1 : 0];
}// fromDays


static void _DecodeDateTime(double val, int *year, int *month, int *day, int *hour, int *minute, int *sec, int *msec)
{
	int _year;
	int _month;
	int _day;
	fromDays((int)val, _year, _month, _day);
	double d = fmod((val + delta2) * 24., 24.);
	int _hour = int(d);
	d = fmod(60. * d, 60.);
	int _minute = int(d);
	d = fmod(60. * d, 60.);
	int _second = int(d);
	d = fmod(1000. * d, 1000.);
	int _msec = int(d);
	if ( year )   *year   = _year;
	if ( month )  *month  = _month;
	if ( day )    *day    = _day;
	if ( hour )   *hour   = _hour;
	if ( minute ) *minute = _minute;
	if ( sec )    *sec    = _second;
	if ( msec )   *msec   = _msec;
}// DecodeDateTime


static double _EncodeDateTime(int year, int month, int day, int hour, int minute, int second, int msec)
{
	return toDays(year, month, day) + (((msec / 1000. + second) / 60. + minute) / 60. + hour) / 24.;
}// _EncodeDateTime



#ifdef __GNUG__
/*
bool tDateTime::getDstDates(int year, double &sD, double &dD) // <= dates for given year
{
	readLocale();

	if ( bias == dst )  return false;

	if ( __ch_year == year ){
		sD = __ch_s;
		dD = __ch_d;
		return true;
	}

	sD = ___findS(year);
	dD = ___findD(year);

	__ch_year = year;
	__ch_s = sD;
	__ch_d = dD;
	return true;
}// tDateTime::getDstDates
*/
#endif // __GNUG__


#ifdef _WIN32
static SYSTEMTIME stdDate;
static SYSTEMTIME dstDate;

static int __ch_year = -1;     // cache - last used year of DST calculations
static double __ch_s = 0.;     // date of turn DST off (in '__ch_year' year)
static double __ch_d = 0.;     // date of turn DST on (in '__ch_year' year)


static double ___cvt(int year, const SYSTEMTIME &st)
{
	double ret = 0;
	if ( st.wYear == 0 ){
		if ( st.wDay == 5 ){
			tDateTime sDT(year, st.wMonth, 
			              _monthDays[st.wMonth - 1][tDateTime::isLeapYear(year) ? 1 : 0], 
			              st.wHour, st.wMinute, 
			              st.wSecond, st.wMilliseconds);
			int dow = sDT.DayOfWeek();
			ret = double(sDT) - (7 + dow - st.wDayOfWeek) % 7;;
		}else{
			tDateTime sDT(year, st.wMonth, 1, st.wHour, st.wMinute, 
			              st.wSecond, st.wMilliseconds);
			int dow = sDT.DayOfWeek();
			ret = double(sDT) + 7. * (st.wDay - 1) + (st.wDayOfWeek - dow);
		}
		/*
		tDateTime sDT(year, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
		sD = sDT; */
		/*        // ??????????????????????????????
		ret = _EncodeDateTime(year, st.wMonth, st.wDay, st.wHour, st.wMinute, 
		                      st.wSecond, st.wMilliseconds); */
	}else{
		ret = _EncodeDateTime(year, st.wMonth, st.wDay, st.wHour, st.wMinute, 
		                      st.wSecond, st.wMilliseconds);
	}
	return ret;
}// ___cvt


bool tDateTime::getDstDates(int year, double &sD, double &dD) // <= dates for given year
{
	readLocale();

	if ( stdDate.wMonth == 0 )  return false;

	if ( __ch_year == year ){
		sD = __ch_s;
		dD = __ch_d;
		return true;
	}

	sD = ___cvt(year, stdDate);
	dD = ___cvt(year, dstDate);

	__ch_year = year;
	__ch_s = sD;
	__ch_d = dD;
	return true;
}// tDateTime::getDstDates

#endif // _WIN32



static double __find1Diff(time_t c)
{
	struct tm *tm1 = gmtime(&c);

	time_t l1 = mktime(tm1);

	struct tm *tm2 = localtime(&c);
	time_t l2 = mktime(tm2);

	//tm2->tm_isdst = 0;
	//time_t l3 = mktime(tm2);
	return difftime(l1, l2);
}// __find1Diff


static void __findDiffs(time_t c, double &dif1, double &dif2)
{
	struct tm *tm1 = gmtime(&c);

	time_t l1 = mktime(tm1);

	struct tm *tm2 = localtime(&c);
	time_t l2 = mktime(tm2);

	tm2->tm_isdst = 0;
	time_t l3 = mktime(tm2);
	dif1 = difftime(l1, l2);
	dif2 = difftime(l1, l3);
}// __findDiffs


void tDateTime::readLocale()
{
	if ( !localeSet ){

#ifdef _WIN32
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDATE, DateSeparator, 8);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIME, TimeSeparator, 8);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_STIMEFORMAT, ShortTimeFormat, 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SSHORTDATE, ShortDateFormat, 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SLONGDATE, LongDateFormat, 64);

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_S1159, TimeAMString, 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_S2359, TimePMString, 16);

		//printf("ShortTimeFormat : [%s]\n", ShortTimeFormat);
		//printf("LongTimeFormat : [%s]\n", LongTimeFormat);
		//printf("ShortDateFormat : [%s]\n", ShortDateFormat);
		//printf("LongDateFormat : [%s]\n", LongDateFormat);

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME1, LongDayNames[0], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME2, LongDayNames[1], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME3, LongDayNames[2], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME4, LongDayNames[3], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME5, LongDayNames[4], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME6, LongDayNames[5], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SDAYNAME7, LongDayNames[6], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME1, ShortDayNames[0], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME2, ShortDayNames[1], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME3, ShortDayNames[2], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME4, ShortDayNames[3], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME5, ShortDayNames[4], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME6, ShortDayNames[5], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVDAYNAME7, ShortDayNames[6], 16);

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME1, LongMonthNames[0], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME2, LongMonthNames[1], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME3, LongMonthNames[2], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME4, LongMonthNames[3], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME5, LongMonthNames[4], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME6, LongMonthNames[5], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME7, LongMonthNames[6], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME8, LongMonthNames[7], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME9, LongMonthNames[8], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME10, LongMonthNames[9], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME11, LongMonthNames[10], 32);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SMONTHNAME12, LongMonthNames[11], 32);

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME1, ShortMonthNames[0], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME2, ShortMonthNames[1], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME3, ShortMonthNames[2], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME4, ShortMonthNames[3], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME5, ShortMonthNames[4], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME6, ShortMonthNames[5], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME7, ShortMonthNames[6], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME8, ShortMonthNames[7], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME9, ShortMonthNames[8], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME10, ShortMonthNames[9], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME11, ShortMonthNames[10], 16);
		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVMONTHNAME12, ShortMonthNames[11], 16);

		GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IFIRSTDAYOFWEEK, DayBeginsWeek, 3);
		tDateTime::sundayBeginsWeek = (DayBeginsWeek[0] == '6');

		TIME_ZONE_INFORMATION tzi;
		/*
		   LONG       Bias; 
		   WCHAR      StandardName[ 32 ]; 
		   SYSTEMTIME StandardDate; 
		   LONG       StandardBias; 
		   WCHAR      DaylightName[ 32 ]; 
		   SYSTEMTIME DaylightDate; 
		   LONG       DaylightBias; 
		*/
		DWORD err= GetTimeZoneInformation(&tzi);
		if ( err != (DWORD)-1 ){
			bias = tzi.Bias;
			//printf("bias : %d\n", bias);
			if ( tzi.StandardBias != 0 )  bias = tzi.StandardBias;
			dst = bias + tzi.DaylightBias;
			//printf("dst : %d\n", dst);
			if ( tzi.StandardDate.wMonth == 0 )  dst = bias;

			stdDate = tzi.StandardDate;
			dstDate = tzi.DaylightDate;

			if ( !bias && !dst )  loc = false;
		}


#endif // _WIN32
#ifdef __GNUG__

//		tzset();
		//bias = timezone / 60;

		time_t ct;
		time(&ct);

		struct tm *tm1 = gmtime(&ct);
		tm1->tm_mon   = 6;
		tm1->tm_mday  = 2;
		tm1->tm_hour  = 12;
		tm1->tm_min   = 0;
		tm1->tm_sec   = 0;
		tm1->tm_isdst = 0;

		double dif1;
		double dif2;
		__findDiffs(mktime(tm1), dif1, dif2);

		bias = int(dif1 / 60.);
		dst  = int(dif2 / 60.);

		if ( !bias && !dst )  loc = false;

#endif // __GNUG__
		localeSet = true;
	}
}// tDateTime::readLocale



/*
Hours :
	h    - Hours without leading zeros for single digit hours (12 hour clock)
	hh   - Hours with leading zeros for single digit hours (12 hour clock)
	H    - Hours without leading zeros for single digit hours (24 hour clock)
	HH   - Hours with leading zeros for single digit hours (24 hour clock)
Minutes 
	m    - Minutes without leading zeros for single digit minutes
	mm   - Minutes with leading zeros for single digit minutes
Seconds 
	s    - Seconds without leading zeros for single digit seconds
	ss   - Seconds with leading zeros for single digit seconds
Time Marker 
	t    - One character time marker string
	tt   - Multi-character time marker string
------------------------------------------------------------------- 
Day :
	d    - Day of the month as digits without leading zeros for single digit days.
	dd   - Day of the month as digits with leading zeros for single digit days
	ddd  - Day of the week as a 3-letter abbreviation as given by a LOCALE_SABBREVDAYNAME value.
	dddd - Day of the week as given by a LOCALE_SDAYNAME value.
Month :
	M    - Month as digits without leading zeros for single digit months.
	MM   - Month as digits with leading zeros for single digit months
	MMM  - Month as a three letter abbreviation as given by a LOCALE_SABBREVMONTHNAME value.
	MMMM - Month as given by a LOCALE_SMONTHNAME value.
Year :
	y    - Year represented only be the last digit.
	yy   - Year represented only be the last two digits.
	yyyy - Year represented by the full 4 digits.
Period/Era :
	gg   - Period/era string as given by the CAL_SERASTRING value.

The "gg" format picture in a date string is ignored if there is no associated era string.
-----------------------------------------------------------------------
*/




tDateTime::tDateTime(int year, int month, int day) :
	val(_EncodeDateTime(year, month, day, 0, 0, 0, 0)),
	local(loc)
{
}// tDateTime::tDateTime


tDateTime::tDateTime(int hour, int minute, int second, int msec) :
	val(_EncodeDateTime(0, 0, 0, hour, minute, second, msec)),
	local(loc)
{
}// tDateTime::tDateTime


tDateTime::tDateTime(int year, int month, int day, int hour, int minute, int second, int msec) :
	val(_EncodeDateTime(year, month, day, hour, minute, second, msec)),
	local(loc)
{
}// tDateTime::tDateTime


tDateTime tDateTime::operator++(int) 
{
	tDateTime ret = *this; 
	val += 1.;
	return ret; 
}// tDateTime::operator++(int) 


tDateTime tDateTime::operator--(int) 
{
	tDateTime ret = *this; 
	val -= 1.;
	return ret; 
}// tDateTime::operator--(int) 


tDateTime::operator string() const
{
	return FormatSysString(string(ShortDateFormat) + " " + string(ShortTimeFormat));
}// tDateTime::operator string()


string tDateTime::DateString() const
{
	readLocale();
	return FormatSysString(string(ShortDateFormat));
}// tDateTime::DateString


string tDateTime::TimeString() const
{
	readLocale();
	return FormatSysString(string(LongTimeFormat));
}// tDateTime::TimeString


string tDateTime::DateTimeString() const
{
	readLocale();
	if ( hasTime() ){
		return FormatSysString(string(ShortDateFormat) + " " + string(LongTimeFormat));
	}else{
		return FormatSysString(string(ShortDateFormat));
	}
}// tDateTime::DateTimeString


string tDateTime::FormatString(const string &fmt) const
{
	readLocale();

	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int msec;
	DecodeDateTime(&year, &month, &day, &hour, &minute, &second, &msec);

	int dow = (DayOfWeek() + 6) % 7;

	string ret = fmt;
	bool ampm = ret.find("am/pm") != string::npos ||
	            ret.find("a/p") != string::npos ||
	            ret.find("ampm") != string::npos;
	string s1 = (hour < 12) ? string(TimeAMString) : string(TimePMString);
	string s2 = (hour < 12) ? string("am") : string("pm");
	string s3 = (hour < 12) ? string("a") : string("p");

	if ( ampm )  hour = hour % 12;

	std::map<string, string> mx;
	mx["yyyy"]  = ToString(year, 4);
	mx["yyy"]   = ToString(year, 4);
	mx["yy"]    = ToString(year % 100, 2);
	mx["hh"]    = ToString(hour, 2);
	mx["h"]     = ToString(hour);
	mx["nn"]    = ToString(minute, 2);
	mx["n"]     = ToString(minute);
	mx["ss"]    = ToString(second, 2);
	mx["s"]     = ToString(second);
	mx["ampm"]  = s1;
	mx["am/pm"] = s2;
	mx["a/p"]   = s3;

	mx["dddd"]  = LongDayNames[dow];
	mx["ddd"]   = ShortDayNames[dow];
	mx["dd"]    = ToString(day, 2);
	mx["d"]     = ToString(day);

	mx["mmmm"]  = LongMonthNames[month - 1];
	mx["mmm"]   = ShortMonthNames[month - 1];
	mx["mm"]    = ToString(month, 2);
	mx["m"]     = ToString(month);

	mx["/"]     = DateSeparator;
	mx[":"]     = TimeSeparator;
	mx["ddddd"] = FormatSysString(ShortDateFormat);
	mx["dddddd"] = FormatSysString(LongDateFormat);
	mx["t"]     = FormatSysString(ShortTimeFormat);
	mx["tt"]    = FormatSysString(LongTimeFormat);
	mx["c"]     = DateTimeString();

	xcodeTable xc = makeCodeTable(mx);
	ret = Replace(ret, xc);
	ret = kill_all_of(ret, "'`");
	return ret;
}// tDateTime::FormatString


string tDateTime::FormatSysString(const string &fmt) const
{
	readLocale();

	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int msec;
	DecodeDateTime(&year, &month, &day, &hour, &minute, &second, &msec);

	int dow = (DayOfWeek() + 6) % 7;

	string ret = fmt;

	string s1 = (hour < 12) ? string(TimeAMString) : string(TimePMString);
	string s2 = (hour < 12) ? string(1, TimeAMString[0]) : string(1, TimePMString[0]);

	std::map<string, string> mx;
	mx["yyyy"] =  ToString(year, 4);
	mx["yy"  ] =  ToString(year % 100, 2);
	mx["y"   ] =  ToString(year % 10);
	mx["HH"  ] =  ToString(hour, 2);
	mx["H"   ] =  ToString(hour);
	mx["hh"  ] =  ToString(hour % 12, 2);
	mx["h"   ] =  ToString(hour % 12);
	mx["mm"  ] =  ToString(minute, 2);
	mx["m"   ] =  ToString(minute);
	mx["ss"  ] =  ToString(second, 2);
	mx["s"   ] =  ToString(second);
	mx["tt"  ] =  s1;
	mx["t"   ] =  s2;
	mx["dddd"] =  LongDayNames[dow];
	mx["ddd" ] =  ShortDayNames[dow];
	mx["dd"  ] =  ToString(day, 2);
	mx["d"   ] =  ToString(day);
	mx["MMMM"] =  LongMonthNames[month - 1];
	mx["MMM" ] =  ShortMonthNames[month - 1];
	mx["MM"  ] =  ToString(month, 2);
	mx["M"   ] =  ToString(month);
	mx["/"   ] =  DateSeparator;
	mx[":"   ] =  TimeSeparator;

	xcodeTable xc = makeCodeTable(mx);
	ret = Replace(ret, xc);
	return ret;
}// tDateTime::FormatSysString


tDateTime tDateTime::ToSystem() const
{
	//printf("local : %d\n", local ? 1 : 0);
	if ( !local )  return tDateTime(*this);
	//printf("localShift : %d\n", localShift());
	tDateTime ret(val + localShift());
	ret.local = false;
	return ret;
}// tDateTime::ToSystem


tDateTime tDateTime::ToLocal() const
{
	if ( local )  return tDateTime(*this);
	tDateTime ret(val - localShift());
	ret.local = false;
	return ret;
}// tDateTime::ToLocal


// if ( sundayBeginsWeek ) :  1-Sunday, ... 7-Saturday
// else                    :  1-Monday, ... 7-Sunday 
int tDateTime::DayOfWeek() const 
{
	if ( sundayBeginsWeek ){
		return ((int(val) + 6) % 7) + 1;
	}else{
		return ((int(val) + 5) % 7) + 1;
	}
}// tDateTime::DayOfWeek


int tDateTime::DayOfYear() const 
{
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	return _startDay[month - 1][isLeapYear(year) ? 1 : 0] + day;
}// tDateTime::DayOfYear


int tDateTime::WeekOfYear() const 
{
	return (6 + DayOfYear() - DayOfWeek()) / 7 + 1;
}// tDateTime::WeekOfYear


int tDateTime::WeekOfMonth() const 
{
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	return (6 + day - DayOfWeek()) / 7 + 1;
}// tDateTime::WeekOfMonth


void tDateTime::DecodeDate(int *year, int *month, int *day) const
{
	_DecodeDateTime(val, year, month, day, NULL, NULL, NULL, NULL);
}// tDateTime::DecodeDate


void tDateTime::DecodeTime(int *hour, int *minute, int *sec, int *msec) const
{
	_DecodeDateTime(val, NULL, NULL, NULL, hour, minute, sec, msec);
}// tDateTime::DecodeTime


void tDateTime::DecodeDateTime(int *year, int *month, int *day, int *hour, int *minute, int *sec, int *msec) const
{
	_DecodeDateTime(val, year, month, day, hour, minute, sec, msec);
}// tDateTime::DecodeDateTime


struct ___ftime {
	unsigned ft_tsec: 5;      // two seconds 
	unsigned ft_min: 6;       // minutes 
	unsigned ft_hour: 5;      // hours
	unsigned ft_day: 5;       // days 
	unsigned ft_month: 4;     // months 
	unsigned ft_year: 7;      // year - 1980
};


int tDateTime::FileDate() const
{
	tDateTime tm = ToSystem();
	if ( tm.val < c19800101 )  return 0;
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	tm.DecodeDateTime(&year, &month, &day, &hour, &minute, &second, NULL);
	struct ___ftime ft;
	ft.ft_tsec  = second / 2;
	ft.ft_min   = minute;
	ft.ft_hour  = hour;
	ft.ft_day   = day;
	ft.ft_month = month;
	ft.ft_year  = year - 1980;
	return *((int *)&ft);
}// tDateTime::FileDate


tDateTime tDateTime::CurrentDate()
{
#if defined( __CYGWIN__ ) || defined ( __GNUG__ )
	tDateTime ret;
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ret.val = c19700101 + (ts.tv_sec / cHMS);
	return ret;
#else
	SYSTEMTIME st;
	if ( loc ){
		GetLocalTime(&st);
		return tDateTime(st.wYear, st.wMonth, st.wDay);
	}else{
		GetSystemTime(&st);
		return tDateTime(st.wYear, st.wMonth, st.wDay);
	}
#endif
}// tDateTime::CurrentDate


tDateTime tDateTime::CurrentTime()
{
#if defined( __CYGWIN__ ) || defined( __GNUG__ )
	tDateTime ret;
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ret.val = (ts.tv_nsec / 1000000000.) / cHMS;
	return ret;
#else 
	SYSTEMTIME st;
	if ( loc ){
		GetLocalTime(&st);
		return tDateTime(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}else{
		GetSystemTime(&st);
		return tDateTime(st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}
#endif
}// tDateTime::CurrentTime


tDateTime tDateTime::CurrentDateTime()
{
#if defined( __CYGWIN__ ) || defined( __GNUG__ )
	tDateTime ret;
	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ret.val = c19700101 + (ts.tv_sec / cHMS) + (ts.tv_nsec / 1000000000.) / cHMS;
	return ret;
#else
	SYSTEMTIME st;
	if ( loc ){
		GetLocalTime(&st);
		return tDateTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}else{
		GetSystemTime(&st);
		return tDateTime(st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	}
#endif
}// tDateTime::CurrentDateTime


tDateTime tDateTime::EncodeDate(int year, int month, int day)
{
	return tDateTime(year, month, day);
}// tDateTime::EncodeDate


tDateTime tDateTime::EncodeTime(int hour, int minute, int second, int msec)
{
	return tDateTime(hour, minute, second, msec);
}// tDateTime::EncodeTime


tDateTime tDateTime::EncodeDateTime(int year, int month, int day, int hour, int minute, int second, int msec)
{
	//return tDateTime(toDays(year, month, day) + (((msec / 1000. + second) / 60. + minute) / 60. + hour) / 24.);
	return tDateTime(year, month, day, hour, minute, second, msec);
}// tDateTime::EncodeDateTime


tDateTime tDateTime::EncodeDateTime(const string &Src)
{
	tDateTime ret;
	;
	return ret;
}// tDateTime::EncodeDateTime


tDateTime tDateTime::FileDateToDateTime(int fileDate)
{
	struct ___ftime *ft = (struct ___ftime *)&fileDate;
	int second = ft->ft_tsec * 2;
	int minute = ft->ft_min;
	int hour   = ft->ft_hour;
	int day    = ft->ft_day;
	int month  = ft->ft_month;
	int year   = ft->ft_year + 1980;
	tDateTime ret(year, month, day, hour, minute, second, 0);
	if ( loc )  return ret.ToLocal();
	return ret;
}// tDateTime::FileDateToDateTime


bool tDateTime::isLeapYear(int year)
{
	if ( year % 4000 == 0 )  return false;
	if ( year % 400 == 0 )   return true;
	if ( year % 100 == 0 )   return false;
	if ( year % 4 == 0 )     return true;
	return false;
}// tDateTime::isLeapYear


void tDateTime::roundUpDay()
{
	if ( hasTime() ){
		val = (int)(val + 1.);
	}
}// tDateTime::roundUpDay


void tDateTime::roundDownDay()
{
	val = (int)val;
}// tDateTime::roundDownDay


void tDateTime::roundUpWeek()
{
	int dow = DayOfWeek();
	if ( dow > 1 || hasTime() ){
		val = (int)(val - dow + 8.);
	}
}// tDateTime::roundUpWeek


void tDateTime::roundDownWeek()
{
	int dow = DayOfWeek();
	if ( dow > 1 || hasTime() ){
		val = (int)(val - dow + 1.);
	}
}// tDateTime::roundDownWeek


void tDateTime::roundUpMonth()
{
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	if ( day != 1 || hasTime() ){
		if ( ++month > 12 ){
			year++;
			month = 1;
		}
		*this = tDateTime(year, month, 1);
	}
}// tDateTime::roundUpMonth


void tDateTime::roundDownMonth()
{
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	if ( day != 1 || hasTime() ){
		*this = tDateTime(year, month, 1);
	}
}// tDateTime::roundDownMonth


void tDateTime::roundUpYear()
{
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	if ( day != 1 || month != 1 || hasTime() ){
		*this = tDateTime(year + 1, 1, 1);
	}
}// tDateTime::roundUpYear


void tDateTime::roundDownYear()
{
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	if ( day != 1 || month != 1 || hasTime() ){
		*this = tDateTime(year, 1, 1);
	}
}// tDateTime::roundDownYear


void tDateTime::addDays(int add)
{
	val += add;
}// tDateTime::addDays


void tDateTime::addWeeks(int add)
{
	val += 7. * add;
}// tDateTime::addWeeks


void tDateTime::addMonths(int add)
{
	if ( !add )  return;
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	if ( add > 0 ){
		month += add;
		year += (month / 12);
		month %= 12;
	}else{
		add = -add;
		year -= (add / 12);
		month -= (add % 12);
		if ( month < 1 ){
			year--;
			month += 12;
		}
	}
	int md = _monthDays[month - 1][isLeapYear(year) ? 1 : 0];
	if ( day > md )  day = md;
	*this = tDateTime(year, month, day);
}// tDateTime::addMonths


void tDateTime::addYears(int add)
{
	if ( !add )  return;
	int year;
	int month;
	int day;
	DecodeDateTime(&year, &month, &day, NULL, NULL, NULL, NULL);
	*this = tDateTime(year + add, month, day);
}// tDateTime::addYears


#ifdef _WIN32

void tDateTime::ToSystemTime(SYSTEMTIME *st) const
{
	if ( st ){
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		int msec;
		_DecodeDateTime(val, &year, &month, &day, &hour, &minute, &second, &msec);
		st->wYear         = year;
		st->wMonth        = month; 
		//st->wDayOfWeek; 
		st->wDay          = day; 
		st->wHour         = hour; 
		st->wMinute       = minute; 
		st->wSecond       = second; 
		st->wMilliseconds = msec; 
	}
}// tDateTime::ToSystemTime


void tDateTime::ToLocalTime(SYSTEMTIME *st) const
{
	if ( st ){
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		int msec;
		DecodeDateTime(&year, &month, &day, &hour, &minute, &second, &msec);
		st->wYear         = year;
		st->wMonth        = month; 
		//st->wDayOfWeek; 
		st->wDay          = day; 
		st->wHour         = hour; 
		st->wMinute       = minute; 
		st->wSecond       = second; 
		st->wMilliseconds = msec; 
	}
}// tDateTime::ToLocalTime


void tDateTime::ToFileTime(FILETIME *ft) const
{
	if ( ft ){
		SYSTEMTIME st;
		ToSystemTime(&st);
		if ( !SystemTimeToFileTime(&st, ft) )  *((__int64 *)ft) = 0;
	}
}// tDateTime::ToFileTime


tDateTime tDateTime::SystemTimeToDateTime(const SYSTEMTIME *st)
{
	tDateTime ret0;
	if ( !st )  return ret0;
	return _EncodeDateTime(st->wYear, st->wMonth, st->wDay, st->wHour, st->wMinute, st->wSecond, st->wMilliseconds);
}// tDateTime::SystemTimeToDateTime


tDateTime tDateTime::LocalTimeToDateTime(const SYSTEMTIME *st)
{
	tDateTime ret0;
	if ( !st )  return ret0;
	return tDateTime(st->wYear, st->wMonth, st->wDay, st->wHour, st->wMinute, st->wSecond, st->wMilliseconds);
}// tDateTime::LocalTimeToDateTime


tDateTime tDateTime::FileTimeToDateTime(const FILETIME *ft)
{
	tDateTime ret0;
	if ( !ft )  return ret0;
	SYSTEMTIME st;
	if ( !FileTimeToSystemTime(ft, &st) )  return ret0;
	return SystemTimeToDateTime(&st);
}// tDateTime::FileTimeToDateTime


#endif // _WIN32



bool tDateTime::sundayBeginsWeek = true;
bool tDateTime::loc = true;    // local time as default
int tDateTime::bias = 0;       // bias = UTC - localtime (in minutes) = standardbias
int tDateTime::dst = 0;        // daylight savings time bias


double tDateTime::localShift() const  // current bias - : UTC(val) = local + bias
{
	readLocale();

	double ret = 0.;
	if ( !loc )  return ret;
	ret = bias / cHM;
	//printf("bias / cHM : %.6f\n", ret);
	if ( bias == dst )  return ret;

#ifdef _WIN32
	int year;
	_DecodeDateTime(val, &year, NULL, NULL, NULL, NULL, NULL, NULL);
	double sD;
	double dD;
	if ( !getDstDates(year, sD, dD) )  return ret;
	if ( val >= dD && val < sD )  ret = dst / cHM;
#endif // _WIN32
#ifdef __GNUG__
	ret = __find1Diff(DateTimeToTime_t()) / cHMS;
#endif // __GNUG__
	//printf("ret : %.6f\n", ret);
	return ret;
}// tDateTime::localShift


bool tDateTime::hasDate() const 
{
	return int(val) != 0; 
}// tDateTime::hasDate


bool tDateTime::hasTime() const 
{
	return fmod(val, 1.) != 0.; 
}// tDateTime::hasTime


int tDateTime::DateTimeToTime_t() const
{
	if ( val < c19700101 )  return 0;
	return int((val - c19700101) * cHMS);
}// tDateTime::DateTimeToTime_t


void tDateTime::Time_tToDateTime(int Sec)
{
	val = c19700101 + Sec / cHMS;
}// tDateTime::Time_tToDateTime


bool tDateTime::operator==(const tDateTime &s) const 
{
	if ( (local && s.local) || (!local && !s.local) )  return val == s.val; 
	if ( local ){
		return ToSystem().val == s.val;
	}else{
		return val == s.ToSystem().val;
	}
}// tDateTime::operator==


bool tDateTime::operator< (const tDateTime &s) const 
{
	if ( (local && s.local) || (!local && !s.local) )  return val < s.val; 
	if ( local ){
		return ToSystem().val < s.val;
	}else{
		return val < s.ToSystem().val;
	}
}// tDateTime::operator<


bool tDateTime::operator> (const tDateTime &s) const 
{
	if ( (local && s.local) || (!local && !s.local) )  return val > s.val; 
	if ( local ){
		return ToSystem().val > s.val;
	}else{
		return val > s.ToSystem().val;
	}
}// tDateTime::operator>


string tDateTime::CurrentDateTimeFormat(const string &fmt)
{
	tDateTime cur = CurrentDateTime();
	return cur.FormatString(fmt);
}// tDateTime::CurrentDateTimeFormat


tDateTime tDateTime::now()
{
	return CurrentDateTime();
}// tDateTime::now


int tDateTime::now_t()
{
	time_t cur_secs;
	time(&cur_secs);
	return (int)cur_secs;
}// tDateTime::now_t



double tDateTime::nullDate = 1/172800.;
