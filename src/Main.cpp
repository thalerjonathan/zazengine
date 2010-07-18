/*
 * Main.cpp
 *
 *  Created on: 27.06.2010
 *      Author: joni
 */

#include "Core/Core.h"

int main(int argc, char** args)
{
	if ( Core::initalize() )
	{
		Core::getInstance().start();
		Core::shutdown();
	}
	else
	{
		return 1;
	}

	return 0;
}
