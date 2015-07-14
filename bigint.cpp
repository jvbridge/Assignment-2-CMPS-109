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

bigint bigint::operator+() const {
   return *this;
}

bigint bigint::operator-() const {
   return {uvalue, not is_negative};
}

bigint bigint::operator+ (const bigint& that) const {

   // variable declaration for the return type;
   ubigint result;

   // if they're the same sign, then it doesn't matter whether they're
   // positive or negative, just return them added and keep the sign
   if (is_negative == that.is_negative){
      result = uvalue + that.uvalue;
      return {result, is_negative};
   }

   // subtract the larger uvalue from the smaller one, and return the
   // sign of the one further from zero
   if(uvalue > that.uvalue){
      result = uvalue - that.uvalue;
      return {result, is_negative};
   }
   if (uvalue < that.uvalue){
      result = that.uvalue - uvalue;
      return {result, that.is_negative};
   }

   // if they are of equal magnitude then return zero
   result = uvalue - that.uvalue;
   return {result, false};

}

bigint bigint::operator- (const bigint& that) const {

   // variable declaration for the return type;
   ubigint result;

   // if their signs are opposite, then it's just addition
   if (is_negative != that.is_negative){
      result = uvalue + that.uvalue;
      return {result, is_negative};
   }

   // subtract the larger uvalue from the smaller one. Sign is
   // determined by whether or not lvalue is larger.
   if(uvalue > that.uvalue){
      result = uvalue - that.uvalue;
      return {result, is_negative};
   }
   if (uvalue < that.uvalue){
      result = that.uvalue - uvalue;
      return {result, !is_negative};
   }

   // if they are of equal magnitude then return zero
   result = uvalue - that.uvalue;
   return {result, false};
}

bigint bigint::operator* (const bigint& that) const {
   // result is always multiplied
   ubigint result = uvalue * that.uvalue;

   // if signs are the same, the result is positive. Otherwise it's
   // negative
   return {result, is_negative != that.is_negative};
}

// pretty much the same deal as *
bigint bigint::operator/ (const bigint& that) const {
   ubigint result = uvalue / that.uvalue;
   return {result, is_negative != that.is_negative};
}

bigint bigint::operator% (const bigint& that) const {
   ubigint result = uvalue % that.uvalue;
   return {result, is_negative};
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


//////////////////
// MY OPERATORS //
//////////////////

// note: I could have defined them as the others but I was getting
// syntax errors from it so I gave up and just used the same sort of
// implmentation that mackey used
bool bigint::operator!= (const bigint& that) const {
   return is_negative != that.is_negative or uvalue != that.uvalue;
}

bool bigint::operator<= (const bigint& that) const{
   if (is_negative != that.is_negative) return is_negative;
   return is_negative ? uvalue >= that.uvalue
                      : uvalue <= that.uvalue;
}

bool bigint::operator> (const bigint& that) const{
   if (is_negative != that.is_negative) return !is_negative;
   return is_negative ? uvalue < that.uvalue
                      : uvalue > that.uvalue;
}

bool bigint::operator>= (const bigint& that) const{
   if (is_negative != that.is_negative) return !is_negative;
   return is_negative ? uvalue <= that.uvalue
                      : uvalue >= that.uvalue;
}
