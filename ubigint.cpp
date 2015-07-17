// $Id: ubigint.cpp,v 1.8 2015-07-03 14:46:41-07 - - $

#include <cstdlib>
#include <exception>
#include <stack>
#include <stdexcept>
#include <string> // needed for string manipulation
#include <sstream> // needed to stream strings
using namespace std;
using udigit_t = unsigned char;

#include "ubigint.h"
#include "debug.h"
#include "general.h" // included for the exception

///////////////////
// CONSTRUCTORS  //
///////////////////

ubigint::ubigint (unsigned long that) {

   // if it's 0 then there is nothing to do
   if (that == 0) return;

   // convert long to chars of a string
   stringstream ss;
   string uvalue;

   ss << that;
   ss >> uvalue;

   // iterate over long backwards and push it to ubig value
   for (auto it = uvalue.rbegin(); it != uvalue.rend(); it++){
      ubig_value.push_back(*it);
      DEBUGF ('~', *it);
   }
}

ubigint::ubigint (const string& that) {
   // iterate over long backwards and push it to ubig value
   for (auto it = that.rbegin(); it != that.rend(); it++){
      ubig_value.push_back(*it);
      DEBUGF ('~', *it);
   }
}

// TODO TEST!
ubigint::ubigint(ubigvalue_t that): ubig_value(that){}

//////////////////////
// Helper functions //
//////////////////////

/**
 * Convert a digit to an integer. Simple helper function. Name comes
 * from std::stoi()
 * @param  digit  the digit to convert
 * @return          integer value of digit
 */
int dtoi(udigit_t digit){
   return digit - 0;
}

/**
 * Convert a integer to a digit. Simple helper function. Name comes
 * from std::stoi()
 * @param  digit the digit to convert
 * @pre    int must be at least 0 and at most 9
 * @return          digit with value of integer
 */
udigit_t itod(int digit){

   // error handling
   if (digit < 0){
      throw ydc_exn("signed int in ubigint.cpp");
   }
   if (digit >= 10){
      throw ydc_exn("int arg > 10");
   }
   // actual conversion
   return '0' + digit;
}

/**
 * Helper function for plus operator to add two digits together. Returns
 * a pair, which has a udigit_t to act as the result, and a boolean to
 * act as a carry.
 *
 * @param  first_digit     a digit to be added
 * @param  second_digit    a digit to be added
 * @param  carry           whether or not a value is being carried in
 *                         from the previous addition
 *
 * @return                 a pair with the resulting digit and a digit
 *                         to represent whether or not there is a carry
 *                         involved
 */
pair<bool, udigit_t> add_digit(udigit_t first_digit,
   udigit_t second_digit, bool carry){

   DEBUGF('+', "first digit: " << first_digit);
   DEBUGF('+', "second digit: " << second_digit);

   // converting the digits to ints
   int first_i = dtoi(first_digit);
   int second_i = dtoi(second_digit);

   DEBUGF('+', "converted first digit: " << first_i);
   DEBUGF('+', "converted second digit: " << second_i);

   // doing the actual addition
   int ret = first_digit + second_digit;

   // carrying over if applicable
   if (carry) ret++;

   // if ret is greater or equal to 10 we subtract 10 and return the
   // char and carry as true
   if (ret >= 10){
      ret = ret - 10;
      udigit_t ret_digit = itod(ret);
      return make_pair(true, ret_digit);
   }
   udigit_t ret_digit = itod(ret);
   return make_pair(false, ret_digit);
}

/**
 *	Helper function that subtracts the second digit from the first.
 *	Works in a similar fashion to add_digit(), however does subtraction
 */
