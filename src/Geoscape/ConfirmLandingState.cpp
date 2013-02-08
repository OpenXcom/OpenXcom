/*
 * Copyright 2010-2012 OpenXcom Developers.
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
#include "ConfirmLandingState.h"
#include <sstream>
#include "../Engine/RNG.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Surface.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Ruleset/Ruleset.h"
#include "../Savegame/TerrorSite.h"
#include "../Savegame/AlienBase.h"
#include "../Battlescape/BriefingState.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Geoscape/GeoscapeState.h"
#include "../Engine/Exception.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Landing window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to confirm.
 * @param texture Texture of the landing site.
 * @param shade Shade of the landing site.
 */
ConfirmLandingState::ConfirmLandingState(Game *game, Craft *craft, int texture, int shade, GeoscapeState *state) : State(game), _craft(craft), _texture(texture), _shade(shade), _state(state)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 160, 20, 20, POPUP_BOTH);
	_btnYes = new TextButton(80, 20, 40, 150);
	_btnNo = new TextButton(80, 20, 136, 150);
	_txtCraft = new Text(206, 16, 25, 40);
	_txtTarget = new Text(206, 16, 25, 88);
	_txtReady = new Text(206, 32, 25, 56);
	_txtBegin = new Text(206, 16, 25, 130);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnYes);
	add(_btnNo);
	add(_txtCraft);
	add(_txtTarget);
	add(_txtReady);
	add(_txtBegin);

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK15.SCR"));

	_btnYes->setColor(Palette::blockOffset(8)+5);
	_btnYes->setText(_game->getLanguage()->getString("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmLandingState::btnYesClick);

	_btnNo->setColor(Palette::blockOffset(8)+5);
	_btnNo->setText(_game->getLanguage()->getString("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmLandingState::btnNoClick);

	_txtCraft->setColor(Palette::blockOffset(8)+10);
	_txtCraft->setBig();
	_txtCraft->setAlign(ALIGN_CENTER);
	_txtCraft->setText(_craft->getName(_game->getLanguage()));

	_txtTarget->setColor(Palette::blockOffset(8)+10);
	_txtTarget->setBig();
	_txtTarget->setAlign(ALIGN_CENTER);
	_txtTarget->setText(_craft->getDestination()->getName(_game->getLanguage()));

	_txtReady->setColor(Palette::blockOffset(8)+5);
	_txtReady->setBig();
	_txtReady->setAlign(ALIGN_CENTER);
	_txtReady->setText(_game->getLanguage()->getString("STR_READY_TO_LAND_NEAR"));

	_txtBegin->setColor(Palette::blockOffset(8)+5);
	_txtBegin->setBig();
	_txtBegin->setAlign(ALIGN_CENTER);
	_txtBegin->setText(_game->getLanguage()->getString("STR_BEGIN_MISSION"));
}

/**
 *
 */
ConfirmLandingState::~ConfirmLandingState()
{

}

/**
 * Enters the mission.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnYesClick(Action *)
{
	_game->popState();
	_state->musicStop();
	Ufo* u = dynamic_cast<Ufo*>(_craft->getDestination());
	TerrorSite* t = dynamic_cast<TerrorSite*>(_craft->getDestination());
	AlienBase* b = dynamic_cast<AlienBase*>(_craft->getDestination());
	size_t month = _game->getSavedGame()->getMonthsPassed();
	if (month > _game->getRuleset()->getAlienItemLevels().size()-1)
		month = _game->getRuleset()->getAlienItemLevels().size()-1;

	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	BattlescapeGenerator bgen = BattlescapeGenerator(_game);
	bgen.setWorldTexture(_texture);
	bgen.setWorldShade(_shade);
	bgen.setCraft(_craft);
	if (u != 0)
	{
		if(u->getStatus() == Ufo::CRASHED)
			bgame->setMissionType("STR_UFO_CRASH_RECOVERY");
		else
			bgame->setMissionType("STR_UFO_GROUND_ASSAULT");
		bgen.setUfo(u);
		bgen.setAlienRace(u->getAlienRace());
	}
	else if (t != 0)
	{
		bgame->setMissionType("STR_TERROR_MISSION");
		bgen.setTerrorSite(t);
		bgen.setAlienRace(t->getAlienRace());

		_game->pushState(new BriefingState(_game, _craft));
	}
	else if (b != 0)
	{
		bgame->setMissionType("STR_ALIEN_BASE_ASSAULT");
		bgen.setAlienBase(b);
		bgen.setAlienRace(b->getAlienRace());
	}
	else
	{
		throw Exception("No mission available!");
	}
	bgen.setAlienItemlevel(_game->getRuleset()->getAlienItemLevels().at(month).at(RNG::generate(0,9)));
	bgen.run();
	_game->pushState(new BriefingState(_game, _craft));
}

/**
 * Returns the craft to base and closes the window.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnNoClick(Action *)
{
	_craft->returnToBase();
	_game->popState();
}

}
