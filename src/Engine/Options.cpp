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
std::string _masterMod;

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
	_info.emplace_back("displayWidth", &displayWidth, Screen::ORIGINAL_WIDTH*2);
	_info.emplace_back("displayHeight", &displayHeight, Screen::ORIGINAL_HEIGHT*2);
	_info.emplace_back("fullscreen", &fullscreen, false);
	_info.emplace_back("asyncBlit", &asyncBlit, true);
	_info.emplace_back("keyboardMode", (int*)&keyboardMode, KEYBOARD_ON);
#endif

	_info.emplace_back("maxFrameSkip", &maxFrameSkip, 0);
	_info.emplace_back("traceAI", &traceAI, false);
	_info.emplace_back("verboseLogging", &verboseLogging, false);
	_info.emplace_back("StereoSound", &StereoSound, true);
    //_info.emplace_back("baseXResolution", &baseXResolution, Screen::ORIGINAL_WIDTH);
    //_info.emplace_back("baseYResolution", &baseYResolution, Screen::ORIGINAL_HEIGHT);
    //_info.emplace_back("baseXGeoscape", &baseXGeoscape, Screen::ORIGINAL_WIDTH);
    //_info.emplace_back("baseYGeoscape", &baseYGeoscape, Screen::ORIGINAL_HEIGHT);
    //_info.emplace_back("baseXBattlescape", &baseXBattlescape, Screen::ORIGINAL_WIDTH);
    //_info.emplace_back("baseYBattlescape", &baseYBattlescape, Screen::ORIGINAL_HEIGHT);
	_info.emplace_back("geoscapeScale", &geoscapeScale, 0);
	_info.emplace_back("battlescapeScale", &battlescapeScale, 0);
	_info.emplace_back("useScaleFilter", &useScaleFilter, false);
	_info.emplace_back("useHQXFilter", &useHQXFilter, false);
	_info.emplace_back("useXBRZFilter", &useXBRZFilter, false);
	_info.emplace_back("useOpenGL", &useOpenGL, false);
	_info.emplace_back("checkOpenGLErrors", &checkOpenGLErrors, false);
	_info.emplace_back("useOpenGLShader", &useOpenGLShader, "Shaders/Raw.OpenGL.shader");
	_info.emplace_back("vSyncForOpenGL", &vSyncForOpenGL, true);
	_info.emplace_back("useOpenGLSmoothing", &useOpenGLSmoothing, true);
	_info.emplace_back("debug", &debug, false);
	_info.emplace_back("debugUi", &debugUi, false);
	_info.emplace_back("soundVolume", &soundVolume, 2*(MIX_MAX_VOLUME/3));
	_info.emplace_back("musicVolume", &musicVolume, 2*(MIX_MAX_VOLUME/3));
	_info.emplace_back("uiVolume", &uiVolume, MIX_MAX_VOLUME/3);
	_info.emplace_back("language", &language, "");
	_info.emplace_back("battleScrollSpeed", &battleScrollSpeed, 8);
	_info.emplace_back("battleEdgeScroll", (int*)&battleEdgeScroll, SCROLL_AUTO);
	_info.emplace_back("battleDragScrollButton", &battleDragScrollButton, SDL_BUTTON_MIDDLE);
	_info.emplace_back("dragScrollTimeTolerance", &dragScrollTimeTolerance, 300); // miliSecond
	_info.emplace_back("dragScrollPixelTolerance", &dragScrollPixelTolerance, 10); // count of pixels
	_info.emplace_back("battleFireSpeed", &battleFireSpeed, 6);
	_info.emplace_back("battleXcomSpeed", &battleXcomSpeed, 30);
	_info.emplace_back("battleAlienSpeed", &battleAlienSpeed, 30);
	_info.emplace_back("battleNewPreviewPath", (int*)&battleNewPreviewPath, PATH_NONE); // requires double-click to confirm moves
	_info.emplace_back("fpsCounter", &fpsCounter, false);
	_info.emplace_back("globeDetail", &globeDetail, true);
	_info.emplace_back("globeRadarLines", &globeRadarLines, true);
	_info.emplace_back("globeFlightPaths", &globeFlightPaths, true);
	_info.emplace_back("globeAllRadarsOnBaseBuild", &globeAllRadarsOnBaseBuild, true);
	_info.emplace_back("audioSampleRate", &audioSampleRate, 22050);
	_info.emplace_back("audioBitDepth", &audioBitDepth, 16);
	_info.emplace_back("audioChunkSize", &audioChunkSize, 1024);
	_info.emplace_back("pauseMode", &pauseMode, 0);
	_info.emplace_back("battleNotifyDeath", &battleNotifyDeath, false);
	_info.emplace_back("showFundsOnGeoscape", &showFundsOnGeoscape, false);
	_info.emplace_back("allowResize", &allowResize, false);
	_info.emplace_back("windowedModePositionX", &windowedModePositionX, 0);
	_info.emplace_back("windowedModePositionY", &windowedModePositionY, 0);
	_info.emplace_back("borderless", &borderless, false);
	_info.emplace_back("captureMouse", (bool*)&captureMouse, false);
	_info.emplace_back("battleTooltips", &battleTooltips, true);
	_info.emplace_back("keepAspectRatio", &keepAspectRatio, true);
	_info.emplace_back("nonSquarePixelRatio", &nonSquarePixelRatio, false);
	_info.emplace_back("cursorInBlackBandsInFullscreen", &cursorInBlackBandsInFullscreen, false);
	_info.emplace_back("cursorInBlackBandsInWindow", &cursorInBlackBandsInWindow, true);
	_info.emplace_back("cursorInBlackBandsInBorderlessWindow", &cursorInBlackBandsInBorderlessWindow, false);
	_info.emplace_back("saveOrder", (int*)&saveOrder, SORT_DATE_DESC);
	_info.emplace_back("geoClockSpeed", &geoClockSpeed, 80);
	_info.emplace_back("dogfightSpeed", &dogfightSpeed, 30);
	_info.emplace_back("geoScrollSpeed", &geoScrollSpeed, 20);
	_info.emplace_back("geoDragScrollButton", &geoDragScrollButton, SDL_BUTTON_MIDDLE);
	_info.emplace_back("preferredMusic", (int*)&preferredMusic, MUSIC_AUTO);
	_info.emplace_back("preferredSound", (int*)&preferredSound, SOUND_AUTO);
	_info.emplace_back("preferredVideo", (int*)&preferredVideo, VIDEO_FMV);
	_info.emplace_back("musicAlwaysLoop", &musicAlwaysLoop, false);
	_info.emplace_back("touchEnabled", &touchEnabled, false);
	_info.emplace_back("rootWindowedMode", &rootWindowedMode, false);
	_info.emplace_back("lazyLoadResources", &lazyLoadResources, true);

	// advanced options
	_info.emplace_back("playIntro", &playIntro, true, "STR_PLAYINTRO", "STR_GENERAL");
	_info.emplace_back("autosave", &autosave, true, "STR_AUTOSAVE", "STR_GENERAL");
	_info.emplace_back("autosaveFrequency", &autosaveFrequency, 5, "STR_AUTOSAVE_FREQUENCY", "STR_GENERAL");
	_info.emplace_back("newSeedOnLoad", &newSeedOnLoad, false, "STR_NEWSEEDONLOAD", "STR_GENERAL");
	_info.emplace_back("mousewheelSpeed", &mousewheelSpeed, 3, "STR_MOUSEWHEEL_SPEED", "STR_GENERAL");
	_info.emplace_back("changeValueByMouseWheel", &changeValueByMouseWheel, 0, "STR_CHANGEVALUEBYMOUSEWHEEL", "STR_GENERAL");
	_info.emplace_back("soldierDiaries", &soldierDiaries, true);

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
	_info.emplace_back("maximizeInfoScreens", &maximizeInfoScreens, false, "STR_MAXIMIZE_INFO_SCREENS", "STR_GENERAL");
