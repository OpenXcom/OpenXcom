#include "MiniMapState.h"
#include <iostream>
#include "../Engine/Game.h"
#include "../Engine/InteractiveSurface.h"
#include "../Engine/SurfaceSet.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Palette.h"
#include "../Engine/CrossPlatform.h"
#include "../Savegame/SavedGame.h"
#include "../Ruleset/MapDataSet.h"
#include "../Interface/Text.h"
#include "MiniMapView.h"
#include <sstream>

namespace OpenXcom
{
MiniMapState::MiniMapState (Game * game, Map * map) : State (game), _map (map), _displayedLevel(0)
{
	_surface = new InteractiveSurface(320, 200);
	_surface->loadSpk(CrossPlatform::getDataFile("UFOGRAPH/SCANBORD.PCK"));
	_miniMapView = new MiniMapView(224, 150, 46, 15, game, map);
	InteractiveSurface * btnLvlUp = new InteractiveSurface(18, 20, 24, 62);
	InteractiveSurface * btnLvlDwn = new InteractiveSurface(18, 20, 24, 88);
	InteractiveSurface * btnOk = new InteractiveSurface(32, 32, 275, 145);
	_txtLevel = new Text (20, 25, 281, 75);
	add(_surface);
	add(_miniMapView);
	add(btnLvlUp);
	add(btnLvlDwn);
	add(btnOk);
	add(_txtLevel);
	btnLvlUp->onMouseClick((ActionHandler)&MiniMapState::btnLevelUpClick);
	btnLvlDwn->onMouseClick((ActionHandler)&MiniMapState::btnLevelDownClick);
	btnOk->onMouseClick((ActionHandler)&MiniMapState::btnOkClick);
	_txtLevel->setBig();
	_txtLevel->setColor(Palette::blockOffset(4));
	std::wstringstream s;
	s << _miniMapView->getDisplayedLevel ();
	_txtLevel->setText(s.str());	
}

void MiniMapState::btnOkClick (Action * action)
{
	_game->popState();
}

void MiniMapState::btnLevelUpClick (Action * action)
{
	_miniMapView->up ();
	std::wstringstream s;
	s << _miniMapView->getDisplayedLevel ();
	_txtLevel->setText(s.str());	
}

void MiniMapState::btnLevelDownClick (Action * action)
{
	_miniMapView->down ();
	std::wstringstream s;
	s << _miniMapView->getDisplayedLevel ();
	_txtLevel->setText(s.str());
}
}
