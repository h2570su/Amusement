#include "CallBacks.h"
#include <ctime>
void CallBacks::idleCallback(MainWindow* mainWindow)
{
	static int clocks = 0;
	if (clock() > clocks)
	{
		mainWindow->tick();
		mainWindow->damage(1);
		clocks = clock() + CLOCKS_PER_SEC / 65;
	}
	Sleep(1);
}
