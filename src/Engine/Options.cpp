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

#include "Options.h"
#include "../version.h"
#include <SDL.h>
#include <SDL_keysym.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <yaml-cpp/yaml.h>
#include "Exception.h"
#include "Logger.h"
#include "CrossPlatform.h"
#include "FileMap.h"
#include "Screen.h"

namespace OpenXcom
{

namespace Options
{
#define OPT
#include "Options.inc.h"
#undef OPT

std::string _dataFolder;
std::vector<std::string> _dataList;
std::string _userFolder;
std::string _configFolder;
std::vector<std::string> _userList;
std::map<std::string, std::string> _commandLine;
std::vector<OptionInfo> _info;
std::map<std::string, ModInfo> _modInfos;

/**
 * Sets up the options by creating their OptionInfo metadata.
 */
void create()
{
#ifdef DINGOO
	_info.push_back(OptionInfo("displayWidth", &displayWidth, Screen::ORIGINAL_WIDTH));
	_info.push_back(OptionInfo("displayHeight", &displayHeight, Screen::ORIGINAL_HEIGHT));
	_info.push_back(OptionInfo("fullscreen", &fullscreen, true));
	_info.push_back(OptionInfo("asyncBlit", &asyncBlit, false));
	_info.push_back(OptionInfo("keyboardMode", (int*)&keyboardMode, KEYBOARD_OFF));
#else
	_info.push_back(OptionInfo("displayWidth", &displayWidth, Screen::ORIGINAL_WIDTH*2));
	_info.push_back(OptionInfo("displayHeight", &displayHeight, Screen::ORIGINAL_HEIGHT*2));
	_info.push_back(OptionInfo("fullscreen", &fullscreen, false));
	_info.push_back(OptionInfo("asyncBlit", &asyncBlit, true));
	_info.push_back(OptionInfo("keyboardMode", (int*)&keyboardMode, KEYBOARD_ON));
#endif

	_info.push_back(OptionInfo("maxFrameSkip", &maxFrameSkip, 0));
	_info.push_back(OptionInfo("traceAI", &traceAI, false));
	_info.push_back(OptionInfo("verboseLogging", &verboseLogging, false));
	_info.push_back(OptionInfo("StereoSound", &StereoSound, true));
	_info.push_back(OptionInfo("baseXResolution", &baseXResolution, Screen::ORIGINAL_WIDTH));
	_info.push_back(OptionInfo("baseYResolution", &baseYResolution, Screen::ORIGINAL_HEIGHT));
	_info.push_back(OptionInfo("baseXGeoscape", &baseXGeoscape, Screen::ORIGINAL_WIDTH));
	_info.push_back(OptionInfo("baseYGeoscape", &baseYGeoscape, Screen::ORIGINAL_HEIGHT));
	_info.push_back(OptionInfo("baseXBattlescape", &baseXBattlescape, Screen::ORIGINAL_WIDTH));
	_info.push_back(OptionInfo("baseYBattlescape", &baseYBattlescape, Screen::ORIGINAL_HEIGHT));
	_info.push_back(OptionInfo("geoscapeScale", &geoscapeScale, 0));
	_info.push_back(OptionInfo("battlescapeScale", &battlescapeScale, 0));
	_info.push_back(OptionInfo("useScaleFilter", &useScaleFilter, false));
	_info.push_back(OptionInfo("useHQXFilter", &useHQXFilter, false));
	_info.push_back(OptionInfo("useXBRZFilter", &useXBRZFilter, false));
	_info.push_back(OptionInfo("useOpenGL", &useOpenGL, false));
	_info.push_back(OptionInfo("checkOpenGLErrors", &checkOpenGLErrors, false));
	_info.push_back(OptionInfo("useOpenGLShader", &useOpenGLShader, "Shaders/Openxcom.OpenGL.shader"));
	_info.push_back(OptionInfo("vSyncForOpenGL", &vSyncForOpenGL, true));
	_info.push_back(OptionInfo("useOpenGLSmoothing", &useOpenGLSmoothing, true));
	_info.push_back(OptionInfo("debug", &debug, false));
	_info.push_back(OptionInfo("debugUi", &debugUi, false));
	_info.push_back(OptionInfo("soundVolume", &soundVolume, 2*(MIX_MAX_VOLUME/3)));
	_info.push_back(OptionInfo("musicVolume", &musicVolume, 2*(MIX_MAX_VOLUME/3)));
	_info.push_back(OptionInfo("uiVolume", &uiVolume, MIX_MAX_VOLUME/3));
	_info.push_back(OptionInfo("language", &language, ""));
	_info.push_back(OptionInfo("battleScrollSpeed", &battleScrollSpeed, 8));
	_info.push_back(OptionInfo("battleEdgeScroll", (int*)&battleEdgeScroll, SCROLL_AUTO));
	_info.push_back(OptionInfo("battleDragScrollButton", &battleDragScrollButton, SDL_BUTTON_MIDDLE));
	_info.push_back(OptionInfo("dragScrollTimeTolerance", &dragScrollTimeTolerance, 300)); // miliSecond
	_info.push_back(OptionInfo("dragScrollPixelTolerance", &dragScrollPixelTolerance, 10)); // count of pixels
	_info.push_back(OptionInfo("battleFireSpeed", &battleFireSpeed, 6));
	_info.push_back(OptionInfo("battleXcomSpeed", &battleXcomSpeed, 30));
	_info.push_back(OptionInfo("battleAlienSpeed", &battleAlienSpeed, 30));
	_info.push_back(OptionInfo("battleNewPreviewPath", (int*)&battleNewPreviewPath, PATH_NONE)); // requires double-click to confirm moves
	_info.push_back(OptionInfo("fpsCounter", &fpsCounter, false));
	_info.push_back(OptionInfo("globeDetail", &globeDetail, true));
	_info.push_back(OptionInfo("globeRadarLines", &globeRadarLines, true));
	_info.push_back(OptionInfo("globeFlightPaths", &globeFlightPaths, true));
	_info.push_back(OptionInfo("globeAllRadarsOnBaseBuild", &globeAllRadarsOnBaseBuild, true));
	_info.push_back(OptionInfo("audioSampleRate", &audioSampleRate, 22050));
	_info.push_back(OptionInfo("audioBitDepth", &audioBitDepth, 16));
	_info.push_back(OptionInfo("pauseMode", &pauseMode, 0));
	_info.push_back(OptionInfo("battleNotifyDeath", &battleNotifyDeath, false));
	_info.push_back(OptionInfo("showFundsOnGeoscape", &showFundsOnGeoscape, false));
	_info.push_back(OptionInfo("allowResize", &allowResize, false));
	_info.push_back(OptionInfo("windowedModePositionX", &windowedModePositionX, -1));
	_info.push_back(OptionInfo("windowedModePositionY", &windowedModePositionY, -1));
	_info.push_back(OptionInfo("borderless", &borderless, false));
	_info.push_back(OptionInfo("captureMouse", (bool*)&captureMouse, false));
	_info.push_back(OptionInfo("battleTooltips", &battleTooltips, true));
	_info.push_back(OptionInfo("keepAspectRatio", &keepAspectRatio, true));
	_info.push_back(OptionInfo("nonSquarePixelRatio", &nonSquarePixelRatio, false));	
	_info.push_back(OptionInfo("cursorInBlackBandsInFullscreen", &cursorInBlackBandsInFullscreen, false));
	_info.push_back(OptionInfo("cursorInBlackBandsInWindow", &cursorInBlackBandsInWindow, true));
	_info.push_back(OptionInfo("cursorInBlackBandsInBorderlessWindow", &cursorInBlackBandsInBorderlessWindow, false));
	_info.push_back(OptionInfo("saveOrder", (int*)&saveOrder, SORT_DATE_DESC));
	_info.push_back(OptionInfo("geoClockSpeed", &geoClockSpeed, 80));
	_info.push_back(OptionInfo("dogfightSpeed", &dogfightSpeed, 30));
	_info.push_back(OptionInfo("geoScrollSpeed", &geoScrollSpeed, 20));
	_info.push_back(OptionInfo("geoDragScrollButton", &geoDragScrollButton, SDL_BUTTON_MIDDLE));
	_info.push_back(OptionInfo("preferredMusic", (int*)&preferredMusic, MUSIC_AUTO));
	_info.push_back(OptionInfo("preferredSound", (int*)&preferredSound, SOUND_AUTO));
	_info.push_back(OptionInfo("musicAlwaysLoop", &musicAlwaysLoop, false));

	// advanced options
	_info.push_back(OptionInfo("playIntro", &playIntro, true, "STR_PLAYINTRO", "STR_GENERAL"));
	_info.push_back(OptionInfo("autosave", &autosave, true, "STR_AUTOSAVE", "STR_GENERAL"));
	_info.push_back(OptionInfo("autosaveFrequency", &autosaveFrequency, 5, "STR_AUTOSAVE_FREQUENCY", "STR_GENERAL"));
	_info.push_back(OptionInfo("newSeedOnLoad", &newSeedOnLoad, false, "STR_NEWSEEDONLOAD", "STR_GENERAL"));
	_info.push_back(OptionInfo("mousewheelSpeed", &mousewheelSpeed, 3, "STR_MOUSEWHEEL_SPEED", "STR_GENERAL"));
	_info.push_back(OptionInfo("changeValueByMouseWheel", &changeValueByMouseWheel, 0, "STR_CHANGEVALUEBYMOUSEWHEEL", "STR_GENERAL"));

// this should probably be any small screen touch-device, i don't know the defines for all of them so i'll cover android and IOS as i imagine they're more common
#ifdef __ANDROID_API__
	_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, true, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
#elif __APPLE__
	// todo: ask grussel how badly i messed this up.
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
		_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, true, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
	#else
		_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, false, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
	#endif
#else
	_info.push_back(OptionInfo("maximizeInfoScreens", &maximizeInfoScreens, false, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL"));
#endif

	_info.push_back(OptionInfo("geoDragScrollInvert", &geoDragScrollInvert, false, "STR_DRAGSCROLLINVERT", "STR_GEOSCAPE")); // true drags away from the cursor, false drags towards (like a grab)
	_info.push_back(OptionInfo("aggressiveRetaliation", &aggressiveRetaliation, false, "STR_AGGRESSIVERETALIATION", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("customInitialBase", &customInitialBase, false, "STR_CUSTOMINITIALBASE", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("allowBuildingQueue", &allowBuildingQueue, false, "STR_ALLOWBUILDINGQUEUE", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("craftLaunchAlways", &craftLaunchAlways, false, "STR_CRAFTLAUNCHALWAYS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("storageLimitsEnforced", &storageLimitsEnforced, false, "STR_STORAGELIMITSENFORCED", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("canSellLiveAliens", &canSellLiveAliens, false, "STR_CANSELLLIVEALIENS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("anytimePsiTraining", &anytimePsiTraining, false, "STR_ANYTIMEPSITRAINING", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("globeSeasons", &globeSeasons, false, "STR_GLOBESEASONS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("psiStrengthEval", &psiStrengthEval, false, "STR_PSISTRENGTHEVAL", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("canTransferCraftsWhileAirborne", &canTransferCraftsWhileAirborne, false, "STR_CANTRANSFERCRAFTSWHILEAIRBORNE", "STR_GEOSCAPE")); // When the craft can reach the destination base with its fuel
	_info.push_back(OptionInfo("canManufactureMoreItemsPerHour", &canManufactureMoreItemsPerHour, false, "STR_CANMANUFACTUREMOREITEMSPERHOUR", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("spendResearchedItems", &spendResearchedItems, false, "STR_SPENDRESEARCHEDITEMS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("fieldPromotions", &fieldPromotions, false, "STR_FIELDPROMOTIONS", "STR_GEOSCAPE"));
	
	_info.push_back(OptionInfo("battleDragScrollInvert", &battleDragScrollInvert, false, "STR_DRAGSCROLLINVERT", "STR_BATTLESCAPE")); // true drags away from the cursor, false drags towards (like a grab)
	_info.push_back(OptionInfo("sneakyAI", &sneakyAI, false, "STR_SNEAKYAI", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleUFOExtenderAccuracy", &battleUFOExtenderAccuracy, false, "STR_BATTLEUFOEXTENDERACCURACY", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("showMoreStatsInInventoryView", &showMoreStatsInInventoryView, false, "STR_SHOWMORESTATSININVENTORYVIEW", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleHairBleach", &battleHairBleach, true, "STR_BATTLEHAIRBLEACH", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleInstantGrenade", &battleInstantGrenade, false, "STR_BATTLEINSTANTGRENADE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("includePrimeStateInSavedLayout", &includePrimeStateInSavedLayout, false, "STR_INCLUDE_PRIMESTATE_IN_SAVED_LAYOUT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleExplosionHeight", &battleExplosionHeight, 0, "STR_BATTLEEXPLOSIONHEIGHT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleAutoEnd", &battleAutoEnd, false, "STR_BATTLEAUTOEND", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleSmoothCamera", &battleSmoothCamera, false, "STR_BATTLESMOOTHCAMERA", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("disableAutoEquip", &disableAutoEquip, false, "STR_DISABLEAUTOEQUIP", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("battleConfirmFireMode", &battleConfirmFireMode, false, "STR_BATTLECONFIRMFIREMODE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("weaponSelfDestruction", &weaponSelfDestruction, false, "STR_WEAPONSELFDESTRUCTION", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("allowPsionicCapture", &allowPsionicCapture, false, "STR_ALLOWPSIONICCAPTURE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("allowPsiStrengthImprovement", &allowPsiStrengthImprovement, false, "STR_ALLOWPSISTRENGTHIMPROVEMENT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("strafe", &strafe, false, "STR_STRAFE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("forceFire", &forceFire, true, "STR_FORCE_FIRE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("skipNextTurnScreen", &skipNextTurnScreen, false, "STR_SKIPNEXTTURNSCREEN", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("TFTDDamage", &TFTDDamage, false, "STR_TFTDDAMAGE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("noAlienPanicMessages", &noAlienPanicMessages, false, "STR_NOALIENPANICMESSAGES", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("alienBleeding", &alienBleeding, false, "STR_ALIENBLEEDING", "STR_BATTLESCAPE"));
	
	// controls
	_info.push_back(OptionInfo("keyOk", &keyOk, SDLK_RETURN, "STR_OK", "STR_GENERAL"));
	_info.push_back(OptionInfo("keyCancel", &keyCancel, SDLK_ESCAPE, "STR_CANCEL", "STR_GENERAL"));
	_info.push_back(OptionInfo("keyScreenshot", &keyScreenshot, SDLK_F12, "STR_SCREENSHOT", "STR_GENERAL"));
	_info.push_back(OptionInfo("keyFps", &keyFps, SDLK_F7, "STR_FPS_COUNTER", "STR_GENERAL"));
	_info.push_back(OptionInfo("keyQuickSave", &keyQuickSave, SDLK_F5, "STR_QUICK_SAVE", "STR_GENERAL"));
	_info.push_back(OptionInfo("keyQuickLoad", &keyQuickLoad, SDLK_F9, "STR_QUICK_LOAD", "STR_GENERAL"));
	_info.push_back(OptionInfo("keyGeoLeft", &keyGeoLeft, SDLK_LEFT, "STR_ROTATE_LEFT", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoRight", &keyGeoRight, SDLK_RIGHT, "STR_ROTATE_RIGHT", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoUp", &keyGeoUp, SDLK_UP, "STR_ROTATE_UP", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoDown", &keyGeoDown, SDLK_DOWN, "STR_ROTATE_DOWN", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoZoomIn", &keyGeoZoomIn, SDLK_PLUS, "STR_ZOOM_IN", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoZoomOut", &keyGeoZoomOut, SDLK_MINUS, "STR_ZOOM_OUT", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoSpeed1", &keyGeoSpeed1, SDLK_1, "STR_5_SECONDS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoSpeed2", &keyGeoSpeed2, SDLK_2, "STR_1_MINUTE", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoSpeed3", &keyGeoSpeed3, SDLK_3, "STR_5_MINUTES", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoSpeed4", &keyGeoSpeed4, SDLK_4, "STR_30_MINUTES", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoSpeed5", &keyGeoSpeed5, SDLK_5, "STR_1_HOUR", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoSpeed6", &keyGeoSpeed6, SDLK_6, "STR_1_DAY", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoIntercept", &keyGeoIntercept, SDLK_i, "STR_INTERCEPT", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoBases", &keyGeoBases, SDLK_b, "STR_BASES", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoGraphs", &keyGeoGraphs, SDLK_g, "STR_GRAPHS", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoUfopedia", &keyGeoUfopedia, SDLK_u, "STR_UFOPAEDIA_UC", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoOptions", &keyGeoOptions, SDLK_ESCAPE, "STR_OPTIONS_UC", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoFunding", &keyGeoFunding, SDLK_f, "STR_FUNDING_UC", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoToggleDetail", &keyGeoToggleDetail, SDLK_TAB, "STR_TOGGLE_COUNTRY_DETAIL", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyGeoToggleRadar", &keyGeoToggleRadar, SDLK_r, "STR_TOGGLE_RADAR_RANGES", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect1", &keyBaseSelect1, SDLK_1, "STR_SELECT_BASE_1", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect2", &keyBaseSelect2, SDLK_2, "STR_SELECT_BASE_2", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect3", &keyBaseSelect3, SDLK_3, "STR_SELECT_BASE_3", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect4", &keyBaseSelect4, SDLK_4, "STR_SELECT_BASE_4", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect5", &keyBaseSelect5, SDLK_5, "STR_SELECT_BASE_5", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect6", &keyBaseSelect6, SDLK_6, "STR_SELECT_BASE_6", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect7", &keyBaseSelect7, SDLK_7, "STR_SELECT_BASE_7", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBaseSelect8", &keyBaseSelect8, SDLK_8, "STR_SELECT_BASE_8", "STR_GEOSCAPE"));
	_info.push_back(OptionInfo("keyBattleLeft", &keyBattleLeft, SDLK_LEFT, "STR_SCROLL_LEFT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleRight", &keyBattleRight, SDLK_RIGHT, "STR_SCROLL_RIGHT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleUp", &keyBattleUp, SDLK_UP, "STR_SCROLL_UP", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleDown", &keyBattleDown, SDLK_DOWN, "STR_SCROLL_DOWN", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleLevelUp", &keyBattleLevelUp, SDLK_PAGEUP, "STR_VIEW_LEVEL_ABOVE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleLevelDown", &keyBattleLevelDown, SDLK_PAGEDOWN, "STR_VIEW_LEVEL_BELOW", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterUnit", &keyBattleCenterUnit, SDLK_HOME, "STR_CENTER_SELECTED_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattlePrevUnit", &keyBattlePrevUnit, SDLK_LSHIFT, "STR_PREVIOUS_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleNextUnit", &keyBattleNextUnit, SDLK_TAB, "STR_NEXT_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleDeselectUnit", &keyBattleDeselectUnit, SDLK_BACKSLASH, "STR_DESELECT_UNIT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleUseLeftHand", &keyBattleUseLeftHand, SDLK_q, "STR_USE_LEFT_HAND", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleUseRightHand", &keyBattleUseRightHand, SDLK_e, "STR_USE_RIGHT_HAND", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleInventory", &keyBattleInventory, SDLK_i, "STR_INVENTORY", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleMap", &keyBattleMap, SDLK_m, "STR_MINIMAP", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleOptions", &keyBattleOptions, SDLK_ESCAPE, "STR_OPTIONS", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleEndTurn", &keyBattleEndTurn, SDLK_BACKSPACE, "STR_END_TURN", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleAbort", &keyBattleAbort, SDLK_a, "STR_ABORT_MISSION", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleStats", &keyBattleStats, SDLK_s, "STR_UNIT_STATS", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleKneel", &keyBattleKneel, SDLK_k, "STR_KNEEL", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleReload", &keyBattleReload, SDLK_r, "STR_RELOAD", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattlePersonalLighting", &keyBattlePersonalLighting, SDLK_l, "STR_TOGGLE_PERSONAL_LIGHTING", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleReserveNone", &keyBattleReserveNone, SDLK_F1, "STR_DONT_RESERVE_TIME_UNITS", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleReserveSnap", &keyBattleReserveSnap, SDLK_F2, "STR_RESERVE_TIME_UNITS_FOR_SNAP_SHOT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleReserveAimed", &keyBattleReserveAimed, SDLK_F3, "STR_RESERVE_TIME_UNITS_FOR_AIMED_SHOT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleReserveAuto", &keyBattleReserveAuto, SDLK_F4, "STR_RESERVE_TIME_UNITS_FOR_AUTO_SHOT", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleReserveKneel", &keyBattleReserveKneel, SDLK_j, "STR_RESERVE_TIME_UNITS_FOR_KNEEL", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleZeroTUs", &keyBattleZeroTUs, SDLK_DELETE, "STR_EXPEND_ALL_TIME_UNITS", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy1", &keyBattleCenterEnemy1, SDLK_1, "STR_CENTER_ON_ENEMY_1", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy2", &keyBattleCenterEnemy2, SDLK_2, "STR_CENTER_ON_ENEMY_2", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy3", &keyBattleCenterEnemy3, SDLK_3, "STR_CENTER_ON_ENEMY_3", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy4", &keyBattleCenterEnemy4, SDLK_4, "STR_CENTER_ON_ENEMY_4", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy5", &keyBattleCenterEnemy5, SDLK_5, "STR_CENTER_ON_ENEMY_5", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy6", &keyBattleCenterEnemy6, SDLK_6, "STR_CENTER_ON_ENEMY_6", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy7", &keyBattleCenterEnemy7, SDLK_7, "STR_CENTER_ON_ENEMY_7", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy8", &keyBattleCenterEnemy8, SDLK_8, "STR_CENTER_ON_ENEMY_8", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy9", &keyBattleCenterEnemy9, SDLK_9, "STR_CENTER_ON_ENEMY_9", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleCenterEnemy10", &keyBattleCenterEnemy10, SDLK_0, "STR_CENTER_ON_ENEMY_10", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyBattleVoxelView", &keyBattleVoxelView, SDLK_F10, "STR_SAVE_VOXEL_VIEW", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyInvCreateTemplate", &keyInvCreateTemplate, SDLK_c, "STR_CREATE_INVENTORY_TEMPLATE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyInvApplyTemplate", &keyInvApplyTemplate, SDLK_v, "STR_APPLY_INVENTORY_TEMPLATE", "STR_BATTLESCAPE"));
	_info.push_back(OptionInfo("keyInvClear", &keyInvClear, SDLK_x, "STR_CLEAR_INVENTORY", "STR_BATTLESCAPE"));

#ifdef __MORPHOS__
	_info.push_back(OptionInfo("FPS", &FPS, 15));
	_info.push_back(OptionInfo("FPS_INACTIVE", &FPSInactive, 15));
#else
	_info.push_back(OptionInfo("FPS", &FPS, 60, "STR_FPS_LIMIT", "STR_GENERAL"));
	_info.push_back(OptionInfo("FPSInactive", &FPSInactive, 30, "STR_FPS_INACTIVE_LIMIT", "STR_GENERAL"));
#endif

}

// we can get fancier with these detection routines, but for now just look for
// *something* in the data folders.  case sensitivity can make actually verifying
// that the *correct* files are there complex.
static bool _gameIsInstalled(const std::string &gameName)
{
	// look for game data in either the data or user directories
	std::string dataGameFolder = CrossPlatform::searchDataFolder(gameName);
	std::string userGameFolder = _userFolder + gameName;
	return (CrossPlatform::folderExists(dataGameFolder)
		&& CrossPlatform::getFolderContents(dataGameFolder).size() > 8)
	    || (CrossPlatform::folderExists(userGameFolder)
		&& CrossPlatform::getFolderContents(userGameFolder).size() > 8);
}

static bool _ufoIsInstalled()
{
	return _gameIsInstalled("UFO");
}

static bool _tftdIsInstalled()
{
	// ensure both the resource data and the mod data is in place
	return _gameIsInstalled("TFTD")
		&& CrossPlatform::fileExists(CrossPlatform::searchDataFile("standard/xcom2/Xcom2Ruleset.rul"));
}

static void _setDefaultMods()
{
	// try to find xcom1
	bool haveUfo = _ufoIsInstalled();
	if (haveUfo)
	{
		Log(LOG_DEBUG) << "detected UFO";
		mods.push_back(std::pair<std::string, bool>("xcom1", true));
	}

	if (_tftdIsInstalled())
	{
		Log(LOG_DEBUG) << "detected TFTD";
		mods.push_back(std::pair<std::string, bool>("xcom2", !haveUfo));
	}
	else if (!haveUfo)
	{
		Log(LOG_ERROR) << "neither UFO or TFTD data was detected";
	}
}

/**
 * Resets the options back to their defaults.
 */
void resetDefault()
{
	for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
	{
		i->reset();
	}
	backupDisplay();

	mods.clear();
	if (!_dataList.empty())
	{
		_setDefaultMods();
	}

	purchaseExclusions.clear();
}

/**
 * Loads options from a set of command line arguments,
 * in the format "-option value".
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 */
void loadArgs(int argc, char *argv[])
{
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if ((arg[0] == '-' || arg[0] == '/') && arg.length() > 1)
		{
			std::string argname;
			if (arg[1] == '-' && arg.length() > 2)
				argname = arg.substr(2, arg.length()-1);
			else
				argname = arg.substr(1, arg.length()-1);
			std::transform(argname.begin(), argname.end(), argname.begin(), ::tolower);
			if (argc > i + 1)
			{
				++i; // we'll be consuming the next argument too

				if (argname == "data")
				{
					_dataFolder = CrossPlatform::endPath(argv[i]);
				}
				else if (argname == "user")
				{
					_userFolder = CrossPlatform::endPath(argv[i]);
				}
				else if (argname == "cfg" || argname == "config")
				{
					_configFolder = CrossPlatform::endPath(argv[i]);
				}
				else
				{
					//save this command line option for now, we will apply it later
					_commandLine[argname] = argv[i];
				}
			}
			else
			{
				Log(LOG_WARNING) << "Unknown option: " << argname;
			}
		}
	}
}

/*
 * Displays command-line help when appropriate.
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 */
bool showHelp(int argc, char *argv[])
{
	std::ostringstream help;
	help << "OpenXcom v" << OPENXCOM_VERSION_SHORT << std::endl;
	help << "Usage: openxcom [OPTION]..." << std::endl << std::endl;
	help << "-data PATH" << std::endl;
	help << "        use PATH as the default Data Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-user PATH" << std::endl;
	help << "        use PATH as the default User Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-cfg PATH  or  -config PATH" << std::endl;
	help << "        use PATH as the default Config Folder instead of auto-detecting" << std::endl << std::endl;
	help << "-KEY VALUE" << std::endl;
	help << "        set option KEY to VALUE instead of default/loaded value (eg. -displayWidth 640)" << std::endl << std::endl;
	help << "-help" << std::endl;
	help << "-?" << std::endl;
	help << "        show command-line help" << std::endl;
	for (int i = 1; i < argc; ++i)
	{
		std::string arg = argv[i];
		if ((arg[0] == '-' || arg[0] == '/') && arg.length() > 1)
		{
			std::string argname;
			if (arg[1] == '-' && arg.length() > 2)
				argname = arg.substr(2, arg.length()-1);
			else
				argname = arg.substr(1, arg.length()-1);
			std::transform(argname.begin(), argname.end(), argname.begin(), ::tolower);
			if (argname == "help" || argname == "?")
			{
				std::cout << help.str();
				return true;
			}
		}
	}
	return false;
}

const std::map<std::string, ModInfo> &getModInfos() { return _modInfos; }

static void _scanMods(const std::string &modsDir)
{
	if (!CrossPlatform::folderExists(modsDir))
	{
		Log(LOG_VERBOSE) << "skipping non-existent mod directory: '" << modsDir << "'";
		return;
	}

	std::vector<std::string> contents = CrossPlatform::getFolderContents(modsDir);
	for (std::vector<std::string>::iterator i = contents.begin(); i != contents.end(); ++i)
	{
		std::string modPath = modsDir + "/" + *i;
		if (!CrossPlatform::folderExists(modPath))
		{
			// skip non-directories (e.g. README.txt)
			continue;
		}

		Log(LOG_VERBOSE) << "- " << modPath;
		ModInfo modInfo(modPath);

		std::string metadataPath = modPath + "/metadata.yml";
		if (!CrossPlatform::fileExists(metadataPath))
		{
			Log(LOG_WARNING) << metadataPath << " not found; using default values for mod: " << *i;
		}
		else
		{
			modInfo.load(metadataPath);
		}

		Log(LOG_VERBOSE) << "  id: " << modInfo.getId();
		Log(LOG_VERBOSE) << "  name: " << modInfo.getName();
		Log(LOG_VERBOSE) << "  version: " << modInfo.getVersion();
		Log(LOG_VERBOSE) << "  description: " << modInfo.getDescription();
		Log(LOG_VERBOSE) << "  author: " << modInfo.getAuthor();
		Log(LOG_VERBOSE) << "  master: " << modInfo.getMaster();
		Log(LOG_VERBOSE) << "  isMaster: " << modInfo.isMaster();
		Log(LOG_VERBOSE) << "  loadResources:";
		std::vector<std::string> externals = modInfo.getExternalResourceDirs();
		for (std::vector<std::string>::iterator j = externals.begin(); j != externals.end(); ++j)
		{
			Log(LOG_VERBOSE) << "    " << *j;
		}

		if (("xcom1" == modInfo.getId() && !_ufoIsInstalled())
		 || ("xcom2" == modInfo.getId() && !_tftdIsInstalled()))
		{
			Log(LOG_VERBOSE) << "skipping " << modInfo.getId() << " since related game data isn't installed";
			continue;
		}

		_modInfos.insert(std::pair<std::string, ModInfo>(modInfo.getId(), modInfo));
	}
}

/**
 * Handles the initialization of setting up default options
 * and finding and loading any existing ones.
 * @param argc Number of arguments.
 * @param argv Array of argument strings.
 * @return Was initialized.
 */
bool init(int argc, char *argv[])
{
	if (showHelp(argc, argv))
		return false;
	create();
	resetDefault();
	loadArgs(argc, argv);
	setFolders();
	_setDefaultMods();
	updateOptions();

	std::string s = getUserFolder();
	s += "openxcom.log";
	Logger::logFile() = s;
	FILE *file = fopen(Logger::logFile().c_str(), "w");
	if (!file)
	{
		throw Exception(s + " not found");
	}
	fflush(file);
	fclose(file);
	Log(LOG_INFO) << "Data folder is: " << _dataFolder;
	Log(LOG_INFO) << "Data search is: ";
	for (std::vector<std::string>::iterator i = _dataList.begin(); i != _dataList.end(); ++i)
	{
		Log(LOG_INFO) << "- " << *i;
	}
	Log(LOG_INFO) << "User folder is: " << _userFolder;
	Log(LOG_INFO) << "Config folder is: " << _configFolder;
	Log(LOG_INFO) << "Options loaded successfully.";

	std::string modPath = CrossPlatform::searchDataFolder("standard");
	Log(LOG_INFO) << "Scanning standard mods in '" << modPath << "'...";
	_scanMods(modPath);
	modPath = _userFolder + "mods";
	Log(LOG_INFO) << "Scanning user mods in '" << modPath << "'...";
	_scanMods(modPath);

	// remove mods from list that no longer exist
	for (std::vector< std::pair<std::string, bool> >::iterator i = mods.begin(); i != mods.end(); )
	{
		std::map<std::string, ModInfo>::const_iterator modIt = _modInfos.find(i->first);
		if (_modInfos.end() == modIt
		 || (i->first == "xcom1" && !_ufoIsInstalled())
		 || (i->first == "xcom2" && !_tftdIsInstalled()))
		{
			Log(LOG_INFO) << "removing references to missing mod: " << i->first;
			i = mods.erase(i);
			continue;
		}
		++i;
	}

	// add in any new mods picked up from the scan and ensure there is but a single
	// master active
	std::string activeMaster;
	std::string inactiveMaster;
	for (std::map<std::string, ModInfo>::const_iterator i = _modInfos.begin(); i != _modInfos.end(); ++i)
	{
		bool found = false;
		for (std::vector< std::pair<std::string, bool> >::iterator j = mods.begin(); j != mods.end(); ++j)
		{
			if (i->first == j->first)
			{
				found = true;
				if (i->second.isMaster())
				{
					if (j->second)
					{
						if (!activeMaster.empty())
						{
							Log(LOG_WARNING) << "too many active masters detected; turning off " << j->first;
							j->second = false;
						}
						else
						{
							activeMaster = j->first;
						}
					}
					else
					{
						// prefer activating standard masters over a possibly broken
						// third party master
						if (inactiveMaster.empty() || j->first == "xcom1" || j->first == "xcom2")
						{
							inactiveMaster = j->first;
						}
					}
				}

				break;
			}
		}
		if (found)
		{
			continue;
		}

		// not active by default
		std::pair<std::string, bool> newMod(i->first, false);
		if (i->second.isMaster())
		{
			// it doesn't matter what order the masters are in since
			// only one can be active at a time anyway
			mods.insert(mods.begin(), newMod);

			if (inactiveMaster.empty())
			{
				inactiveMaster = i->first;
			}
		}
		else
		{
			mods.push_back(newMod);
		}
	}

	if (activeMaster.empty())
	{
		if (inactiveMaster.empty())
		{
			Log(LOG_ERROR) << "no mod masters available";
		}
		else
		{
			Log(LOG_INFO) << "no master already active; activating " << inactiveMaster;
			std::find(mods.begin(), mods.end(), std::pair<std::string, bool>(inactiveMaster, false))->second = true;
		}
	}

	mapResources();

	return true;
}

std::string getActiveMaster()
{
	std::string curMaster;
	for (std::vector< std::pair<std::string, bool> >::const_iterator i = mods.begin(); i != mods.end(); ++i)
	{
		if (!i->second)
		{
			// we're only looking for active mods
			continue;
		}

		ModInfo modInfo = _modInfos.find(i->first)->second;
		if (!modInfo.isMaster())
		{
			continue;
		}

		curMaster = modInfo.getId();
		break;
	}
	if (curMaster.empty())
	{
		Log(LOG_ERROR) << "cannot determine current active master";
	}
	return curMaster;
}

static void _loadMod(const ModInfo &modInfo, std::set<std::string> circDepCheck)
{
	if (circDepCheck.end() != circDepCheck.find(modInfo.getId()))
	{
		Log(LOG_WARNING) << "circular dependency found in master chain: " << modInfo.getId();
		return;
	}
	
	FileMap::load(modInfo.getId(), modInfo.getPath(), false);
	for (std::vector<std::string>::const_iterator i = modInfo.getExternalResourceDirs().begin(); i != modInfo.getExternalResourceDirs().end(); ++i)
	{
		// use external resource folders from the user dir if they exist
		// and if not, fall back to searching the data dirs
		std::string extResourceFolder = _userFolder + *i;
		if (!CrossPlatform::folderExists(extResourceFolder))
		{
			extResourceFolder = CrossPlatform::searchDataFolder(*i);
		}

		// always ignore ruleset files in external resource dirs
		FileMap::load(modInfo.getId(), extResourceFolder, true);
	}

	// if this is a master but it has a master of its own, allow it to
	// chainload the "super" master, including its rulesets
	if (modInfo.isMaster() && !modInfo.getMaster().empty())
	{
		// add self to circDepCheck so we can avoid circular dependencies
		circDepCheck.insert(modInfo.getId());
		const ModInfo &masterInfo = _modInfos.find(modInfo.getMaster())->second;
		_loadMod(masterInfo, circDepCheck);
	}
}

void mapResources()
{
	Log(LOG_INFO) << "Mapping resource files...";
	FileMap::clear();

	std::string curMaster = getActiveMaster();
	for (std::vector< std::pair<std::string, bool> >::reverse_iterator i = mods.rbegin(); i != mods.rend(); ++i)
	{
		if (!i->second)
		{
			Log(LOG_VERBOSE) << "skipping inactive mod: " << i->first;
			continue;
		}

		const ModInfo &modInfo = _modInfos.find(i->first)->second;
		if (!modInfo.isMaster() && !modInfo.getMaster().empty() && modInfo.getMaster() != curMaster)
		{
			Log(LOG_VERBOSE) << "skipping mod for non-current master: " << i->first << "(" << modInfo.getMaster() << " != " << curMaster << ")";
			continue;
		}

		std::set<std::string> circDepCheck;
		_loadMod(modInfo, circDepCheck);
	}
	// pick up stuff in common
	FileMap::load("common", CrossPlatform::searchDataFolder("common"), true);
	Log(LOG_INFO) << "Resources files mapped successfully.";
}

/**
 * Sets up the game's Data folder where the data files
 * are loaded from and the User folder and Config
 * folder where settings and saves are stored in.
 */
void setFolders()
{
	_dataList = CrossPlatform::findDataFolders();
	if (!_dataFolder.empty())
	{
		_dataList.insert(_dataList.begin(), _dataFolder);
	}
	if (_userFolder.empty())
	{
		std::vector<std::string> user = CrossPlatform::findUserFolders();
		_configFolder = CrossPlatform::findConfigFolder();

		// Look for an existing user folder
		for (std::vector<std::string>::reverse_iterator i = user.rbegin(); i != user.rend(); ++i)
		{
			if (CrossPlatform::folderExists(*i))
			{
				_userFolder = *i;
				break;
			}
		}

		// Set up folders
		if (_userFolder.empty())
		{
			for (std::vector<std::string>::iterator i = user.begin(); i != user.end(); ++i)
			{
				if (CrossPlatform::createFolder(*i))
				{
					_userFolder = *i;
					break;
				}
			}
		}
	}
	if (!_userFolder.empty())
	{
		// create mods subfolder if it doesn't already exist
		std::string modsFolder = _userFolder + "mods";
		if (!CrossPlatform::folderExists(modsFolder))
		{
			if (CrossPlatform::createFolder(modsFolder))
			{
				Log(LOG_INFO) << "created mods folder: '" << modsFolder << "'";
			}
			else
			{
				Log(LOG_WARNING) << "failed to create mods folder: '" << modsFolder << "'";
			}
		}
	}

	if (_configFolder.empty())
	{
		_configFolder = _userFolder;
	}
}

/**
 * Updates the game's options with those in the configuration
 * file, if it exists yet, and any supplied on the command line.
 */
void updateOptions()
{
	// Load existing options
	if (CrossPlatform::folderExists(_configFolder))
	{
		if (CrossPlatform::fileExists(_configFolder + "options.cfg"))
		{
			load();
		}
		else
		{
			save();
		}
	}
	// Create config folder and save options
	else
	{
		CrossPlatform::createFolder(_configFolder);
		save();
	}

	// now apply options set on the command line, overriding defaults and those loaded from config file
	//if (!_commandLine.empty())
	for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
	{
		i->load(_commandLine);
	}
}

/**
 * Loads options from a YAML file.
 * @param filename YAML filename.
 */
void load(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	try
	{
		YAML::Node doc = YAML::LoadFile(s);
		// Ignore old options files
		if (doc["options"]["NewBattleMission"])
		{
			return;
		}
		for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
		{
			i->load(doc["options"]);
		}
		purchaseExclusions = doc["purchaseexclusions"].as< std::vector<std::string> >(purchaseExclusions);

		mods.clear();
		for (YAML::const_iterator i = doc["mods"].begin(); i != doc["mods"].end(); ++i)
		{
			std::string id = (*i)["id"].as<std::string>();
			bool active = (*i)["active"].as<bool>(false);
			mods.push_back(std::pair<std::string, bool>(id, active));
		}
		if (mods.empty())
		{
			_setDefaultMods();
		}
	}
	catch (YAML::Exception e)
	{
		Log(LOG_WARNING) << e.what();
	}
}

/**
 * Saves options to a YAML file.
 * @param filename YAML filename.
 */
void save(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		Log(LOG_WARNING) << "Failed to save " << filename << ".cfg";
		return;
	}
	try
	{
		YAML::Emitter out;

		YAML::Node doc, node;
		for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
		{
			i->save(node);
		}
		doc["options"] = node;
		doc["purchaseexclusions"] = purchaseExclusions;

		for (std::vector< std::pair<std::string, bool> >::iterator i = mods.begin(); i != mods.end(); ++i)
		{
			YAML::Node mod;
			mod["id"] = i->first;
			mod["active"] = i->second;
			doc["mods"].push_back(mod);
		}

		out << doc;

		sav << out.c_str();
	}
	catch (YAML::Exception e)
	{
		Log(LOG_WARNING) << e.what();
	}
	sav.close();
}

/**
 * Returns the game's current Data folder where resources
 * and X-Com files are loaded from.
 * @return Full path to Data folder.
 */
std::string getDataFolder()
{
	return _dataFolder;
}

/**
 * Changes the game's current Data folder where resources
 * and X-Com files are loaded from.
 * @param folder Full path to Data folder.
 */
void setDataFolder(const std::string &folder)
{
	_dataFolder = folder;
}

/**
 * Returns the game's list of possible Data folders.
 * @return List of Data paths.
 */
const std::vector<std::string> &getDataList()
{
	return _dataList;
}

/**
 * Returns the game's User folder where
 * saves are stored in.
 * @return Full path to User folder.
 */
std::string getUserFolder()
{
	return _userFolder;
}

/**
 * Returns the game's Config folder where
 * settings are stored in. Normally the same
 * as the User folder.
 * @return Full path to Config folder.
 */
std::string getConfigFolder()
{
	return _configFolder;
}

/**
 * Returns the game's list of all available option information.
 * @return List of OptionInfo's.
 */
const std::vector<OptionInfo> &getOptionInfo()
{
	return _info;
}

/**
 * Saves display settings temporarily to be able
 * to revert to old ones.
 */
void backupDisplay()
{
	Options::newDisplayWidth = Options::displayWidth;
	Options::newDisplayHeight = Options::displayHeight;
	Options::newBattlescapeScale = Options::battlescapeScale;
	Options::newGeoscapeScale = Options::geoscapeScale;
	Options::newOpenGL = Options::useOpenGL;
	Options::newScaleFilter = Options::useScaleFilter;
	Options::newHQXFilter = Options::useHQXFilter;
	Options::newOpenGLShader = Options::useOpenGLShader;
	Options::newXBRZFilter = Options::useXBRZFilter;
}

/**
 * Switches old/new display options for temporarily
 * testing a new display setup.
 */
void switchDisplay()
{	
	std::swap(displayWidth, newDisplayWidth);
	std::swap(displayHeight, newDisplayHeight);
	std::swap(useOpenGL, newOpenGL);
	std::swap(useScaleFilter, newScaleFilter);
	std::swap(battlescapeScale, newBattlescapeScale);
	std::swap(geoscapeScale, newGeoscapeScale);
	std::swap(useHQXFilter, newHQXFilter);
	std::swap(useOpenGLShader, newOpenGLShader);
	std::swap(useXBRZFilter, newXBRZFilter);
}

}
}
