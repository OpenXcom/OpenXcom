set ( basescape_src
  Basescape/BaseView.cpp
  Basescape/BaseView.h
  Basescape/CraftSoldiersState.cpp
  Basescape/CraftSoldiersState.h
  Basescape/TransfersState.cpp
  Basescape/TransfersState.h
  Basescape/StoresState.cpp
  Basescape/StoresState.h
  Basescape/SellState.h
  Basescape/SellState.cpp
  Basescape/CraftInfoState.cpp
  Basescape/CraftInfoState.h
  Basescape/PlaceLiftState.h
  Basescape/PlaceLiftState.cpp
  Basescape/DismantleFacilityState.h
  Basescape/DismantleFacilityState.cpp
  Basescape/SoldierInfoState.h
  Basescape/SoldierInfoState.cpp
  Basescape/CraftsState.cpp
  Basescape/CraftsState.h
  Basescape/PlaceFacilityState.h
  Basescape/PlaceFacilityState.cpp
  Basescape/TransferItemsState.cpp
  Basescape/TransferItemsState.h
  Basescape/CraftArmorState.h
  Basescape/CraftArmorState.cpp
  Basescape/MonthlyCostsState.cpp
  Basescape/MonthlyCostsState.h
  Basescape/CraftEquipmentState.cpp
  Basescape/CraftEquipmentState.h
  Basescape/BaseInfoState.h
  Basescape/BaseInfoState.cpp
  Basescape/ResearchState.h
  Basescape/ResearchState.cpp
  Basescape/PurchaseState.cpp
  Basescape/PurchaseState.h
  Basescape/TransferBaseState.cpp
  Basescape/TransferBaseState.h
  Basescape/CraftWeaponsState.h
  Basescape/CraftWeaponsState.cpp
  Basescape/ManufactureState.h
  Basescape/ManufactureState.cpp
  Basescape/BuildFacilitiesState.h
  Basescape/BuildFacilitiesState.cpp
  Basescape/MiniBaseView.cpp
  Basescape/MiniBaseView.h
  Basescape/BasescapeState.cpp
  Basescape/BasescapeState.h
  Basescape/TransferConfirmState.cpp
  Basescape/TransferConfirmState.h
  Basescape/SoldierArmorState.h
  Basescape/SoldierArmorState.cpp
  Basescape/SoldiersState.cpp
  Basescape/SoldiersState.h
  Basescape/NewResearchListState.h
  Basescape/NewResearchListState.cpp
  Basescape/ResearchInfoState.h
  Basescape/ResearchInfoState.cpp
  Basescape/NewManufactureListState.h
  Basescape/NewManufactureListState.cpp
  Basescape/ManufactureStartState.h
  Basescape/ManufactureStartState.cpp
  Basescape/ManufactureInfoState.h
  Basescape/ManufactureInfoState.cpp
  Basescape/PlaceStartFacilityState.h
  Basescape/PlaceStartFacilityState.cpp
  Basescape/SelectStartFacilityState.h
  Basescape/SelectStartFacilityState.cpp
)

