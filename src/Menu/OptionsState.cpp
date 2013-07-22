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
#include "OptionsState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Engine/Action.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Interface/ArrowButton.h"
#include "LanguageState.h"
#include "MainMenuState.h"
#include "OptionsControlsState.h"
#include "AdvancedOptionsState.h"
#include "../Engine/CrossPlatform.h"

namespace OpenXcom
{

const std::string OptionsState::GL_EXT = "OpenGL.shader";
const std::string OptionsState::GL_FOLDER = "Shaders/";
const std::string OptionsState::GL_STRING = " (OpenGL)";

/**
 * Initializes all the elements in the Options window.
 * @param game Pointer to the core game.
 */
OptionsState::OptionsState(Game *game) : State(game)
{
	_wClicked = false;
	_hClicked = false;
	// Create objects
	_window = new Window(this, 320, 200, 0, 0, POPUP_BOTH);
	_txtTitle = new Text(320, 16, 0, 8);
	_btnLanguage = new TextButton(100, 16, 8, 154);
	_btnControls = new TextButton(100, 16, 110, 154);
	_btnAdvanced = new TextButton(100, 16, 212, 154);
	_btnOk = new TextButton(100, 16, 8, 176);
	_btnCancel = new TextButton(100, 16, 110, 176);
	_btnDefault = new TextButton(100, 16, 212, 176);

	_txtDisplayResolution = new Text(140, 9, 28, 32);
	_txtDisplayWidth = new TextEdit(48, 16, 30, 47);
	_txtDisplayX = new Text(16, 16, 76, 47);
	_txtDisplayHeight = new TextEdit(48, 16, 88, 47);
	_btnDisplayUp = new ArrowButton(ARROW_BIG_UP, 14, 14, 140, 36);
	_btnDisplayDown = new ArrowButton(ARROW_BIG_DOWN, 14, 14, 140, 58);

	_txtDisplayMode = new Text(140, 9, 28, 72);
	_btnDisplayWindowed = new TextButton(126, 16, 28, 82);
	_btnDisplayFullscreen = new TextButton(126, 16, 28, 100);

	_txtDisplayFilter = new Text(140, 9, 28, 122);
	_btnDisplayFilter = new TextButton(126, 16, 28, 132);

	_txtMusicVolume = new Text(140, 9, 174, 32);
	_btnMusicVolume1 = new TextButton(22, 14, 174, 42);
	_btnMusicVolume2 = new TextButton(22, 18, 198, 42);
	_btnMusicVolume3 = new TextButton(22, 22, 222, 42);
	_btnMusicVolume4 = new TextButton(22, 26, 246, 42);
	_btnMusicVolume5 = new TextButton(22, 30, 270, 42);

	_txtSoundVolume = new Text(140, 9, 174, 72);
	_btnSoundVolume1 = new TextButton(22, 14, 174, 82);
	_btnSoundVolume2 = new TextButton(22, 18, 198, 82);
	_btnSoundVolume3 = new TextButton(22, 22, 222, 82);
	_btnSoundVolume4 = new TextButton(22, 26, 246, 82);
	_btnSoundVolume5 = new TextButton(22, 30, 270, 82);

	/* Get available fullscreen/hardware modes */
	_res = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWPALETTE);
	if (_res > (SDL_Rect**)0)
	{
		int i;
		int width = Options::getInt("displayWidth");
		int height = Options::getInt("displayHeight");
		_resCurrent = -1;
		for (i = 0; _res[i]; ++i)
		{
			if (_resCurrent == -1 && ((_res[i]->w == width && _res[i]->h <= height) || _res[i]->w < width))
			{
				_resCurrent = i;
			}
		}
		_resAmount = i;
	}

	if (Options::getBool("fullscreen"))
		_displayMode = _btnDisplayFullscreen;
	else
		_displayMode = _btnDisplayWindowed;

	switch (Options::getInt("musicVolume"))
	{
	case 0: _musicVolume = _btnMusicVolume1; break;
	case 32: _musicVolume = _btnMusicVolume2; break;
	case 64: _musicVolume = _btnMusicVolume3; break;
	case 96: _musicVolume = _btnMusicVolume4; break;
	case 128: _musicVolume = _btnMusicVolume5; break;
	default: _musicVolume = 0; break;
	}