#endif

	_info.emplace_back("geoDragScrollInvert", &geoDragScrollInvert, false, "STR_DRAGSCROLLINVERT", "STR_GEOSCAPE"); // true drags away from the cursor, false drags towards (like a grab)
	_info.emplace_back("aggressiveRetaliation", &aggressiveRetaliation, false, "STR_AGGRESSIVERETALIATION", "STR_GEOSCAPE");
	_info.emplace_back("customInitialBase", &customInitialBase, false, "STR_CUSTOMINITIALBASE", "STR_GEOSCAPE");
	_info.emplace_back("allowBuildingQueue", &allowBuildingQueue, false, "STR_ALLOWBUILDINGQUEUE", "STR_GEOSCAPE");
	_info.emplace_back("craftLaunchAlways", &craftLaunchAlways, false, "STR_CRAFTLAUNCHALWAYS", "STR_GEOSCAPE");
	_info.emplace_back("storageLimitsEnforced", &storageLimitsEnforced, false, "STR_STORAGELIMITSENFORCED", "STR_GEOSCAPE");
	_info.emplace_back("canSellLiveAliens", &canSellLiveAliens, false, "STR_CANSELLLIVEALIENS", "STR_GEOSCAPE");
	_info.emplace_back("anytimePsiTraining", &anytimePsiTraining, false, "STR_ANYTIMEPSITRAINING", "STR_GEOSCAPE");
	_info.emplace_back("globeSeasons", &globeSeasons, false, "STR_GLOBESEASONS", "STR_GEOSCAPE");
	_info.emplace_back("psiStrengthEval", &psiStrengthEval, false, "STR_PSISTRENGTHEVAL", "STR_GEOSCAPE");
	_info.emplace_back("canTransferCraftsWhileAirborne", &canTransferCraftsWhileAirborne, false, "STR_CANTRANSFERCRAFTSWHILEAIRBORNE", "STR_GEOSCAPE"); // When the craft can reach the destination base with its fuel
	_info.emplace_back("retainCorpses", &retainCorpses, false, "STR_RETAINCORPSES", "STR_GEOSCAPE");
	_info.emplace_back("fieldPromotions", &fieldPromotions, false, "STR_FIELDPROMOTIONS", "STR_GEOSCAPE");
	_info.emplace_back("meetingPoint", &meetingPoint, false, "STR_MEETINGPOINT", "STR_GEOSCAPE");

	_info.emplace_back("battleDragScrollInvert", &battleDragScrollInvert, false, "STR_DRAGSCROLLINVERT", "STR_BATTLESCAPE"); // true drags away from the cursor, false drags towards (like a grab)
	_info.emplace_back("sneakyAI", &sneakyAI, false, "STR_SNEAKYAI", "STR_BATTLESCAPE");
	_info.emplace_back("battleUFOExtenderAccuracy", &battleUFOExtenderAccuracy, false, "STR_BATTLEUFOEXTENDERACCURACY", "STR_BATTLESCAPE");
	_info.emplace_back("showMoreStatsInInventoryView", &showMoreStatsInInventoryView, false, "STR_SHOWMORESTATSININVENTORYVIEW", "STR_BATTLESCAPE");
	_info.emplace_back("battleHairBleach", &battleHairBleach, true, "STR_BATTLEHAIRBLEACH", "STR_BATTLESCAPE");
	_info.emplace_back("battleInstantGrenade", &battleInstantGrenade, false, "STR_BATTLEINSTANTGRENADE", "STR_BATTLESCAPE");
	_info.emplace_back("includePrimeStateInSavedLayout", &includePrimeStateInSavedLayout, false, "STR_INCLUDE_PRIMESTATE_IN_SAVED_LAYOUT", "STR_BATTLESCAPE");
	_info.emplace_back("battleExplosionHeight", &battleExplosionHeight, 0, "STR_BATTLEEXPLOSIONHEIGHT", "STR_BATTLESCAPE");
	_info.emplace_back("battleAutoEnd", &battleAutoEnd, false, "STR_BATTLEAUTOEND", "STR_BATTLESCAPE");
	_info.emplace_back("battleSmoothCamera", &battleSmoothCamera, false, "STR_BATTLESMOOTHCAMERA", "STR_BATTLESCAPE");
	_info.emplace_back("disableAutoEquip", &disableAutoEquip, false, "STR_DISABLEAUTOEQUIP", "STR_BATTLESCAPE");
	_info.emplace_back("battleConfirmFireMode", &battleConfirmFireMode, false, "STR_BATTLECONFIRMFIREMODE", "STR_BATTLESCAPE");
	_info.emplace_back("weaponSelfDestruction", &weaponSelfDestruction, false, "STR_WEAPONSELFDESTRUCTION", "STR_BATTLESCAPE");
	_info.emplace_back("allowPsionicCapture", &allowPsionicCapture, false, "STR_ALLOWPSIONICCAPTURE", "STR_BATTLESCAPE");
	_info.emplace_back("allowPsiStrengthImprovement", &allowPsiStrengthImprovement, false, "STR_ALLOWPSISTRENGTHIMPROVEMENT", "STR_BATTLESCAPE");
	_info.emplace_back("strafe", &strafe, false, "STR_STRAFE", "STR_BATTLESCAPE");
	_info.emplace_back("forceFire", &forceFire, true, "STR_FORCE_FIRE", "STR_BATTLESCAPE");
	_info.emplace_back("skipNextTurnScreen", &skipNextTurnScreen, false, "STR_SKIPNEXTTURNSCREEN", "STR_BATTLESCAPE");
	_info.emplace_back("noAlienPanicMessages", &noAlienPanicMessages, false, "STR_NOALIENPANICMESSAGES", "STR_BATTLESCAPE");
	_info.emplace_back("alienBleeding", &alienBleeding, false, "STR_ALIENBLEEDING", "STR_BATTLESCAPE");

	// controls
	_info.emplace_back("keyOk", &keyOk, SDLK_RETURN, "STR_OK", "STR_GENERAL");
	_info.emplace_back("keyCancel", &keyCancel, SDLK_ESCAPE, "STR_CANCEL", "STR_GENERAL");
	_info.emplace_back("keyScreenshot", &keyScreenshot, SDLK_F12, "STR_SCREENSHOT", "STR_GENERAL");
	_info.emplace_back("keyFps", &keyFps, SDLK_F7, "STR_FPS_COUNTER", "STR_GENERAL");
	_info.emplace_back("keyQuickSave", &keyQuickSave, SDLK_F5, "STR_QUICK_SAVE", "STR_GENERAL");
	_info.emplace_back("keyQuickLoad", &keyQuickLoad, SDLK_F9, "STR_QUICK_LOAD", "STR_GENERAL");
	_info.emplace_back("keyGeoLeft", &keyGeoLeft, SDLK_LEFT, "STR_ROTATE_LEFT", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoRight", &keyGeoRight, SDLK_RIGHT, "STR_ROTATE_RIGHT", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoUp", &keyGeoUp, SDLK_UP, "STR_ROTATE_UP", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoDown", &keyGeoDown, SDLK_DOWN, "STR_ROTATE_DOWN", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoZoomIn", &keyGeoZoomIn, SDLK_PLUS, "STR_ZOOM_IN", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoZoomOut", &keyGeoZoomOut, SDLK_MINUS, "STR_ZOOM_OUT", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoSpeed1", &keyGeoSpeed1, SDLK_1, "STR_5_SECONDS", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoSpeed2", &keyGeoSpeed2, SDLK_2, "STR_1_MINUTE", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoSpeed3", &keyGeoSpeed3, SDLK_3, "STR_5_MINUTES", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoSpeed4", &keyGeoSpeed4, SDLK_4, "STR_30_MINUTES", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoSpeed5", &keyGeoSpeed5, SDLK_5, "STR_1_HOUR", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoSpeed6", &keyGeoSpeed6, SDLK_6, "STR_1_DAY", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoIntercept", &keyGeoIntercept, SDLK_i, "STR_INTERCEPT", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoBases", &keyGeoBases, SDLK_b, "STR_BASES", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoGraphs", &keyGeoGraphs, SDLK_g, "STR_GRAPHS", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoUfopedia", &keyGeoUfopedia, SDLK_u, "STR_UFOPAEDIA_UC", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoOptions", &keyGeoOptions, SDLK_ESCAPE, "STR_OPTIONS_UC", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoFunding", &keyGeoFunding, SDLK_f, "STR_FUNDING_UC", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoToggleDetail", &keyGeoToggleDetail, SDLK_TAB, "STR_TOGGLE_COUNTRY_DETAIL", "STR_GEOSCAPE");
	_info.emplace_back("keyGeoToggleRadar", &keyGeoToggleRadar, SDLK_r, "STR_TOGGLE_RADAR_RANGES", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect1", &keyBaseSelect1, SDLK_1, "STR_SELECT_BASE_1", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect2", &keyBaseSelect2, SDLK_2, "STR_SELECT_BASE_2", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect3", &keyBaseSelect3, SDLK_3, "STR_SELECT_BASE_3", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect4", &keyBaseSelect4, SDLK_4, "STR_SELECT_BASE_4", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect5", &keyBaseSelect5, SDLK_5, "STR_SELECT_BASE_5", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect6", &keyBaseSelect6, SDLK_6, "STR_SELECT_BASE_6", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect7", &keyBaseSelect7, SDLK_7, "STR_SELECT_BASE_7", "STR_GEOSCAPE");
	_info.emplace_back("keyBaseSelect8", &keyBaseSelect8, SDLK_8, "STR_SELECT_BASE_8", "STR_GEOSCAPE");
	_info.emplace_back("keyBattleLeft", &keyBattleLeft, SDLK_LEFT, "STR_SCROLL_LEFT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleRight", &keyBattleRight, SDLK_RIGHT, "STR_SCROLL_RIGHT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleUp", &keyBattleUp, SDLK_UP, "STR_SCROLL_UP", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleDown", &keyBattleDown, SDLK_DOWN, "STR_SCROLL_DOWN", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleLevelUp", &keyBattleLevelUp, SDLK_PAGEUP, "STR_VIEW_LEVEL_ABOVE", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleLevelDown", &keyBattleLevelDown, SDLK_PAGEDOWN, "STR_VIEW_LEVEL_BELOW", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterUnit", &keyBattleCenterUnit, SDLK_HOME, "STR_CENTER_SELECTED_UNIT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattlePrevUnit", &keyBattlePrevUnit, SDLK_LSHIFT, "STR_PREVIOUS_UNIT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleNextUnit", &keyBattleNextUnit, SDLK_TAB, "STR_NEXT_UNIT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleDeselectUnit", &keyBattleDeselectUnit, SDLK_BACKSLASH, "STR_DESELECT_UNIT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleUseLeftHand", &keyBattleUseLeftHand, SDLK_q, "STR_USE_LEFT_HAND", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleUseRightHand", &keyBattleUseRightHand, SDLK_e, "STR_USE_RIGHT_HAND", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleInventory", &keyBattleInventory, SDLK_i, "STR_INVENTORY", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleMap", &keyBattleMap, SDLK_m, "STR_MINIMAP", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleOptions", &keyBattleOptions, SDLK_ESCAPE, "STR_OPTIONS", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleEndTurn", &keyBattleEndTurn, SDLK_BACKSPACE, "STR_END_TURN", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleAbort", &keyBattleAbort, SDLK_a, "STR_ABORT_MISSION", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleStats", &keyBattleStats, SDLK_s, "STR_UNIT_STATS", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleKneel", &keyBattleKneel, SDLK_k, "STR_KNEEL", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleReload", &keyBattleReload, SDLK_r, "STR_RELOAD", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattlePersonalLighting", &keyBattlePersonalLighting, SDLK_l, "STR_TOGGLE_PERSONAL_LIGHTING", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleReserveNone", &keyBattleReserveNone, SDLK_F1, "STR_DONT_RESERVE_TIME_UNITS", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleReserveSnap", &keyBattleReserveSnap, SDLK_F2, "STR_RESERVE_TIME_UNITS_FOR_SNAP_SHOT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleReserveAimed", &keyBattleReserveAimed, SDLK_F3, "STR_RESERVE_TIME_UNITS_FOR_AIMED_SHOT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleReserveAuto", &keyBattleReserveAuto, SDLK_F4, "STR_RESERVE_TIME_UNITS_FOR_AUTO_SHOT", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleReserveKneel", &keyBattleReserveKneel, SDLK_j, "STR_RESERVE_TIME_UNITS_FOR_KNEEL", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleZeroTUs", &keyBattleZeroTUs, SDLK_DELETE, "STR_EXPEND_ALL_TIME_UNITS", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy1", &keyBattleCenterEnemy1, SDLK_1, "STR_CENTER_ON_ENEMY_1", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy2", &keyBattleCenterEnemy2, SDLK_2, "STR_CENTER_ON_ENEMY_2", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy3", &keyBattleCenterEnemy3, SDLK_3, "STR_CENTER_ON_ENEMY_3", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy4", &keyBattleCenterEnemy4, SDLK_4, "STR_CENTER_ON_ENEMY_4", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy5", &keyBattleCenterEnemy5, SDLK_5, "STR_CENTER_ON_ENEMY_5", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy6", &keyBattleCenterEnemy6, SDLK_6, "STR_CENTER_ON_ENEMY_6", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy7", &keyBattleCenterEnemy7, SDLK_7, "STR_CENTER_ON_ENEMY_7", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy8", &keyBattleCenterEnemy8, SDLK_8, "STR_CENTER_ON_ENEMY_8", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy9", &keyBattleCenterEnemy9, SDLK_9, "STR_CENTER_ON_ENEMY_9", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleCenterEnemy10", &keyBattleCenterEnemy10, SDLK_0, "STR_CENTER_ON_ENEMY_10", "STR_BATTLESCAPE");
	_info.emplace_back("keyBattleVoxelView", &keyBattleVoxelView, SDLK_F10, "STR_SAVE_VOXEL_VIEW", "STR_BATTLESCAPE");
	_info.emplace_back("keyInvCreateTemplate", &keyInvCreateTemplate, SDLK_c, "STR_CREATE_INVENTORY_TEMPLATE", "STR_BATTLESCAPE");
	_info.emplace_back("keyInvApplyTemplate", &keyInvApplyTemplate, SDLK_v, "STR_APPLY_INVENTORY_TEMPLATE", "STR_BATTLESCAPE");
	_info.emplace_back("keyInvClear", &keyInvClear, SDLK_x, "STR_CLEAR_INVENTORY", "STR_BATTLESCAPE");
	_info.emplace_back("keyInvAutoEquip", &keyInvAutoEquip, SDLK_z, "STR_AUTO_EQUIP", "STR_BATTLESCAPE");

#ifdef __MORPHOS__
	_info.push_back(OptionInfo("FPS", &FPS, 15, "STR_FPS_LIMIT", "STR_GENERAL"));
	_info.push_back(OptionInfo("FPSInactive", &FPSInactive, 15, "STR_FPS_INACTIVE_LIMIT", "STR_GENERAL"));
#else
	_info.emplace_back("FPS", &FPS, 60, "STR_FPS_LIMIT", "STR_GENERAL");
	_info.emplace_back("FPSInactive", &FPSInactive, 30, "STR_FPS_INACTIVE_LIMIT", "STR_GENERAL");
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
	return (CrossPlatform::folderExists(dataGameFolder)	&& CrossPlatform::getFolderContents(dataGameFolder).size() > 8)
	    || (CrossPlatform::folderExists(userGameFolder)	&& CrossPlatform::getFolderContents(userGameFolder).size() > 8);
}

