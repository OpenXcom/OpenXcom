#ifndef OPENXCOM_PRODUCTIONSTARTSTATE_H
#define OPENXCOM_PRODUCTIONSTARTSTATE_H

#include "../Engine/State.h"

namespace OpenXcom
{
class Base;
class RuleItem;
class Window;
class TextButton;
class Text;
class TextList;

/**
 * Screen which display needed elements to start productions(items/required workshop state/cost to build a unit, ...).
*/
class ProductionStartState : public State
{
private:
	Base * _base;
	RuleItem * _item;
	Window * _window;
	TextButton * _btnCancel, * _btnStart;
	Text * _txtTitle, * _txtManHour, * _txtCost, * _txtWorkSpace, * _txtNeedeItemsTitle, * _txtItemNameColumn, * _txtUnitRequiredColumn, * _txtUnitAvailableColumn;
	TextList * _lstNeededItems;
public:
	/// Create the State
	ProductionStartState(Game * game, Base * base, RuleItem * item);
	/// Handler for the Cancel button
	void btnCancelClick(Action * action);
	/// Handler for the start button
	void btnStartClick(Action * action);
};
}
#endif
