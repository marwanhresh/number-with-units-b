#include "NumberWithUnits.hpp"
#include <map>
#include <vector>
#include <cmath>
#include <limits>
using namespace ariel;

unordered_map<string, list< pair<string, double> >> NumberWithUnits::unitsMap;
const double NumberWithUnits::EPSILON = 1E-4;

bool NumberWithUnits::isUnitExists(string const & unit){
    return unitsMap.find(unit) != unitsMap.end();
}

void NumberWithUnits::read_units(ifstream& units_file){
    int one = 1;
    char eq = '=';
    double mult = 1;
    string srcUnit, dstUnit;
    while (units_file >> one){
        units_file >> srcUnit >> eq >> mult >> dstUnit;
        unitsMap[srcUnit].push_back(make_pair(dstUnit, mult));
        unitsMap[dstUnit].push_back(make_pair(srcUnit, 1 / mult));
    }
}

bool isUnitInPath(const string & unit, vector<string>& path)
{
    vector<string>::iterator it = path.begin();
    while (it != path.end()){
        if ((*it) == unit){
            return true;
        }
        it++;
    }
    return false;
}

bool NumberWithUnits::isPossibleToConvert(NumberWithUnits const & other, double & convertedValue) const{
    if (isSameUnit(other)){
        convertedValue = other.amount;
        return true;
    }
    list<pair<vector<string>, double> > allPossiblePaths;
    vector<string> path;
    path.push_back(other.unit);
    allPossiblePaths.push_back(make_pair(path, 1));

    while (!allPossiblePaths.empty()) {
        path = get<0>(allPossiblePaths.front());
        double mult = get<1>(allPossiblePaths.front());
        allPossiblePaths.pop_front();

        string last = path[path.size() - 1];

        // we found a path from unit other to unis *this
        if (this->unit == last){
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
    return this->unit == other.unit;
}


NumberWithUnits::NumberWithUnits(double amount, string const & unit){
    if (!isUnitExists(unit)){
        throw invalid_argument("Unit does not appear in file.");
    }
    this->unit = unit;
    this->amount = amount;
}

double NumberWithUnits::convertFromTo(const NumberWithUnits& a, const NumberWithUnits& b){
    double converted = 0;
    if (!b.isPossibleToConvert(a, converted)){
        throw logic_error("Units does not match.");
    }
    return converted;
}

NumberWithUnits NumberWithUnits::operator+(NumberWithUnits const & rhs) const{
    double converted = convertFromTo(rhs, *this);
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
    double converted = convertFromTo(rhs, *this);
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

bool NumberWithUnits::isEqual(double a, double b)
{
    return fabs(a - b) <= EPSILON;
}

bool NumberWithUnits::isBigger(double a, double b){
    return (a - b) > EPSILON;
}

bool NumberWithUnits::isSmaller(double a, double b){
    return (a - b) < EPSILON;
}

bool NumberWithUnits::operator==(NumberWithUnits const & rhs) const{
    NumberWithUnits r = this->operator-(rhs);
    return isEqual(r.amount, 0);
}

bool NumberWithUnits::operator!=(NumberWithUnits const & rhs) const{
    NumberWithUnits r = this->operator-(rhs);
    return !isEqual(r.amount, 0);
}

bool NumberWithUnits::operator<(NumberWithUnits const & rhs) const{
    NumberWithUnits r = this->operator-(rhs);
    return r.amount < 0;
}

bool NumberWithUnits::operator<=(NumberWithUnits const & rhs) const{
    NumberWithUnits r = this->operator-(rhs);
    return isSmaller(r.amount, 0) || isEqual(r.amount, 0);
}

bool NumberWithUnits::operator>(NumberWithUnits const & rhs) const{
    NumberWithUnits r = this->operator-(rhs);
    return isBigger(r.amount, 0);
}

bool NumberWithUnits::operator>=(NumberWithUnits const & rhs) const{
    NumberWithUnits r = this->operator-(rhs);
    return isBigger(r.amount, 0) || isEqual(r.amount, 0);
}

NumberWithUnits & NumberWithUnits::operator++(){
    this->amount += 1;
    return *this;
}

NumberWithUnits NumberWithUnits::operator++(int){
    NumberWithUnits tmp{ *this };
    this->amount += 1;
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

ostream& operator<<(ostream& o, const ariel::NumberWithUnits & n){
    o << n.amount << "[" << n.unit << "]";
    return o;
}

istream& operator>>(istream& i, ariel::NumberWithUnits& n){
    char brackets = '[';
    i >> n.amount;
    i >> skipws >> brackets >> n.unit;
    size_t found = n.unit.find(']');
    if (found != string::npos){
        n.unit.erase(found);
    }
    else{
        i >> brackets;
    }

    if (!NumberWithUnits::isUnitExists(n.unit)){
        throw invalid_argument("Unit does not appear in file.");
    }

    return i;
}