static bool _ufoIsInstalled()
{
	return _gameIsInstalled("UFO");
}

static bool _tftdIsInstalled()
{
	return _gameIsInstalled("TFTD");
}

static void _setDefaultMods()
{
	bool haveUfo = _ufoIsInstalled();
	if (haveUfo)
	{
		mods.emplace_back("xcom1", true);
	}

	if (_tftdIsInstalled())
	{
		mods.emplace_back("xcom2", !haveUfo);
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
		std::string modPath = modsDir + CrossPlatform::PATH_SEPARATOR + *i;
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
			Log(LOG_VERBOSE) << metadataPath << " not found; using default values for mod: " << *i;
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
 * @return Do we start the game?
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
	if (file)
	{
		fflush(file);
		fclose(file);
	}
	else
	{
		Log(LOG_WARNING) << "Couldn't create log file, switching to stderr";
	}

	Log(LOG_INFO) << "OpenXcom Version: " << OPENXCOM_VERSION_SHORT << OPENXCOM_VERSION_GIT;
#ifdef _WIN32
	Log(LOG_INFO) << "Platform: Windows";
#elif __APPLE__
	Log(LOG_INFO) << "Platform: OSX";
#elif  __ANDROID_API__
	Log(LOG_INFO) << "Platform: Android";
#else
	Log(LOG_INFO) << "Platform: Unix-like";
#endif

	Log(LOG_INFO) << "Data folder is: " << _dataFolder;
	Log(LOG_INFO) << "Data search is: ";
	for (std::vector<std::string>::iterator i = _dataList.begin(); i != _dataList.end(); ++i)
	{
		Log(LOG_INFO) << "- " << *i;
	}
	Log(LOG_INFO) << "User folder is: " << _userFolder;
	Log(LOG_INFO) << "Config folder is: " << _configFolder;
	Log(LOG_INFO) << "Options loaded successfully.";

	return true;
}

void updateMods()
{
	// pick up stuff in common before-hand
	FileMap::load("common", CrossPlatform::searchDataFolder("common"), true);

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
			Log(LOG_VERBOSE) << "removing references to missing mod: " << i->first;
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
			throw Exception("No X-COM installations found");
		}
		else
		{
			Log(LOG_INFO) << "no master already active; activating " << inactiveMaster;
			std::find(mods.begin(), mods.end(), std::pair<std::string, bool>(inactiveMaster, false))->second = true;
			_masterMod = inactiveMaster;
		}
	}
	else
	{
		_masterMod = activeMaster;
	}

	updateReservedSpace();
	mapResources();
	userSplitMasters();
}

