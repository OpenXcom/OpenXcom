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
#include "OptionsVideoState.h"
#include "../Engine/Language.h"
#include "../Interface/TextButton.h"
#include "../Engine/Action.h"
#include "../Interface/Text.h"
#include "../Interface/TextEdit.h"
#include "../Interface/ToggleTextButton.h"
#include "../Engine/Options.h"
#include "../Engine/Screen.h"
#include "../Interface/ArrowButton.h"
#include "../Engine/FileMap.h"
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
OptionsVideoState::OptionsVideoState(OptionsOrigin origin) : OptionsBaseState(origin)
{
	setCategory(_btnVideo);

	// Create objects
	_displaySurface = new InteractiveSurface(110, 32, 94, 18);
	_txtDisplayResolution = new Text(114, 9, 94, 8);
	_txtDisplayWidth = new TextEdit(this, 40, 17, 94, 26);
	_txtDisplayX = new Text(16, 17, 132, 26);
	_txtDisplayHeight = new TextEdit(this, 40, 17, 144, 26);
	_btnDisplayResolutionUp = new ArrowButton(ARROW_BIG_UP, 14, 14, 186, 18);
	_btnDisplayResolutionDown = new ArrowButton(ARROW_BIG_DOWN, 14, 14, 186, 36);

	_txtLanguage = new Text(114, 9, 94, 52);
	_cbxLanguage = new ComboBox(this, 104, 16, 94, 62);

	_txtFilter = new Text(114, 9, 206, 52);
	_cbxFilter = new ComboBox(this, 104, 16, 206, 62);

	_txtMode = new Text(114, 9, 206, 22);
	_cbxDisplayMode = new ComboBox(this, 104, 16, 206, 32);
	
	_txtGeoScale = new Text(114, 9, 94, 82);
	_cbxGeoScale = new ComboBox(this, 104, 16, 94, 92);
	
	_txtBattleScale = new Text(114, 9, 94, 112);
	_cbxBattleScale = new ComboBox(this, 104, 16, 94, 122);

	_txtOptions = new Text(114, 9, 206, 82);
	_btnLetterbox = new ToggleTextButton(104, 16, 206, 92);
	_btnLockMouse = new ToggleTextButton(104, 16, 206, 110);

	// Get available fullscreen modes
	_res = SDL_ListModes(NULL, SDL_FULLSCREEN);
	if (_res != (SDL_Rect**)-1 && _res != (SDL_Rect**)0)
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

	add(_displaySurface);
	add(_txtDisplayResolution, "text", "videoMenu");
	add(_txtDisplayWidth, "resolution", "videoMenu");
	add(_txtDisplayX, "resolution", "videoMenu");
	add(_txtDisplayHeight, "resolution", "videoMenu");
	add(_btnDisplayResolutionUp, "button", "videoMenu");
	add(_btnDisplayResolutionDown, "button", "videoMenu");

	add(_txtLanguage, "text", "videoMenu");
	add(_txtFilter, "text", "videoMenu");

	add(_txtMode, "text", "videoMenu");

	add(_txtOptions, "text", "videoMenu");
	add(_btnLetterbox, "button", "videoMenu");
	add(_btnLockMouse, "button", "videoMenu");


	add(_cbxFilter, "button", "videoMenu");
	add(_cbxDisplayMode, "button", "videoMenu");
	
	add(_txtBattleScale, "text", "videoMenu");
	add(_cbxBattleScale, "button", "videoMenu");

	add(_txtGeoScale, "text", "videoMenu");
	add(_cbxGeoScale, "button", "videoMenu");

	add(_cbxLanguage, "button", "videoMenu");
	centerAllSurfaces();

	// Set up objects
	_txtDisplayResolution->setText(tr("STR_DISPLAY_RESOLUTION"));

	_displaySurface->setTooltip("STR_DISPLAY_RESOLUTION_DESC");
	_displaySurface->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_displaySurface->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

	_txtDisplayWidth->setAlign(ALIGN_CENTER);
	_txtDisplayWidth->setBig();
	_txtDisplayWidth->setNumerical(true);
	_txtDisplayWidth->onChange((ActionHandler)&OptionsVideoState::txtDisplayWidthChange);

	_txtDisplayX->setAlign(ALIGN_CENTER);
	_txtDisplayX->setBig();
	_txtDisplayX->setText(L"x");

	_txtDisplayHeight->setAlign(ALIGN_CENTER);
	_txtDisplayHeight->setBig();
	_txtDisplayHeight->setNumerical(true);
	_txtDisplayHeight->onChange((ActionHandler)&OptionsVideoState::txtDisplayHeightChange);

	std::wostringstream ssW, ssH;
	ssW << Options::displayWidth;
	ssH << Options::displayHeight;
	_txtDisplayWidth->setText(ssW.str());
	_txtDisplayHeight->setText(ssH.str());

	_btnDisplayResolutionUp->onMouseClick((ActionHandler)&OptionsVideoState::btnDisplayResolutionUpClick);
	_btnDisplayResolutionDown->onMouseClick((ActionHandler)&OptionsVideoState::btnDisplayResolutionDownClick);

	_txtMode->setText(tr("STR_DISPLAY_MODE"));

	_txtOptions->setText(tr("STR_DISPLAY_OPTIONS"));

	_btnLetterbox->setText(tr("STR_LETTERBOXED"));
	_btnLetterbox->setPressed(Options::keepAspectRatio);
	_btnLetterbox->onMouseClick((ActionHandler)&OptionsVideoState::btnLetterboxClick);
	_btnLetterbox->setTooltip("STR_LETTERBOXED_DESC");
	_btnLetterbox->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnLetterbox->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	
	_btnLockMouse->setText(tr("STR_LOCK_MOUSE"));
	_btnLockMouse->setPressed(Options::captureMouse == SDL_GRAB_ON);
	_btnLockMouse->onMouseClick((ActionHandler)&OptionsVideoState::btnLockMouseClick);
	_btnLockMouse->setTooltip("STR_LOCK_MOUSE_DESC");
	_btnLockMouse->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_btnLockMouse->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	
	_txtLanguage->setText(tr("STR_DISPLAY_LANGUAGE"));
	
	std::vector<std::wstring> names;
	Language::getList(_langs, names);
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
	filterNames.push_back(tr("STR_DISABLED"));
	filterNames.push_back(L"Scale");
	filterNames.push_back(L"HQx");
	filterNames.push_back(L"xBRZ");
	_filters.push_back("");
	_filters.push_back("");
	_filters.push_back("");
	_filters.push_back("");
	
#ifndef __NO_OPENGL
	std::set<std::string> filters = FileMap::filterFiles(FileMap::getVFolderContents(GL_FOLDER), GL_EXT);
	for (std::set<std::string>::iterator i = filters.begin(); i != filters.end(); ++i)
	{
		std::string file = (*i);
		std::string path = GL_FOLDER + file;
		std::string name = file.substr(0, file.length() - GL_EXT.length() - 1) + GL_STRING;
		filterNames.push_back(Language::fsToWstr(name));
		_filters.push_back(path);
	}
#endif
	
	size_t selFilter = 0;
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
	else if (Options::useXBRZFilter)
	{
		selFilter = 3;
	}

	_txtFilter->setText(tr("STR_DISPLAY_FILTER"));

	_cbxFilter->setOptions(filterNames);
	_cbxFilter->setSelected(selFilter);
	_cbxFilter->onChange((ActionHandler)&OptionsVideoState::cbxFilterChange);
	_cbxFilter->setTooltip("STR_DISPLAY_FILTER_DESC");
	_cbxFilter->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_cbxFilter->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	
	
	std::vector<std::string> displayModes;
	displayModes.push_back("STR_WINDOWED");
	displayModes.push_back("STR_FULLSCREEN");
	displayModes.push_back("STR_BORDERLESS");
	displayModes.push_back("STR_RESIZABLE");

	int displayMode = 0;
	if (Options::fullscreen)
	{
		displayMode = 1;
	}
	else if (Options::borderless)
	{
		displayMode = 2;
	}
	else if (Options::allowResize)
	{
		displayMode = 3;
	}

	_cbxDisplayMode->setOptions(displayModes);
	_cbxDisplayMode->setSelected(displayMode);
	_cbxDisplayMode->onChange((ActionHandler)&OptionsVideoState::updateDisplayMode);
	_cbxDisplayMode->setTooltip("STR_DISPLAY_MODE_DESC");
	_cbxDisplayMode->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_cbxDisplayMode->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	
	_txtGeoScale->setText(tr("STR_GEOSCAPE_SCALE"));
	
	std::vector<std::string> scales;
	scales.push_back("STR_ORIGINAL");
	scales.push_back("STR_1_5X");
	scales.push_back("STR_2X");
	scales.push_back("STR_THIRD_DISPLAY");
	scales.push_back("STR_HALF_DISPLAY");
	scales.push_back("STR_FULL_DISPLAY");

	_cbxGeoScale->setOptions(scales);
	_cbxGeoScale->setSelected(Options::geoscapeScale);
	_cbxGeoScale->onChange((ActionHandler)&OptionsVideoState::updateGeoscapeScale);
	_cbxGeoScale->setTooltip("STR_GEOSCAPESCALE_SCALE_DESC");
	_cbxGeoScale->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_cbxGeoScale->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);
	
	_txtBattleScale->setText(tr("STR_BATTLESCAPE_SCALE"));
	
	_cbxBattleScale->setOptions(scales);
	_cbxBattleScale->setSelected(Options::battlescapeScale);
	_cbxBattleScale->onChange((ActionHandler)&OptionsVideoState::updateBattlescapeScale);
	_cbxBattleScale->setTooltip("STR_BATTLESCAPE_SCALE_DESC");
	_cbxBattleScale->onMouseIn((ActionHandler)&OptionsVideoState::txtTooltipIn);
	_cbxBattleScale->onMouseOut((ActionHandler)&OptionsVideoState::txtTooltipOut);

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
	// Update resolution mode
	if (_res != (SDL_Rect**)-1 && _res != (SDL_Rect**)0)
	{
		int i;
		_resCurrent = -1;
		for (i = 0; _res[i]; ++i)
		{
			if (_resCurrent == -1 &&
				((_res[i]->w == Options::newDisplayWidth && _res[i]->h <= Options::newDisplayHeight) || _res[i]->w < Options::newDisplayWidth))
			{
				_resCurrent = i;
			}
		}
	}
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
	// Update resolution mode
	if (_res != (SDL_Rect**)-1 && _res != (SDL_Rect**)0)
	{
		int i;
		_resCurrent = -1;
		for (i = 0; _res[i]; ++i)
		{
			if (_resCurrent == -1 &&
				((_res[i]->w == Options::newDisplayWidth && _res[i]->h <= Options::newDisplayHeight) || _res[i]->w < Options::newDisplayWidth))
			{
				_resCurrent = i;
			}
		}
	}
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
		Options::newXBRZFilter = false;
		break;
	case 1:
		Options::newOpenGL = false;
		Options::newScaleFilter = true;
		Options::newHQXFilter = false;
		Options::newXBRZFilter = false;
		break;
	case 2:
		Options::newOpenGL = false;
		Options::newScaleFilter = false;
		Options::newHQXFilter = true;
		Options::newXBRZFilter = false;
		break;
	case 3:
		Options::newOpenGL = false;
		Options::newScaleFilter = false;
		Options::newHQXFilter = false;
		Options::newXBRZFilter = true;
		break;
	default:
		Options::newOpenGL = true;
		Options::newScaleFilter = false;
		Options::newHQXFilter = false;
		Options::newXBRZFilter = false;
		Options::newOpenGLShader = _filters[_cbxFilter->getSelected()];
		break;
	}
}

