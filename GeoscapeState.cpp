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
#include "GeoscapeState.h"

GeoscapeState::GeoscapeState(Game *game) : State(game), _rotLon(0), _rotLat(0)
{
	// Create objects
	_bg = new Surface(320, 200, 0, 0);
	_globe = new Globe(130, 100, 256, 200, 0, 0);

	_btnIntercept = new CustomButton(63, 11, 257, 0);
	_btnBases = new CustomButton(63, 11, 257, 12);
	_btnGraphs = new CustomButton(63, 11, 257, 24);
	_btnUfopaedia = new CustomButton(63, 11, 257, 36);
	_btnOptions = new CustomButton(63, 11, 257, 48);
	_btnFunding = new CustomButton(63, 11, 257, 60);

	_btn5Secs = new CustomButton(31, 13, 257, 112);
	_btn1Min = new CustomButton(31, 13, 289, 112);
	_btn5Mins = new CustomButton(31, 13, 257, 126);
	_btn30Mins = new CustomButton(31, 13, 289, 126);
	_btn1Hour = new CustomButton(31, 13, 257, 140);
	_btn1Day = new CustomButton(31, 13, 289, 140);

	_btnRotateLeft = new InteractiveSurface(12, 10, 259, 176);
	_btnRotateRight = new InteractiveSurface(12, 10, 283, 176);
	_btnRotateUp = new InteractiveSurface(13, 12, 271, 162);
	_btnRotateDown = new InteractiveSurface(13, 12, 271, 187);
	_btnZoomIn = new InteractiveSurface(23, 23, 295, 156);
	_btnZoomOut = new InteractiveSurface(13, 17, 300, 182);

	_txtHour = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 20, 13, 259, 74);
	_txtHourSep = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 4, 13, 279, 74);
	_txtMin = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 20, 13, 283, 74);
	_txtMinSep = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 4, 13, 303, 74);
	_txtSec = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 11, 8, 307, 80);
	_txtWeekday = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 59, 8, 259, 87);
	_txtDay = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 29, 8, 259, 94);
	_txtMonth = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 29, 8, 288, 94);
	_txtYear = new Text(game->getFont("BIGLETS.DAT"), game->getFont("SMALLSET.DAT"), 59, 8, 259, 101);

	_timer = _btn5Secs;
	
	add(_bg);
	add(_globe);
	
	add(_btnIntercept);
	add(_btnBases);
	add(_btnGraphs);
	add(_btnUfopaedia);
	add(_btnOptions);
	add(_btnFunding);
	
	add(_btn5Secs);
	add(_btn1Min);
	add(_btn5Mins);
	add(_btn30Mins);
	add(_btn1Hour);
	add(_btn1Day);
	
	add(_btnRotateLeft);
	add(_btnRotateRight);
	add(_btnRotateUp);
	add(_btnRotateDown);
	add(_btnZoomIn);
	add(_btnZoomOut);
	
	add(_txtHour);
	add(_txtHourSep);
	add(_txtMin);
	add(_txtMinSep);
	add(_txtSec);
	add(_txtWeekday);
	add(_txtDay);
	add(_txtMonth);
	add(_txtYear);
	
	// Set up objects
	_game->getSurface("GEOBORD.SCR")->blit(_bg);
	if (_game->getLanguageName() != "ENGLISH.DAT")
	{
		Surface* sidebar;
		if (_game->getLanguageName() == "GERMAN.DAT")
			sidebar = _game->getSurface("LANG1.DAT");
		else if (_game->getLanguageName() == "FRENCH.DAT")
			sidebar = _game->getSurface("LANG2.DAT");
		sidebar->setX(320 - sidebar->getWidth());
		sidebar->setY(0);
		sidebar->blit(_bg);
	}
	
	stringstream s;
	s << DATA_FOLDER << "GEODATA/WORLD.DAT";
	_globe->loadDat(s.str());
	_globe->setTexture(game->getSurfaceSet("TEXTURE.DAT"));
	_globe->onMouseClick((EventHandler)&GeoscapeState::globeClick);

	_btnIntercept->copy(_bg);
	_btnIntercept->setColor(Palette::blockOffset(15)+8);
	_btnIntercept->onMouseClick((EventHandler)&GeoscapeState::btnInterceptClick);

	_btnBases->copy(_bg);
	_btnBases->setColor(Palette::blockOffset(15)+8);
	_btnBases->onMouseClick((EventHandler)&GeoscapeState::btnBasesClick);

	_btnGraphs->copy(_bg);
	_btnGraphs->setColor(Palette::blockOffset(15)+8);
	_btnGraphs->onMouseClick((EventHandler)&GeoscapeState::btnGraphsClick);

	_btnUfopaedia->copy(_bg);
	_btnUfopaedia->setColor(Palette::blockOffset(15)+8);
	_btnUfopaedia->onMouseClick((EventHandler)&GeoscapeState::btnUfopaediaClick);

	_btnOptions->copy(_bg);
	_btnOptions->setColor(Palette::blockOffset(15)+8);
	_btnOptions->onMouseClick((EventHandler)&GeoscapeState::btnOptionsClick);

	_btnFunding->copy(_bg);
	_btnFunding->setColor(Palette::blockOffset(15)+8);
	_btnFunding->onMouseClick((EventHandler)&GeoscapeState::btnFundingClick);

	_btn5Secs->copy(_bg);
	_btn5Secs->setColor(Palette::blockOffset(15)+8);
	_btn5Secs->setGroup(&_timer);

	_btn1Min->copy(_bg);
	_btn1Min->setColor(Palette::blockOffset(15)+8);
	_btn1Min->setGroup(&_timer);

	_btn5Mins->copy(_bg);
	_btn5Mins->setColor(Palette::blockOffset(15)+8);
	_btn5Mins->setGroup(&_timer);

	_btn30Mins->copy(_bg);
	_btn30Mins->setColor(Palette::blockOffset(15)+8);
	_btn30Mins->setGroup(&_timer);

	_btn1Hour->copy(_bg);
	_btn1Hour->setColor(Palette::blockOffset(15)+8);
	_btn1Hour->setGroup(&_timer);

	_btn1Day->copy(_bg);
	_btn1Day->setColor(Palette::blockOffset(15)+8);
	_btn1Day->setGroup(&_timer);
	
	_btnRotateLeft->onMousePress((EventHandler)&GeoscapeState::btnRotateLeftPress);
	_btnRotateLeft->onMouseRelease((EventHandler)&GeoscapeState::btnRotateLeftRelease);

	_btnRotateRight->onMousePress((EventHandler)&GeoscapeState::btnRotateRightPress);
	_btnRotateRight->onMouseRelease((EventHandler)&GeoscapeState::btnRotateRightRelease);

	_btnRotateUp->onMousePress((EventHandler)&GeoscapeState::btnRotateUpPress);
	_btnRotateUp->onMouseRelease((EventHandler)&GeoscapeState::btnRotateUpRelease);

	_btnRotateDown->onMousePress((EventHandler)&GeoscapeState::btnRotateDownPress);
	_btnRotateDown->onMouseRelease((EventHandler)&GeoscapeState::btnRotateDownRelease);

	_btnZoomIn->onMouseClick((EventHandler)&GeoscapeState::btnZoomInClick);

	_btnZoomOut->onMouseClick((EventHandler)&GeoscapeState::btnZoomOutClick);

	_txtHour->setBig();
	_txtHour->setColor(Palette::blockOffset(15)+4);
	_txtHour->setText("22");

	_txtHourSep->setBig();
	_txtHourSep->setColor(Palette::blockOffset(15)+4);
	_txtHourSep->setText(":");

	_txtMin->setBig();
	_txtMin->setColor(Palette::blockOffset(15)+4);
	_txtMin->setText("22");

	_txtMinSep->setBig();
	_txtMinSep->setColor(Palette::blockOffset(15)+4);
	_txtMinSep->setText(":");

	_txtSec->setSmall();
	_txtSec->setColor(Palette::blockOffset(15)+4);
	_txtSec->setText("22");

	_txtWeekday->setSmall();
	_txtWeekday->setColor(Palette::blockOffset(15)+4);
	_txtWeekday->setText("FRIDAY");
	_txtWeekday->setAlign(ALIGN_CENTER);

	_txtDay->setSmall();
	_txtDay->setColor(Palette::blockOffset(15)+4);
	_txtDay->setText("1st");
	_txtDay->setAlign(ALIGN_CENTER);

	_txtMonth->setSmall();
	_txtMonth->setColor(Palette::blockOffset(15)+4);
	_txtMonth->setText("Jan");
	_txtMonth->setAlign(ALIGN_CENTER);

	_txtYear->setSmall();
	_txtYear->setColor(Palette::blockOffset(15)+4);
	_txtYear->setText("1999");
	_txtYear->setAlign(ALIGN_CENTER);
}

