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
#include "BaseView.h"
#include <sstream>
#include <cmath>
#include "../Menu/ErrorMessageState.h"
#include "../Engine/Game.h"
#include "../Engine/Language.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Action.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleCraft.h"
#include "../Interface/Text.h"
#include "../Interface/Cursor.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include "DismantleFacilityState.h"
#include <limits>

namespace OpenXcom
{

/**
 * Sets up a base view with the specified size and position.
 * @param game Pointer to the core game.
 * @param leftClickHandler Handler for clicking with left-mouse button.
 * @param rightClickHandler Handler for clicking with right-mouse button.
 * @param showExtraSpace indicates whether to show the extra space (where new facilities can be built) at the sides.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
BaseView::BaseView(Game *game, BaseViewClickHandler leftClickHandler, BaseViewClickHandler rightClickHandler, bool showExtraSpace, int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _game(game), _leftClickHandler(leftClickHandler), _rightClickHandler(rightClickHandler), _showExtraSpace(showExtraSpace), _base(0), _texture(0), _selFacility(0), _big(0), _small(0), _lang(0), _gridX(0), _gridY(0), _selSize(0), _selector(0), _blink(true), _cameraPosX(0), _cameraPosY(0), isMouseScrolling(false), isMouseScrolled(false)
{
	_dragButton = Options::getInt("battleScrollDragButton");
	_dragInvert = Options::getBool("battleScrollDragInvert");
	_dragTimeTolerance = Options::getInt("battleScrollDragTimeTolerance");
	_dragPixelTolerance = Options::getInt("battleScrollDragPixelTolerance");

	// We need a shadow surface to avoid drawing outside (without this, the selector-rectangle can draw outside).
	_shadowSurface = new Surface(width, height, x, y);

	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&BaseView::blink);
	_timer->start();
}

/**
 * Deletes contents.
 */
BaseView::~BaseView()
{
	delete _shadowSurface;
	delete _selector;
	delete _timer;
}

/**
 * Replaces a certain amount of colors in the surface's palette.
 * @param colors Pointer to the set of colors.
 * @param firstcolor Offset of the first color to replace.
 * @param ncolors Amount of colors to replace.
 */
void BaseView::setPalette(SDL_Color *colors, int firstcolor, int ncolors)
{
	InteractiveSurface::setPalette(colors, firstcolor, ncolors);
	_shadowSurface->setPalette(colors);
	_shadowSurface->setVisible(true);
}

/**
 * Changes the various resources needed for text rendering.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text, and the language affects
 * how the text is rendered.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 * @param lang Pointer to current language.
 */
void BaseView::initText(Font *big, Font *small, Language *lang)
{
	_big = big;
	_small = small;
	_lang = lang;
}

/**
 * Changes the current base to display and
 * initializes the internal base grid.
 * @param base Pointer to base to display.
 */
void BaseView::setBase(Base *base)
{
	// Changing the base? -> reset the camera!
	if (_base != base)
	{
		_cameraPosX = 0;
		_cameraPosY = 0;
	}

	_base = base;
	_selFacility = 0;

	// Determine the sides
	if (_base->getFacilities()->size() > 0) _base->getSides(_sideLeft, _sideTop, _sideRight, _sideBottom, true);
	else
	{ // Ok, we have an empty base
		_sideLeft = _sideTop = 0;
		_sideRight = _sideBottom = MIN_BASE_SIZE-1;
	}

	// Do we want to show the extra space (where new facilities can be built) at the sides?
	if (_showExtraSpace)
	{
		int largestSize = _game->getRuleset()->getLargestFacilitySize();
		_sideLeft-=largestSize; _sideTop-=largestSize;
		_sideRight+=largestSize; _sideBottom+=largestSize;
	}

	// We always want to show the original 6x6 space at (0,0)
	if (0 < _sideLeft) _sideLeft = 0;
	if (MIN_BASE_SIZE-1 > _sideRight) _sideRight = MIN_BASE_SIZE-1;
	if (0 < _sideTop) _sideTop = 0;
	if (MIN_BASE_SIZE-1 > _sideBottom) _sideBottom = MIN_BASE_SIZE-1;

	// Correct the Camera
	if (0 != _cameraPosX || 0 != _cameraPosY)
	{
		if (-_cameraPosX < _sideLeft*GRID_SIZE) _cameraPosX = -_sideLeft*GRID_SIZE;
		else if ((_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE < -_cameraPosX) _cameraPosX = -(_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE;
		if (-_cameraPosY < _sideTop*GRID_SIZE) _cameraPosY = -_sideTop*GRID_SIZE;
		else if ((_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE < -_cameraPosY) _cameraPosY = -(_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE;
	}

	// Clear grid
	_facilities.clear();

	// Fill grid with base facilities
	for (std::vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
	{
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); ++y)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); ++x)
			{
				_facilities[x][y] = *i;
			}
		}
	}

	_redraw = true;
}

/**
 * Changes the texture to use for drawing
 * the various base elements.
 * @param texture Pointer to SurfaceSet to use.
 */
void BaseView::setTexture(SurfaceSet *texture)
{
	_texture = texture;
}

/**
 * Returns the facility the mouse is currently over.
 * @return Pointer to base facility (0 if none).
 */
BaseFacility *BaseView::getSelectedFacility() const
{
	return _selFacility;
}

/**
 * Gets the name of the currently selected facility.
 * @return wstring containing the name of the currently selected facility.
 */
std::wstring BaseView::getSelectedFacilityName() const
{
	std::wostringstream ss;
	if (_selFacility != 0)
	{
		if (_selFacility->getRules()->getCrafts() == 0 || _selFacility->getBuildTime() > 0)
		{
			ss << _game->getLanguage()->getString(_selFacility->getRules()->getType());
		}
		else
		{
			ss << _game->getLanguage()->getString(_selFacility->getRules()->getType());
			if (_selFacility->getCraft() != 0)
			{
				ss << L" " << _game->getLanguage()->getString("STR_CRAFT_").arg(_selFacility->getCraft()->getName(_game->getLanguage()));
			}
		}
	}
	return ss.str();
}

/**
 * Returns the Facility at the given indexes (searches _facilities), or returns 0 if it does not exist
 * @return Pointer to base facility (0 if none).
 */
BaseFacility *BaseView::getFacilityAt(int x, int y) const
{
	try { return _facilities.at(x).at(y); }
	catch (...) { return 0; }
}

/**
 * Prevents any mouseover bugs on dismantling base facilities before setBase has had time to update the base.
 */
void BaseView::resetSelectedFacility()
{
    _facilities[_selFacility->getX()][_selFacility->getY()] = 0;
    _selFacility = 0;
}


/**
 * Returns the X position of the grid square
 * the mouse is currently over.
 * @return X position on the grid.
 */
int BaseView::getGridX() const
{
	return _gridX;
}

/**
 * Returns the Y position of the grid square
 * the mouse is currently over.
 * @return Y position on the grid.
 */
int BaseView::getGridY() const
{
	return _gridY;
}

/**
 * Gets the X position of the camera.
 * @return X position.
 */
int BaseView::getCameraPosX() const
{
	return _cameraPosX;
}

/**
 * Gets the Y position of the camera.
 * @return Y position.
 */
int BaseView::getCameraPosY() const
{
	return _cameraPosY;
}

/**
 * Sets the position of the camera.
 * Note: setBase() must be called (at least 1 after the constructor) before calling this! (we have to know the limits)
 * @param x x-position of the camera.
 * @param y y-position of the camera.
 */
void BaseView::setCameraPos(int x, int y)
{
	_cameraPosX = x;
	_cameraPosY = y;

	// Keep the limits...
	if (-_cameraPosX < _sideLeft*GRID_SIZE) _cameraPosX = -_sideLeft*GRID_SIZE;
	else if ((_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE < -_cameraPosX) _cameraPosX = -(_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE;
	if (-_cameraPosY < _sideTop*GRID_SIZE) _cameraPosY = -_sideTop*GRID_SIZE;
	else if ((_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE < -_cameraPosY) _cameraPosY = -(_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE;

	_redraw = true;
}

/**
 * If enabled, the base view will respond to player input,
 * highlighting the selected facility.
 * @param size Facility length (0 disables it).
 */
void BaseView::setSelectable(int size)
{
	_selSize = size;
	if (_selSize > 0)
	{
		_selector = new Surface(size * GRID_SIZE, size * GRID_SIZE, _x, _y);
		_selector->setPalette(getPalette());
		SDL_Rect r;
		r.w = _selector->getWidth();
		r.h = _selector->getHeight();
		r.x = 0;
		r.y = 0;
		_selector->drawRect(&r, Palette::blockOffset(1));
		r.w -= 2;
		r.h -= 2;
		r.x++;
		r.y++;
		_selector->drawRect(&r, 0);
		_selector->setVisible(false);
	}
	else
	{
		delete _selector;
	}
}

/**
 * Returns if a certain facility can be successfully
 * placed on the currently selected square.
 * @param rule Facility type.
 * @return True if placeable, False otherwise.
 */
bool BaseView::isPlaceable(RuleBaseFacility *rule) const
{
	// Don't allow, if the selector is not visible for some reason! (ex. right-edge of the base with selectorsize=2)
	if (!_selector->getVisible()) return false;

	// Check if square isn't occupied
	for (int y = _gridY; y < _gridY + rule->getSize(); ++y)
		for (int x = _gridX; x < _gridX + rule->getSize(); ++x)
			if (getFacilityAt(x,y) != 0) return false;

	bool bq=Options::allowBuildingQueue;

	// Check for another facility to connect to
	for (int i = 0; i < rule->getSize(); ++i)
	{
		BaseFacility *leftNeighbor = getFacilityAt(_gridX-1, _gridY+i);
		BaseFacility *topNeighbor = getFacilityAt(_gridX+i, _gridY-1);
		BaseFacility *rightNeighbor = getFacilityAt(_gridX+rule->getSize(), _gridY+i);
		BaseFacility *bottomNeighbor = getFacilityAt(_gridX+i, _gridY+rule->getSize());
		if ((leftNeighbor != 0 && (bq || leftNeighbor->getBuildTime() == 0)) ||
			(topNeighbor != 0 && (bq || topNeighbor->getBuildTime() == 0)) ||
			(rightNeighbor != 0 && (bq || rightNeighbor->getBuildTime() == 0)) ||
			(bottomNeighbor != 0 && (bq || bottomNeighbor->getBuildTime() == 0)))
		{
			return true;
		}
	}

	return false;
}

/**
 * Returns if the placed facility is placed in queue or not.
 * @param rule Facility type.
 * @return True if queued, False otherwise.
 */
bool BaseView::isQueuedBuilding(RuleBaseFacility *rule) const
{
	for (int i = 0; i < rule->getSize(); ++i)
	{
		BaseFacility *leftNeighbor = getFacilityAt(_gridX-1, _gridY+i);
		BaseFacility *topNeighbor = getFacilityAt(_gridX+i, _gridY-1);
		BaseFacility *rightNeighbor = getFacilityAt(_gridX+rule->getSize(), _gridY+i);
		BaseFacility *bottomNeighbor = getFacilityAt(_gridX+i, _gridY+rule->getSize());
		if ((leftNeighbor != 0 && leftNeighbor->getBuildTime() == 0) ||
			(topNeighbor != 0 && topNeighbor->getBuildTime() == 0) ||
			(rightNeighbor != 0 && rightNeighbor->getBuildTime() == 0) ||
			(bottomNeighbor != 0 && bottomNeighbor->getBuildTime() == 0))
		{
			return false;
		}
	}
	return true;
}

/**
 * ReCalculates the remaining build-time of all queued buildings.
 */
void BaseView::reCalcQueuedBuildings()
{
	setBase(_base);
	std::vector<BaseFacility*> facilities;
	for (std::vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
		if ((*i)->getBuildTime() > 0)
		{
			// Set all queued buildings to infinite.
			if ((*i)->getBuildTime() > (*i)->getRules()->getBuildTime()) (*i)->setBuildTime(std::numeric_limits<int>::max());
			facilities.push_back(*i);
		}

	// Applying a simple Dijkstra Algorithm
	while (!facilities.empty())
	{
		std::vector<BaseFacility*>::iterator min = facilities.begin();
		for (std::vector<BaseFacility*>::iterator i = facilities.begin(); i != facilities.end(); ++i)
			if ((*i)->getBuildTime() < (*min)->getBuildTime()) min=i;
		BaseFacility* facility=(*min);
		facilities.erase(min);
		RuleBaseFacility *rule=facility->getRules();
		int x=facility->getX(), y=facility->getY();
		for (int i = 0; i < rule->getSize(); ++i)
		{
			updateNeighborFacilityBuildTime(facility, getFacilityAt(x-1, y+i));
			updateNeighborFacilityBuildTime(facility, getFacilityAt(x+i, y-1));
			updateNeighborFacilityBuildTime(facility, getFacilityAt(x+rule->getSize(), y+i));
			updateNeighborFacilityBuildTime(facility, getFacilityAt(x+i, y+rule->getSize()));
		}
	}
}

/**
 * Updates the neighborFacility's build time. This is for internal use only (reCalcQueuedBuildings()).
 * @param facility Pointer to a base facility.
 * @param neighbor Pointer to a neighboring base facility.
 */
void BaseView::updateNeighborFacilityBuildTime(BaseFacility* facility, BaseFacility* neighbor)
{
	if (0 != facility && 0 != neighbor
	&& neighbor->getBuildTime() > neighbor->getRules()->getBuildTime()
	&& facility->getBuildTime() + neighbor->getRules()->getBuildTime() < neighbor->getBuildTime())
		neighbor->setBuildTime(facility->getBuildTime() + neighbor->getRules()->getBuildTime());
}

/**
 * Keeps the animation timers running.
 */
void BaseView::think()
{
	_timer->think(0, this);
}

/**
 * Makes the facility selector blink.
 */
void BaseView::blink()
{
	_blink = !_blink;

	if (_selSize > 0)
	{
		SDL_Rect r;
		if (_blink)
		{
			r.w = _selector->getWidth();
			r.h = _selector->getHeight();
			r.x = 0;
			r.y = 0;
			_selector->drawRect(&r, Palette::blockOffset(1));
			r.w -= 2;
			r.h -= 2;
			r.x++;
			r.y++;
			_selector->drawRect(&r, 0);
		}
		else
		{
			r.w = _selector->getWidth();
			r.h = _selector->getHeight();
			r.x = 0;
			r.y = 0;
			_selector->drawRect(&r, 0);
		}
	}
}

/**
 * Draws the view of all the facilities in the base, connectors
 * between them and crafts landed in hangars.
 */
void BaseView::draw()
{
	Surface::draw();

	// Draw grid squares
	for (int x = _sideLeft; x <= _sideRight; ++x)
	{
		for (int y = _sideTop; y <= _sideBottom; ++y)
		{
			Surface *frame = _texture->getFrame(0);
			frame->setX(_cameraPosX + x * GRID_SIZE);
			frame->setY(_cameraPosY + y * GRID_SIZE);
			frame->blit(this);
		}
	}

	std::vector<Craft*>::iterator craft = _base->getCrafts()->begin();

	for (std::vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
	{
		// Draw facility shape
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); ++y)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); ++x)
			{
				Surface *frame;

				if ((*i)->getBuildTime() == 0)
					frame = _texture->getFrame((*i)->getRules()->getSpriteShape() + num);
				else
					frame = _texture->getFrame((*i)->getRules()->getSpriteShape() + num + 2 + (*i)->getRules()->getSize());

				frame->setX(_cameraPosX + x * GRID_SIZE);
				frame->setY(_cameraPosY + y * GRID_SIZE);
				frame->blit(this);

				num++;
			}
		}
	}

	for (std::vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
	{
		// Draw connectors
		if ((*i)->getBuildTime() == 0)
		{
			// Facilities to the right
			int x = (*i)->getX() + (*i)->getRules()->getSize();
			for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); ++y)
			{
				BaseFacility *facility = getFacilityAt(x, y);
				if (facility != 0 && facility->getBuildTime() == 0)
				{
					Surface *frame = _texture->getFrame(7);
					frame->setX(_cameraPosX + x * GRID_SIZE - GRID_SIZE / 2);
					frame->setY(_cameraPosY + y * GRID_SIZE);
					frame->blit(this);
				}
			}

			// Facilities to the bottom
			int y = (*i)->getY() + (*i)->getRules()->getSize();
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); ++x)
			{
				BaseFacility *facility = getFacilityAt(x, y);
				if (facility != 0 && facility->getBuildTime() == 0)
				{
					Surface *frame = _texture->getFrame(8);
					frame->setX(_cameraPosX + x * GRID_SIZE);
					frame->setY(_cameraPosY + y * GRID_SIZE - GRID_SIZE / 2);
					frame->blit(this);
				}
			}
		}
	}

	for (std::vector<BaseFacility*>::iterator i = _base->getFacilities()->begin(); i != _base->getFacilities()->end(); ++i)
	{
		// Draw facility graphic
		int num = 0;
		for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); ++y)
		{
			for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); ++x)
			{
				if ((*i)->getRules()->getSize() == 1)
				{
					Surface *frame = _texture->getFrame((*i)->getRules()->getSpriteFacility() + num);
					frame->setX(_cameraPosX + x * GRID_SIZE);
					frame->setY(_cameraPosY + y * GRID_SIZE);
					frame->blit(this);
				}

				num++;
			}
		}

		// Draw crafts
		if ((*i)->getBuildTime() == 0 && (*i)->getRules()->getCrafts() > 0)
		{
			if (craft != _base->getCrafts()->end())
			{
				if ((*craft)->getStatus() != "STR_OUT")
				{
					Surface *frame = _texture->getFrame((*craft)->getRules()->getSprite() + 33);
					frame->setX(_cameraPosX + (*i)->getX() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2 + 2);
					frame->setY(_cameraPosY + (*i)->getY() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2 - 4);
					frame->blit(this);
					(*i)->setCraft(*craft);
				}
				else
				{
					(*i)->setCraft(0);
				}
				++craft;
			}
			else
			{
				(*i)->setCraft(0);
			}
		}

		// Draw time remaining
		if ((*i)->getBuildTime() > 0)
		{
			Text *text = new Text(GRID_SIZE * (*i)->getRules()->getSize(), 16, 0, 0);
			text->setPalette(getPalette());
			text->initText(_big, _small, _lang);
			text->setX(_cameraPosX + (*i)->getX() * GRID_SIZE);
			text->setY(_cameraPosY + (*i)->getY() * GRID_SIZE + (GRID_SIZE * (*i)->getRules()->getSize() - 16) / 2);
			text->setBig();
			std::wostringstream ss;
			ss << (*i)->getBuildTime();
			text->setAlign(ALIGN_CENTER);
			text->setColor(Palette::blockOffset(13)+5);
			text->setText(ss.str());
			text->blit(this);
			delete text;
		}
	}
}