pair<bool, udigit_t> sub_digit(udigit_t first_digit,
   udigit_t second_digit, bool carry){

   DEBUGF('+', "first digit: " << first_digit);
   DEBUGF('+', "second digit: " << second_digit);

   // converting the digits to ints
   int first_i = dtoi(first_digit);
   int second_i = dtoi(second_digit);

   DEBUGF('+', "converted first digit: " << first_i);
   DEBUGF('+', "converted second digit: " << second_i);

   // doing the actual subtraction
   int ret = first_digit - second_digit;

   // checking if the carry from the last digit happened
   if (carry) ret--;

   // if ret is less than zero, carry over, and return true to indicate
   // that a carry did happen
   if (ret < 0){
      ret = ret + 10;
      udigit_t ret_digit = itod(ret);
      return make_pair(true, ret_digit);
   }
   udigit_t ret_digit = itod(ret);
   return make_pair(false, ret_digit);
}
////////////////
// OPERATORS  //
////////////////

ubigint ubigint::operator+ (const ubigint& that) const {
   // variable declaration
   ubigvalue_t larger; // reference to the larger ubigvalue
   ubigvalue_t smaller; // reference to the smaller ubig value
   ubigvalue_t ret; // return reference


   // find the larger one and the smaller one
   if (*this < that){
      larger = that.ubig_value;
      smaller = ubig_value;
   } else {
      larger = ubig_value;
      smaller = that.ubig_value;
   }

   // iterators for the vectors
   auto small_it = smaller.begin();
   auto large_it = larger.begin();

   // a boolean to know if we should carry. False for the first
   // iteration
   bool carry = false;

   // iterate over the vectors, adding them together. Finished when we
   // get to the end of the small one
   while (small_it != smaller.end()){

      // pair declaration for the result
      pair<bool, udigit_t> result;

      // handoff to helper function for ease of code readability
      result = add_digit(*small_it, *large_it, carry);

      // update carry from the result.
      carry = result.first;

      // push the digit to the back of the return
      ret.push_back(result.second);

      small_it++;
      large_it++;
   }

   // fill in the rest of the larger digit
   while (large_it != larger.end()){
      // reference to make
      udigit_t curr = *large_it;

      // carries can still happen from the last addition, ex 999+99
      if (carry){
         // add one to the current digit
         pair<bool, udigit_t> tmp = add_digit(curr, '1', false);

         // set carry to the new value
         carry = tmp.first;
         curr = tmp.second;
      }

      // push it to the larger one
      ret.push_back(curr);

   }

   // if after all that iteration we still have a carry, push a 1 onto
   // the end of the vector
   if (carry){
      ret.push_back('1');
   }

   return ret;

}

ubigint ubigint::operator- (const ubigint& that) const {
   if (*this < that) throw domain_error ("ubigint::operator-(a<b)");

   // declaration of variables similar to + operator, but the larger
   // one is already known so we can assign them immediately.
   // note: these references are mostly used for code readability
   ubigvalue_t larger  = ubig_value;
   ubigvalue_t smaller = that.ubig_value;
   ubigvalue_t ret;

   // iterators for the vectors
   auto small_it = smaller.begin();
   auto large_it = larger.begin();

   // a boolean to know if we have carried. False for the first
   // iteration
   bool carry = false;

   // iterate over all the vectors, subtracting them. Finished when  we
   // get to the smaller one.
   while (small_it != smaller.end()){
      // pair declaration for the result
      pair<bool, udigit_t> result;

      // handoff to helper function for ease of code readability
      result = sub_digit(*small_it, *large_it, carry);

      // update carry from the result.
      carry = result.first;

      // push the digit to the back of the return
      ret.push_back(result.second);

      small_it++;
      large_it++;
   }

   // fill in the rest of the larger digit
   while (large_it != larger.end()){
      // reference to make
      udigit_t curr = *large_it;

      // carries can still happen from the last subtraction, ex 500 - 1
      if (carry){
         // add one to the current digit
         pair<bool, udigit_t> tmp = add_digit(curr, '1', false);

         // set carry to the new value
         carry = tmp.first;
         curr = tmp.second;
      }

      // push it to the larger one
      ret.push_back(curr);

   }

   // after this point we should be done, we should check to see if we
   // have any trailing zeros. This has the side effect of making it so
   // that zero is the empty vector
   for (auto it = ret.rbegin(); it != ret.rend(); it++){
      int curr = dtoi(*it);
      if (curr != 0) break;
      ret.pop_back();
   }

   return ret;

}

