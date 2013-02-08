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
#include "BaseDefenseState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Ruleset/Ruleset.h"
#include "../Ruleset/RuleBaseFacility.h"
#include "../Savegame/Ufo.h"
#include "../Interface/TextList.h"
#include "GeoscapeState.h"
#include "../Engine/Action.h"
#include "../Engine/RNG.h"
#include "../Battlescape/BriefingState.h"
#include "../Battlescape/BattlescapeGenerator.h"
#include "../Engine/Sound.h"
#include "BaseDestroyedState.h"
#include <ctime>

namespace OpenXcom
{

/**
 * Initializes all the elements in the Base Defense screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base being attacked.
 * @param ufo Pointer to the attacking ufo.
 */
BaseDefenseState::BaseDefenseState(Game *game, Base *base, Ufo *ufo, GeoscapeState *state) : State(game), _state(state)
{
	_base = base;
	_action = BDA_NONE;
	_row = -1;
	_passes = 0;
	_attacks = 0;
	_thinkcycles = 0;
	_ufo = ufo;
	// Create objects
	_window = new Window(this, 320, 200, 0, 0);
	_txtTitle = new Text(300, 16, 16, 6);
	_txtInit = new Text(300, 10, 16, 24);
	_lstDefenses = new TextList(300, 130, 16, 40);
	_btnOk = new TextButton(120, 18, 100, 170);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors());
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_1")->getColors(Palette::blockOffset(14)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_txtTitle);
	add(_txtInit);
	add(_lstDefenses);

	// Set up objects
	_window->setColor(Palette::blockOffset(15)+6);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK04.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+10);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BaseDefenseState::btnOkClick);
	_btnOk->setVisible(false);

	_txtTitle->setColor(Palette::blockOffset(13)+10);
	_txtTitle->setBig();
	std::wstringstream ss;
	ss << _base->getName() << _game->getLanguage()->getString("STR_UNDER_ATTACK");
	_txtTitle->setText(ss.str());
	_txtInit->setVisible(false);

	_txtInit->setColor(Palette::blockOffset(13)+10);
	_txtInit->setText(_game->getLanguage()->getString("STR_BASE_DEFENSES_INITIATED"));

	_lstDefenses->setColor(Palette::blockOffset(13)+10);
	_lstDefenses->setColumns(3, 134, 70, 50);
	_gravShields = _base->getGravShields();
	_defenses = _base->getDefenses()->size();
	_nextEventTime = clock();
}
/**
 *
 */
BaseDefenseState::~BaseDefenseState()
{
}

void BaseDefenseState::think()
{
	if (_thinkcycles == -1 || clock() < _nextEventTime)
		return;
	
	_nextEventTime = clock() + 750;

	if (_thinkcycles == 1)
	{
		_txtInit->setVisible(true);
		++_thinkcycles;
		return;
	}

	if (_thinkcycles != -1)
	{
		++_thinkcycles;
	}

	if (_thinkcycles > 1)
	{
		if (_action == BDA_DESTROY)
		{
			_lstDefenses->addRow(2, _game->getLanguage()->getString("STR_UFO_DESTROYED").c_str()," "," ");
			_game->getResourcePack()->getSound("GEO.CAT", 11)->play();
			_action = BDA_END;
			return;
		}

		if (_action == BDA_END)
		{
			_btnOk->setVisible(true);
			_thinkcycles = -1;
			return;
		}

		if (_attacks == _defenses && _passes == _gravShields)
		{
			_action = BDA_END;
			return;
		}
		else if (_attacks == _defenses && _passes < _gravShields)
		{
			_lstDefenses->addRow(3, _game->getLanguage()->getString("STR_GRAV_SHIELD_REPELS_UFO").c_str()," "," ");
			++_row;
			++_passes;
			_attacks = 0;
			return;
		}
		
	

		BaseFacility* def = _base->getDefenses()->at(_attacks);

		if (_action == BDA_NONE)
		{
			_lstDefenses->addRow(3, _game->getLanguage()->getString((def)->getRules()->getType()).c_str()," "," ");
			++_row;
			_action = BDA_FIRE;
			return;
		}

		if (_action == BDA_FIRE)
		{
			_lstDefenses->setCellText(_row, 1, _game->getLanguage()->getString("STR_FIRING").c_str());
			_game->getResourcePack()->getSound("GEO.CAT", (def)->getRules()->getFireSound())->play();
			_action = BDA_RESOLVE;
			return;
		}

		if (_action == BDA_RESOLVE)
		{
			if (RNG::generate(0, 100) > (def)->getRules()->getHitRatio())
			{
				_lstDefenses->setCellText(_row, 2, _game->getLanguage()->getString("STR_MISSED").c_str());
			}
			else
			{
				_lstDefenses->setCellText(_row, 2, _game->getLanguage()->getString("STR_HIT").c_str());
				_game->getResourcePack()->getSound("GEO.CAT", (def)->getRules()->getHitSound())->play();
				_ufo->setDamage(_ufo->getDamage() + (def)->getRules()->getDefenseValue());
			}
			if (_ufo->getStatus() == Ufo::DESTROYED)
				_action = BDA_DESTROY;
			else
				_action = BDA_NONE;
			++_attacks;
			return;
		}
	}
}
/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BaseDefenseState::btnOkClick(Action *)
{
	_game->setPalette(_game->getResourcePack()->getPalette("PALETTES.DAT_0")->getColors());
	_game->popState();
	if(_ufo->getStatus() != Ufo::DESTROYED)
	{
		if (_base->getSoldiers()->size() > 0)
		{
			size_t month = _game->getSavedGame()->getMonthsPassed();
			if (month > _game->getRuleset()->getAlienItemLevels().size()-1)
				month = _game->getRuleset()->getAlienItemLevels().size()-1;
			SavedBattleGame *bgame = new SavedBattleGame();
			_game->getSavedGame()->setBattleGame(bgame);
			bgame->setMissionType("STR_BASE_DEFENSE");
			BattlescapeGenerator bgen = BattlescapeGenerator(_game);
			bgen.setBase(_base);
			bgen.setAlienRace(_ufo->getAlienRace());
			bgen.setAlienItemlevel(_game->getRuleset()->getAlienItemLevels().at(month).at(RNG::generate(0,9)));
			bgen.run();
			_state->musicStop();
			_game->pushState(new BriefingState(_game, 0, _base));
		}
		else
		{
			_game->pushState(new BaseDestroyedState(_game, _base));
		}
	}
	// Whatever happens in the base defense, the UFO has finished its duty
	_ufo->setStatus(Ufo::DESTROYED);
}
}
