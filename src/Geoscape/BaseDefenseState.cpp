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
#include "BaseDefenseState.h"
#include "../Engine/Game.h"
#include "../Mod/ResourcePack.h"
#include "../Engine/LocalizedText.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Savegame/Base.h"
#include "../Savegame/BaseFacility.h"
#include "../Mod/RuleBaseFacility.h"
#include "../Savegame/Ufo.h"
#include "../Interface/TextList.h"
#include "GeoscapeState.h"
#include "../Engine/Action.h"
#include "../Engine/RNG.h"
#include "../Engine/Sound.h"
#include "../Engine/Timer.h"
#include "../Engine/Options.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Base Defense screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base being attacked.
 * @param ufo Pointer to the attacking ufo.
 * @param state Pointer to the Geoscape.
 */
BaseDefenseState::BaseDefenseState(Base *base, Ufo *ufo, GeoscapeState *state) : _state(state)
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
	_txtTitle = new Text(300, 17, 16, 6);
	_txtInit = new Text(300, 10, 16, 24);
	_lstDefenses = new TextList(300, 128, 16, 40);
	_btnOk = new TextButton(120, 18, 100, 170);

	// Set palette
	setInterface("baseDefense");

	add(_window, "window", "baseDefense");
	add(_btnOk, "button", "baseDefense");
	add(_txtTitle, "text", "baseDefense");
	add(_txtInit, "text", "baseDefense");
	add(_lstDefenses, "text", "baseDefense");

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK04.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&BaseDefenseState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&BaseDefenseState::btnOkClick, Options::keyOk);
	_btnOk->onKeyboardPress((ActionHandler)&BaseDefenseState::btnOkClick, Options::keyCancel);
	_btnOk->setVisible(false);

	_txtTitle->setBig();
	_txtTitle->setText(tr("STR_BASE_UNDER_ATTACK").arg(_base->getName()));
	_txtInit->setVisible(false);

	_txtInit->setText(tr("STR_BASE_DEFENSES_INITIATED"));

	_lstDefenses->setColumns(3, 134, 70, 50);
	_gravShields = _base->getGravShields();
	_defenses = _base->getDefenses()->size();
	_timer = new Timer(250);
	_timer->onTimer((StateHandler)&BaseDefenseState::nextStep);
	_timer->start();

	_explosionCount = 0;
}
/**
 *
 */
BaseDefenseState::~BaseDefenseState()
{
	delete _timer;
}

void BaseDefenseState::think()
{
	_timer->think(this, 0);
}

void BaseDefenseState::nextStep()
{
	if (_thinkcycles == -1)
		return;

	++_thinkcycles;

	if (_thinkcycles == 1)
	{
		_txtInit->setVisible(true);
		return;
	}

	if (_thinkcycles > 1)
	{
		switch (_action)
		{
		case BDA_DESTROY:
			if (!_explosionCount)
			{
				_lstDefenses->addRow(2, tr("STR_UFO_DESTROYED").c_str(),L" ",L" ");
				++_row;
				if (_row > 14)
				{
					_lstDefenses->scrollDown(true);
				}
			}
			_game->getResourcePack()->getSound("GEO.CAT", ResourcePack::UFO_EXPLODE)->play();
			if (++_explosionCount == 3)
			{
				_action = BDA_END;
			}
			return;
		case BDA_END:
			_btnOk->setVisible(true);
			_thinkcycles = -1;
			return;
		default:
			break;
		}
		if (_attacks == _defenses && _passes == _gravShields)
		{
			_action = BDA_END;
			return;
		}
		else if (_attacks == _defenses && _passes < _gravShields)
		{
			_lstDefenses->addRow(3, tr("STR_GRAV_SHIELD_REPELS_UFO").c_str(),L" ",L" ");
			if (_row > 14)
			{
				_lstDefenses->scrollDown(true);
			}
			++_row;
			++_passes;
			_attacks = 0;
			return;
		}



		BaseFacility* def = _base->getDefenses()->at(_attacks);

		switch (_action)
		{
		case  BDA_NONE:
			_lstDefenses->addRow(3, tr((def)->getRules()->getType()).c_str(),L" ",L" ");
			++_row;
			_action = BDA_FIRE;
			if (_row > 14)
			{
				_lstDefenses->scrollDown(true);
			}
			return;
		case BDA_FIRE:
			_lstDefenses->setCellText(_row, 1, tr("STR_FIRING").c_str());
			_game->getResourcePack()->getSound("GEO.CAT", (def)->getRules()->getFireSound())->play();
			_timer->setInterval(333);
			_action = BDA_RESOLVE;
			return;
		case BDA_RESOLVE:
			if (!RNG::percent((def)->getRules()->getHitRatio()))
			{
				_lstDefenses->setCellText(_row, 2, tr("STR_MISSED").c_str());
			}
			else
			{
				_lstDefenses->setCellText(_row, 2, tr("STR_HIT").c_str());
				_game->getResourcePack()->getSound("GEO.CAT", (def)->getRules()->getHitSound())->play();
				int dmg = (def)->getRules()->getDefenseValue();
				_ufo->setDamage(_ufo->getDamage() + (dmg / 2 + RNG::generate(0, dmg)));
			}
			if (_ufo->getStatus() == Ufo::DESTROYED)
				_action = BDA_DESTROY;
			else
				_action = BDA_NONE;
			++_attacks;
			_timer->setInterval(250);
			return;
		default:
			break;
		}
	}
}
/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void BaseDefenseState::btnOkClick(Action *)
{
	_timer->stop();
	_game->popState();
	if (_ufo->getStatus() != Ufo::DESTROYED)
	{
		_state->handleBaseDefense(_base, _ufo);
	}
	else
	{
		_base->cleanupDefenses(true);
	}
}
}