GeoscapeState::~GeoscapeState()
{
	State::~State();
}

void GeoscapeState::think()
{
	_globe->rotate(_rotLon, _rotLat);
}

void GeoscapeState::globeClick(SDL_Event *ev, int scale)
{
	double pos[2];
	_globe->cartToPolar(ev->button.x / scale, ev->button.y / scale, &pos[0], &pos[1]);
	
	if (ev->button.button == SDL_BUTTON_LEFT)
	{
		_globe->test(pos[0], pos[1]);
	}
	else if (ev->button.button == SDL_BUTTON_RIGHT)
	{
		_globe->center(pos[0], pos[1]);
	}
}

void GeoscapeState::btnInterceptClick(SDL_Event *ev, int scale)
{
	_game->setState(new InterceptState(_game));
}

void GeoscapeState::btnBasesClick(SDL_Event *ev, int scale)
{

}

void GeoscapeState::btnGraphsClick(SDL_Event *ev, int scale)
{

}

void GeoscapeState::btnUfopaediaClick(SDL_Event *ev, int scale)
{

}

void GeoscapeState::btnOptionsClick(SDL_Event *ev, int scale)
{
	_game->setState(new MainMenuState(_game));
}

void GeoscapeState::btnFundingClick(SDL_Event *ev, int scale)
{
	_game->setState(new FundingState(_game));
}