/**
 * Gets the currently active master mod.
 * @return Mod id.
 */
std::string getActiveMaster()
{
	return _masterMod;
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
		std::map<std::string, ModInfo>::const_iterator it = _modInfos.find(modInfo.getMaster());
		if (it != _modInfos.end())
		{
			const ModInfo &masterInfo = it->second;
			_loadMod(masterInfo, circDepCheck);
		}
		else
		{
			throw Exception(modInfo.getId() + " mod requires " + modInfo.getMaster() + " master");
		}
	}
}

void updateReservedSpace()
{
	Log(LOG_VERBOSE) << "Updating reservedSpace for master mods if necessary...";

	Log(LOG_VERBOSE) << "_masterMod = " << _masterMod;

	int maxSize = 1;
	for (std::vector< std::pair<std::string, bool> >::reverse_iterator i = mods.rbegin(); i != mods.rend(); ++i)
	{
		if (!i->second)
		{
			Log(LOG_VERBOSE) << "skipping inactive mod: " << i->first;
			continue;
		}

		const ModInfo &modInfo = _modInfos.find(i->first)->second;
		if (!modInfo.canActivate(_masterMod))
		{
			Log(LOG_VERBOSE) << "skipping mod for non-current master: " << i->first << "(" << modInfo.getMaster() << " != " << _masterMod << ")";
			continue;
		}

		if (modInfo.getReservedSpace() > maxSize)
		{
			maxSize = modInfo.getReservedSpace();
		}
	}

	if (maxSize > 1)
	{
		// Small hack: update ALL masters, not only active master!
		// this is because, there can be a hierarchy of multiple masters (e.g. xcom1 master > fluffyUnicorns master > some fluffyUnicorns mod)
		// and the one that needs to be updated is actually the "root", i.e. xcom1 master
		for (std::map<std::string, ModInfo>::iterator i = _modInfos.begin(); i != _modInfos.end(); ++i)
		{
			if (i->second.isMaster())
			{
				if (i->second.getReservedSpace() < maxSize)
				{
					i->second.setReservedSpace(maxSize);
					Log(LOG_INFO) << "reservedSpace for: " << i->first << " updated to: " << i->second.getReservedSpace();
				}
				else
				{
					Log(LOG_INFO) << "reservedSpace for: " << i->first << " is: " << i->second.getReservedSpace();
				}
			}
		}
	}
}