	switch (Options::getInt("soundVolume"))
	{
	case 0: _soundVolume = _btnSoundVolume1; break;
	case 32: _soundVolume = _btnSoundVolume2; break;
	case 64: _soundVolume = _btnSoundVolume3; break;
	case 96: _soundVolume = _btnSoundVolume4; break;
	case 128: _soundVolume = _btnSoundVolume5; break;
	default: _soundVolume = 0; break;
	}

	add(_window);
	add(_txtTitle);
	add(_btnOk);
	add(_btnCancel);
	add(_btnDefault);
	add(_btnLanguage);
	add(_btnControls);
	add(_btnAdvanced);

	add(_txtDisplayResolution);
	add(_txtDisplayWidth);
	add(_txtDisplayX);
	add(_txtDisplayHeight);
	add(_btnDisplayUp);
	add(_btnDisplayDown);

	add(_txtDisplayMode);
	add(_btnDisplayWindowed);
	add(_btnDisplayFullscreen);

	add(_txtDisplayFilter);
    add(_btnDisplayFilter);

	add(_txtMusicVolume);
	add(_btnMusicVolume1);
	add(_btnMusicVolume2);
	add(_btnMusicVolume3);
	add(_btnMusicVolume4);
	add(_btnMusicVolume5);

	add(_txtSoundVolume);
	add(_btnSoundVolume1);
	add(_btnSoundVolume2);
	add(_btnSoundVolume3);
	add(_btnSoundVolume4);
	add(_btnSoundVolume5);

	centerAllSurfaces();

	// Set up objects
	_window->setColor(Palette::blockOffset(8)+5);
	_window->setBackground(_game->getResourcePack()->getSurface("BACK01.SCR"));

	_txtTitle->setColor(Palette::blockOffset(8)+10);
	_txtTitle->setAlign(ALIGN_CENTER);
	_txtTitle->setBig();
	_txtTitle->setText(_game->getLanguage()->getString("STR_OPTIONS_UC"));

	_btnOk->setColor(Palette::blockOffset(8)+5);
	_btnOk->setText(_game->getLanguage()->getString("STR_OK"));
	_btnOk->onMouseClick((ActionHandler)&OptionsState::btnOkClick);
	//_btnOk->onKeyboardPress((ActionHandler)&OptionsState::btnOkClick, (SDLKey)Options::getInt("keyOk"));

	_btnCancel->setColor(Palette::blockOffset(8)+5);
	_btnCancel->setText(_game->getLanguage()->getString("STR_CANCEL"));
	_btnCancel->onMouseClick((ActionHandler)&OptionsState::btnCancelClick);
	_btnCancel->onKeyboardPress((ActionHandler)&OptionsState::btnCancelClick, (SDLKey)Options::getInt("keyCancel"));

	_btnDefault->setColor(Palette::blockOffset(8)+5);
	_btnDefault->setText(_game->getLanguage()->getString("STR_RESTORE_DEFAULTS"));
	_btnDefault->onMouseClick((ActionHandler)&OptionsState::btnDefaultClick);

	_btnLanguage->setColor(Palette::blockOffset(8)+5);
	_btnLanguage->setText(_game->getLanguage()->getString("STR_LANGUAGE"));
	_btnLanguage->onMouseClick((ActionHandler)&OptionsState::btnLanguageClick);

	_btnControls->setColor(Palette::blockOffset(8)+5);
	_btnControls->setText(_game->getLanguage()->getString("STR_CONTROLS"));
	_btnControls->onMouseClick((ActionHandler)&OptionsState::btnControlsClick);
	
	_btnAdvanced->setColor(Palette::blockOffset(8)+5);
	_btnAdvanced->setText(_game->getLanguage()->getString("STR_ADVANCED"));
	_btnAdvanced->onMouseClick((ActionHandler)&OptionsState::btnAdvancedClick);

	_txtDisplayResolution->setColor(Palette::blockOffset(8)+10);
	_txtDisplayResolution->setText(_game->getLanguage()->getString("STR_DISPLAY_RESOLUTION"));

