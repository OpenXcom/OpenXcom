/*
 * Copyright 2010-2015 OpenXcom Developers.
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
#include <sstream>
#include "version.h"
#include "Engine/Logger.h"
#include "Engine/CrossPlatform.h"
#include "Engine/Game.h"
#include "Engine/Options.h"
#include "Menu/StartState.h"

/** @mainpage
 * @author OpenXcom Developers
 *
 * OpenXcom is an open-source clone of the original X-Com
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
int main(int argc, char *argv[])
{
	// Uncomment to check memory leaks in VS
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#ifndef _DEBUG
	try
	{
		Logger::reportingLevel() = LOG_INFO;
#else
		Logger::reportingLevel() = LOG_DEBUG;
#endif
		if (!Options::init(argc, argv))
			return EXIT_SUCCESS;
		std::ostringstream title;
		title << "OpenXcom " << OPENXCOM_VERSION_SHORT << OPENXCOM_VERSION_GIT;
		if (Options::verboseLogging)
			Logger::reportingLevel() = LOG_VERBOSE;
		Options::baseXResolution = Options::displayWidth;
		Options::baseYResolution = Options::displayHeight;
		game = new Game(title.str());
		State::setGamePtr(game);
		game->setState(new StartState);
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


#ifdef __MORPHOS__
const char Version[] = "$VER: OpenXCom " OPENXCOM_VERSION_SHORT " (" __AMIGADATE__  ")";
#endif