set ( battlescape_src
  Battlescape/ActionMenuState.cpp
  Battlescape/ActionMenuState.h
  Battlescape/PathfindingNode.cpp
  Battlescape/PathfindingNode.h
  Battlescape/Position.h
  Battlescape/Position.cpp
  Battlescape/Map.h
  Battlescape/Map.cpp
  Battlescape/Pathfinding.h
  Battlescape/Pathfinding.cpp
  Battlescape/ExplosionBState.h
  Battlescape/ExplosionBState.cpp
  Battlescape/UnitTurnBState.cpp
  Battlescape/UnitTurnBState.h
  Battlescape/ActionMenuItem.h
  Battlescape/ActionMenuItem.cpp
  Battlescape/BriefingState.h
  Battlescape/BriefingState.cpp
  Battlescape/ProjectileFlyBState.cpp
  Battlescape/ProjectileFlyBState.h
  Battlescape/PrimeGrenadeState.cpp
  Battlescape/PrimeGrenadeState.h
  Battlescape/UnitInfoState.h
  Battlescape/UnitInfoState.cpp
  Battlescape/BattlescapeState.cpp
  Battlescape/BattlescapeState.h
  Battlescape/BattlescapeGenerator.h
  Battlescape/BattlescapeGenerator.cpp
  Battlescape/BulletSprite.h
  Battlescape/BulletSprite.cpp
  Battlescape/Camera.h
  Battlescape/Camera.cpp
  Battlescape/Projectile.cpp
  Battlescape/Projectile.h
  Battlescape/UnitDieBState.h
  Battlescape/UnitDieBState.cpp
  Battlescape/Explosion.cpp
  Battlescape/Explosion.h
  Battlescape/InventoryState.cpp
  Battlescape/InventoryState.h
  Battlescape/UnitSprite.h
  Battlescape/UnitSprite.cpp
  Battlescape/BattleState.h
  Battlescape/BattleState.cpp
  Battlescape/UnitWalkBState.h
  Battlescape/UnitWalkBState.cpp
  Battlescape/NextTurnState.cpp
  Battlescape/NextTurnState.h
  Battlescape/NoContainmentState.cpp
  Battlescape/NoContainmentState.h
  Battlescape/AggroBAIState.cpp
  Battlescape/AggroBAIState.h
  Battlescape/BattleAIState.cpp
  Battlescape/BattleAIState.h
  Battlescape/BattlescapeMessage.cpp
  Battlescape/BattlescapeMessage.h
  Battlescape/Inventory.cpp
  Battlescape/Inventory.h
  Battlescape/PatrolBAIState.cpp
  Battlescape/PatrolBAIState.h
  Battlescape/WarningMessage.cpp
  Battlescape/WarningMessage.h
  Battlescape/BattlescapeOptionsState.cpp
  Battlescape/BattlescapeOptionsState.h
  Battlescape/AbortMissionState.cpp
  Battlescape/AbortMissionState.h
  Battlescape/DebriefingState.cpp
  Battlescape/DebriefingState.h
  Battlescape/InfoboxState.cpp
  Battlescape/InfoboxState.h
  Battlescape/InfoboxOKState.cpp
  Battlescape/InfoboxOKState.h
  Battlescape/TileEngine.cpp
  Battlescape/TileEngine.h
  Battlescape/MiniMapView.h
  Battlescape/MiniMapView.cpp
  Battlescape/MiniMapState.h
  Battlescape/MiniMapState.cpp
  Battlescape/MedikitState.cpp
  Battlescape/MedikitState.h
  Battlescape/MedikitView.cpp
  Battlescape/MedikitView.h
  Battlescape/ScannerState.cpp
  Battlescape/ScannerState.h
  Battlescape/ScannerView.cpp
  Battlescape/ScannerView.h
  Battlescape/PromotionsState.cpp
  Battlescape/PromotionsState.h
  Battlescape/BattlescapeGame.cpp
  Battlescape/BattlescapeGame.h
  Battlescape/CannotReequipState.cpp
  Battlescape/CannotReequipState.h
  Battlescape/PathfindingOpenSet.cpp
  Battlescape/PathfindingOpenSet.h
)

