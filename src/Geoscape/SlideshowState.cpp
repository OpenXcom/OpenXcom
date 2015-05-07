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
#include "SlideshowState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Engine/FileMap.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/Text.h"
#include "../Engine/InteractiveSurface.h"
#include "../Savegame/SavedGame.h"
#include "../Menu/MainMenuState.h"
#include "../Engine/Music.h"
#include "../Engine/Timer.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"

namespace OpenXcom
{

SlideshowState::SlideshowState(const std::vector<SlideshowSlide> *slideshowRule)
		: _slideshowRule(slideshowRule), _curScreen(-1)
{
	Options::baseXResolution = Screen::ORIGINAL_WIDTH;
	Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
	_game->getScreen()->resetDisplay(false);

	// pre-render and queue up all the frames
	for (std::vector<SlideshowSlide>::const_iterator it = _slideshowRule->begin(); it != _slideshowRule->end(); ++it)
	{
		InteractiveSurface *slide =
			new InteractiveSurface(Screen::ORIGINAL_WIDTH, Screen::ORIGINAL_HEIGHT, 0, 0);
		slide->loadImage(FileMap::getFilePath(it->imagePath));
		slide->onMouseClick((ActionHandler)&SlideshowState::screenClick);
		slide->setVisible(false);
		_slides.push_back(slide);
		setPalette(slide->getPalette());
		add(slide);

		// initialize with default rect; may get overridden by
		// category/id definition
		Text *caption = new Text(Screen::ORIGINAL_WIDTH - 10, 18, 5, Screen::ORIGINAL_HEIGHT - 27);
		caption->setText(tr(it->caption));
		caption->setWordWrap(true);
		caption->setVisible(false);
		_captions.push_back(caption);
		add(caption, it->captionId, it->captionCategory, slide);
	}

	centerAllSurfaces();

	_transitionTimer = new Timer(0);
	_transitionTimer->onTimer((StateHandler)&SlideshowState::screenTimer);

	screenClick(0);
}

SlideshowState::~SlideshowState()
{
	delete _transitionTimer;
}

/**
 * Shows the next screen on a timed basis.
 */
void SlideshowState::screenTimer()
{
	screenClick(0);
}

/**
 * Handle timers.
 */
void SlideshowState::think()
{
	_transitionTimer->think(this, 0);
}

/**
 * Shows the next screen in the slideshow; pops the state when there are no more slides
 */
void SlideshowState::screenClick(Action *)
{
	if (_curScreen >= 0)
	{
		_slides[_curScreen]->setVisible(false);
		_captions[_curScreen]->setVisible(false);
	}

	++_curScreen;

	// next screen
	if (_curScreen < _slideshowRule->size())
	{
		_transitionTimer->setInterval(_slideshowRule->at(_curScreen).durationSeconds * 1000);
		_transitionTimer->start();

		// if music same as previous slide, music keeps playing uninterrupted
		// otherwise start playing new music
		bool playNewMusic = true;
		std::string curMusicFile = _slideshowRule->at(_curScreen).musicId;
		if (_curScreen > 0)
		{
			std::string prevMusicFile = _slideshowRule->at(_curScreen - 1).musicId;
			playNewMusic = (prevMusicFile != curMusicFile);
		}
		if (playNewMusic)
		{
			_game->getResourcePack()->playMusic(curMusicFile);
		}

		setPalette(_slides[_curScreen]->getPalette());
		_slides[_curScreen]->setVisible(true);
		_captions[_curScreen]->setVisible(true);
		init();
	}
	else
	{
		// slideshow is over.  restore the screen scale and pop the state
		Screen::updateScale(Options::geoscapeScale, Options::geoscapeScale, Options::baseXGeoscape, Options::baseYGeoscape, true);
		_game->getScreen()->resetDisplay(false);
		_game->popState();
	}
}

}
