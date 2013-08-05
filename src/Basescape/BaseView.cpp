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
#include "../Engine/SurfaceSet.h"
#include "../Engine/Action.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Interface/Text.h"
#include "../Engine/Palette.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"
#include <limits>

namespace OpenXcom
{

/**
 * Sets up a base view with the specified size and position.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @param x X position in pixels.
 * @param y Y position in pixels.
 */
BaseView::BaseView(int width, int height, int x, int y) : InteractiveSurface(width, height, x, y), _base(0), _texture(0), _selFacility(0), _big(0), _small(0), _gridX(0), _gridY(0), _selSize(0), _selector(0), _blink(true)
{
	for (int x = 0; x < BASE_SIZE; ++x)
		for (int y = 0; y < BASE_SIZE; ++y)
			_facilities[x][y] = 0;

	_timer = new Timer(100);
	_timer->onTimer((SurfaceHandler)&BaseView::blink);
	_timer->start();
}

/**
 * Deletes contents.
 */
BaseView::~BaseView()
{
	delete _selector;
	delete _timer;
}

/**
 * Changes the various fonts for the text to use.
 * The different fonts need to be passed in advance since the
 * text size can change mid-text.
 * @param big Pointer to large-size font.
 * @param small Pointer to small-size font.
 */
void BaseView::setFonts(Font *big, Font *small)
{
	_big = big;
	_small = small;
}

/**
 * Changes the current base to display and
 * initializes the internal base grid.
 * @param base Pointer to base to display.
 */
void BaseView::setBase(Base *base)
{
	_base = base;
	_selFacility = 0;

	// Clear grid
	for (int x = 0; x < BASE_SIZE; ++x)
		for (int y = 0; y < BASE_SIZE; ++y)
			_facilities[x][y] = 0;

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
	// Check if square isn't occupied
	for (int y = _gridY; y < _gridY + rule->getSize(); ++y)
	{
		for (int x = _gridX; x < _gridX + rule->getSize(); ++x)
		{
			if (x < 0 || x >= BASE_SIZE || y < 0 || y >= BASE_SIZE)
			{
				return false;
			}
			if (_facilities[x][y] != 0)
			{
				return false;
			}
		}
	}

	bool bq=Options::getBool("allowBuildingQueue");

	// Check for another facility to connect to
	for (int i = 0; i < rule->getSize(); ++i)
	{
		if ((_gridX > 0 && _facilities[_gridX - 1][_gridY + i] != 0 && (bq || _facilities[_gridX - 1][_gridY + i]->getBuildTime() == 0)) ||
			(_gridY > 0 && _facilities[_gridX + i][_gridY - 1] != 0 && (bq || _facilities[_gridX + i][_gridY - 1]->getBuildTime() == 0)) ||
			(_gridX + rule->getSize() < BASE_SIZE && _facilities[_gridX + rule->getSize()][_gridY + i] != 0 && (bq || _facilities[_gridX + rule->getSize()][_gridY + i]->getBuildTime() == 0)) ||
			(_gridY + rule->getSize() < BASE_SIZE && _facilities[_gridX + i][_gridY + rule->getSize()] != 0 && (bq || _facilities[_gridX + i][_gridY + rule->getSize()]->getBuildTime() == 0)))
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
		if ((_gridX > 0 && _facilities[_gridX - 1][_gridY + i] != 0 && _facilities[_gridX - 1][_gridY + i]->getBuildTime() == 0) ||
			(_gridY > 0 && _facilities[_gridX + i][_gridY - 1] != 0 && _facilities[_gridX + i][_gridY - 1]->getBuildTime() == 0) ||
			(_gridX + rule->getSize() < BASE_SIZE && _facilities[_gridX + rule->getSize()][_gridY + i] != 0 && _facilities[_gridX + rule->getSize()][_gridY + i]->getBuildTime() == 0) ||
			(_gridY + rule->getSize() < BASE_SIZE && _facilities[_gridX + i][_gridY + rule->getSize()] != 0 && _facilities[_gridX + i][_gridY + rule->getSize()]->getBuildTime() == 0))
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
			if (x > 0) updateNeighborFacilityBuildTime(facility,_facilities[x - 1][y + i]);
			if (y > 0) updateNeighborFacilityBuildTime(facility,_facilities[x + i][y - 1]);
			if (x + rule->getSize() < BASE_SIZE) updateNeighborFacilityBuildTime(facility,_facilities[x + rule->getSize()][y + i]);
			if (y + rule->getSize() < BASE_SIZE) updateNeighborFacilityBuildTime(facility,_facilities[x + i][y + rule->getSize()]);
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
 * Counts all the occupied squares connected to a certain position in the
 * grid inclusive, INCLUDING facilities under construction.
 * Mostly used to ensure a base stays connected to the Access Lift.
 * -1 = Unoccupied, 0 = Occupied, 1 = Connected.
 * @param x X position in grid.
 * @param y Y position in grid.
 * @param grid Pointer to connection grid (Null to create one from scratch).
 * @param remove Facility to ignore (in case of facility dismantling).
 * @return Number of squares connected to the starting position.
 */
int BaseView::countConnected(int x, int y, int **grid, BaseFacility *remove) const
{
	bool newgrid = (grid == 0);

	// Create connection grid
	if (newgrid)
	{
		grid = new int*[BASE_SIZE];

		for (int xx = 0; xx < BASE_SIZE; ++xx)
		{
			grid[xx] = new int[BASE_SIZE];
			for (int yy = 0; yy < BASE_SIZE; ++yy)
			{
				if (_facilities[xx][yy] == 0 || _facilities[xx][yy] == remove)
				{
					grid[xx][yy] = -1;
				}
				else
				{
					grid[xx][yy] = 0;
				}
			}
		}
	}

	if (x < 0 || x >= BASE_SIZE || y < 0 || y >= BASE_SIZE || grid[x][y] != 0)
	{
		return 0;
	}

	// Add connected (neighbor) facilities to grid
	int total = 1;
	grid[x][y]++;

	if (0 == _facilities[x][y]->getBuildTime()
	|| (x-1>=0 && 0!=_facilities[x-1][y] && (_facilities[x-1][y] == _facilities[x][y] || _facilities[x-1][y]->getBuildTime() > _facilities[x-1][y]->getRules()->getBuildTime())))
		total += countConnected(x - 1, y, grid, remove);
	if (0 == _facilities[x][y]->getBuildTime()
	|| (y-1>=0 && 0!=_facilities[x][y-1] && (_facilities[x][y-1] == _facilities[x][y] || _facilities[x][y-1]->getBuildTime() > _facilities[x][y-1]->getRules()->getBuildTime())))
		total += countConnected(x, y - 1, grid, remove);
	if (0 == _facilities[x][y]->getBuildTime()
	|| (x+1<BASE_SIZE && 0!=_facilities[x+1][y] && (_facilities[x+1][y] == _facilities[x][y] || _facilities[x+1][y]->getBuildTime() > _facilities[x+1][y]->getRules()->getBuildTime())))
		total += countConnected(x + 1, y, grid, remove);
	if (0 == _facilities[x][y]->getBuildTime()
	|| (y+1<BASE_SIZE && 0!=_facilities[x][y+1] && (_facilities[x][y+1] == _facilities[x][y] || _facilities[x][y+1]->getBuildTime() > _facilities[x][y+1]->getRules()->getBuildTime())))
		total += countConnected(x, y + 1, grid, remove);

	// Delete connection grid
	if (newgrid)
	{
		for (int xx = 0; xx < BASE_SIZE; ++xx)
		{
			delete[] grid[xx];
		}
		delete[] grid;
	}

	return total;
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
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			Surface *frame = _texture->getFrame(0);
			frame->setX(x * GRID_SIZE);
			frame->setY(y * GRID_SIZE);
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

				frame->setX(x * GRID_SIZE);
				frame->setY(y * GRID_SIZE);
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
			if (x < BASE_SIZE)
			{
				for (int y = (*i)->getY(); y < (*i)->getY() + (*i)->getRules()->getSize(); ++y)
				{
					if (_facilities[x][y] != 0 && _facilities[x][y]->getBuildTime() == 0)
					{
						Surface *frame = _texture->getFrame(7);
						frame->setX(x * GRID_SIZE - GRID_SIZE / 2);
						frame->setY(y * GRID_SIZE);
						frame->blit(this);
					}
				}
			}

			// Facilities to the bottom
			int y = (*i)->getY() + (*i)->getRules()->getSize();
			if (y < BASE_SIZE)
			{
				for (int x = (*i)->getX(); x < (*i)->getX() + (*i)->getRules()->getSize(); ++x)
				{
					if (_facilities[x][y] != 0 && _facilities[x][y]->getBuildTime() == 0)
					{
						Surface *frame = _texture->getFrame(8);
						frame->setX(x * GRID_SIZE);
						frame->setY(y * GRID_SIZE - GRID_SIZE / 2);
						frame->blit(this);
					}
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
					frame->setX(x * GRID_SIZE);
					frame->setY(y * GRID_SIZE);
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
					frame->setX((*i)->getX() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2 + 2);
					frame->setY((*i)->getY() * GRID_SIZE + ((*i)->getRules()->getSize() - 1) * GRID_SIZE / 2 - 4);
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
			text->setFonts(_big, _small);
			text->setX((*i)->getX() * GRID_SIZE);
			text->setY((*i)->getY() * GRID_SIZE + (GRID_SIZE * (*i)->getRules()->getSize() - 16) / 2);
			text->setBig();
			std::wstringstream ss;
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
	Surface::blit(surface);
	if (_selector != 0)
	{
		_selector->blit(surface);
	}
}

/**
 * Selects the facility the mouse is over.
 * @param action Pointer to an action.
 * @param state State that the action handlers belong to.
 */
void BaseView::mouseOver(Action *action, State *state)
{
	_gridX = (int)floor(action->getRelativeXMouse() / (GRID_SIZE * action->getXScale()));
	_gridY = (int)floor(action->getRelativeYMouse() / (GRID_SIZE * action->getYScale()));
	if (_gridX >= 0 && _gridX < BASE_SIZE && _gridY >= 0 && _gridY < BASE_SIZE)
	{
		_selFacility = _facilities[_gridX][_gridY];
		if (_selSize > 0)
		{
			if (_gridX + _selSize - 1 < BASE_SIZE && _gridY + _selSize - 1 < BASE_SIZE)
			{
				_selector->setX(_x + _gridX * GRID_SIZE);
				_selector->setY(_y + _gridY * GRID_SIZE);
				_selector->setVisible(true);
			}
			else
			{
				_selector->setVisible(false);
			}
		}
	}
	else
	{
		_selFacility = 0;
		if (_selSize > 0)
		{
			_selector->setVisible(false);
		}
	}

	InteractiveSurface::mouseOver(action, state);
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

}
