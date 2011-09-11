#ifndef OPENXCOM_GEORESEARCHSTATE_H
#define OPENXCOM_GEORESEARCHSTATE_H

#include "../Basescape/ResearchState.h"

namespace OpenXcom
{
/**
 * Helper class which allow to revert to the GeoscapeState instead of the NewPossibleResearchState.
 */
class GeoResearchState : public ResearchState
{
public:
	~GeoResearchState();
	GeoResearchState(Game *game, Base *base);
	void btnOkClick(Action *action);
};

}

#endif