/**
 * Changes the Display Mode options.
 * @param action Pointer to an action.
 */
void OptionsVideoState::updateDisplayMode(Action *)
{
	switch(_cbxDisplayMode->getSelected())
	{
	case 0:
		Options::fullscreen = false;
		Options::borderless = false;
		Options::allowResize = false;
		break;
	case 1:
		Options::fullscreen = true;
		Options::borderless = false;
		Options::allowResize = false;
		break;
	case 2:
		Options::fullscreen = false;
		Options::borderless = true;
		Options::allowResize = false;
		break;
	case 3:
		Options::fullscreen = false;
		Options::borderless = false;
		Options::allowResize = true;
		break;
	default:
		break;
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
 * Changes the Lock Mouse option.
 * @param action Pointer to an action.
 */
void OptionsVideoState::btnLockMouseClick(Action *)
{
	Options::captureMouse = (SDL_GrabMode)_btnLockMouse->getPressed();
	SDL_WM_GrabInput(Options::captureMouse);
}

/**
 * Changes the geoscape scale.
 * @param action Pointer to an action.
 */
void OptionsVideoState::updateGeoscapeScale(Action *)
{
	Options::newGeoscapeScale = _cbxGeoScale->getSelected();
}

/**
 * Updates the Battlescape scale.
 * @param action Pointer to an action.
 */
void OptionsVideoState::updateBattlescapeScale(Action *)
{
	Options::newBattlescapeScale = _cbxBattleScale->getSelected();
}

/**
 * Updates the scale.
 * @param dX delta of X;
 * @param dY delta of Y;
 */
void OptionsVideoState::resize(int &dX, int &dY)
{
	OptionsBaseState::resize(dX, dY);
	std::wostringstream ss;
	ss << Options::displayWidth;
	_txtDisplayWidth->setText(ss.str());
	ss.str(L"");
	ss << Options::displayHeight;
	_txtDisplayHeight->setText(ss.str());
}

/**
 * Takes care of any events from the core game engine.
 * @param action Pointer to an action.
 */
void OptionsVideoState::handle(Action *action)
{
	State::handle(action);
	if (action->getDetails()->type == SDL_KEYDOWN && action->getDetails()->key.keysym.sym == SDLK_g && (SDL_GetModState() & KMOD_CTRL) != 0)
	{
		_btnLockMouse->setPressed(Options::captureMouse == SDL_GRAB_ON);
	}
}
}
