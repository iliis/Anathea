#include "input_mgr.hpp"

//------------------------------------------------------------------------------------------------
bool isAMouseKey(KEY name)
{return name == KEY_MOUSE_LEFT || name == KEY_MOUSE_MIDDLE || name == KEY_MOUSE_RIGHT;}
//------------------------------------------------------------------------------------------------
string getMouseName(KEY name)
{
	switch(name)
	{
		case KEY_MOUSE_LEFT: case KEY_MOUSE_MIDDLE: case KEY_MOUSE_RIGHT:
			return "mouse1"; break;
		default:
			return ""; break;
	}
};/*
//------------------------------------------------------------------------------------------------
KEY getMouseKey(string name)
{
	if(name == "mouse"){return KEY_MOUSE_LEFT;}
	else if(name == "tuioCursor0"){return KEY_TUIOCURSOR_0;}
	else if(name == "tuioCursor1"){return KEY_TUIOCURSOR_1;}
	else if(name == "tuioCursor2"){return KEY_TUIOCURSOR_2;}
	else if(name == "tuioCursor3"){return KEY_TUIOCURSOR_3;}
	else if(name == "tuioCursor4"){return KEY_TUIOCURSOR_4;}
	else if(name == "tuioCursor5"){return KEY_TUIOCURSOR_5;}
	else if(name == "tuioCursor6"){return KEY_TUIOCURSOR_6;}
	else if(name == "tuioCursor7"){return KEY_TUIOCURSOR_7;}
	else if(name == "tuioCursor8"){return KEY_TUIOCURSOR_8;}
	else if(name == "tuioCursor9"){return KEY_TUIOCURSOR_9;}
	else if(name == "tuioCursor10"){return KEY_TUIOCURSOR_10;}
	return KEY_UNKNOWN;
};*/
//------------------------------------------------------------------------------------------------
