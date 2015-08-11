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
#include "State.h"
#include "InteractiveSurface.h"
#include "Game.h"
#include "Screen.h"
#include "Surface.h"
#include "Language.h"
#include "LocalizedText.h"
#include "Palette.h"
#include "../Resource/ResourcePack.h"
#include "../Interface/Window.h"
#include "../Interface/TextButton.h"
#include "../Interface/TextEdit.h"
#include "../Interface/TextList.h"
#include "../Interface/BattlescapeButton.h"
#include "../Interface/ArrowButton.h"
#include "../Interface/Slider.h"
#include "../Interface/ComboBox.h"
#include "../Interface/Cursor.h"
#include "../Interface/FpsCounter.h"
#include "../Savegame/SavedBattleGame.h"

namespace OpenXcom
{

/// Initializes static member
Game* State::_game = 0;

/**
 * Initializes a brand new state with no child elements.
 * By default states are full-screen.
 * @param game Pointer to the core game.
 */
State::State() : _screen(true), _modal(0), _ruleInterface(0), _ruleInterfaceParent(0)
{
	// initialize palette to all black
	memset(_palette, 0, sizeof(_palette));
	_cursorColor = _game->getCursor()->getColor();
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
 * Set interface data from the ruleset, also sets the palette for the state.
 * @param category Name of the interface set.
 * @param alterPal Should we swap out the backpal colors?
 * @param battleGame Should we use battlescape palette? (this only applies to options screens)
 */
void State::setInterface(const std::string& category, bool alterPal, SavedBattleGame *battleGame)
{
	int backPal = -1;
	std::string pal = "PAL_GEOSCAPE";

	_ruleInterface = _game->getRuleset()->getInterface(category);
	if (_ruleInterface)
	{
		_ruleInterfaceParent = _game->getRuleset()->getInterface(_ruleInterface->getParent());
		pal = _ruleInterface->getPalette();
		Element *element = _ruleInterface->getElement("palette");
		if (_ruleInterfaceParent)
		{
			if (!element)
			{
				element = _ruleInterfaceParent->getElement("palette");
			}
			if (pal.empty())
			{
				pal = _ruleInterfaceParent->getPalette();
			}
		}
		if (element)
		{
			int color = alterPal ? element->color2 : element->color;
			if (color != INT_MAX)
			{
				backPal = color;
			}
		}
		if (!_ruleInterface->getMusic().empty())
		{
			_game->getResourcePack()->playMusic(_ruleInterface->getMusic());
		}
	}
	if (battleGame)
	{
		battleGame->setPaletteByDepth(this);
	}
	else if (pal.empty())
	{
		pal = "PAL_GEOSCAPE";
		setPalette(pal, backPal);
	}
	else
	{
		setPalette(pal, backPal);
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
	surface->setPalette(_palette);

	// Set default text resources
	if (_game->getLanguage() && _game->getResourcePack())
		surface->initText(_game->getResourcePack()->getFont("FONT_BIG"), _game->getResourcePack()->getFont("FONT_SMALL"), _game->getLanguage());

	_surfaces.push_back(surface);
}

/**
 * As above, except this adds a surface based on an
 * interface element defined in the ruleset.
 * @note that this function REQUIRES the ruleset to have been loaded prior to use.
 * @param surface Child surface.
 * @param id the ID of the element defined in the ruleset, if any.
 * @param category the category of elements this interface is associated with.
 * @param parent the surface to base the coordinates of this element off.
 * @note if no parent is defined the element will not be moved.
 */
void State::add(Surface *surface, const std::string &id, const std::string &category, Surface *parent)
{
	// Set palette
	surface->setPalette(_palette);

	// this only works if we're dealing with a battlescape button
	BattlescapeButton *bsbtn = dynamic_cast<BattlescapeButton*>(surface);

	if (_game->getRuleset()->getInterface(category))
	{
		Element *element = _game->getRuleset()->getInterface(category)->getElement(id);
		if (element)
		{
			if (parent && element->w != INT_MAX && element->h != INT_MAX)
			{
				surface->setWidth(element->w);
				surface->setHeight(element->h);
			}

			if (parent && element->x != INT_MAX && element->y != INT_MAX)
			{
				surface->setX(parent->getX() + element->x);
				surface->setY(parent->getY() + element->y);
			}

			surface->setTFTDMode(element->TFTDMode);

			if (element->color != INT_MAX)
			{
				surface->setColor(element->color);
			}
			if (element->color2 != INT_MAX)
			{
				surface->setSecondaryColor(element->color2);
			}
			if (element->border != INT_MAX)
			{
				surface->setBorderColor(element->border);
			}
		}
	}

	if (bsbtn)
	{
		// this will initialize the graphics and settings of the battlescape button.
		bsbtn->copy(parent);
		bsbtn->initSurfaces();
	}

	// Set default text resources
	if (_game->getLanguage() && _game->getResourcePack())
		surface->initText(_game->getResourcePack()->getFont("FONT_BIG"), _game->getResourcePack()->getFont("FONT_SMALL"), _game->getLanguage());

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
 * keep the previous screen in display while the window
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
	_game->getScreen()->setPalette(_palette);
	_game->getCursor()->setPalette(_palette);
	_game->getCursor()->setColor(_cursorColor);
	_game->getCursor()->draw();
	_game->getFpsCounter()->setPalette(_palette);
	_game->getFpsCounter()->setColor(_cursorColor);
	_game->getFpsCounter()->draw();
	if (_game->getResourcePack() != 0)
	{
		_game->getResourcePack()->setPalette(_palette);
	}
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
	if (!_modal)
	{
		for (std::vector<Surface*>::reverse_iterator i = _surfaces.rbegin(); i != _surfaces.rend(); ++i)
		{
			InteractiveSurface* j = dynamic_cast<InteractiveSurface*>(*i);
			if (j != 0)
				j->handle(action, this);
		}
	}
	else
	{
		_modal->handle(action, this);
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
			//s->setFocus(false);
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

/**
 * centers all the surfaces on the screen.
 */
void State::centerAllSurfaces()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setX((*i)->getX() + _game->getScreen()->getDX());
		(*i)->setY((*i)->getY() + _game->getScreen()->getDY());
	}
}

/**
 * drop all the surfaces by half the screen height
 */
void State::lowerAllSurfaces()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setY((*i)->getY() + _game->getScreen()->getDY() / 2);
	}
}

/**
 * switch all the colours to something a little more battlescape appropriate.
 */
void State::applyBattlescapeTheme()
{
	Element * element = _game->getRuleset()->getInterface("mainMenu")->getElement("battlescapeTheme");
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		Window* window = dynamic_cast<Window*>(*i);
		if (window)
		{
			window->setColor(element->color);
			window->setHighContrast(true);
			window->setBackground(_game->getResourcePack()->getSurface("TAC00.SCR"));
			continue;
		}
		Text* text = dynamic_cast<Text*>(*i);
		if (text)
		{
			text->setColor(element->color);
			text->setHighContrast(true);
			continue;
		}
		TextButton* button = dynamic_cast<TextButton*>(*i);
		if (button)
		{
			button->setColor(element->color);
			button->setHighContrast(true);
			continue;
		}
		TextEdit* edit = dynamic_cast<TextEdit*>(*i);
		if (edit)
		{
			edit->setColor(element->color);
			edit->setHighContrast(true);
			continue;
		}
		TextList* list = dynamic_cast<TextList*>(*i);
		if (list)
		{
			list->setColor(element->color);
			list->setArrowColor(element->border);
			list->setHighContrast(true);
			continue;
		}
		ArrowButton *arrow = dynamic_cast<ArrowButton*>(*i);
		if (arrow)
		{
			arrow->setColor(element->border);
			continue;
		}
		Slider *slider = dynamic_cast<Slider*>(*i);
		if (slider)
		{
			slider->setColor(element->color);
			slider->setHighContrast(true);
			continue;
		}
		ComboBox *combo = dynamic_cast<ComboBox*>(*i);
		if (combo)
		{
			combo->setColor(element->color);
			combo->setArrowColor(element->border);
			combo->setHighContrast(true);
		}
	}
}

