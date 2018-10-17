#pragma once
#include <sstream>
using namespace std;
// Number to string
template<typename T>
void Number2Str(const T &int_temp, string &string_temp) {
	stringstream stream;
	stream << int_temp;
	string_temp = stream.str();
}

// String to number
template<typename T>
void Str2Number(T &int_temp, const string &string_temp) {
	stringstream stream(string_temp);
	stream >> int_temp;
}