	_txtDisplayWidth->setColor(Palette::blockOffset(15)-1);
	_txtDisplayWidth->setAlign(ALIGN_CENTER);
	_txtDisplayWidth->setBig();
	_txtDisplayWidth->setText(Language::utf8ToWstr(Options::getString("displayWidth")));
	_txtDisplayWidth->setNumerical(true);
	_txtDisplayWidth->onMouseClick((ActionHandler)&OptionsState::txtDisplayWidthClick);

	_txtDisplayX->setColor(Palette::blockOffset(15)-1);
	_txtDisplayX->setAlign(ALIGN_CENTER);
	_txtDisplayX->setBig();
	_txtDisplayX->setText(L"x");

	_txtDisplayHeight->setColor(Palette::blockOffset(15)-1);
	_txtDisplayHeight->setAlign(ALIGN_CENTER);
	_txtDisplayHeight->setBig();
	_txtDisplayHeight->setText(Language::utf8ToWstr(Options::getString("displayHeight")));
	_txtDisplayHeight->setNumerical(true);
	_txtDisplayHeight->onMouseClick((ActionHandler)&OptionsState::txtDisplayHeightClick);

	_btnDisplayUp->setColor(Palette::blockOffset(15)-1);
	_btnDisplayUp->onMouseClick((ActionHandler)&OptionsState::btnDisplayUpClick);

	_btnDisplayDown->setColor(Palette::blockOffset(15)-1);
	_btnDisplayDown->onMouseClick((ActionHandler)&OptionsState::btnDisplayDownClick);

	_txtDisplayMode->setColor(Palette::blockOffset(8)+10);
	_txtDisplayMode->setText(_game->getLanguage()->getString("STR_DISPLAY_MODE"));

	_btnDisplayWindowed->setColor(Palette::blockOffset(15)-1);
	_btnDisplayWindowed->setText(_game->getLanguage()->getString("STR_WINDOWED"));
	_btnDisplayWindowed->setGroup(&_displayMode);

	_btnDisplayFullscreen->setColor(Palette::blockOffset(15)-1);
	_btnDisplayFullscreen->setText(_game->getLanguage()->getString("STR_FULLSCREEN"));
	_btnDisplayFullscreen->setGroup(&_displayMode);
	
	_filters.push_back("-");
	_filters.push_back("Scale");
	_filters.push_back("HQX");
	_filterPaths.push_back("");
	_filterPaths.push_back("");
	_filterPaths.push_back("");

	std::vector<std::string> filters = CrossPlatform::getFolderContents(CrossPlatform::getDataFolder(GL_FOLDER), GL_EXT);
	for (std::vector<std::string>::iterator i = filters.begin(); i != filters.end(); ++i)
	{
		std::string file = (*i);
		std::string path = GL_FOLDER + file;
		std::string name = file.substr(0, file.length() - GL_EXT.length() - 1) + GL_STRING;
		_filters.push_back(name);
		_filterPaths.push_back(path);
	}
	
	_selFilter = 0;
	if (Options::getBool("useOpenGL"))
	{
		std::string path = Options::getString("useOpenGLShader");
		for (size_t i = 0; i < _filterPaths.size(); ++i)
		{
			if (_filterPaths[i] == path)
			{
				_selFilter = i;
			}
		}
	}
	else if (Options::getBool("useScaleFilter"))
	{
		_selFilter = 1;
	}
	else if (Options::getBool("useHQXFilter"))
	{
		_selFilter = 2;
	}

	_txtDisplayFilter->setColor(Palette::blockOffset(8)+10);
	_txtDisplayFilter->setText(_game->getLanguage()->getString("STR_DISPLAY_FILTER"));

    _btnDisplayFilter->setColor(Palette::blockOffset(15)-1);
    _btnDisplayFilter->setText(Language::utf8ToWstr(_filters[_selFilter]));
	_btnDisplayFilter->onMouseClick((ActionHandler)&OptionsState::btnDisplayFilterClick, 0);

	_txtMusicVolume->setColor(Palette::blockOffset(8)+10);
	_txtMusicVolume->setText(_game->getLanguage()->getString("STR_MUSIC_VOLUME"));

