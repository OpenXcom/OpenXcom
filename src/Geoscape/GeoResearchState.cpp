#include "GeoResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
/**
 * Initializes all the elements in the Research screen.
 * @param game Pointer to the core game.
 * @param base Pointer to the base to get info from.
 */
GeoResearchState::GeoResearchState(Game *game, Base *base) : ResearchState(game, base)
{
}

/**
 *
 */
GeoResearchState::~GeoResearchState()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
}

/**
 * Return to the GeoscapeState
 * @param action Pointer to an action.
*/
void GeoResearchState::btnOkClick(Action *action)
{
	_game->popState ();
	_game->popState ();
}
}
