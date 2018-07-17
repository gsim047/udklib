#ifndef __fKey_h
#define __fKey_h 1

#include <string>
using std::string;


struct keyCode {
	enum{
		CtrlA          =   1,
		CtrlB          =   2,
		CtrlC          =   3,
		CtrlD          =   4,
		CtrlE          =   5,
		CtrlF          =   6,
		CtrlG          =   7,
		Bell           =   7,
		CtrlH          =   8,
		Bs             =   8,
		CtrlI          =   9,
		Tab            =   9,
		CtrlJ          =  10,
		CtrlEnter      =  10,
		Lf             =  10,             /* Line Feed - e.g. ^Enter */
		CtrlK          =  11,
		CtrlL          =  12,
		CtrlM          =  13,
		Cr             =  13,
		Enter          =  13,
		CtrlN          =  14,
		CtrlO          =  15,
		CtrlP          =  16,
		CtrlQ          =  17,
		CtrlR          =  18,
		CtrlS          =  19,
		CtrlT          =  20,
		CtrlU          =  21,
		CtrlV          =  22,
		CtrlW          =  23,
		CtrlX          =  24,
		CtrlY          =  25,
		CtrlZ          =  26,
		Esc            =  27,
		CtrlLSB        =  27,            // Left Square Brakets
		CtrlBackSlash  =  28,
		CtrlRSB        =  29,            // Right Square Brakets

		CtrlMinus      =  31,
		Space          =  32,

		CtrlBs         = 127,             /* ^BackSpace */

		AltBs          = 270,
		BackTab        = 271,             /* Shift + Tab */
		ShiftTab       = 271,
		AltQ           = 272,
		AltW           = 273,
		AltE           = 274,
		AltR           = 275,
		AltT           = 276,
		AltY           = 277,
		AltU           = 278,
		AltI           = 279,
		AltO           = 280,
		AltP           = 281,
		AltLSB         = 282,
		AltRSB         = 283,

		AltA           = 286,
		AltS           = 287,
		AltD           = 288,
		AltF           = 289,
		AltG           = 290,
		AltH           = 291,
		AltJ           = 292,
		AltK           = 293,
		AltL           = 294,
		AltSemiColumn  = 295,
		AltRQuote      = 296,
		AltLQuote      = 297,

		AltBackSlash   = 299,
		AltZ           = 300,
		AltX           = 301,
		AltC           = 302,
		AltV           = 303,
		AltB           = 304,
		AltN           = 305,
		AltM           = 306,
		AltComma       = 307,
		AltDot         = 308,
		AltSlash       = 309,

		F1             = 315,             /* Fx */
		F2             = 316,             /* ... */
		F3             = 317,
		F4             = 318,
		F5             = 319,
		F6             = 320,
		F7             = 321,
		F8             = 322,
		F9             = 323,
		F10            = 324,

		Home           = 327,
		Up             = 328,
		PgUp           = 329,
		AltGrMinus     = 330,
		Left           = 331,
		Key5           = 332,
		Right          = 333,
		AltGrPlus      = 334,
		End            = 335,
		Down           = 336,
		PgDn           = 337,
		Ins            = 338,
		Del            = 339,
		ShiftF1        = 340,             /* Shift + Fx */
		ShiftF2        = 341,             /* ... */
		ShiftF3        = 342,
		ShiftF4        = 343,
		ShiftF5        = 344,
		ShiftF6        = 345,
		ShiftF7        = 346,
		ShiftF8        = 347,
		ShiftF9        = 348,
		ShiftF10       = 349,
		CtrlF1         = 350,             /* ^Fx */
		CtrlF2         = 351,             /* ... */
		CtrlF3         = 352,
		CtrlF4         = 353,
		CtrlF5         = 354,
		CtrlF6         = 355,
		CtrlF7         = 356,
		CtrlF8         = 357,
		CtrlF9         = 358,
		CtrlF10        = 359,
		AltF1          = 360,             /* Alt + Fx */
		AltF2          = 361,             /* ... */
		AltF3          = 362,
		AltF4          = 363,
		AltF5          = 364,
		AltF6          = 365,
		AltF7          = 366,
		AltF8          = 367,
		AltF9          = 368,
		AltF10         = 369,

		CtrlLeft       = 371,
		CtrlRight      = 372,
		CtrlEnd        = 373,
		CtrlPgDn       = 374,
		CtrlHome       = 375,
		Alt1           = 376,
		Alt2           = 377,
		Alt3           = 378,
		Alt4           = 379,
		Alt5           = 380,
		Alt6           = 381,
		Alt7           = 382,
		Alt8           = 383,
		Alt9           = 384,
		Alt0           = 385,
		AltMinus       = 386,
		AltEqual       = 387,
		CtrlPgUp       = 388,
		F11            = 389,
		F12            = 390,
		ShiftF11       = 391,
		ShiftF12       = 392,
		CtrlF11        = 393,
		CtrlF12        = 394,
		AltF11         = 395,
		AltF12         = 396,
		CtrlUp         = 397,
		CtrlGrMinus    = 398,
		CtrlKey5       = 399,
		CtrlGrPlus     = 400,
		CtrlDown       = 401,
		CtrlIns        = 402,
		CtrlDel        = 403,
		CtrlTab        = 404,
		CtrlSlash      = 405,
		CtrlGrMul      = 406,
		AltHome        = 407,
		AltUp          = 408,
		AltPgUp        = 409,

		AltLeft        = 411,

		AltRight       = 413,

		AltEnd         = 415,
		AltDown        = 416,
		AltPgDn        = 417,
		AltIns         = 418,
		AltDel         = 419,
		AltGrSlash     = 420,

		AltGrEnter     = 422,

		Ctrl1          = 431,
		Ctrl2          = 432,
		Ctrl3          = 433,
		Ctrl4          = 434,
		Ctrl5          = 435,
		Ctrl6          = 436,
		Ctrl7          = 437,
		Ctrl8          = 438,
		Ctrl9          = 439,
		Ctrl0          = 440,

		CtrlEqual      = 442,
		CtrlSemiColumn = 443,
		CtrlRQuote     = 444,
		CtrlLQuote     = 445,
		CtrlComma      = 446,
		CtrlDot        = 447

	};
};


int getKey();

string getString(bool passw = false);


#endif // __fKey_h
