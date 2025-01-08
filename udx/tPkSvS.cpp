#include "tPkSv.h"
#include <iostream>
#include <stdio.h>




tPk *tPk_Login::exec(const string &) const 
{
	if ( !doLogin(login, password) )  return NULL;
	return new tPk_Logged();
}// tPk_Login::exec


tPk *tPk_Logged::exec(const string &) const 
{
	return NULL; 
}// tPk_Logged::exec


int tPk_Ping::N = 0;

tPk *tPk_Ping::exec(const string &) const 
{
	N = n;
	return new tPk_Ping(*this);
}// tPk_Ping::exec


tPk *tPk_Message::exec(const string &) const 
{
	return NULL; 
}// tPk_Message::exec


/*
void initPackets()
{
	registerPacket<tPk_Login  >         ();
	registerPacket<tPk_Ping   >         ();
}// initPackets
*/
