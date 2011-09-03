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

class ListPossibleProductionState : public State
{
private:
	Base *_base;
	TextButton *_btnOk;
	Window *_window;
	Text *_txtTitle, *_txtItem, *_txtCategory;
	TextList *_lstManufacture;
	std::vector<RuleItem *> _possibleProductions;

public:
	ListPossibleProductionState(Game *game, Base *base);
	void btnOkClick(Action * action);
	void lstProdClick (Action * action);
};
}
#endif
