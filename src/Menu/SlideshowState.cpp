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
#include "CutsceneState.h"
#include "../Engine/FileMap.h"
#include "../Engine/Game.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Screen.h"
#include "../Engine/Timer.h"
#include "../Interface/Text.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

SlideshowState::SlideshowState(const SlideshowHeader &slideshowHeader,
			       const std::vector<SlideshowSlide> *slideshowSlides)
		: _slideshowHeader(slideshowHeader), _slideshowSlides(slideshowSlides), _curScreen(-1)
{
	_wasLetterboxed = CutsceneState::initDisplay();

	// pre-render and queue up all the frames
	for (std::vector<SlideshowSlide>::const_iterator it = _slideshowSlides->begin(); it != _slideshowSlides->end(); ++it)
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
		Text *caption = new Text(it->w, it->h, it->x, it->y);
		caption->setColor(it->color);
		caption->setText(tr(it->caption));
		caption->setWordWrap(true);
		caption->setVisible(false);
		_captions.push_back(caption);
		add(caption);
	}

	centerAllSurfaces();

	_transitionTimer = new Timer(slideshowHeader.transitionSeconds * 1000);
	_transitionTimer->onTimer((StateHandler)&SlideshowState::screenTimer);

	_game->getResourcePack()->playMusic(slideshowHeader.musicId);

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
	if (_curScreen < _slideshowSlides->size())
	{
		_transitionTimer->start();
		setPalette(_slides[_curScreen]->getPalette());
		_slides[_curScreen]->setVisible(true);
		_captions[_curScreen]->setVisible(true);
		init();
	}
	else
	{
		// slideshow is over.  restore the screen scale and pop the state
		CutsceneState::resetDisplay(_wasLetterboxed);
		_game->popState();
	}
}

}
