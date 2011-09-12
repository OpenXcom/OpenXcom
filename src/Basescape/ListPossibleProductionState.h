#ifndef OPENXCOM_LISTPOSSIBLEPRODUCTIONSTATE_H
#define OPENXCOM_LISTPOSSIBLEPRODUCTIONSTATE_H

#include "../Engine/State.h"
#include <vector>

namespace OpenXcom
{
class Base;
class TextButton;
class Window;
class Text;
class TextList;
class RuleItem;
class ManufactureState;

/**
 * Screen which list possible productions.
 */
class ListPossibleProductionState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtCategory;
	TextList *_lstManufacture;
	ManufactureState * _manufactureState;
	std::vector<RuleItem *> _possibleProductions;

public:
	/// Create the state
	ListPossibleProductionState(Game *game, Base *base);
	/// Initialize state
	void init ();
	/// Handler for clicking the OK button
	void btnOkClick(Action * action);
	/// Handler for clicking on the list
	void lstProdClick (Action * action);
	/// Fill the list with Base production's
	void fillProductionList();
};
}
#endif
