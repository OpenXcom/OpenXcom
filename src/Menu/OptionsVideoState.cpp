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
#include "OptionsVideoState.h"
#include "../Engine/Game.h"
#include "../Resource/ResourcePack.h"
#include "../Engine/Language.h"
#include "../Engine/Palette.h"
#include "../Interface/TextButton.h"
#include "../Engine/Action.h"
#include "../Interface/Window.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Interface/ToggleTextButton.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/CrossPlatform.h"
#include "../Engine/Logger.h"
#include "../Interface/ComboBox.h"

namespace OpenXcom
{

const std::string OptionsVideoState::GL_EXT = "OpenGL.shader";
const std::string OptionsVideoState::GL_FOLDER = "Shaders/";
const std::string OptionsVideoState::GL_STRING = "*";

/**
 * Initializes all the elements in the Video Options screen.
 * @param game Pointer to the core game.
 * @param origin Game section that originated this state.
 */
OptionsVideoState::OptionsVideoState(Game *game, OptionsOrigin origin) : OptionsBaseState(game, origin)
{
	setCategory(_btnVideo);

	// Create objects
	_displaySurface = new InteractiveSurface(110, 32, 94, 18);
	_txtDisplayResolution = new Text(110, 9, 94, 8);
	_txtDisplayWidth = new TextEdit(40, 17, 94, 26);
	_txtDisplayX = new Text(16, 17, 132, 26);
	_txtDisplayHeight = new TextEdit(40, 17, 144, 26);
	_btnDisplayResolutionUp = new ArrowButton(ARROW_BIG_UP, 14, 14, 186, 18);
	_btnDisplayResolutionDown = new ArrowButton(ARROW_BIG_DOWN, 14, 14, 186, 36);

	_gameSurface = new InteractiveSurface(110, 32, 210, 18);
	_txtGameResolution = new Text(110, 9, 210, 8);
	_txtGameSize = new Text(80, 17, 210, 26);
	_btnGameResolutionUp = new ArrowButton(ARROW_BIG_UP, 14, 14, 296, 18);
	_btnGameResolutionDown = new ArrowButton(ARROW_BIG_DOWN, 14, 14, 296, 36);

	_txtLanguage = new Text(110, 9, 94, 52);
	_cbxLanguage = new ComboBox(this, 100, 16, 94, 62);

	_txtFilter = new Text(110, 9, 210, 52);
	_cbxFilter = new ComboBox(this, 100, 16, 210, 62);

	_txtMode = new Text(110, 9, 94, 82);
	_btnWindowed = new TextButton(100, 16, 94, 92);
	_btnFullscreen = new TextButton(100, 16, 94, 110);
	_btnBorderless = new TextButton(100, 16, 94, 128);

	_txtOptions = new Text(110, 9, 210, 82);
	_btnLetterbox = new ToggleTextButton(100, 16, 210, 92);
	_btnResize = new ToggleTextButton(100, 16, 210, 110);
	_btnLockMouse = new ToggleTextButton(100, 16, 210, 128);

	/* Get available fullscreen modes */
	_res = SDL_ListModes(NULL, SDL_FULLSCREEN);
	if (_res > (SDL_Rect**)0)
	{
		int i;
		_resCurrent = -1;
		for (i = 0; _res[i]; ++i)
		{
			if (_resCurrent == -1 &&
				((_res[i]->w == Options::displayWidth && _res[i]->h <= Options::displayHeight) || _res[i]->w < Options::displayWidth))
			{
				_resCurrent = i;
			}
		}
		_resAmount = i;
	}
	else
	{
		_resCurrent = -1;
		_resAmount = 0;
		_btnDisplayResolutionDown->setVisible(false);
		_btnDisplayResolutionUp->setVisible(false);
		Log(LOG_WARNING) << "Couldn't get display resolutions";
	}

	_gameRes.push_back(tr("STR_ORIGINAL"));
	_gameRes.push_back(L"1x");
	_gameRes.push_back(L"2x");

	if (Options::baseXResolution == Screen::ORIGINAL_WIDTH && Options::baseYResolution == Screen::ORIGINAL_HEIGHT)
		_gameCurrent = 0;
	else if (Options::baseXResolution == Options::displayWidth && Options::baseYResolution == Options::displayHeight)
		_gameCurrent = 1;
	else if (Options::baseXResolution == Options::displayWidth/2 && Options::baseYResolution == Options::displayHeight/2)
		_gameCurrent = 2;
	else
		_gameCurrent = -1;
		
	if (Options::fullscreen)
		_displayMode = _btnFullscreen;
	else if (Options::borderless)
		_displayMode = _btnBorderless;
	else
		_displayMode = _btnWindowed;

	add(_displaySurface);
	add(_txtDisplayResolution);
	add(_txtDisplayWidth);
	add(_txtDisplayX);
	add(_txtDisplayHeight);
	add(_btnDisplayResolutionUp);
	add(_btnDisplayResolutionDown);

	add(_gameSurface);
	add(_txtGameResolution);
	add(_txtGameSize);
	add(_btnGameResolutionUp);
	add(_btnGameResolutionDown);
	
	add(_txtLanguage);	
	add(_txtFilter);

	add(_txtMode);
	add(_btnWindowed);
	add(_btnFullscreen);
	add(_btnBorderless);

	add(_txtOptions);
	add(_btnLetterbox);
	add(_btnResize);
	add(_btnLockMouse);

    add(_cbxLanguage);
    add(_cbxFilter);

	centerAllSurfaces();

	// Set up objects
	_txtDisplayResolution->setColor(Palette::blockOffset(8)+10);
	_txtDisplayResolution->setText(tr("STR_DISPLAY_RESOLUTION"));

	_displaySurface->setTooltip("STR_DISPLAY_RESOLUTION_DESC");
	_displaySurface->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_displaySurface->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_txtDisplayWidth->setColor(Palette::blockOffset(15)-1);
	_txtDisplayWidth->setAlign(ALIGN_CENTER);
	_txtDisplayWidth->setBig();
	_txtDisplayWidth->setNumerical(true);
	_txtDisplayWidth->onChange((ActionHandler)&OptionsVideoState::txtDisplayWidthChange);
	_txtDisplayWidth->onMouseClick((ActionHandler)&OptionsVideoState::txtDisplayWidthClick);

	_txtDisplayX->setColor(Palette::blockOffset(15)-1);
	_txtDisplayX->setAlign(ALIGN_CENTER);
	_txtDisplayX->setBig();
	_txtDisplayX->setText(L"x");

	_txtDisplayHeight->setColor(Palette::blockOffset(15)-1);
	_txtDisplayHeight->setAlign(ALIGN_CENTER);
	_txtDisplayHeight->setBig();
	_txtDisplayHeight->setNumerical(true);
	_txtDisplayHeight->onChange((ActionHandler)&OptionsVideoState::txtDisplayHeightChange);
	_txtDisplayHeight->onMouseClick((ActionHandler)&OptionsVideoState::txtDisplayHeightClick);

	std::wostringstream ssW, ssH;
	ssW << Options::displayWidth;
	ssH << Options::displayHeight;
	_txtDisplayWidth->setText(ssW.str());
	_txtDisplayHeight->setText(ssH.str());

	_btnDisplayResolutionUp->setColor(Palette::blockOffset(15)-1);
	_btnDisplayResolutionUp->onMouseClick((ActionHandler)&OptionsVideoState::btnDisplayResolutionUpClick);

	_btnDisplayResolutionDown->setColor(Palette::blockOffset(15)-1);
	_btnDisplayResolutionDown->onMouseClick((ActionHandler)&OptionsVideoState::btnDisplayResolutionDownClick);

	_txtGameResolution->setColor(Palette::blockOffset(8)+10);
	_txtGameResolution->setText(tr("STR_GAME_RESOLUTION"));
	
	_gameSurface->setTooltip("STR_GAME_RESOLUTION_DESC");
	_gameSurface->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_gameSurface->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_txtGameSize->setColor(Palette::blockOffset(15)-1);
	_txtGameSize->setAlign(ALIGN_CENTER);
	_txtGameSize->setBig();
	updateGameResolution();

	_btnGameResolutionUp->setColor(Palette::blockOffset(15)-1);
	_btnGameResolutionUp->onMouseClick((ActionHandler)&OptionsVideoState::btnGameResolutionUpClick);

	_btnGameResolutionDown->setColor(Palette::blockOffset(15)-1);
	_btnGameResolutionDown->onMouseClick((ActionHandler)&OptionsVideoState::btnGameResolutionDownClick);

	_txtMode->setColor(Palette::blockOffset(8)+10);
	_txtMode->setText(tr("STR_DISPLAY_MODE"));

	_btnWindowed->setColor(Palette::blockOffset(15)-1);
	_btnWindowed->setText(tr("STR_WINDOWED"));
	_btnWindowed->setGroup(&_displayMode);
	_btnWindowed->onMouseClick((ActionHandler)&OptionsVideoState::btnModeClick);
	_btnWindowed->setTooltip("STR_WINDOWED_DESC");
	_btnWindowed->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnWindowed->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_btnFullscreen->setColor(Palette::blockOffset(15)-1);
	_btnFullscreen->setText(tr("STR_FULLSCREEN"));
	_btnFullscreen->setGroup(&_displayMode);
	_btnFullscreen->onMouseClick((ActionHandler)&OptionsVideoState::btnModeClick);
	_btnFullscreen->setTooltip("STR_FULLSCREEN_DESC");
	_btnFullscreen->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnFullscreen->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_btnBorderless->setColor(Palette::blockOffset(15)-1);
	_btnBorderless->setText(tr("STR_BORDERLESS"));
	_btnBorderless->setGroup(&_displayMode);
	_btnBorderless->onMouseClick((ActionHandler)&OptionsVideoState::btnModeClick);
	_btnBorderless->setTooltip("STR_BORDERLESS_DESC");
	_btnBorderless->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnBorderless->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_txtOptions->setColor(Palette::blockOffset(8)+10);
	_txtOptions->setText(tr("STR_DISPLAY_OPTIONS"));

	_btnLetterbox->setColor(Palette::blockOffset(15)-1);
	_btnLetterbox->setText(tr("STR_LETTERBOXED"));
	_btnLetterbox->setPressed(Options::keepAspectRatio);
	_btnLetterbox->onMouseClick((ActionHandler)&OptionsVideoState::btnLetterboxClick);
	_btnLetterbox->setTooltip("STR_LETTERBOXED_DESC");
	_btnLetterbox->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnLetterbox->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_btnResize->setColor(Palette::blockOffset(15)-1);
	_btnResize->setText(tr("STR_RESIZABLE"));
	_btnResize->setPressed(Options::allowResize);
	_btnResize->onMouseClick((ActionHandler)&OptionsVideoState::btnResizeClick);
	_btnResize->setTooltip("STR_RESIZABLE_DESC");
	_btnResize->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnResize->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_btnLockMouse->setColor(Palette::blockOffset(15)-1);
	_btnLockMouse->setText(tr("STR_LOCK_MOUSE"));
	_btnLockMouse->setPressed(Options::captureMouse);
	_btnLockMouse->onMouseClick((ActionHandler)&OptionsVideoState::btnLockMouseClick);
	_btnLockMouse->setTooltip("STR_LOCK_MOUSE_DESC");
	_btnLockMouse->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnLockMouse->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	
	_txtLanguage->setColor(Palette::blockOffset(8)+10);
	_txtLanguage->setText(tr("STR_DISPLAY_LANGUAGE"));
	
	std::vector<std::wstring> names;
	Language::getList(_langs, names);
    _cbxLanguage->setColor(Palette::blockOffset(15)-1);
	_cbxLanguage->setOptions(names);
	for (size_t i = 0; i < names.size(); ++i)
	{
		if (_langs[i] == Options::language)
		{
			_cbxLanguage->setSelected(i);
			break;
		}
	}
    _cbxLanguage->onChange((ActionHandler)&OptionsVideoState::cbxLanguageChange);
	_cbxLanguage->setTooltip("STR_DISPLAY_LANGUAGE_DESC");
	_cbxLanguage->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_cbxLanguage->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	std::vector<std::wstring> filterNames;
	filterNames.push_back(L"-");
	filterNames.push_back(L"Scale");
	filterNames.push_back(L"HQX");
	_filters.push_back("");
	_filters.push_back("");
	_filters.push_back("");
	
#ifndef __NO_OPENGL
	std::vector<std::string> filters = CrossPlatform::getFolderContents(CrossPlatform::getDataFolder(GL_FOLDER), GL_EXT);
	for (std::vector<std::string>::iterator i = filters.begin(); i != filters.end(); ++i)
	{
		std::string file = (*i);
		std::string path = GL_FOLDER + file;
		std::string name = file.substr(0, file.length() - GL_EXT.length() - 1) + GL_STRING;
		filterNames.push_back(Language::fsToWstr(name));
		_filters.push_back(path);
	}
#endif
	
	int selFilter = 0;
	if (Screen::isOpenGLEnabled())
	{
#ifndef __NO_OPENGL
		std::string path = Options::useOpenGLShader;
		for (size_t i = 0; i < _filters.size(); ++i)
		{
			if (_filters[i] == path)
			{
				selFilter = i;
			}
		}
#endif
	}
	else if (Options::useScaleFilter)
	{
		selFilter = 1;
	}
	else if (Options::useHQXFilter)
	{
		selFilter = 2;
	}

	_txtFilter->setColor(Palette::blockOffset(8)+10);
	_txtFilter->setText(tr("STR_DISPLAY_FILTER"));

    _cbxFilter->setColor(Palette::blockOffset(15)-1);
	_cbxFilter->setOptions(filterNames);
	_cbxFilter->setSelected(selFilter);
	_cbxFilter->onChange((ActionHandler)&OptionsVideoState::cbxFilterChange);
	_cbxFilter->setTooltip("STR_DISPLAY_FILTER_DESC");
	_cbxFilter->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_cbxFilter->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	// Save old display options for safekeeping
	Options::newDisplayWidth = Options::displayWidth;
	Options::newDisplayHeight = Options::displayHeight;
	Options::newOpenGL = Options::useOpenGL;
	Options::newScaleFilter = Options::useScaleFilter;
	Options::newHQXFilter = Options::useHQXFilter;
	Options::newOpenGLShader = Options::useOpenGLShader;
}

/**
 *
 */
OptionsVideoState::~OptionsVideoState()
{

}

/**
 * Selects a bigger display resolution.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnDisplayResolutionUpClick(Action *)
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
	updateDisplayResolution();
}

/**
 * Selects a smaller display resolution.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnDisplayResolutionDownClick(Action *)
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
	updateDisplayResolution();
}

/**
 * Updates the display resolution based on the selection.
 */
void OptionsVideoState::updateDisplayResolution()
{	
	std::wostringstream ssW, ssH;
	ssW << (int)_res[_resCurrent]->w;
	ssH << (int)_res[_resCurrent]->h;
	_txtDisplayWidth->setText(ssW.str());
	_txtDisplayHeight->setText(ssH.str());
	
	Options::newDisplayWidth = _res[_resCurrent]->w;
	Options::newDisplayHeight = _res[_resCurrent]->h;
}

/**
 * Selects a bigger game resolution.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnGameResolutionUpClick(Action *)
{
	if (_gameCurrent == _gameRes.size() - 1)
		_gameCurrent = 0;
	else
		_gameCurrent++;
	updateGameResolution();
}

/**
 * Selects a smaller game resolution.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnGameResolutionDownClick(Action *)
{
	if (_gameCurrent == 0)
		_gameCurrent = _gameRes.size() - 1;
	else
		_gameCurrent--;
	updateGameResolution();
}

/**
 * Updates the game resolution based on the selection.
 */
void OptionsVideoState::updateGameResolution()
{
	switch (_gameCurrent)
	{
	case 0:
		Options::baseXResolution = Screen::ORIGINAL_WIDTH;
		Options::baseYResolution = Screen::ORIGINAL_HEIGHT;
		break;
	case 1:
		Options::baseXResolution = Options::displayWidth;
		Options::baseYResolution = Options::displayHeight;
		break;
	case 2:
		Options::baseXResolution = Options::displayWidth / 2;
		Options::baseYResolution = Options::displayHeight / 2;
		break;
	default:
		_txtGameSize->setText(L"-");
		break;
	}
	if (_gameCurrent >= 0 && _gameCurrent < _gameRes.size())
	{
		_txtGameSize->setText(_gameRes[_gameCurrent]);
	}
}

/**
 * Unselects the other display size.
 * @param action Pointer to an action.
 */
void OptionsVideoState::txtDisplayWidthClick(Action *)
{
	_txtDisplayHeight->setFocus(false);
}

/**
 * Changes the Display Width option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::txtDisplayWidthChange(Action *)
{
	std::wstringstream ss;
	int width = 0;
	ss << std::dec << _txtDisplayWidth->getText();
	ss >> std::dec >> width;
	Options::newDisplayWidth = width;
}

/**
 * Unselects the other display size.
 * @param action Pointer to an action.
 */
void OptionsVideoState::txtDisplayHeightClick(Action *)
{
	_txtDisplayWidth->setFocus(false);
}

/**
 * Changes the Display Height option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::txtDisplayHeightChange(Action *)
{
	std::wstringstream ss;
	int height = 0;
	ss << std::dec << _txtDisplayHeight->getText();
	ss >> std::dec >> height;
	Options::newDisplayHeight = height;
}

/**
 * Changes the Language option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::cbxLanguageChange(Action *)
{
	Options::language = _langs[_cbxLanguage->getSelected()];
}

/**
 * Changes the Filter options.
 * @param action Pointer to an action.
 */
void OptionsVideoState::cbxFilterChange(Action *)
{
	switch (_cbxFilter->getSelected())
	{
	case 0:
		Options::newOpenGL = false;
		Options::newScaleFilter = false;
		Options::newHQXFilter = false;
		break;
	case 1:
		Options::newOpenGL = false;
		Options::newScaleFilter = true;
		Options::newHQXFilter = false;
		break;
	case 2:
		Options::newOpenGL = false;
		Options::newScaleFilter = false;
		Options::newHQXFilter = true;
		break;
	default:
		Options::newOpenGL = true;
		Options::newScaleFilter = false;
		Options::newHQXFilter = false;
		Options::newOpenGLShader = _filters[_cbxFilter->getSelected()];
		break;
	}
}

/**
 * Changes the Display Mode options.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnModeClick(Action *)
{
	if (_displayMode == _btnWindowed)
	{
		Options::fullscreen = false;
		Options::borderless = false;
	}
	else if (_displayMode == _btnFullscreen)
	{
		Options::fullscreen = true;
		Options::borderless = false;
	}
	else if (_displayMode == _btnBorderless)
	{
		Options::fullscreen = false;
		Options::borderless = true;
	}
}

/**
 * Changes the Letterboxing option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnLetterboxClick(Action *)
{
	Options::keepAspectRatio = _btnLetterbox->getPressed();
}

/**
 * Changes the Resizable option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnResizeClick(Action *)
{
	Options::allowResize = _btnResize->getPressed();
}

/**
 * Changes the Lock Mouse option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnLockMouseClick(Action *)
{
	Options::captureMouse = (SDL_GrabMode)_btnLockMouse->getPressed();
}

}