set ( engine_src
  Engine/State.h
  Engine/State.cpp
  Engine/CatFile.cpp
  Engine/CatFile.h
  Engine/RNG.h
  Engine/RNG.cpp
  Engine/Exception.h
  Engine/Exception.cpp
  Engine/Music.h
  Engine/Music.cpp
  Engine/Timer.cpp
  Engine/Timer.h
  Engine/Language.cpp
  Engine/Language.h
  Engine/Game.cpp
  Engine/Game.h
  Engine/Action.cpp
  Engine/Action.h
  Engine/Palette.cpp
  Engine/Palette.h
  Engine/SoundSet.cpp
  Engine/SoundSet.h
  Engine/GMCat.h
  Engine/GMCat.cpp
  Engine/InteractiveSurface.cpp
  Engine/InteractiveSurface.h
  Engine/Surface.cpp
  Engine/Surface.h
  Engine/Font.cpp
  Engine/Font.h
  Engine/Options.cpp
  Engine/Options.h
  Engine/CrossPlatform.cpp
  Engine/CrossPlatform.h
  Engine/Sound.h
  Engine/Sound.cpp
  Engine/SurfaceSet.cpp
  Engine/SurfaceSet.h
  Engine/Screen.cpp
  Engine/Screen.h
  Engine/Logger.h
)

set ( geoscape_src
  Geoscape/GeoscapeCraftState.cpp
  Geoscape/GeoscapeCraftState.h
  Geoscape/UfoDetectedState.h
  Geoscape/UfoDetectedState.cpp
  Geoscape/UfoHyperDetectedState.h
  Geoscape/UfoHyperDetectedState.cpp
  Geoscape/InterceptState.h
  Geoscape/InterceptState.cpp
  Geoscape/Polyline.h
  Geoscape/Polyline.cpp
  Geoscape/ConfirmNewBaseState.h
  Geoscape/ConfirmNewBaseState.cpp
  Geoscape/ConfirmDestinationState.h
  Geoscape/ConfirmDestinationState.cpp
  Geoscape/Globe.cpp
  Geoscape/Globe.h
  Geoscape/SelectDestinationState.cpp
  Geoscape/SelectDestinationState.h
  Geoscape/FundingState.h
  Geoscape/FundingState.cpp
  Geoscape/BuildNewBaseState.h
  Geoscape/BuildNewBaseState.cpp
  Geoscape/MonthlyReportState.cpp
  Geoscape/MonthlyReportState.h
  Geoscape/CraftErrorState.cpp
  Geoscape/CraftErrorState.h
  Geoscape/ProductionCompleteState.h
  Geoscape/ProductionCompleteState.cpp
  Geoscape/TargetInfoState.h
  Geoscape/TargetInfoState.cpp
  Geoscape/LowFuelState.h
  Geoscape/LowFuelState.cpp
  Geoscape/CraftPatrolState.cpp
  Geoscape/CraftPatrolState.h
  Geoscape/Polygon.h
  Geoscape/Polygon.cpp
  Geoscape/UfoLostState.cpp
  Geoscape/UfoLostState.h
  Geoscape/AbandonGameState.cpp
  Geoscape/AbandonGameState.h
  Geoscape/GraphsState.cpp
  Geoscape/GraphsState.h
  Geoscape/ConfirmLandingState.cpp
  Geoscape/ConfirmLandingState.h
  Geoscape/GeoscapeOptionsState.h
  Geoscape/GeoscapeOptionsState.cpp
  Geoscape/BaseNameState.cpp
  Geoscape/BaseNameState.h
  Geoscape/BaseDefenseState.cpp
  Geoscape/BaseDefenseState.h
  Geoscape/MultipleTargetsState.cpp
  Geoscape/MultipleTargetsState.h
  Geoscape/ItemsArrivingState.cpp
  Geoscape/ItemsArrivingState.h
  Geoscape/DogfightState.cpp
  Geoscape/DogfightState.h
  Geoscape/GeoscapeState.cpp
  Geoscape/GeoscapeState.h
  Geoscape/ResearchCompleteState.h
  Geoscape/ResearchCompleteState.cpp
  Geoscape/NewPossibleResearchState.h
  Geoscape/NewPossibleResearchState.cpp
  Geoscape/AllocatePsiTrainingState.h
  Geoscape/AllocatePsiTrainingState.cpp
  Geoscape/PsiTrainingState.h
  Geoscape/PsiTrainingState.cpp
  Geoscape/AlienBaseState.h
  Geoscape/AlienBaseState.cpp
  Geoscape/AlienTerrorState.h
  Geoscape/AlienTerrorState.cpp
  Geoscape/NewPossibleManufactureState.h
  Geoscape/NewPossibleManufactureState.cpp
  Geoscape/VictoryState.h
  Geoscape/VictoryState.cpp
  Geoscape/DefeatState.h
  Geoscape/DefeatState.cpp
)

