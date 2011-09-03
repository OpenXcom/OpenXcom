#ifndef OPENXCOM_RULEMANUFACTUREINFO_H
#define OPENXCOM_RULEMANUFACTUREINFO_H

#include <string>
#include <map>

namespace OpenXcom
{
class RuleManufactureInfo
{
public:
	RuleManufactureInfo(int workspace, int time, int cost);
	void setRequiredSpace (int);
	int getRequiredSpace () const;
	void setManufactureTime (int);
	int getManufactureTime () const;
	void setManufactureCost (int);
	int getManufactureCost () const;
	const std::map<std::string, int> & getNeededItems() const;
	void addNeededItem (std::string type, int quantity);
private:
	int _requiredWorkspace;
	std::map<std::string, int> _neededItems;
	int _manufactureTime;
	int _manufactureCost;
};
}
#endif
