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
#include "SDL.h"

#include "Game.h"
#include "TestState.h"
#include "StartState.h"

#include <iostream>

//#define XCOM_TEST

int main(int argc, char** args)
{
	Game *game = new Game("Open X-Com", 320, 200, 8);
	game->load();
	
	//game->getScreen()->setFullscreen(true);
	game->getScreen()->setScale(2);
#ifdef XCOM_TEST
	game->setState(new TestState(game));
#else
	game->setState(new StartState(game));
#endif

	try
	{
		game->run();
	}
	catch(char* c)
	{
		cout << "ERROR: " << c << endl;
		exit(-1);
	}
	
	delete game;

	SDL_Quit();

	return 0;
}