set ( interface_src
  Interface/Window.cpp
  Interface/Window.h
  Interface/NumberText.cpp
  Interface/NumberText.h
  Interface/ArrowButton.cpp
  Interface/ArrowButton.h
  Interface/TextButton.cpp
  Interface/TextButton.h
  Interface/Text.h
  Interface/Text.cpp
  Interface/Bar.h
  Interface/Bar.cpp
  Interface/FpsCounter.h
  Interface/FpsCounter.cpp
  Interface/ImageButton.h
  Interface/ImageButton.cpp
  Interface/TextEdit.cpp
  Interface/TextEdit.h
  Interface/TextList.h
  Interface/TextList.cpp
  Interface/Cursor.h
  Interface/Cursor.cpp
)

set ( menu_src
  Menu/StartState.cpp
  Menu/StartState.h
  Menu/NoteState.cpp
  Menu/NoteState.h
  Menu/MainMenuState.cpp
  Menu/MainMenuState.h
  Menu/TestState.h
  Menu/TestState.cpp
  Menu/LanguageState.h
  Menu/LanguageState.cpp
  Menu/SaveState.cpp
  Menu/SaveState.h
  Menu/LoadState.cpp
  Menu/LoadState.h
  Menu/NewGameState.cpp
  Menu/NewGameState.h
  Menu/NewBattleState.cpp
  Menu/NewBattleState.h
  Menu/OptionsState.cpp
  Menu/OptionsState.h
  Menu/ErrorMessageState.cpp
  Menu/ErrorMessageState.h
  Menu/SavedGameState.cpp
  Menu/SavedGameState.h
)

set ( resource_src
  Resource/ResourcePack.h
  Resource/ResourcePack.cpp
  Resource/XcomResourcePack.cpp
  Resource/XcomResourcePack.h
)

set ( ruleset_src
  Ruleset/RuleCraft.cpp
  Ruleset/RuleCraft.h
  Ruleset/Armor.cpp
  Ruleset/Armor.h
  Ruleset/MapDataSet.h
  Ruleset/MapDataSet.cpp
  Ruleset/RuleSoldier.h
  Ruleset/RuleSoldier.cpp
  Ruleset/RuleRegion.cpp
  Ruleset/RuleRegion.h
  Ruleset/RuleItem.h
  Ruleset/RuleItem.cpp
  Ruleset/RuleCraftWeapon.cpp
  Ruleset/RuleCraftWeapon.h
  Ruleset/RuleInventory.h
  Ruleset/RuleInventory.cpp
  Ruleset/MapBlock.h
  Ruleset/MapBlock.cpp
  Ruleset/RuleBaseFacility.cpp
  Ruleset/RuleBaseFacility.h
  Ruleset/SoldierNamePool.h
  Ruleset/SoldierNamePool.cpp
  Ruleset/Ruleset.h
  Ruleset/Ruleset.cpp
  Ruleset/RuleCountry.cpp
  Ruleset/RuleCountry.h
  Ruleset/RuleUfo.h
  Ruleset/RuleUfo.cpp
  Ruleset/ArticleDefinition.cpp
  Ruleset/ArticleDefinition.h
  Ruleset/City.cpp
  Ruleset/City.h
  Ruleset/RuleTerrain.cpp
  Ruleset/RuleTerrain.h
  Ruleset/MapData.cpp
  Ruleset/MapData.h
  Ruleset/RuleResearch.h
  Ruleset/RuleResearch.cpp
  Ruleset/RuleManufacture.h
  Ruleset/RuleManufacture.cpp
  Ruleset/AlienDeployment.h
  Ruleset/AlienDeployment.cpp
  Ruleset/AlienRace.h
  Ruleset/AlienRace.cpp
  Ruleset/Unit.h
  Ruleset/Unit.cpp
  Ruleset/UfoTrajectory.cpp
  Ruleset/UfoTrajectory.h
  Ruleset/RuleAlienMission.cpp
  Ruleset/RuleAlienMission.h
)

