/* This file was created by MkFile ver. 1.0.0.9 [2016-08-16 17:19:50] */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <malloc.h>


static char buf[0x801] = { "" };


static void About()
{
	printf("You can call any executable module and get errorlevel/errorcode. \n\n");    // some description
	printf("Call :  Erl  programm[.exe] [any_key/option_for_programm [...]]\n\n");
	printf("Keys :  \n");
	printf("  -?              - this info\n");
	printf("\n");
//	exit(0);
}// About


int main(int ac, char **aa)
{
	printf("Erl.  ver 1.0.1.6  (c) Gerasim# [2016-08-xx]\n");
	for ( int i = 1; i < ac; i++ ){
		char* a0 = aa[i];
		if ( !a0 ) break;
		if ( !strlen(a0) ) continue;
//		printf("[%s] ", a0);

		if ( strlen(buf) > 0 ){
			strcat(buf, " ");
			strcat(buf, aa[i]);
		}else{
			if ( a0[0] == '-' ){
				if ( strlen(a0) > 1 ){
					if ( a0[1] == '?' ){
						About();
						return 0;
					}
				}else{
					// we got single '-'
				}
			}
			strcat(buf, aa[i]);
		}
	}

	if ( strlen(buf) == 0 ){
		About();
		return 0;
	}
	printf("Executing:\n[%s]\n---------------------:\n", buf);
	int err = system(buf);
	printf("\n---------------------:\nResult: %d (0x%X)\n", err, err);

	return 0;
}// main
