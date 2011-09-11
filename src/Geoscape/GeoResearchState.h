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
	/// Create the Research screen
	GeoResearchState(Game *game, Base *base);
	/// Cleans up the Research state.
	~GeoResearchState();
	/// Handler for clicking the OK button
	void btnOkClick(Action *action);
};

}

#endif