ubigint ubigint::operator* (const ubigint& that) const {
   return ubigint (uvalue * that.uvalue);
}

void ubigint::multiply_by_2() {
   uvalue *= 2;
}

void ubigint::divide_by_2() {
   uvalue /= 2;
}


ubigint::quot_rem ubigint::divide (const ubigint& that) const {
   static const ubigint zero = 0;
   if (that == zero) throw domain_error ("ubigint::divide: by 0");
   ubigint power_of_2 = 1;
   ubigint divisor = that; // right operand, divisor
   ubigint quotient = 0;
   ubigint remainder = *this; // left operand, dividend
   while (divisor < remainder) {
      divisor.multiply_by_2();
      power_of_2.multiply_by_2();
   }
   while (power_of_2 > zero) {
      if (divisor <= remainder) {
         remainder = remainder - divisor;
         quotient = quotient + power_of_2;
      }
      divisor.divide_by_2();
      power_of_2.divide_by_2();
   }
   return {quotient, remainder};
}

ubigint ubigint::operator/ (const ubigint& that) const {
   return divide (that).first;
}

ubigint ubigint::operator% (const ubigint& that) const {
   return divide (that).second;
}

bool ubigint::operator== (const ubigint& that) const {

   // check the obvious case
   if (ubigvalue.size() != that.ubigvalue.size()) return false;


   // iterate down the vectors and return false if any isnt the same

   equal(ubigvalue.begin(), ubigvalue.end(), that.ubigvalue, )

   auto this_it = ubigvalue.begin();
   auto that_it = that.ubigvalue.begin();

   while (this_it != ubigvalue.end()
      && that_it != that.ubigvalue.end()){

      ++that_it;
      ++this_it;
   }

   return true;
}

bool ubigint::operator!= (const ubigint& that) const {
   // pretty easy, just the opposite of the normal one
   return !(*this == that);
}

bool ubigint::operator< (const ubigint& that) const {
   return uvalue < that.uvalue;

   DEBUGF ('<', "this size: " << ubigvalue.size());
   DEBUGF ('<', "that size: " << that.ubigvalue.size());

   DEBUGF ('>', "this size: " << ubigvalue.size());
   DEBUGF ('>', "that size: " << that.ubigvalue.size());

   // check the obvious cases first
   if (ubigvalue.size() < that.ubigvalue.size()) return true;
   if (ubigvalue.size() > that.ubigvalue.size()) return false;

   // They're now guaranteed to be the same size, so we should just
   // check to see which one is the larger one

   // iterate over them backwards so we can check most significant
   // digit first

   auto this_it = ubigvalue.rbegin();
   auto that_it = that.ubigvalue.rbegin();

   while (this_it != ubigvalue.rend()){

      if (*this_it < *that_it) return true;

      ++this_it;
      ++that_it;
   }
   // passed all tests
   return false;

}

bool ubigint::operator<= (const ubigint& that) const {
   // solved with simple logic
   return !(that < *this);
}

bool ubigint::operator> (const ubigint& that) const {
   // just flip the sign
   return that < *this;
}

bool ubigint::operator>= (const ubigint& that) const {
   // same as <= just in reverse
   return !(*this < that);
}

ostream& operator<< (ostream& out, const ubigint& that) {
   return out << "ubigint(" << that.uvalue << ")";
}

/**
 * small helper function that tells whether or not the number is zero
 * @return true if the ubigint is zero, false otherwise
 */
bool ubigint::is_zero() const{
   if (ubig_value.size() == 0) return true;
   return false;
}
