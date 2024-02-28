//===- NaturalSort.h -----------------------------------------------------====//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// This file defines a comparator to sort strings in the natural order.
//
//===----------------------------------------------------------------------===//
#ifndef NATURAL_SORT_H
#define NATURAL_SORT_H

#include <cstring>
#include <iostream>
#include <vector>

#define GREATER false
#define LESSTHAN true
#define ISEQUAL false

#define LEFT_IS_SMALLER 1
#define LEFT_IS_GREATER -1
#define LEFT_EQUAL_RIGHT 0

namespace natsort {

int comp_digits(std::string::const_iterator itl,
                std::string::const_iterator itr) {
  // compares the digits pointed to by the iterators
  // treat the numbers as right aligned
  // a sequence of digits is the greater of the two if:
  // (1) the sequence is longer
  // (2) if the length is equal, then the sequence that has the greater value

  int count = 0;
  while (1) {
    // if both chars are not digits
    if (!std::isdigit(*itl) && !std::isdigit(*itr)) {
      return count;
    }

    // the longer the digit string is, the greater the value it is
    if (!std::isdigit(*itl)) {
      return LEFT_IS_SMALLER;
    }

    if (!std::isdigit(*itr)) {
      return LEFT_IS_GREATER;
    }
    // if lhs number is smaller, and if the numbers were equal before,
    // mark count as False to indicate rhs is greater
    if (*itl < *itr) {
      if (count == LEFT_EQUAL_RIGHT)
        count = LEFT_IS_SMALLER;
    }
    // if lhs number is greater, and if the numbers were equal before,
    // mark count as True to indicate lhs is greater
    else if (*itl > *itr) {
      if (count == LEFT_EQUAL_RIGHT)
        count = LEFT_IS_GREATER;
    }
    // we reached the end of both numbers
    else if (!*itl && !*itr) {
      return count;
    }

    itl++;
    itr++;
  }

  return LEFT_EQUAL_RIGHT;
}

int comp_digits_lead_zero(std::string::const_iterator itl,
                          std::string::const_iterator itr) {
  // compares the digits pointed to by the iterators
  // since at least one of rhl and lhs has a leading zero
  // the first string to have a greater digit or the longer digit string
  // is the greater of the two
  // no need to reach the last char of both digits

  while (1) {
    // if both chars are not digits
    if (!std::isdigit(*itl) && !std::isdigit(*itr)) {
      return LEFT_EQUAL_RIGHT;
    }
    // if lhs value is no longer a digit or is the end of the string
    // return False to indicate rhs is greater
    // the longer the digit string is, the greater the value it is
    if (!std::isdigit(*itl))
      return LEFT_IS_SMALLER;
    // similarly, return True to indicate lhs is greater
    if (!std::isdigit(*itr))
      return LEFT_IS_GREATER;

    // if lhs number is smaller
    // return False to indicate rhs is greater
    if (*itl < *itr)
      return LEFT_IS_SMALLER;

    // if lhs number is greater
    // return True to indicate lhs is greater
    else if (*itl > *itr)
      return LEFT_IS_GREATER;

    itl++;
    itr++;
  }

  return LEFT_EQUAL_RIGHT;
}

bool nat_comp(const std::string &lhs, const std::string &rhs) {
  // A comparator used to sort strings in ascending order
  // returns True if lhs is before rhs after sorting
  // returns False if lhs is after rhs or equal to rhs
  // known limitations:
  //  - is not case sensitive
  //  - does not skip over white spaces
  //  - numbers with leading zeros will be treated as fractional

  std::string::const_iterator itr = rhs.begin();
  std::string::const_iterator itl = lhs.begin();

  while (1) {
    // if both iterators are pointing at numbers
    // begin comparing the numbers
    if (std::isdigit(*itl) && std::isdigit(*itr)) {
      int digit_comp_res;
      // if either digits has leading zeros
      if (*itl == '0' || *itr == '0') {
        digit_comp_res = comp_digits_lead_zero(itl, itr);
      }
      // otherwise use the default digit compare function
      else {
        digit_comp_res = comp_digits(itl, itr);
      }
      if (digit_comp_res != 0) {
        return (digit_comp_res == 1);
      }
    }

    if (*itl > *itr) {
      return GREATER;
    }
    if (*itl < *itr) {
      return LESSTHAN;
    }
    // if both iterators reach the end of the strings, they are equal
    if (itl == lhs.end() && itr == rhs.end()) {
      return ISEQUAL;
    }

    // otherwise the two chars are the same, increment the iterators
    itl++;
    itr++;
  }
  return ISEQUAL;
}
} // namespace natsort
#endif