#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <list>

using namespace std;

namespace ariel{ class NumberWithUnits; }
ariel::NumberWithUnits operator*(double const & lhs, ariel::NumberWithUnits const & rhs);
ostream& operator<<(ostream& o, ariel::NumberWithUnits n);
istream& operator>>(istream& i, ariel::NumberWithUnits& n);

namespace ariel{
	class NumberWithUnits{
	private:
		static unordered_map<string, list< pair<string, double> >> unitsMap;
		string unit;
		double amount;

		static bool isUnitExists(string unit);
		bool isPossibleToConvert(NumberWithUnits const & other, double & convertedValue) const;
        bool isSameUnit(NumberWithUnits const & other) const;
	public:
		static void read_units(ifstream& units_file);

		NumberWithUnits(double amount, string unit);

		NumberWithUnits operator+(NumberWithUnits const & rhs) const;
		NumberWithUnits operator+() const;
		NumberWithUnits & operator+=(NumberWithUnits const & rhs);

		NumberWithUnits operator-(NumberWithUnits const & rhs) const;
		NumberWithUnits operator-() const;
		NumberWithUnits & operator-=(NumberWithUnits const & rhs);

		bool operator==(NumberWithUnits const & rhs) const;
		bool operator!=(NumberWithUnits const & rhs) const;
		bool operator<(NumberWithUnits const & rhs) const;
		bool operator<=(NumberWithUnits const & rhs) const;
		bool operator>(NumberWithUnits const & rhs) const;
		bool operator>=(NumberWithUnits const & rhs) const;

		NumberWithUnits & operator++();
		NumberWithUnits operator++(int);

		NumberWithUnits & operator--();
		NumberWithUnits operator--(int);

		NumberWithUnits operator*(double const & rhs) const;

		friend NumberWithUnits (::operator*(double const & lhs, ariel::NumberWithUnits const & rhs));
		friend ostream& ::operator<<(ostream& o, ariel::NumberWithUnits n);
		friend istream& ::operator>>(istream& i, ariel::NumberWithUnits& n);
	};
}