	_btnMusicVolume1->setColor(Palette::blockOffset(15)-1);
	_btnMusicVolume1->setText(L"0");
	_btnMusicVolume1->setGroup(&_musicVolume);

	_btnMusicVolume2->setColor(Palette::blockOffset(15)-1);
	_btnMusicVolume2->setText(L"1");
	_btnMusicVolume2->setGroup(&_musicVolume);

	_btnMusicVolume3->setColor(Palette::blockOffset(15)-1);
	_btnMusicVolume3->setText(L"2");
	_btnMusicVolume3->setGroup(&_musicVolume);

	_btnMusicVolume4->setColor(Palette::blockOffset(15)-1);
	_btnMusicVolume4->setText(L"3");
	_btnMusicVolume4->setGroup(&_musicVolume);

	_btnMusicVolume5->setColor(Palette::blockOffset(15)-1);
	_btnMusicVolume5->setText(L"4");
	_btnMusicVolume5->setGroup(&_musicVolume);


	_txtSoundVolume->setColor(Palette::blockOffset(8)+10);
	_txtSoundVolume->setText(_game->getLanguage()->getString("STR_SFX_VOLUME"));

	_btnSoundVolume1->setColor(Palette::blockOffset(15)-1);
	_btnSoundVolume1->setText(L"0");
	_btnSoundVolume1->setGroup(&_soundVolume);

	_btnSoundVolume2->setColor(Palette::blockOffset(15)-1);
	_btnSoundVolume2->setText(L"1");
	_btnSoundVolume2->setGroup(&_soundVolume);

	_btnSoundVolume3->setColor(Palette::blockOffset(15)-1);
	_btnSoundVolume3->setText(L"2");
	_btnSoundVolume3->setGroup(&_soundVolume);

	_btnSoundVolume4->setColor(Palette::blockOffset(15)-1);
	_btnSoundVolume4->setText(L"3");
	_btnSoundVolume4->setGroup(&_soundVolume);

	_btnSoundVolume5->setColor(Palette::blockOffset(15)-1);
	_btnSoundVolume5->setText(L"4");
	_btnSoundVolume5->setGroup(&_soundVolume);
}

/**
 *
 */
OptionsState::~OptionsState()
{
	
}

/**
 * Saves the options.
 * @param action Pointer to an action.
 */
void OptionsState::btnOkClick(Action *)
{
	Options::setString("displayWidth", Language::wstrToUtf8(_txtDisplayWidth->getText()));
	Options::setString("displayHeight", Language::wstrToUtf8(_txtDisplayHeight->getText()));

	if (_displayMode == _btnDisplayWindowed)
		Options::setBool("fullscreen", false);
	else if (_displayMode == _btnDisplayFullscreen)
		Options::setBool("fullscreen", true);

	if (_musicVolume == _btnMusicVolume1)
		Options::setInt("musicVolume", 0);
	else if (_musicVolume == _btnMusicVolume2)
		Options::setInt("musicVolume", 32);
	else if (_musicVolume == _btnMusicVolume3)
		Options::setInt("musicVolume", 64);
	else if (_musicVolume == _btnMusicVolume4)
		Options::setInt("musicVolume", 96);
	else if (_musicVolume == _btnMusicVolume5)
		Options::setInt("musicVolume", 128);

	if (_soundVolume == _btnSoundVolume1)
		Options::setInt("soundVolume", 0);
	else if (_soundVolume == _btnSoundVolume2)
		Options::setInt("soundVolume", 32);
	else if (_soundVolume == _btnSoundVolume3)
		Options::setInt("soundVolume", 64);
	else if (_soundVolume == _btnSoundVolume4)
		Options::setInt("soundVolume", 96);
	else if (_soundVolume == _btnSoundVolume5)
		Options::setInt("soundVolume", 128);

	switch (_selFilter)
	{
	case 0:
		Options::setBool("useOpenGL", false);
		Options::setBool("useScaleFilter", false);
		Options::setBool("useHQXFilter", false);
		break;
	case 1:
		Options::setBool("useOpenGL", false);
		Options::setBool("useScaleFilter", true);
		Options::setBool("useHQXFilter", false);
		break;
	case 2:
		Options::setBool("useOpenGL", false);
		Options::setBool("useScaleFilter", false);
		Options::setBool("useHQXFilter", true);
		break;
	default:
		Options::setBool("useOpenGL", true);
		Options::setBool("useScaleFilter", false);
		Options::setBool("useHQXFilter", false);
		Options::setString("useOpenGLShader", _filterPaths[_selFilter]);
		break;
	}

	_game->getScreen()->setResolution(Options::getInt("displayWidth"), Options::getInt("displayHeight"));
	_game->getScreen()->setFullscreen(Options::getBool("fullscreen"));
	_game->setVolume(Options::getInt("soundVolume"), Options::getInt("musicVolume"));

	Options::save();
	_game->popState();
}

