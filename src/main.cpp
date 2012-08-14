/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include <exception>
#include <iostream>
#include "Engine/CrossPlatform.h"
#include "Engine/Game.h"
#include "Engine/Screen.h"
#include "Engine/Options.h"
#include "Menu/StartState.h"

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

using namespace OpenXcom;

Game *game = 0;

// If you can't tell what the main() is for you should have your
// programming license revoked...
int main(int argc, char** args)
{
#ifndef _DEBUG
	try
	{
#endif
		std::cout << "Starting OpenXcom..." << std::endl << std::endl;
		Options::init(argc, args);
		game = new Game("OpenXcom " + Options::getVersion(), 320, 200, 8);
		game->getScreen()->setResolution(Options::getInt("displayWidth"), Options::getInt("displayHeight"));
		game->getScreen()->setFullscreen(Options::getBool("fullscreen"));
		game->setVolume(Options::getInt("soundVolume"), Options::getInt("musicVolume"));
		game->setState(new StartState(game));
		game->run();
#ifndef _DEBUG
	}
	catch (std::exception &e)
	{
		CrossPlatform::showError(e.what());
		exit(EXIT_FAILURE);
	}
#endif
	Options::save();

	// Comment this for faster exit.
	delete game;
	return EXIT_SUCCESS;
}