/**
 * Blits the base view and selector.
 * @param surface Pointer to surface to blit onto.
 */
void BaseView::blit(Surface *surface)
{
	int oldX = _x, oldY = _y;
	_shadowSurface->setX(0); _shadowSurface->setY(0);
	setX(0); setY(0);
	Surface::blit(_shadowSurface);
	if (_selector != 0)
	{
		_selector->setX(_selector->getX() - oldX); _selector->setY(_selector->getY() - oldY);
		_selector->blit(_shadowSurface);
		_selector->setX(_selector->getX() + oldX); _selector->setY(_selector->getY() + oldY);
	}
	_shadowSurface->setX(oldX); _shadowSurface->setY(oldY);
	setX(oldX); setY(oldY);
	_shadowSurface->blit(surface);
}

/**
 * Sets the left-click handler.
 * @param leftClickHandler Pointer to Handler code.
 */
void BaseView::setLeftClickHandler(BaseViewClickHandler leftClickHandler)
{
	_leftClickHandler = leftClickHandler;
}

/**
 * Sets the right-click handler.
 * @param rightClickHandler Pointer to Handler code.
 */
void BaseView::setRightClickHandler(BaseViewClickHandler rightClickHandler)
{
	_rightClickHandler = rightClickHandler;
}

/**
 * Handles mouse presses on the BaseView. Enters mouse-moving mode when the right button is used.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BaseView::mousePress(Action *action, State *state)
{
	InteractiveSurface::mousePress(action, state);

	if (((Uint8)-1) != _dragButton)
	{
		if (action->getDetails()->button.button == _dragButton)
		{
			isMouseScrolling = true;
			isMouseScrolled = false;
			SDL_GetMouseState(&xBeforeMouseScrolling, &yBeforeMouseScrolling);
			posBeforeMouseScrollingX = _cameraPosX; posBeforeMouseScrollingY = _cameraPosY;
			mouseScrollX = 0; mouseScrollY = 0;
			totalMouseMoveX = 0; totalMouseMoveY = 0;
			mouseMovedOverThreshold = false;
			mouseScrollingStartTime = SDL_GetTicks();
		}
	}
}

/**
 * Handles mouse clicks on the BaseView. Will change the camera.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BaseView::mouseClick (Action *action, State *state)
{
	InteractiveSurface::mouseClick(action, state);

	// The following is the workaround for a rare problem where sometimes
	// the mouse-release event is missed for any reason.
	// However if the SDL is also missed the release event, then it is to no avail :(
	// (this part handles the release if it is missed and now an other button is used)
	if (isMouseScrolling)
	{
		if (action->getDetails()->button.button != _dragButton
		&& 0==(SDL_GetMouseState(0,0)&SDL_BUTTON(_dragButton)))
		{ // so we missed again the mouse-release :(
			// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
			if ((!mouseMovedOverThreshold) && (SDL_GetTicks() - mouseScrollingStartTime <= ((Uint32)_dragTimeTolerance)))
			{
				_cameraPosX = posBeforeMouseScrollingX; _cameraPosY = posBeforeMouseScrollingY;
				_redraw = true;
			}
			isMouseScrolled = isMouseScrolling = false;
		}
	}

	// Drag-Scroll release: release mouse-scroll-mode
	if (isMouseScrolling)
	{
		// While scrolling, other buttons are ineffective
		if (action->getDetails()->button.button == _dragButton) isMouseScrolling = false; else return;
		// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
		if ((!mouseMovedOverThreshold) && (SDL_GetTicks() - mouseScrollingStartTime <= ((Uint32)_dragTimeTolerance)))
		{
			isMouseScrolled = false;
			_cameraPosX = posBeforeMouseScrollingX; _cameraPosY = posBeforeMouseScrollingY;
			_redraw = true;
		}
		if (isMouseScrolled) return;
	}

	if (action->getDetails()->button.button == SDL_BUTTON_LEFT && 0 !=_leftClickHandler) (state->*_leftClickHandler)(action);
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT & 0 != _rightClickHandler) (state->*_rightClickHandler)(action);
}

/**
 * Handles moving over the BaseView.
 * Will change the camera when the mouse is moved in mouse-moving mode.
 * And selects the facility the mouse is over.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BaseView::mouseOver(Action *action, State *state)
{
	if (isMouseScrolling && action->getDetails()->type == SDL_MOUSEMOTION)
	{
		// The following is the workaround for a rare problem where sometimes
		// the mouse-release event is missed for any reason.
		// However if the SDL is also missed the release event, then it is to no avail :(
		// (checking: is the dragScroll-mouse-button still pressed?)
		if (0==(SDL_GetMouseState(0,0)&SDL_BUTTON(_dragButton)))
		{ // so we missed again the mouse-release :(
			// Check if we have to revoke the scrolling, because it was too short in time, so it was a click
			if ((!mouseMovedOverThreshold) && (SDL_GetTicks() - mouseScrollingStartTime <= ((Uint32)_dragTimeTolerance)))
			{
				_cameraPosX = posBeforeMouseScrollingX; _cameraPosY = posBeforeMouseScrollingY;
				_redraw = true;
			}
			isMouseScrolled = isMouseScrolling = false;
			return;
		}

		isMouseScrolled = true;

		if (_dragInvert)
		{
			// Set the mouse cursor back
			SDL_EventState(SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse(xBeforeMouseScrolling, yBeforeMouseScrolling);
			SDL_EventState(SDL_MOUSEMOTION, SDL_ENABLE);
		}

		// Check the threshold
		totalMouseMoveX += action->getDetails()->motion.xrel;
		totalMouseMoveY += action->getDetails()->motion.yrel;
		if (!mouseMovedOverThreshold)
			mouseMovedOverThreshold = ((std::abs(totalMouseMoveX) > _dragPixelTolerance) || (std::abs(totalMouseMoveY) > _dragPixelTolerance));

		// Calculate the move
		int newX;
		int newY;
		if (_dragInvert)
		{
			mouseScrollX += action->getDetails()->motion.xrel;
			mouseScrollY += action->getDetails()->motion.yrel;
			newX = posBeforeMouseScrollingX - mouseScrollX / 4;
			newY = posBeforeMouseScrollingY - mouseScrollY / 4;

			// Keep the limits...
			if (-newX < _sideLeft*GRID_SIZE)
			{
				newX = -_sideLeft*GRID_SIZE;
				mouseScrollX = (posBeforeMouseScrollingX-newX)*4;
			}
			else if ((_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE < -newX)
			{
				newX = -(_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE;
				mouseScrollX = (posBeforeMouseScrollingX-newX)*4;
			}
			if (-newY < _sideTop*GRID_SIZE)
			{
				newY = -_sideTop*GRID_SIZE;
				mouseScrollY = (posBeforeMouseScrollingY-newY)*4;
			}
			else if ((_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE < -newY)
			{
				newY = -(_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE;
				mouseScrollY = (posBeforeMouseScrollingY-newY)*4;
			}
		}
		else
		{
			newX = posBeforeMouseScrollingX + (int)((double)totalMouseMoveX / action->getXScale());
			newY = posBeforeMouseScrollingY + (int)((double)totalMouseMoveY / action->getYScale());

			// Keep the limits...
			if (-newX < _sideLeft*GRID_SIZE) newX = -_sideLeft*GRID_SIZE;
			else if ((_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE < -newX) newX = -(_sideRight-(MIN_BASE_SIZE-1))*GRID_SIZE;
			if (-newY < _sideTop*GRID_SIZE) newY = -_sideTop*GRID_SIZE;
			else if ((_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE < -newY) newY = -(_sideBottom-(MIN_BASE_SIZE-1))*GRID_SIZE;
		}

		// Scrolling
		_cameraPosX = newX; _cameraPosY = newY;
		_redraw = true;

		if (_dragInvert)
		{
			// We don't want to look the mouse-cursor jumping :)
			action->getDetails()->motion.x=xBeforeMouseScrolling; action->getDetails()->motion.y=yBeforeMouseScrolling;
			action->setMouseAction(xBeforeMouseScrolling, yBeforeMouseScrolling, getX(), getY());
			_game->getCursor()->handle(action);
		}
	}

	// Update the selector...
	_gridX = (int)floor((action->getRelativeXMouse() / action->getXScale() - _cameraPosX) / GRID_SIZE);
	_gridY = (int)floor((action->getRelativeYMouse() / action->getYScale() - _cameraPosY) / GRID_SIZE);
	_selFacility = getFacilityAt(_gridX, _gridY);
	if (_selSize > 0)
	{
		if (_gridX + _selSize - 1 <= _sideRight && _gridY + _selSize - 1 <= _sideBottom)
		{
			_selector->setX(_x + _gridX * GRID_SIZE + _cameraPosX);
			_selector->setY(_y + _gridY * GRID_SIZE + _cameraPosY);
			_selector->setVisible(true);
		}
		else
		{
			_selector->setVisible(false);
		}
	}

	InteractiveSurface::mouseOver(action, state);
}

/**
 * Handles moving into the BaseView.
 * Stops the mouse-scrolling mode, if its left on.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BaseView::mouseIn(Action *action, State *state)
{
	InteractiveSurface::mouseIn(action, state);

	isMouseScrolling = false;
	setButtonPressed(SDL_BUTTON_RIGHT, false);
}

/**
 * Deselects the facility.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BaseView::mouseOut(Action *action, State *state)
{
	_selFacility = 0;
	if (_selSize > 0)
	{
		_selector->setVisible(false);
	}

	InteractiveSurface::mouseOut(action, state);
}

/**
 * Tries to dismantle the selected facility.
 */
void BaseView::tryDismantle()
{
	if (_selFacility != 0)
	{
		// Is facility in use?
		if (_selFacility->inUse())
		{
			_game->pushState(new ErrorMessageState(_game, "STR_FACILITY_IN_USE", _palette, Palette::blockOffset(15)+1, "BACK13.SCR", 6));
		}
		// Would base become disconnected?
		else if (!_base->getDisconnectedFacilities(_selFacility).empty())
		{
			_game->pushState(new ErrorMessageState(_game, "STR_CANNOT_DISMANTLE_FACILITY", _palette, Palette::blockOffset(15)+1, "BACK13.SCR", 6));
		}
		else
		{
			_game->pushState(new DismantleFacilityState(_game, _base, this, _selFacility));
		}
	}
}

}
