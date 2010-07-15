/*
 * Copyright 2010 Daniel Albano
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <iostream>
#include <cstring>

#include "Game.h"
#include "Screen.h"
#include "StartState.h"

/** @mainpage
 * @author SupSuper
 *
 * OpenXcom is an open-source reimplementation of the original X-Com
 * written entirely in C++ and SDL. This documentation contains info
 * on every class contained in the source code and its public methods.
 * The code itself also contains in-line comments for more complicated
 * code blocks. Hopefully all of this will make the code a lot more
 * readable for you in case you which to learn or make use of it in
 * your own projects, though note that all the source code is licensed
 * under the GNU General Public License. Enjoy!
 */

Game *game;

// If you can't tell what the main() is for you should have your
// programming license revoked...
int main(int argc, char** args)
{
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF );
	try
	{
		game = new Game("OpenXcom", 320, 200, 8);
		
		// Handles command line arguments
		int scale = 2;
		for (int i = 1; i < argc; i++)
		{
			if (strcmp(args[i], "-fullscreen") == 0)
				game->getScreen()->setFullscreen(true);
			if (strcmp(args[i], "-scale") == 0 && argc > i + 1)
				scale = atoi(args[i+1]);
		}
		game->getScreen()->setScale(scale);
		game->setState(new StartState(game));
		game->run();
	}
	catch (char* c)
	{
		cerr << "ERROR: " << c << endl;
		exit(-1);
	}
	
	delete game;

	return 0;
}