void GeoscapeState::btnRotateLeftPress(SDL_Event *ev, int scale)
{
	_rotLon += LONGITUDE_SPEED;
}

void GeoscapeState::btnRotateLeftRelease(SDL_Event *ev, int scale)
{
	_rotLon -= LONGITUDE_SPEED;
}

void GeoscapeState::btnRotateRightPress(SDL_Event *ev, int scale)
{
	_rotLon += -LONGITUDE_SPEED;
}

void GeoscapeState::btnRotateRightRelease(SDL_Event *ev, int scale)
{
	_rotLon -= -LONGITUDE_SPEED;
}

void GeoscapeState::btnRotateUpPress(SDL_Event *ev, int scale)
{
	_rotLat += LATITUDE_SPEED;
}

void GeoscapeState::btnRotateUpRelease(SDL_Event *ev, int scale)
{
	_rotLat -= LATITUDE_SPEED;
}

void GeoscapeState::btnRotateDownPress(SDL_Event *ev, int scale)
{
	_rotLat += -LATITUDE_SPEED;
}

void GeoscapeState::btnRotateDownRelease(SDL_Event *ev, int scale)
{
	_rotLat -= -LATITUDE_SPEED;
}

void GeoscapeState::btnZoomInClick(SDL_Event *ev, int scale)
{
	_globe->zoom(0.3);
}

void GeoscapeState::btnZoomOutClick(SDL_Event *ev, int scale)
{
	_globe->zoom(-0.3);
}