void mapResources()
{
	Log(LOG_INFO) << "Mapping resource files...";
	FileMap::clear();

	for (std::vector< std::pair<std::string, bool> >::reverse_iterator i = mods.rbegin(); i != mods.rend(); ++i)
	{
		if (!i->second)
		{
			Log(LOG_VERBOSE) << "skipping inactive mod: " << i->first;
			continue;
		}

		const ModInfo &modInfo = _modInfos.find(i->first)->second;
		if (!modInfo.canActivate(_masterMod))
		{
			Log(LOG_VERBOSE) << "skipping mod for non-current master: " << i->first << "(" << modInfo.getMaster() << " != " << _masterMod << ")";
			continue;
		}

		std::set<std::string> circDepCheck;
		_loadMod(modInfo, circDepCheck);
	}
	// TODO: Figure out why we still need to check common here
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

		if (_configFolder.empty())
		{
			_configFolder = CrossPlatform::findConfigFolder();
		}

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
		// create mod folder if it doesn't already exist
		CrossPlatform::createFolder(_userFolder + "mods");
	}

	if (_configFolder.empty())
	{
		_configFolder = _userFolder;
	}
}

/**
 * Splits the game's User folder by master mod,
 * creating a subfolder for each one and moving
 * the apppropriate user data among them.
 */