set ( savegame_src
  Savegame/AlienBase.h
  Savegame/AlienBase.cpp
  Savegame/AlienMission.cpp
  Savegame/AlienMission.h
  Savegame/BattleItem.h
  Savegame/BattleItem.cpp
  Savegame/Ufo.cpp
  Savegame/Ufo.h
  Savegame/MovingTarget.cpp
  Savegame/MovingTarget.h
  Savegame/Base.h
  Savegame/Base.cpp
  Savegame/SavedBattleGame.cpp
  Savegame/SavedBattleGame.h
  Savegame/BattleUnit.h
  Savegame/BattleUnit.cpp
  Savegame/BaseFacility.h
  Savegame/BaseFacility.cpp
  Savegame/Craft.cpp
  Savegame/Craft.h
  Savegame/Country.cpp
  Savegame/Country.h
  Savegame/NodeLink.cpp
  Savegame/NodeLink.h
  Savegame/Transfer.cpp
  Savegame/Transfer.h
  Savegame/Region.h
  Savegame/Region.cpp
  Savegame/Node.h
  Savegame/Node.cpp
  Savegame/ItemContainer.cpp
  Savegame/ItemContainer.h
  Savegame/GameTime.cpp
  Savegame/GameTime.h
  Savegame/Tile.cpp
  Savegame/Tile.h
  Savegame/CraftWeapon.cpp
  Savegame/CraftWeapon.h
  Savegame/CraftWeaponProjectile.cpp
  Savegame/CraftWeaponProjectile.h
  Savegame/SavedGame.h
  Savegame/SavedGame.cpp
  Savegame/Soldier.h
  Savegame/Soldier.cpp
  Savegame/Waypoint.h
  Savegame/Waypoint.cpp
  Savegame/Target.h
  Savegame/Target.cpp
  Savegame/ResearchProject.h
  Savegame/ResearchProject.cpp
  Savegame/Production.h
  Savegame/Production.cpp
  Savegame/TerrorSite.h
  Savegame/TerrorSite.cpp
  Savegame/Vehicle.h
  Savegame/Vehicle.cpp
  Savegame/EquipmentLayoutItem.h
  Savegame/EquipmentLayoutItem.cpp
  Savegame/WeightedOptions.cpp
  Savegame/WeightedOptions.h
  Savegame/AlienStrategy.cpp
  Savegame/AlienStrategy.h
)

set ( ufopedia_src
  Ufopaedia/ArticleStateBaseFacility.cpp
  Ufopaedia/ArticleStateBaseFacility.h
  Ufopaedia/UfopaediaStartState.cpp
  Ufopaedia/UfopaediaStartState.h
  Ufopaedia/UfopaediaSelectState.cpp
  Ufopaedia/UfopaediaSelectState.h
  Ufopaedia/ArticleStateUfo.h
  Ufopaedia/ArticleStateUfo.cpp
  Ufopaedia/ArticleStateText.cpp
  Ufopaedia/ArticleStateText.h
  Ufopaedia/ArticleState.h
  Ufopaedia/ArticleState.cpp
  Ufopaedia/ArticleStateCraft.cpp
  Ufopaedia/ArticleStateCraft.h
  Ufopaedia/ArticleStateItem.cpp
  Ufopaedia/ArticleStateItem.h
  Ufopaedia/Ufopaedia.h
  Ufopaedia/Ufopaedia.cpp
  Ufopaedia/ArticleStateCraftWeapon.h
  Ufopaedia/ArticleStateCraftWeapon.cpp
  Ufopaedia/ArticleStateTextImage.cpp
  Ufopaedia/ArticleStateTextImage.h
  Ufopaedia/ArticleStateArmor.cpp
  Ufopaedia/ArticleStateArmor.h
  Ufopaedia/ArticleStateVehicle.cpp
  Ufopaedia/ArticleStateVehicle.h
)