/**
 * redraw all the text-type surfaces.
 */
void State::redrawText()
{
	for (std::vector<Surface*>::iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		Text* text = dynamic_cast<Text*>(*i);
		TextButton* button = dynamic_cast<TextButton*>(*i);
		TextEdit* edit = dynamic_cast<TextEdit*>(*i);
		TextList* list = dynamic_cast<TextList*>(*i);
		if (text || button || edit || list)
		{
			(*i)->draw();
		}
	}
}

/**
 * Changes the current modal surface. If a surface is modal,
 * then only that surface can receive events. This is used
 * when an element needs to take priority over everything else,
 * eg. focus.
 * @param surface Pointer to modal surface, NULL for no modal.
 */
void State::setModal(InteractiveSurface *surface)
{
	_modal = surface;
}

/**
 * Replaces a certain amount of colors in the state's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 * @param immediately Apply changes immediately, otherwise wait in case of multiple setPalettes.
 */
void State::setPalette(SDL_Color *colors, int firstcolor, int ncolors, bool immediately)
{
	if (colors)
	{
		memcpy(_palette + firstcolor, colors, ncolors * sizeof(SDL_Color));
	}
	if (immediately)
	{
		_game->getCursor()->setPalette(_palette);
		_game->getCursor()->draw();
		_game->getFpsCounter()->setPalette(_palette);
		_game->getFpsCounter()->draw();
		if (_game->getResourcePack() != 0)
		{
			_game->getResourcePack()->setPalette(_palette);
		}
	}
}

