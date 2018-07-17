#include "fKey.h"
#include "tTimer.h"
//#include "fString.h"
#include <stdio.h>

#ifdef _WIN32
#include <conio.h>
#endif

#ifdef __GNUG__
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#endif


#ifdef __GNUG__
static void changemode(int dir)
{
  static struct termios oldt, newt;
 
  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
 
static int kbhit()
{
	int ret = 0;

  struct timeval tv;
  fd_set rdfs;
 
  tv.tv_sec = 0;
  tv.tv_usec = 100000;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
	if ( FD_ISSET(STDIN_FILENO, &rdfs) ){
		ret = getchar();
	}
	return ret;
}
#endif


int getKey()
{
	int key = 0;
#ifdef __GNUG__
	changemode(1);
	key = kbhit();
	changemode(0);
#else
	if ( kbhit() ){
		key = (unsigned char)getchar();
		if ( key == 0 )  key = 256 + getchar();
	}
#endif
	return key;
}// getKey


string getString(bool passw)
{
	string ret;
	for (;;){
		int key = getKey();
		if ( key == 0 ){
			tTimer::delay(1);
			continue;
		}
		if ( key == keyCode::Enter || key == keyCode::Esc ){
			if ( key == keyCode::Esc ) ret = "";
			printf("\n");
			break;
		}
		if ( key == keyCode::Bs ){
			if ( ret.size() > 0 ){
				ret.resize(ret.size() - 1);
				printf("\x08 \x08");
			}
			continue;
		}
		if ( key >= ' ' ){
			ret += string(1, (char)key);
			printf("%c", passw ? '*' : key);
		}
	}
	return ret;
}// getString