void userSplitMasters()
{
	// get list of master mods
	std::vector<std::string> masters;
	for (std::map<std::string, ModInfo>::const_iterator i = _modInfos.begin(); i != _modInfos.end(); ++i)
	{
		if (i->second.isMaster())
		{
			masters.push_back(i->first);
		}
	}

	// create master subfolders if they don't already exist
	std::vector<std::string> saves;
	for (std::vector<std::string>::const_iterator i = masters.begin(); i != masters.end(); ++i)
	{
		std::string masterFolder = _userFolder + (*i);
		if (!CrossPlatform::folderExists(masterFolder))
		{
			CrossPlatform::createFolder(masterFolder);
			// move any old saves to the appropriate folders
			if (saves.empty())
			{
				saves = CrossPlatform::getFolderContents(_userFolder, "sav");
				std::vector<std::string> autosaves = CrossPlatform::getFolderContents(_userFolder, "asav");
				saves.insert(saves.end(), autosaves.begin(), autosaves.end());
			}
			for (std::vector<std::string>::iterator j = saves.begin(); j != saves.end();)
			{
				std::string srcFile = _userFolder + (*j);
				YAML::Node doc = YAML::LoadFile(srcFile);
				std::vector<std::string> srcMods = doc["mods"].as<std::vector< std::string> >(std::vector<std::string>());
				if (std::find(srcMods.begin(), srcMods.end(), (*i)) != srcMods.end())
				{
					std::string dstFile = masterFolder + CrossPlatform::PATH_SEPARATOR + (*j);
					CrossPlatform::moveFile(srcFile, dstFile);
					j = saves.erase(j);
				}
				else
				{
					++j;
				}
			}
		}
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
 * @return Was the loading successful?
 */
bool load(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	try
	{
		YAML::Node doc = YAML::LoadFile(s);
		// Ignore old options files
		if (doc["options"]["NewBattleMission"])
		{
			return false;
		}
		for (std::vector<OptionInfo>::iterator i = _info.begin(); i != _info.end(); ++i)
		{
			i->load(doc["options"]);
		}

		mods.clear();
		for (YAML::const_iterator i = doc["mods"].begin(); i != doc["mods"].end(); ++i)
		{
			std::string id = (*i)["id"].as<std::string>();
			bool active = (*i)["active"].as<bool>(false);
            mods.emplace_back(std::move(id), active);
		}
		if (mods.empty())
		{
			_setDefaultMods();
		}
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_WARNING) << e.what();
		return false;
	}
	return true;
}

void writeNode(const YAML::Node& node, YAML::Emitter& emitter)
{
	switch (node.Type())
	{
		case YAML::NodeType::Sequence:
		{
			emitter << YAML::BeginSeq;
			for (size_t i = 0; i < node.size(); i++)
			{
				writeNode(node[i], emitter);
			}
			emitter << YAML::EndSeq;
			break;
		}
		case YAML::NodeType::Map:
		{
			emitter << YAML::BeginMap;

			// First collect all the keys
			std::vector<std::string> keys(node.size());
			int key_it = 0;
			for (YAML::const_iterator it = node.begin(); it != node.end(); ++it)
			{
				keys[key_it++] = it->first.as<std::string>();
			}

			// Then sort them
			std::sort(keys.begin(), keys.end());

			// Then emit all the entries in sorted order.
			for(size_t i = 0; i < keys.size(); i++)
			{
				emitter << YAML::Key;
				emitter << keys[i];
				emitter << YAML::Value;
				writeNode(node[keys[i]], emitter);
			}
			emitter << YAML::EndMap;
			break;
		}
		default:
			emitter << node;
			break;
	}
}

/**
 * Saves options to a YAML file.
 * @param filename YAML filename.
 * @return Was the saving successful?
 */
bool save(const std::string &filename)
{
	std::string s = _configFolder + filename + ".cfg";
	std::ofstream sav(s.c_str());
	if (!sav)
	{
		Log(LOG_WARNING) << "Failed to save " << filename << ".cfg";
		return false;
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

		for (std::vector< std::pair<std::string, bool> >::iterator i = mods.begin(); i != mods.end(); ++i)
		{
			YAML::Node mod;
			mod["id"] = i->first;
			mod["active"] = i->second;
			doc["mods"].push_back(mod);
		}

		writeNode(doc, out);

		sav << out.c_str() << std::endl;
	}
	catch (YAML::Exception &e)
	{
		Log(LOG_WARNING) << e.what();
		return false;
	}
	sav.close();
	if (!sav)
	{
		Log(LOG_WARNING) << "Failed to save " << filename << ".cfg";
		return false;
	}
	return true;
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
 * Returns the game's User folder for the
 * currently loaded master mod.
 * @return Full path to User folder.
 */
std::string getMasterUserFolder()
{
	return _userFolder + _masterMod + CrossPlatform::PATH_SEPARATOR;
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
 * Returns a list of currently active mods.
 * They must be enabled and activable.
 * @sa ModInfo::canActivate
 * @return List of info for the active mods.
 */
std::vector<const ModInfo *> getActiveMods()
{
	std::vector<const ModInfo*> activeMods;
	for (std::vector< std::pair<std::string, bool> >::iterator i = mods.begin(); i != mods.end(); ++i)
	{
		if (i->second)
		{
			const ModInfo *info = &_modInfos.at(i->first);
			if (info->canActivate(_masterMod))
			{
				activeMods.push_back(info);
			}
		}
	}
	return activeMods;
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
	Options::newRootWindowedMode = Options::rootWindowedMode;
	Options::newWindowedModePositionX = Options::windowedModePositionX;
	Options::newWindowedModePositionY = Options::windowedModePositionY;
	Options::newFullscreen = Options::fullscreen;
	Options::newAllowResize = Options::allowResize;
	Options::newBorderless = Options::borderless;
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
	std::swap(rootWindowedMode, newRootWindowedMode);
	std::swap(windowedModePositionX, newWindowedModePositionX);
	std::swap(windowedModePositionY, newWindowedModePositionY);
	std::swap(fullscreen, newFullscreen);
	std::swap(allowResize, newAllowResize);
	std::swap(borderless, newBorderless);
}

}

}
