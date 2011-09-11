#include "GeoResearchState.h"
#include "../Engine/Game.h"

namespace OpenXcom
{
GeoResearchState::GeoResearchState(Game *game, Base *base) : ResearchState(game, base)
{
}

void GeoResearchState::btnOkClick(Action *action)
{
	_game->popState ();
	_game->popState ();
}
}
