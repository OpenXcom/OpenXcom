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
#include "ConfirmLandingState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Mod/Mod.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextButton.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Craft.h"
#include "../Savegame/Target.h"
#include "../Savegame/Ufo.h"
#include "../Savegame/Base.h"
#include "../Savegame/MissionSite.h"
#include "../Savegame/AlienBase.h"
#include "../Battlescape/BriefingState.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Engine/Exception.h"
#include "../Engine/Options.h"
#include "../Mod/AlienDeployment.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Confirm Landing window.
 * @param game Pointer to the core game.
 * @param craft Pointer to the craft to confirm.
 * @param texture Texture of the landing site.
 * @param shade Shade of the landing site.
 */
ConfirmLandingState::ConfirmLandingState(Craft *craft, Texture *texture, int shade) : _craft(craft), _texture(texture), _shade(shade)
{
	_screen = false;

	// Create objects
	_window = new Window(this, 216, 160, 20, 20, POPUP_BOTH);
	_btnYes = new TextButton(80, 20, 40, 150);
	_btnNo = new TextButton(80, 20, 136, 150);
	_txtMessage = new Text(206, 80, 25, 40);
	_txtBegin = new Text(206, 17, 25, 130);

	// Set palette
	setInterface("confirmLanding");

	add(_window, "window", "confirmLanding");
	add(_btnYes, "button", "confirmLanding");
	add(_btnNo, "button", "confirmLanding");
	add(_txtMessage, "text", "confirmLanding");
	add(_txtBegin, "text", "confirmLanding");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getMod()->getSurface("BACK15.SCR"));

	_btnYes->setText(tr("STR_YES"));
	_btnYes->onMouseClick((ActionHandler)&ConfirmLandingState::btnYesClick);
	_btnYes->onKeyboardPress((ActionHandler)&ConfirmLandingState::btnYesClick, Options::keyOk);

	_btnNo->setText(tr("STR_NO"));
	_btnNo->onMouseClick((ActionHandler)&ConfirmLandingState::btnNoClick);
	_btnNo->onKeyboardPress((ActionHandler)&ConfirmLandingState::btnNoClick, Options::keyCancel);

	_txtMessage->setBig();
	_txtMessage->setAlign(ALIGN_CENTER);
	_txtMessage->setWordWrap(true);
	_txtMessage->setText(tr("STR_CRAFT_READY_TO_LAND_NEAR_DESTINATION")
						 .arg(_craft->getName(_game->getLanguage()))
						 .arg(_craft->getDestination()->getName(_game->getLanguage())));

	_txtBegin->setBig();
	_txtBegin->setAlign(ALIGN_CENTER);
	std::wostringstream ss;
	ss << L'\x01' << tr("STR_BEGIN_MISSION");
	_txtBegin->setText(ss.str());
}

/**
 *
 */
ConfirmLandingState::~ConfirmLandingState()
{

}

/*
 * Make sure we aren't returning to base.
 */
void ConfirmLandingState::init()
{
	State::init();
	Base* b = dynamic_cast<Base*>(_craft->getDestination());
	if (b == _craft->getBase())
		_game->popState();
}

/**
 * Enters the mission.
 * @param action Pointer to an action.
 */
void ConfirmLandingState::btnYesClick(Action *)
{
	_game->popState();
	Ufo* u = dynamic_cast<Ufo*>(_craft->getDestination());
	MissionSite* m = dynamic_cast<MissionSite*>(_craft->getDestination());
	AlienBase* b = dynamic_cast<AlienBase*>(_craft->getDestination());

	SavedBattleGame *bgame = new SavedBattleGame();
	_game->getSavedGame()->setBattleGame(bgame);
	BattlescapeGenerator bgen(_game);
	bgen.setWorldTexture(_texture);
	bgen.setWorldShade(_shade);
	bgen.setCraft(_craft);
	if (u != 0)
	{
		if (u->getStatus() == Ufo::CRASHED)
			bgame->setMissionType("STR_UFO_CRASH_RECOVERY");
		else
			bgame->setMissionType("STR_UFO_GROUND_ASSAULT");
		bgen.setUfo(u);
		bgen.setAlienRace(u->getAlienRace());
	}
	else if (m != 0)
	{
		bgame->setMissionType(m->getDeployment()->getType());
		bgen.setMissionSite(m);
		bgen.setAlienRace(m->getAlienRace());
	}
	else if (b != 0)
	{
		bgame->setMissionType(b->getDeployment()->getType());
		bgen.setAlienBase(b);
		bgen.setAlienRace(b->getAlienRace());
		bgen.setWorldTexture(0);
	}
	else
	{
		throw Exception("No mission available!");
	}
	bgen.run();
	_game->pushState(new BriefingState(_craft));
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