set ( data_install_dir bin )
if ( CMAKE_COMPILER_IS_GNUCXX AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
  add_definitions ( -D_DEBUG )
endif ()
if ( CMAKE_COMPILER_IS_GNUCXX AND ( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR ENABLE_WARNING) )
    # Enable more GCC warnings if requested or we are doing a Debug build.
    add_definitions ( -Wall
      -Wsign-compare
      -Winit-self
      -Wmissing-include-dirs
      -Wunknown-pragmas
      -Wmissing-format-attribute
      -Wredundant-decls
      -Winline
      -Wformat-security
      -Wtype-limits
      -Wclobbered
      -Wempty-body
      -Wlogical-op
      -Wuninitialized
      -Wshadow
# These two warning options actually generate way too many warnings. Should be enabled later.
#      -Wignored-qualifiers
#      -Weffc++
      )
    if ( FATAL_WARNING )
      add_definitions ( -Werror )
    endif ( FATAL_WARNING )
endif()

if ( MSVC )
  if ( ENABLE_WARNING )
    add_definitions ( /W${MSVC_WARNING_LEVEL} )
  else ()
    # Set warning levels for Debug and Release with debug info
    set ( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W${MSVC_WARNING_LEVEL}" )
    set ( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /W${MSVC_WARNING_LEVEL}" )
  endif ()
  if ( FATAL_WARNING )
    add_definitions ( /WX )
  endif ( FATAL_WARNING )
endif ()

if ( WIN32 )
  set ( application_type WIN32 )
endif ()

set ( openxcom_src main.cpp lodepng.cpp lodepng.h dirent.h ${basescape_src} ${battlescape_src} ${engine_src} ${geoscape_src} ${interface_src} ${menu_src} ${resource_src} ${ruleset_src} ${savegame_src} ${ufopedia_src} )

set ( install_dest RUNTIME )
set ( set_exec_path ON )
set ( install_dest_dir bin )
if ( UNIX AND NOT APPLE )
  set ( data_install_dir ${CMAKE_INSTALL_PREFIX}/share/openxcom )
endif ()
if ( APPLE )
  set ( openxcom_src ${openxcom_src} ${MACOS_SDLMAIN_M_PATH} )
  if ( CREATE_BUNDLE )
    set ( application_type MACOSX_BUNDLE )
    set ( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR} )
    set ( install_dest BUNDLE )
    set ( set_exec_path OFF )
    set ( install_dest_dir "" )
    set ( data_install_dir bin )
  else ()
    set ( data_install_dir ${CMAKE_INSTALL_PREFIX}/share/openxcom )
  endif ()
endif ()
if ( set_exec_path )
  set ( EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin )
endif ()

if ( FORCE_INSTALL_DATA_TO_BIN )
  set ( data_install_dir bin )
endif ()

add_executable ( openxcom  ${application_type} ${openxcom_src} )
install ( TARGETS openxcom ${install_dest} DESTINATION bin )
# Extra link flags for Windows. They need to be set before the SDL/YAML link flags, otherwise you will get strange link errors ('Undefined reference to WinMain@16')
if ( WIN32 )
  set ( basic_windows_libs advapi32.lib shell32.lib shlwapi.lib )
  if ( MINGW )
    set ( basic_windows_libs ${basic_windows_libs} mingw32 -mwindows )
    set ( static_flags  -static )
    set ( SDLMIXER_LIBRARY "${SDLMIXER_LIBRARY} -lwinmm" )
  endif ()
  if ( MSVC )
    set_target_properties ( openxcom
      PROPERTIES PREFIX ../ )
  endif ()
  set ( system_libs ${basic_windows_libs} SDLmain ${static_flags} )
endif ()
target_link_libraries ( openxcom ${system_libs} ${SDLIMAGE_LIBRARY} ${SDLMIXER_LIBRARY} ${SDLGFX_LIBRARY} ${SDL_LIBRARY} ${YAMLCPP_LIBRARY} )

add_custom_command ( TARGET openxcom
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/bin/data ${EXECUTABLE_OUTPUT_PATH}/data )
install ( DIRECTORY ${CMAKE_SOURCE_DIR}/bin/data DESTINATION ${data_install_dir} )

# Copy Windows DLLs to bin folder
if ( WIN32 )
  if ( CMAKE_CL_64 )
    file ( GLOB Windows_dll "${DEPS_DIR}/lib/x64/*.dll" )
  else ( )
    file ( GLOB Windows_dll "${DEPS_DIR}/lib/Win32/*.dll" )
  endif()
  foreach ( dll ${Windows_dll} )
    add_custom_command ( TARGET openxcom
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy ${dll} ${EXECUTABLE_OUTPUT_PATH} )
    install ( FILES ${dll} DESTINATION bin)
  endforeach()
endif ()

if ( APPLE AND CREATE_BUNDLE )
  list ( APPEND frameworks "${SDLGFX_LIBRARY}" )
  list ( APPEND frameworks "${SDLMIXER_LIBRARY}" )
  list ( APPEND frameworks "${SDLIMAGE_LIBRARY}" )
  list ( APPEND frameworks "${SDL_LIBRARY}" )
  list ( APPEND frameworks "${YAMLCPP_LIBRARY}" )

  foreach ( framework ${frameworks} )
    get_filename_component ( framework_name ${framework} NAME )
    if ( EXISTS "${framework}" )
      if ( "${CMAKE_GENERATOR}" STREQUAL "Xcode" )
	add_custom_command ( TARGET openxcom
	  POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/Debug/openxcom.app/Contents/Frameworks/${framework_name}
	  COMMAND ${CMAKE_COMMAND} -E copy_directory ${framework} ${EXECUTABLE_OUTPUT_PATH}/Debug/openxcom.app/Contents/Frameworks/${framework_name}
	  COMMAND ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/Release/openxcom.app/Contents/Frameworks/${framework_name}
	  COMMAND ${CMAKE_COMMAND} -E copy_directory ${framework} ${EXECUTABLE_OUTPUT_PATH}/Release/openxcom.app/Contents/Frameworks/${framework_name} )
      else ()
	add_custom_command ( TARGET openxcom
	  POST_BUILD
	  COMMAND ${CMAKE_COMMAND} -E make_directory ${EXECUTABLE_OUTPUT_PATH}/openxcom.app/Contents/Frameworks/${framework_name}
	  COMMAND ${CMAKE_COMMAND} -E copy_directory ${framework} ${EXECUTABLE_OUTPUT_PATH}/openxcom.app/Contents/Frameworks/${framework_name} )
      endif ()
    endif ()
  endforeach ()
endif ()

#Setup source groups for IDE
if ( MSVC )
  source_group ( "Basescape" FILES ${basescape_src} )
  source_group ( "Battlescape" FILES ${battlescape_src} )
  source_group ( "Engine" FILES ${engine_src} )
  source_group ( "Geoscape" FILES ${geoscape_src} )
  source_group ( "Interface" FILES ${interface_src} )
  source_group ( "Menu" FILES ${menu_src} )
  source_group ( "Resource" FILES ${resource_src} )
  source_group ( "Ruleset" FILES ${ruleset_src} )
  source_group ( "SaveGame" FILES ${savegame_src} )
  source_group ( "Ufopedia" FILES ${ufopedia_src} )
endif ()
