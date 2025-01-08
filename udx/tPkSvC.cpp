#include "tPkSv.h"
//#include "tLogQue.h"
#include <iostream>
#include <stdio.h>





tPk *tPk_Login::exec(const string &) const 
{
	return NULL; 
}// tPk_Login::exec



int tPk_Ping::N = 0;


tPk *tPk_Ping::exec(const string &) const 
{
	return NULL;
}// tPk_Ping::exec


/*
void initPackets()
{
	registerPacket<tPk_Logged>  ();
	registerPacket<tPk_Ping>    ();
	registerPacket<tPk_Message> ();
}// initPackets
*/
