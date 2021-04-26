#include "NumberWithUnits.hpp"
#include <map>
#include <vector>
using namespace ariel;

unordered_map<string, list< pair<string, double> >> NumberWithUnits::unitsMap;

bool NumberWithUnits::isUnitExists(string unit){
	return unitsMap.find(unit) != unitsMap.end();
}

void NumberWithUnits::read_units(ifstream& units_file){
	int one;
	char eq;
	double mult;
	string srcUnit, dstUnit;
    unitsMap.clear();
	while (units_file >> one){
		units_file >> srcUnit >> eq >> mult >> dstUnit;
		unitsMap[srcUnit].push_back(make_pair(dstUnit, mult));
		unitsMap[dstUnit].push_back(make_pair(srcUnit, 1 / mult));
	}
}

int isUnitInPath(string unit, vector<string>& path)
{
	vector<string>::iterator it = path.begin();
	while(it != path.end()){
		if (it->compare(unit) == 0)
			return 1;
		it++;
	}
	return 0;
}

bool NumberWithUnits::isPossibleToConvert(NumberWithUnits const & other, double & convertedValue) const{
    if (isSameUnit(other)){
        convertedValue = other.amount;
        return true;
    }
	list<pair<vector<string>, double> > allPossiblePaths;
	vector<string> path;
	path.push_back(other.unit);
	allPossiblePaths.push_back(make_pair(path,1));

	while (!allPossiblePaths.empty()) {
		path = get<0>(allPossiblePaths.front());
		double mult = get<1>(allPossiblePaths.front());
		allPossiblePaths.pop_front();

		string last = path[path.size() - 1];

		// we found a path from unit other to unis *this
		if (this->unit.compare(last) == 0){
			convertedValue = mult*other.amount;
			return true;
		}

		// did not found yet, traverse all possible routes
		list<pair<string, double> >::iterator it = unitsMap[last].begin();
		while (it != unitsMap[last].end()) {
			if (!isUnitInPath(it->first, path)) {
				vector<string> newpath(path);
				newpath.push_back(it->first);
				allPossiblePaths.push_back(make_pair(newpath, mult*(it->second)));
			}
            it++;
		}
	}
	return false;

}

bool NumberWithUnits::isSameUnit(NumberWithUnits const & other) const{
    return this->unit.compare(other.unit) == 0;
}


NumberWithUnits::NumberWithUnits(double amount, string unit){
	if (!isUnitExists(unit)){
		throw invalid_argument("Unit does not appear in file.");
	}
	this->unit = unit;
	this->amount = amount;
}

NumberWithUnits NumberWithUnits::operator+(NumberWithUnits const & rhs) const{
	double converted;
	if (!isPossibleToConvert(rhs, converted)){
		throw logic_error("Units does not match.");
	}
	return NumberWithUnits(amount + converted, unit);
}

NumberWithUnits NumberWithUnits::operator+() const{
	return *this;
}

NumberWithUnits & NumberWithUnits::operator+=(NumberWithUnits const & rhs){
    *this = *this + rhs;
	return *this;
}

NumberWithUnits NumberWithUnits::operator-(NumberWithUnits const & rhs) const{
	double converted;
	if (!isPossibleToConvert(rhs, converted)){
		throw logic_error("Units does not match.");
	}
	return NumberWithUnits(amount - converted, unit);
}

NumberWithUnits NumberWithUnits::operator-() const{
	return NumberWithUnits(-amount, unit);
}

NumberWithUnits & NumberWithUnits::operator-=(NumberWithUnits const & rhs){
	NumberWithUnits r = *this - rhs;
	*this = r;
	return *this;
}

bool NumberWithUnits::operator==(NumberWithUnits const & rhs) const{
	NumberWithUnits r = this->operator-(rhs);
	return r.amount == 0;
}

bool NumberWithUnits::operator!=(NumberWithUnits const & rhs) const{
	NumberWithUnits r = this->operator-(rhs);
	return r.amount != 0;
}

bool NumberWithUnits::operator<(NumberWithUnits const & rhs) const{
	NumberWithUnits r = this->operator-(rhs);
	return r.amount < 0;
}

bool NumberWithUnits::operator<=(NumberWithUnits const & rhs) const{
	NumberWithUnits r = this->operator-(rhs);
	return r.amount <= 0;
}

bool NumberWithUnits::operator>(NumberWithUnits const & rhs) const{
	NumberWithUnits r = this->operator-(rhs);
	return r.amount > 0;
}

bool NumberWithUnits::operator>=(NumberWithUnits const & rhs) const{
	NumberWithUnits r = this->operator-(rhs);
	return r.amount >= 0;
}

NumberWithUnits & NumberWithUnits::operator++(){
	this->amount += 1;
	return *this;
}

NumberWithUnits NumberWithUnits::operator++(int){
	NumberWithUnits tmp{ *this };
	this->amount+= 1;
	return tmp;
}

NumberWithUnits & NumberWithUnits::operator--(){
	this->amount -= 1;
	return *this;
}

NumberWithUnits NumberWithUnits::operator--(int){
	NumberWithUnits tmp{ *this };
	this->amount -= 1;
	return tmp;
}

NumberWithUnits NumberWithUnits::operator*(double const & rhs) const{
	return NumberWithUnits(amount*rhs, unit);
}

NumberWithUnits operator*(double const & lhs, NumberWithUnits const & rhs){
	return rhs*lhs;
}

ostream& operator<<(ostream& o, ariel::NumberWithUnits n){
	o << n.amount << "[" << n.unit << "]";
	return o;
}

istream& operator>>(istream& i, ariel::NumberWithUnits& n){
	char tmp;
	i >> n.amount;
	i >> skipws >> tmp >> n.unit >> noskipws;
	return i;
}