/**
 * Returns to the previous screen.
 * @param action Pointer to an action.
 */
void OptionsState::btnCancelClick(Action *)
{
	_game->popState();
}

/**
 * Restores the Options to default settings.
 * @param action Pointer to an action.
 */
void OptionsState::btnDefaultClick(Action *)
{
	Options::createDefault();
	Options::setString("language", "English");
	Options::save();
	_game->getScreen()->setResolution(Options::getInt("displayWidth"), Options::getInt("displayHeight"));
	_game->getScreen()->setFullscreen(Options::getBool("fullscreen"));
	_game->setVolume(Options::getInt("soundVolume"), Options::getInt("musicVolume"));
	
	_game->popState();
}

/**
 * Shows the Language screen.
 * @param action Pointer to an action.
 */
void OptionsState::btnLanguageClick(Action *)
{
	_game->setState(new LanguageState(_game));
}

/**
 * Shows the Controls screen.
 * @param action Pointer to an action.
 */
void OptionsState::btnControlsClick(Action *)
{
	_game->pushState(new OptionsControlsState(_game));
}

/**
 * Selects a bigger resolution.
 * @param action Pointer to an action.
 */
void OptionsState::btnDisplayUpClick(Action *)
{
	if (_resAmount == 0)
		return;
	if (_resCurrent <= 0)
	{
		_resCurrent = _resAmount-1;
	}
	else
	{
		_resCurrent--;
	}
	std::wstringstream ssW, ssH;
	ssW << (int)_res[_resCurrent]->w;
	ssH << (int)_res[_resCurrent]->h;
	_txtDisplayWidth->setText(ssW.str());
	_txtDisplayHeight->setText(ssH.str());
}

/**
 * Selects a smaller resolution.
 * @param action Pointer to an action.
 */
void OptionsState::btnDisplayDownClick(Action *)
{
	if (_resAmount == 0)
		return;
	if (_resCurrent >= _resAmount-1)
	{
		_resCurrent = 0;
	}
	else
	{
		_resCurrent++;
	}
	std::wstringstream ssW, ssH;
	ssW << (int)_res[_resCurrent]->w;
	ssH << (int)_res[_resCurrent]->h;
	_txtDisplayWidth->setText(ssW.str());
	_txtDisplayHeight->setText(ssH.str());
}

void OptionsState::txtDisplayWidthClick(Action *)
{
	_wClicked = true;
	if (_hClicked)
	{
		_txtDisplayHeight->deFocus();
		_hClicked = false;
	}
}
void OptionsState::txtDisplayHeightClick(Action *)
{
	_hClicked = true;
	if (_wClicked)
	{
		_txtDisplayWidth->deFocus();
		_wClicked = false;
	}
}

void OptionsState::btnDisplayFilterClick(Action *action)
{
	int change = 0;
	if (action->getDetails()->button.button == SDL_BUTTON_LEFT)
	{
		change = 1;
	}
	else if (action->getDetails()->button.button == SDL_BUTTON_RIGHT)
	{
		change = -1;
	}
	int i = _selFilter;
	if (i + change >= (int)_filters.size())
	{
		_selFilter = 0;
	}
	else if (i + change < 0)
	{
		_selFilter = _filters.size() - 1;
	}
	else
	{
		_selFilter += change;
	}
	_btnDisplayFilter->setText(Language::utf8ToWstr(_filters[_selFilter]));
}

void OptionsState::btnAdvancedClick(Action *)
{
	_game->pushState(new AdvancedOptionsState(_game));
}

}
