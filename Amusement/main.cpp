#include <iostream>
#include <FL/Fl.H>
#include "CallBacks.h"

#include "MainWindow.h"
int main()
{
	MainWindow mainWindow(100, 100, 1280, 720);
	mainWindow.show();
	mainWindow.init();
	Fl::add_idle((void(*)(void*))&(CallBacks::idleCallback),&mainWindow);
	Fl::run();
	return 0;
}