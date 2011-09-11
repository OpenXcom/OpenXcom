#include "GeoResearchState.h"
#include "../Engine/Game.h"
#include "../Engine/Palette.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{
GeoResearchState::GeoResearchState(Game *game, Base *base) : ResearchState(game, base)
{
}

GeoResearchState::~GeoResearchState()
{
	_game->setPalette(_game->getResourcePack()->getPalette("BACKPALS.DAT")->getColors(Palette::blockOffset(6)), Palette::backPos, 16);
}

void GeoResearchState::btnOkClick(Action *action)
{
	_game->popState ();
	_game->popState ();
}
}