/**
 * Loads palettes from the game resources into the state.
 * @param palette String ID of the palette to load.
 * @param backpals BACKPALS.DAT offset to use.
 */
void State::setPalette(const std::string &palette, int backpals)
{
	setPalette(_game->getResourcePack()->getPalette(palette)->getColors(), 0, 256, false);
	if (palette == "PAL_GEOSCAPE")
	{
		_cursorColor = ResourcePack::GEOSCAPE_CURSOR;
	}
	else if (palette == "PAL_BASESCAPE")
	{
		_cursorColor = ResourcePack::BASESCAPE_CURSOR;
	}
	else if (palette == "PAL_UFOPAEDIA")
	{
		_cursorColor = ResourcePack::UFOPAEDIA_CURSOR;
	}
	else if (palette == "PAL_GRAPHS")
	{
		_cursorColor = ResourcePack::GRAPHS_CURSOR;
	}
	else
	{
		_cursorColor = ResourcePack::BATTLESCAPE_CURSOR;
	}
	if (backpals != -1)
		setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(backpals)), Palette::backPos, 16, false);
	setPalette(NULL); // delay actual update to the end
}

/**
 * Returns the state's 8bpp palette.
 * @return Pointer to the palette's colors.
 */
SDL_Color *const State::getPalette()
{
	return _palette;
}

/**
 * Each state will probably need its own resize handling,
 * so this space intentionally left blank
 * @param dX delta of X;
 * @param dY delta of Y;
 */
void State::resize(int &dX, int &dY)
{
	recenter(dX, dY);
}

/**
 * Re-orients all the surfaces in the state.
 * @param dX delta of X;
 * @param dY delta of Y;
 */
void State::recenter(int dX, int dY)
{
	for (std::vector<Surface*>::const_iterator i = _surfaces.begin(); i != _surfaces.end(); ++i)
	{
		(*i)->setX((*i)->getX() + dX / 2);
		(*i)->setY((*i)->getY() + dY / 2);
	}
}

void State::setGamePtr(Game* game)
{
    _game = game;
}

}
