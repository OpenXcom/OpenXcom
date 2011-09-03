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
class ManufactureState;

class ProductionStartState : public State
{
private:
	Base * _base;
	RuleItem * _item;
	ManufactureState * _manufactureState;
	Window * _window;
	TextButton * _btnCancel, * _btnStart;
	Text * _txtTitle, * _txtManHour, * _txtCost, * _txtWorkSpace, * _txtNeedeItemsTitle, * _txtItemNameColumn, * _txtUnitRequiredColumn, * _txtUnitAvailableColumn;
	TextList * _lstNeededItems;
public:
	ProductionStartState(Game * game, Base * base, RuleItem * item, ManufactureState * manufactureState);
	void btnCancelClick(Action * action);
	void btnStartClick(Action * action);
};
}
#endif
