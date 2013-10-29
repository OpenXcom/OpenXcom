/*
 * Copyright 2010-2013 OpenXcom Developers.
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
#include "State.h"
#include "InteractiveSurface.h"
#include "Game.h"
#include "Screen.h"
#include "Surface.h"
#include "Font.h"
#include "Language.h"
#include "LocalizedText.h"
#include "Palette.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextEdit.h"
#include "../Interface/TextList.h"
#include "../Interface/ArrowButton.h"
#include "../Interface/Slider.h"

namespace OpenXcom
{

/**
 * Initializes a brand new state with no child elements.
 * By default states are full-screen.
 * @param game Pointer to the core game.
 */
State::State(Game *game) : _game(game), _surfaces(), _screen(true)
{

}

/**
 * Deletes all the child elements contained in the state.
 */
State::~State()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i < _surfaces.end(); ++i)
	{
		delete *i;
	}
}

/**
 * Adds a new child surface for the state to take care of,
 * giving it the game's display palette. Once associated,
 * the state handles all of the surface's behaviour
 * and management automatically.
 * @param surface Child surface.
 * @note Since visible elements can overlap one another,
 * they have to be added in ascending Z-Order to be blitted
 * correctly onto the screen.
 */
void State::add(Surface *surface)
{
	// Set palette
	surface->setPalette(_game->getScreen()->getPalette());

	// Set default fonts
	if (_game->getResourcePack())
		surface->setFonts(_game->getResourcePack()->getFont("FONT_BIG"), _game->getResourcePack()->getFont("FONT_SMALL"));

	_surfaces.push_back(surface);
}

/**
 * Returns whether this is a full-screen state.
 * This is used to optimize the state machine since full-screen
 * states automatically cover the whole screen, (whether they
 * actually use it all or not) so states behind them can be
 * safely ignored since they'd be covered up.
 * @return True if it's a screen, False otherwise.
 */
bool State::isScreen() const
{
	return _screen;
}

/**
 * Toggles the full-screen flag. Used by windows to
 * keep the previous screen is display while the window
 * is still "popping up".
 */
void State::toggleScreen()
{
	_screen = !_screen;
}

/**
 * Initializes the state and its child elements. This is
 * used for settings that have to be reset every time the
 * state is returned to focus (eg. palettes), so can't
 * just be put in the constructor (remember there's a stack
 * of states, so they can be created once while being
 * repeatedly switched back into focus).
 */
void State::init()
{

}

/**
 * Runs any code the state needs to keep updating every
 * game cycle, like timers and other real-time elements.
 */
void State::think()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
		(*i)->think();
}

/**
 * Takes care of any events from the core game engine,
 * and passes them on to its InteractiveSurface child elements.
 * @param action Pointer to an action.
 */
void State::handle(Action *action)
{
	for (std::vector<Surface*>::reverse_iterator i = _surfaces.rbegin(); i != _surfaces.rend(); ++i)
	{
		InteractiveSurface* j = dynamic_cast<InteractiveSurface*>(*i);
		if (j != 0)
			j->handle(action, this);
	}
}

/**
 * Blits all the visible Surface child elements onto the
 * display screen, by order of addition.
 */
void State::blit()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
		(*i)->blit(_game->getScreen()->getSurface());
}

/**
 * Hides all the Surface child elements on display.
 */
void State::hideAll()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
			(*i)->setHidden(true);
}

/**
 * Shows all the hidden Surface child elements.
 */
void State::showAll()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
		(*i)->setHidden(false);
}

/**
 * Resets the status of all the Surface child elements,
 * like unpressing buttons.
 */
void State::resetAll()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		InteractiveSurface *s = dynamic_cast<InteractiveSurface*>(*i);
		if (s != 0)
		{
			s->unpress(this);
		}
	}
}

/**
 * Get the localized text for dictionary key @a id.
 * This function forwards the call to Language::getString(const std::string &).
 * @param id The dictionary key to search for.
 * @return A reference to the localized text.
 */
const LocalizedText &State::tr(const std::string &id) const
{
	return _game->getLanguage()->getString(id);
}

/**
 * Get a modifiable copy of the localized text for dictionary key @a id.
 * This function forwards the call to Language::getString(const std::string &, unsigned).
 * @param id The dictionary key to search for.
 * @param n The number to use for the proper version.
 * @return A copy of the localized text.
 */
LocalizedText State::tr(const std::string &id, unsigned n) const
{
	return _game->getLanguage()->getString(id, n);
}

void State::centerAllSurfaces()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setX((*i)->getX() + Screen::getDX());
		(*i)->setY((*i)->getY() + Screen::getDY());
	}
}

void State::lowerAllSurfaces()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setY((*i)->getY() + Screen::getDY() / 2);
	}
}

void State::applyBattlescapeTheme()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		Window* window = dynamic_cast<Window*>(*i);
		if (window)
		{
			window->setColor(Palette::blockOffset(0)-1);
			window->setHighContrast(true);
			window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));
		}
		Text* text = dynamic_cast<Text*>(*i);
		if (text)
		{
			text->setColor(Palette::blockOffset(0)-1);
			text->setHighContrast(true);
		}
		TextButton* button = dynamic_cast<TextButton*>(*i);
		if (button)
		{
			button->setColor(Palette::blockOffset(0)-1);
			button->setHighContrast(true);
		}
		TextEdit* edit = dynamic_cast<TextEdit*>(*i);
		if (edit)
		{
			edit->setColor(Palette::blockOffset(0)-1);
			edit->setHighContrast(true);
		}
		TextList* list = dynamic_cast<TextList*>(*i);
		if (list)
		{
			list->setColor(Palette::blockOffset(0)-1);
			list->setArrowColor(Palette::blockOffset(0));
			list->setHighContrast(true);
		}
		ArrowButton *arrow = dynamic_cast<ArrowButton*>(*i);
		if (arrow)
		{
			arrow->setColor(Palette::blockOffset(0)-1);
		}
		Slider *slider = dynamic_cast<Slider*>(*i);
		if (slider)
		{
			slider->setColor(Palette::blockOffset(0)-1);
			slider->setHighContrast(true);
		}
	}
}

}
