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
#include "CraftInfoState.h"
#include <sstream>
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Engine/Options.h"
#include "../Interface/TextButton.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/SurfaceSet.h"
#include "../Engine/Action.h"
#include "../Savegame/Craft.h"
#include "../Ruleset/RuleCraft.h"
#include "../Savegame/CraftWeapon.h"
#include "../Ruleset/RuleCraftWeapon.h"
#include "../Savegame/Base.h"
#include "../Savegame/SavedGame.h"
#include "CraftSoldiersState.h"
#include "CraftWeaponsState.h"
#include "CraftEquipmentState.h"
#include "CraftArmorState.h"

namespace OpenXcom
{

/**
 * Initializes all the elements in the Craft Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craftId ID of the selected craft.
 */
CraftInfoState::CraftInfoState(Base *base, size_t craftId) : _base(base), _craftId(craftId)
{
	// Create objects
	if (_game->getSavedGame()->getMonthsPassed() != -1)
	{
		_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	}
	else
	{
		_window = new Window(this, 320, 200, 0, 0, POPUP_NONE);
	}
	_btnOk = new TextButton(64, 24, 128, 168);
	_btnW1 = new TextButton(24, 32, 14, 48);
	_btnW2 = new TextButton(24, 32, 282, 48);
	_btnCrew = new TextButton(64, 16, 14, 96);
	_btnEquip = new TextButton(64, 16, 14, 120);
	_btnArmor = new TextButton(64, 16, 14, 144);
	_edtCraft = new TextEdit(this, 140, 16, 80, 8);
	_txtDamage = new Text(100, 17, 14, 24);
	_txtFuel = new Text(82, 17, 228, 24);
	_txtW1Name = new Text(95, 16, 46, 48);
	_txtW1Ammo = new Text(75, 24, 46, 64);
	_txtW2Name = new Text(95, 16, 184, 48);
	_txtW2Ammo = new Text(75, 24, 204, 64);
	_sprite = new Surface(32, 40, 144, 52);
	_weapon1 = new Surface(15, 17, 121, 63);
	_weapon2 = new Surface(15, 17, 184, 63);
	_crew = new Surface(220, 18, 85, 96);
	_equip = new Surface(220, 18, 85, 121);

	// Set palette
	setInterface("craftInfo");

	add(_window, "window", "craftInfo");
	add(_btnOk, "button", "craftInfo");
	add(_btnW1, "button", "craftInfo");
	add(_btnW2, "button", "craftInfo");
	add(_btnCrew, "button", "craftInfo");
	add(_btnEquip, "button", "craftInfo");
	add(_btnArmor, "button", "craftInfo");
	add(_edtCraft, "text1", "craftInfo");
	add(_txtDamage, "text1", "craftInfo");
	add(_txtFuel, "text1", "craftInfo");
	add(_txtW1Name, "text2", "craftInfo");
	add(_txtW1Ammo, "text2", "craftInfo");
	add(_txtW2Name, "text2", "craftInfo");
	add(_txtW2Ammo, "text2", "craftInfo");
	add(_sprite);
	add(_weapon1);
	add(_weapon2);
	add(_crew);
	add(_equip);

	centerAllSurfaces();

	// Set up objects
	_window->setBackground(_game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setText(tr("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&CraftInfoState::btnOkClick);
	_btnOk->onKeyboardPress((ActionHandler)&CraftInfoState::btnOkClick, Options::keyCancel);

	_btnW1->setText(L"1");
	_btnW1->onMouseClick((ActionHandler)&CraftInfoState::btnW1Click);

	_btnW2->setText(L"2");
	_btnW2->onMouseClick((ActionHandler)&CraftInfoState::btnW2Click);

	_btnCrew->setText(tr("STR_CREW"));
	_btnCrew->onMouseClick((ActionHandler)&CraftInfoState::btnCrewClick);

	_btnEquip->setText(tr("STR_EQUIPMENT_UC"));
	_btnEquip->onMouseClick((ActionHandler)&CraftInfoState::btnEquipClick);

	_btnArmor->setText(tr("STR_ARMOR"));
	_btnArmor->onMouseClick((ActionHandler)&CraftInfoState::btnArmorClick);

	_edtCraft->setBig();
	_edtCraft->setAlign(ALIGN_CENTER);
	_edtCraft->onChange((ActionHandler)&CraftInfoState::edtCraftChange);

	_txtW1Name->setWordWrap(true);

	_txtW2Name->setWordWrap(true);
}

/**
 *
 */
CraftInfoState::~CraftInfoState()
{

}

/**
 * The craft info can change
 * after going into other screens.
 */
void CraftInfoState::init()
{
	State::init();

	_craft = _base->getCrafts()->at(_craftId);

	_edtCraft->setText(_craft->getName(_game->getLanguage()));

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(_craft->getRules()->getSprite() + 33)->setX(0);
	texture->getFrame(_craft->getRules()->getSprite() + 33)->setY(0);
	texture->getFrame(_craft->getRules()->getSprite() + 33)->blit(_sprite);

	std::wostringstream ss;
	ss << tr("STR_DAMAGE_UC_").arg(Text::formatPercentage(_craft->getDamagePercentage()));
	if (_craft->getStatus() == "STR_REPAIRS" && _craft->getDamage() > 0)
	{
		int damageHours = (int)ceil((double)_craft->getDamage() / _craft->getRules()->getRepairRate());
		ss << formatTime(damageHours);
	}
	_txtDamage->setText(ss.str());

	std::wostringstream ss2;
	ss2 << tr("STR_FUEL").arg(Text::formatPercentage(_craft->getFuelPercentage()));
	if (_craft->getStatus() == "STR_REFUELLING" && _craft->getRules()->getMaxFuel() - _craft->getFuel() > 0)
	{
		int fuelHours = (int)ceil((double)(_craft->getRules()->getMaxFuel() - _craft->getFuel()) / _craft->getRules()->getRefuelRate() / 2.0);
		ss2 << formatTime(fuelHours);
	}
	_txtFuel->setText(ss2.str());

	if (_craft->getRules()->getSoldiers() > 0)
	{
		_crew->clear();
		_equip->clear();

		Surface *frame1 = texture->getFrame(38);
		frame1->setY(0);
		for (int i = 0, x = 0; i < _craft->getNumSoldiers(); ++i, x += 10)
		{
			frame1->setX(x);
			frame1->blit(_crew);
		}

		Surface *frame2 = texture->getFrame(40);
		frame2->setY(0);
		int x = 0;
		for (int i = 0; i < _craft->getNumVehicles(); ++i, x += 10)
		{
			frame2->setX(x);
			frame2->blit(_equip);
		}
		Surface *frame3 = texture->getFrame(39);
		for (int i = 0; i < _craft->getNumEquipment(); i += 4, x += 10)
		{
			frame3->setX(x);
			frame3->blit(_equip);
		}
	}
	else
	{
		_crew->setVisible(false);
		_equip->setVisible(false);
		_btnCrew->setVisible(false);
		_btnEquip->setVisible(false);
		_btnArmor->setVisible(false);
	}

	if (_craft->getRules()->getWeapons() > 0)
	{
		CraftWeapon *w1 = _craft->getWeapons()->at(0);

		if (w1 != 0)
		{
			Surface *frame = texture->getFrame(w1->getRules()->getSprite() + 48);
			frame->setX(0);
			frame->setY(0);
			frame->blit(_weapon1);

			std::wostringstream ss;
			ss << L'\x01' << tr(w1->getRules()->getType());
			_txtW1Name->setText(ss.str());
			ss.str(L"");
			ss << tr("STR_AMMO_").arg(w1->getAmmo()) << L"\n\x01";
			ss << tr("STR_MAX").arg(w1->getRules()->getAmmoMax());
			if (_craft->getStatus() == "STR_REARMING" && w1->getAmmo() < w1->getRules()->getAmmoMax())
			{
				int rearmHours = (int)ceil((double)(w1->getRules()->getAmmoMax() - w1->getAmmo()) / w1->getRules()->getRearmRate());
				ss << formatTime(rearmHours);
			}
			_txtW1Ammo->setText(ss.str());
		}
		else
		{
			_weapon1->clear();
			_txtW1Name->setText(L"");
			_txtW1Ammo->setText(L"");
		}
	}
	else
	{
		_weapon1->setVisible(false);
		_btnW1->setVisible(false);
		_txtW1Name->setVisible(false);
		_txtW1Ammo->setVisible(false);
	}

	if (_craft->getRules()->getWeapons() > 1)
	{
		CraftWeapon *w2 = _craft->getWeapons()->at(1);

		if (w2 != 0)
		{
			Surface *frame = texture->getFrame(w2->getRules()->getSprite() + 48);
			frame->setX(0);
			frame->setY(0);
			frame->blit(_weapon2);

			std::wostringstream ss;
			ss << L'\x01' << tr(w2->getRules()->getType());
			_txtW2Name->setText(ss.str());
			ss.str(L"");
			ss << tr("STR_AMMO_").arg(w2->getAmmo()) << L"\n\x01";
			ss << tr("STR_MAX").arg(w2->getRules()->getAmmoMax());
			if (_craft->getStatus() == "STR_REARMING" && w2->getAmmo() < w2->getRules()->getAmmoMax())
			{
				int rearmHours = (int)ceil((double)(w2->getRules()->getAmmoMax() - w2->getAmmo()) / w2->getRules()->getRearmRate());
				ss << formatTime(rearmHours);
			}
			_txtW2Ammo->setText(ss.str());
		}
		else
		{
			_weapon2->clear();
			_txtW2Name->setText(L"");
			_txtW2Ammo->setText(L"");
		}
	}
	else
	{
		_weapon2->setVisible(false);
		_btnW2->setVisible(false);
		_txtW2Name->setVisible(false);
		_txtW2Ammo->setVisible(false);
	}
	_defaultName = tr("STR_CRAFTNAME").arg(tr(_craft->getRules()->getType())).arg(_craft->getId());
}

/**
 * Turns an amount of time into a
 * day/hour string.
 * @param total
 */
std::wstring CraftInfoState::formatTime(int total)
{
	std::wostringstream ss;
	int days = total / 24;
	int hours = total % 24;
	ss << L"\n(";
	if (days > 0)
	{
		ss << tr("STR_DAY", days) << L"/";
	}
	if (hours > 0)
	{
		ss << tr("STR_HOUR", hours);
	}
	ss << L")";
	return ss.str();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnOkClick(Action *)
{
	_game->popState();
}

/**
 * Goes to the Select Armament window for
 * the first weapon.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnW1Click(Action *)
{
	_game->pushState(new CraftWeaponsState(_base, _craftId, 0));
}

/**
 * Goes to the Select Armament window for
 * the second weapon.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnW2Click(Action *)
{
	_game->pushState(new CraftWeaponsState(_base, _craftId, 1));
}

/**
 * Goes to the Select Squad screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnCrewClick(Action *)
{
	_game->pushState(new CraftSoldiersState(_base, _craftId));
}

/**
 * Goes to the Select Equipment screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnEquipClick(Action *)
{
	_game->pushState(new CraftEquipmentState(_base, _craftId));
}

/**
 * Goes to the Select Armor screen.
 * @param action Pointer to an action.
 */
void CraftInfoState::btnArmorClick(Action *)
{
	_game->pushState(new CraftArmorState(_base, _craftId));
}

/**
 * Changes the Craft name.
 * @param action Pointer to an action.
 */
void CraftInfoState::edtCraftChange(Action *action)
{
	_craft->setName(_edtCraft->getText());
	if (_craft->getName(_game->getLanguage()) == _defaultName)
	{
		_craft->setName(L"");
	}
	if (action->getDetails()->key.keysym.sym == SDLK_RETURN ||
		action->getDetails()->key.keysym.sym == SDLK_KP_ENTER)
	{
		_edtCraft->setText(_craft->getName(_game->getLanguage()));
	}
}

}
