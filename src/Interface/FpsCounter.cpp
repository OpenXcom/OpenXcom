/*
 * Copyright 2010-2016 OpenXcom Developers.
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
#include <iomanip>
#include "FpsCounter.h"
#include "../Engine/Action.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "../Engine/Font.h"
#include "../Engine/Language.h"
#include "../Interface/Text.h"

namespace OpenXcom
{

/**
 * Creates a FPS counter of the specified size.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
FpsCounter::FpsCounter(int width, int height, int x, int y)  : Surface(width, height, x, y),
				_text(), _font(0), _lang(0),
				_input(), _logic(), _blit(), _idle(), _frame(), _wsconverter()
{
	_visible = Options::fpsCounter;
	_font = new Font();
	_font->loadTomThumbNew();
	_lang = new Language();
	_text = new Text(width, height, x, y);
	_text->initText(_font, _font, _lang);
	_text->setSmall();
	_text->setVisible(true);
	_text->setHidden(false);
	_text->setAlign(ALIGN_LEFT);
	_text->setVerticalAlign(ALIGN_TOP);
}

/**
 * Deletes FPS counter content.
 */
FpsCounter::~FpsCounter()
{
	delete _text;
}

/**
 * Replaces a certain amount of colors in the FPS counter palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void FpsCounter::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	Surface::setPalette(colors, firstcolor, ncolors);
	_text->setPalette(colors, firstcolor, ncolors);
}

/**
 * Sets the text color of the counter.
 * @param color The color to set.
 */
void FpsCounter::setColor(Uint8 color)
{
	_text->setColor(color);
}

/**
 * Shows / hides the FPS counter.
 * @param action Pointer to an action.
 */
void FpsCounter::handle(Action *action)
{
	if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == Options::keyFps)
	{
		_visible = !_visible;
		Options::fpsCounter = _visible;
	}
}

/**
 * Draws the FPS counter.
 */
void FpsCounter::draw()
{
	Surface::draw();
	std::ostringstream actual_text;
	actual_text<<"H:"<<std::setw(2)<<static_cast<int>(round(_input.get()));
	actual_text<<" L:"<<std::setw(2)<<static_cast<int>(round(_logic.get()));
	actual_text<<" B:"<<std::setw(2)<<static_cast<int>(round(_blit.get()));
	actual_text<<" I:"<<std::setw(2)<<static_cast<int>(round(_idle.get()));
	actual_text<<" F:"<<std::setw(2)<<static_cast<int>(round(_frame.get()));
	actual_text<<"/"<<std::setw(2)<<static_cast<int>(_limit);
	actual_text<<" FPS: "<<std::setw(2)<<static_cast<int>(round(1000.0/_frame.get()));
	_text->setText(_wsconverter.from_bytes(actual_text.str().c_str()));
	_text->draw();
	_text->blit(this);
}

void FpsCounter::addFrame(const int  inputProcessingTime, const int  logicProcessingTime, const int  blittingTime, const int idleTime, const int limit)
{
	_input.update(inputProcessingTime);
	_logic.update(logicProcessingTime);
	_blit.update(blittingTime);
	_idle.update(idleTime);
	_frame.update(inputProcessingTime + logicProcessingTime + blittingTime + idleTime);
	_limit = limit;
}


}

