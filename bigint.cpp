// $Id: bigint.cpp,v 1.73 2015-07-03 14:46:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
using namespace std;

#include "bigint.h"
#include "debug.h"
#include "relops.h"

// Constructor using a long
bigint::bigint (long that): uvalue (that), is_negative (that < 0) {
   DEBUGF ('~', this << " -> " << uvalue)
}

// Constructor using a ubigint and a value
bigint::bigint (const ubigint& uvalue, bool is_negative):
                uvalue(uvalue), is_negative(is_negative) {
}

// Constructor using a string
bigint::bigint (const string& that) {
   is_negative = that.size() > 0 and that[0] == '_';
   uvalue = ubigint (that.substr (is_negative ? 1 : 0));
}

/*
Here is where the operators are defined. We define
 */
bigint bigint::operator+() const {
   return *this;
}

bigint bigint::operator-() const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {
   ubigint result = uvalue + that.uvalue;
   return result;
}

bigint bigint::operator- (const bigint& that) const {
   ubigint result = uvalue - that.uvalue;
   return result;
}

bigint bigint::operator* (const bigint& that) const {
   bigint result = uvalue * that.uvalue;
   return result;
}


bigint bigint::operator/ (const bigint& that) const {
   bigint result = uvalue / that.uvalue;
   return result;
}

bigint bigint::operator% (const bigint& that) const {
   bigint result = uvalue % that.uvalue;
   return result;
}

bool bigint::operator== (const bigint& that) const {
   return is_negative == that.is_negative and uvalue == that.uvalue;
}

bool bigint::operator< (const bigint& that) const {
   // if they're different signs return the sign of this one since if
   // it's negative it'll true and thus less, otherwise itll be false
   // and thus greater than (logically consistant with operator)
   if (is_negative != that.is_negative) return is_negative;
   // if they're the same sign check if this one is negative, then
   // return the value of the one locially smaller
   return is_negative ? uvalue > that.uvalue
                      : uvalue < that.uvalue;
}

// overloaded operator to print out bigint properly
ostream& operator<< (ostream& out, const bigint& that) {
   // check if it's negative, then add the container
   return out << "bigint(" << (that.is_negative ? "'-'" : "'+'")
              << that.uvalue << ")";
}
