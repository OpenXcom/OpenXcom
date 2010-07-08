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
#include "CraftInfoState.h"

/**
 * Initializes all the elements in the Craft Info screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param craft ID of the selected craft.
 */
CraftInfoState::CraftInfoState(Game *game, Base *base, unsigned int craft) : State(game), _base(base), _craft(craft)
{
	// Create objects
	_window = new Window(320, 200, 0, 0, POPUP_BOTH);
	_btnOk = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 64, 24, 128, 168);
	_btnW1 = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 24, 32, 24, 48);
	_btnW2 = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 24, 32, 272, 48);
	_btnCrew = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 64, 16, 24, 96);
	_btnEquip = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 64, 16, 24, 120);
	_btnArmour = new TextButton(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 64, 16, 24, 144);
	_txtCraft = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 310, 13, 5, 8);
	_txtDamage = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 70, 8, 24, 24);
	_txtFuel = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 70, 8, 232, 24);
	_txtW1Name = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 90, 8, 56, 48);
	_txtW1Ammo = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 8, 56, 64);
	_txtW1Max = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 8, 56, 72);
	_txtW2Name = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 90, 8, 204, 48);
	_txtW2Ammo = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 8, 204, 64);
	_txtW2Max = new Text(game->getResourcePack()->getFont("BIGLETS.DAT"), game->getResourcePack()->getFont("SMALLSET.DAT"), 60, 8, 204, 72);
	_sprite = new Surface(32, 32, 144, 52);
	_w1 = new Surface(15, 17, 121, 63);
	_w2 = new Surface(15, 17, 184, 63);
	_crew = new Surface(210, 16, 95, 96);
	_equip = new Surface(210, 16, 95, 121);

	// Set palette
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(3)), Palette::backPos, 16);

	add(_window);
	add(_btnOk);
	add(_btnW1);
	add(_btnW2);
	add(_btnCrew);
	add(_btnEquip);
	add(_btnArmour);
	add(_txtCraft);
	add(_txtDamage);
	add(_txtFuel);
	add(_txtW1Name);
	add(_txtW1Ammo);
	add(_txtW1Max);
	add(_txtW2Name);
	add(_txtW2Ammo);
	add(_txtW2Max);
	add(_sprite);
	add(_w1);
	add(_w2);
	add(_crew);
	add(_equip);

	// Set up objects
	_window->setColor(Palette::blockOffset(13)+13);
	_window->setBackground(game->getResourcePack()->getSurface("BACK14.SCR"));

	_btnOk->setColor(Palette::blockOffset(13)+13);
	_btnOk->setText(_game->getResourcePack()->getLanguage()->getString(STR_OK));
	_btnOk->onMouseClick((EventHandler)&CraftInfoState::btnOkClick);

	_btnW1->setColor(Palette::blockOffset(13)+13);
	_btnW1->setText("1");

	_btnW2->setColor(Palette::blockOffset(13)+13);
	_btnW2->setText("2");

	_btnCrew->setColor(Palette::blockOffset(13)+13);
	_btnCrew->setText(_game->getResourcePack()->getLanguage()->getString(STR_CREW));
	_btnCrew->onMouseClick((EventHandler)&CraftInfoState::btnCrewClick);

	_btnEquip->setColor(Palette::blockOffset(13)+13);
	_btnEquip->setText(_game->getResourcePack()->getLanguage()->getString(STR_EQUIPMENT));

	_btnArmour->setColor(Palette::blockOffset(13)+13);
	_btnArmour->setText(_game->getResourcePack()->getLanguage()->getString(STR_ARMOUR));

	_txtCraft->setColor(Palette::blockOffset(13)+10);
	_txtCraft->setBig();
	_txtCraft->setAlign(ALIGN_CENTER);

	_txtDamage->setColor(Palette::blockOffset(13)+10);

	_txtFuel->setColor(Palette::blockOffset(13)+10);

	_txtW1Name->setColor(Palette::blockOffset(13)+5);

	_txtW1Ammo->setColor(Palette::blockOffset(13)+10);

	_txtW1Max->setColor(Palette::blockOffset(13)+10);

	_txtW2Name->setColor(Palette::blockOffset(13)+5);

	_txtW2Ammo->setColor(Palette::blockOffset(13)+10);

	_txtW2Max->setColor(Palette::blockOffset(13)+10);
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
	Craft *c = _base->getCrafts()->at(_craft);

	stringstream ss;
	ss << _game->getResourcePack()->getLanguage()->getString(c->getRules()->getType()) << "-" << c->getId();
	_txtCraft->setText(ss.str());

	SurfaceSet *texture = _game->getResourcePack()->getSurfaceSet("BASEBITS.PCK");
	texture->getFrame(c->getRules()->getSprite())->setX(0);
	texture->getFrame(c->getRules()->getSprite())->setY(0);
	texture->getFrame(c->getRules()->getSprite())->blit(_sprite);

	stringstream ss2;
	ss2 << _game->getResourcePack()->getLanguage()->getString(STR_DAMAGE) << "0%";
	_txtDamage->setText(ss2.str());

	stringstream ss3;
	ss3 << _game->getResourcePack()->getLanguage()->getString(STR_FUEL) << "0%";
	_txtFuel->setText(ss3.str());
	
	if (c->getRules()->getSoldiers() > 0)
	{
		_crew->clear();
		_equip->clear();

		texture->getFrame(38)->setY(0);
		for (int i = 0, x = 0; i < c->getSoldiers(_base->getSoldiers()); i++, x += 10)
		{
			texture->getFrame(38)->setX(x);
			texture->getFrame(38)->blit(_crew);
		}

		/*
		texture->getFrame(39)->setX(0);
		texture->getFrame(39)->setY(0);
		texture->getFrame(39)->blit(_equip);
		*/
	}
	else
	{
		_crew->setVisible(false);
		_equip->setVisible(false);
		_btnCrew->setVisible(false);
		_btnEquip->setVisible(false);
		_btnArmour->setVisible(false);
	}

	if (c->getRules()->getWeapons() > 0)
	{
		texture->getFrame(48)->setX(0);
		texture->getFrame(48)->setY(0);
		texture->getFrame(48)->blit(_w1);

		_txtW1Name->setText("STINGRAY");

		stringstream ss4;
		ss4 << _game->getResourcePack()->getLanguage()->getString(STR_AMMO) << "0";
		_txtW1Ammo->setText(ss4.str());

		stringstream ss5;
		ss5 << _game->getResourcePack()->getLanguage()->getString(STR_MAX) << "0";
		_txtW1Max->setText(ss5.str());
	}
	else
	{
		_w1->setVisible(false);
		_btnW1->setVisible(false);
		_txtW1Name->setVisible(false);
		_txtW1Ammo->setVisible(false);
		_txtW1Max->setVisible(false);
	}

	if (c->getRules()->getWeapons() > 1)
	{
		texture->getFrame(48)->setX(0);
		texture->getFrame(48)->setY(0);
		texture->getFrame(48)->blit(_w2);

		_txtW2Name->setText("STINGRAY");

		stringstream ss6;
		ss6 << _game->getResourcePack()->getLanguage()->getString(STR_AMMO) << "0";
		_txtW2Ammo->setText(ss6.str());

		stringstream ss7;
		ss7 << _game->getResourcePack()->getLanguage()->getString(STR_MAX) << "0";
		_txtW2Max->setText(ss7.str());
	}
	else
	{
		_w2->setVisible(false);
		_btnW2->setVisible(false);
		_txtW2Name->setVisible(false);
		_txtW2Ammo->setVisible(false);
		_txtW2Max->setVisible(false);
	}
}

/**
 * Returns to the previous screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void CraftInfoState::btnOkClick(SDL_Event *ev, int scale)
{
	_game->popState();
}

/**
 * Goes to the Select Squad screen.
 * @param ev Pointer to the SDL_Event.
 * @param scale Scale of the screen.
 */
void CraftInfoState::btnCrewClick(SDL_Event *ev, int scale)
{
	_game->pushState(new CraftSoldiersState(_game, _base, _craft));
}
