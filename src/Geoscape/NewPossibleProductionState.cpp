#include "NewPossibleProductionState.h"
#include "../Interface/TextButton.h"
#include "../Interface/Text.h"
#include "../Interface/TextList.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Engine/Language.h"
#include "../Basescape/ManufactureState.h"
#include "../Ruleset/RuleResearchProject.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the NewPossibleProductionState screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 * @param project The completed ResearchProject
 */
NewPossibleProductionState::NewPossibleProductionState(Game * game, Base * base,RuleResearchProject * project) : NewPossibleResearchState(game, base)
{
	_btnAllocate->setText(_game->getLanguage()->getString("STR_ALLOCATE_MANUFACTURE"));
	_txtTitle->setText(_game->getLanguage()->getString("STR_WE_CAN_NOW_PRODUCE"));
	_lstPossibilities->addRow (1, _game->getLanguage()->getString(project->getName()).c_str());
}

/**
 * Open the ManufactureState so the player can dispatch available engineers.
 * @param action Pointer to an action.
 */
void NewPossibleProductionState::btnAllocateClick(Action *action)
{
	_game->popState ();
	_game->pushState (new ManufactureState(_game, _base));
}
}
