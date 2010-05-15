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
#include "Game.h"
#include "TestState.h"
#include "StartState.h"
#include "XcomResourcePack.h"
#include "XcomRuleset.h"
#include "SavedGame.h"

#include <iostream>

#define DATA_FOLDER "./DATA/"

Game *game;

int main(int argc, char** args)
{
	try
	{
		game = new Game("OpenXcom", 320, 200, 8);
		game->setResourcePack(new XcomResourcePack(DATA_FOLDER));
		game->setRuleset(new XcomRuleset());
		
		if (argc == 2 && strcmp(args[1], "-fullscreen") == 0)
			game->getScreen()->setFullscreen(true);
		game->getScreen()->setScale(2);
		//game->setState(new TestState(game));
		game->setState(new StartState(game));
		game->run();
	}
	catch(char* c)
	{
		cout << "ERROR: " << c << endl;
		getc(stdin);
		exit(-1);
	}
	
	delete game;

	return 0;
}