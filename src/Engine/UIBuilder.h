/*
 * Externalized creation of UI elements (BATTLESCAPE ONLY!)
 *
 */




#ifndef OPENXCOM_UIBUILDER_H
#define OPENXCOM_UIBUILDER_H

#include <string>
#include <map>
#include <yaml.h>
#include "State.h"
#include "InteractiveSurface.h"
#include "../Resource/ResourcePack.h"

namespace OpenXcom
{

namespace UIBuilder
{
	// Array of event names. Please add new events 
	/*static const std::string _events[] = {
		"onMouseClick",
		"onMousePress",
		"onMouseRelease",
		"onMouseIn",
		"onMouseOver",
		"onMouseOut",
		"onKeyboardPress",
		"onKeyboardRelease"
	};*/


	std::map<std::string, Surface*> buildUI(State *currentState,
				ResourcePack* resourcePack,
				std::map<std::string, ActionHandler> &handlers,
				std::map<std::string, SDLKey> &kbShortcuts, 
				std::string file);

}

}

#endif //OPENXCOM_UIBUILDER_H