//===- QuantumClassicalSimplify.h - ---------------------------------------===//
//
// Copyright (C) 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
//
//===----------------------------------------------------------------------===//
//
// Classical arithmetic simplification routine and supporting class
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_AQCC_QCSIMPLIFY_H
#define LLVM_AQCC_QCSIMPLIFY_H

#include <map>
#include <set>

namespace llvm {
namespace aqcc {

////////////////////////////////////////////////////////////////
/// The following functions should be specialized for an appropriate
/// Variable class (see Polynomial class description)

/// Return a Variable representation of the input Variable taken to
/// the integral power int. Module_t is an class in which the Variables
/// might live
template <class Module_t, class Variable>
Variable takePower(Module_t &m, const Variable &, int);

/// Returns a Variable which represents conditionally the cases
/// where the passed variable might be zero and divided by.
/// i.e., handleDivisionByZero(a) = (a == 0) ? nan : 1.

template <class Variable> Variable handleDivisionByZero(const Variable &);

/// Returns true if the passed Variable is a return from the above
/// function
template <class Variable> bool isHandleDivisionByZero(const Variable &);

/// returns a Varaible which represents the passed variable conditioned
/// on all the "handleDivisionByZero" returned values.
///  i.e. conditionOnHandleDivisionByZeros(c, {(a == 0) ? nan : 1, (b == 0) ?
///  nan : 1}) = (a == 0) ? nan : ((b == 0) ? nan : c)
template <class Variable>
Variable conditionOnHandleDivisionByZeros(const Variable &,
                                          std::set<Variable> &);

// Are two Coefficients nearly equal (within what ever desired tolerance)
template <class Coefficient>
bool isNearEq(const Coefficient &, const Coefficient &);

//////////////////////////////////////////////////////////////////////////////////
/// Polynomial is a class to represent a generic 'sum-over-products' form
/// Variable only requires an operator overload for '<'
/// Ideally, Variable also has arithmetic operator overloads but does not
/// require it. But if it doesn't, some functions below need to be specialized.
/// Coefficient requires a double constructor and arithmetic operator overloads
/// Module_t is any structure in which the Variable might live. It has no
/// specific requirements.

/// NOTE: This class ASSUMES all algebraic manipulations and properties are
/// exact. This may not be strictly true in the case of instructions run on
/// a hardware backend.

template <class Module_t, class Variable, class Coefficient> class Polynomial {

public:
  /// Term is a wrapper class representing a product of Variables
  /// it is mostly a map Variable* -> int such that each entry (V, i) -> V^i
  class Term;
  typedef typename std::map<Term, Coefficient> Terms;
  using iterator = typename std::map<Term, Coefficient>::iterator;
  using const_iterator = typename std::map<Term, Coefficient>::const_iterator;

protected:
  Terms terms_;
  Module_t *m_;

  Coefficient &get(const Term &T) {
    auto t_iter = terms_.find(T);
    if (t_iter == terms_.end()) {
      auto in_pair = terms_.insert(std::make_pair(T, Coefficient(0.)));
      assert(in_pair.second);
      t_iter = in_pair.first;
    }
    return t_iter->second;
  }

public:
  Polynomial() = delete;

  Polynomial(Module_t &m) : m_(&m) {}

  Polynomial(Module_t &m, const Term &T) : m_(&m) { get(T) = Coefficient(1.0); }

  Polynomial(Module_t &m, const Coefficient &c) : m_(&m) { get(Term()) = c; }

  Polynomial(Module_t &m, const Term &T, const Coefficient &c) : m_(&m) {
    get(T) = c;
  }

  Polynomial(Module_t &m, const Variable &V) : m_(&m) { build(V); }

  Polynomial(const Polynomial &A) = default;

  Polynomial(Polynomial &&A) : m_(A.m_) { std::swap(terms_, A.terms_); }

  Polynomial &operator=(const Polynomial &A) {
    terms_ = A.terms_;
    m_ = A.m_;
    return *this;
  }

  Polynomial &operator=(Polynomial &&A) {
    std::swap(terms_, A.terms_);
    std::swap(m_, A.m_);
    return *this;
  }

  ~Polynomial() = default;

  void clear() { terms_.clear(); }

  bool hasUniqueTerm() const { return terms_.size() == 1; }

  Module_t &getModule() { return *m_; }

  iterator begin() { return terms_.begin(); }
  iterator end() { return terms_.end(); }
  const_iterator begin() const { return terms_.begin(); }
  const_iterator end() const { return terms_.end(); }

  /// build generates a Polynomial representative of the passed
  /// Root object.
  /// Though it is typically called immediately after construction, build is
  /// implicity additive to whatever the current version of Polynomial is.
  /// The build function must be specialized for the given variable and
  /// coeficient.
  bool build(const Variable &root);

  /// the constructVariable returns a member of the Variable class representing
  /// the sum-over-product outcome for this object This must be specialized for
  /// the given Variable class if Variable doesn't have a Variable(Coefficient)
  /// constructor and arithmetic overloads
  Variable constructVariable() const {
    // we assume we don't want '0 + <everthing else>' so we
    // start with the first element (if it exists)
    if (terms_.empty())
      return Variable(Coefficient(0.));

    const_iterator iter = terms_.cbegin();
    Variable out;
    std::set<Variable> div_by_zeros;
    // only check the first entry for a constant
    // as we assume lexographical ordering of the map
    if ((iter->first).isOne()) {
      out = Variable(iter->second);
    } else {
      auto temp = (iter->first).getHandleDivisionByZeros();
      div_by_zeros.insert(temp.begin(), temp.end());
      if (isNearEq(iter->second, Coefficient(1.0)))
        out = (iter->first).constructVariable(m_);
      else
        out = Variable(iter->second) * (iter->first).constructVariable(m_);
    }
    ++iter;

    while (iter != terms_.cend()) {
      auto temp = (iter->first).getHandleDivisionByZeros();
      div_by_zeros.insert(temp.begin(), temp.end());
      if (isNearEq(iter->second, Coefficient(1.0)))
        out += (iter->first).constructVariable(m_);
      else
        out += Variable(iter->second) * (iter->first).constructVariable(m_);
      ++iter;
    }

    return conditionOnHandleDivisionByZeros(out, div_by_zeros);
  }

  /// scale entire Polynomial by a Coefficient
  Polynomial &scale(const Coefficient &c) {
    if (isNearEq(c, Coefficient(0.)))
      clear();
    else if (!isNearEq(c, Coefficient(1.0))) {
      for (auto &pair : terms_) {
        pair.second *= c;
      }
    }
    return *this;
  }

  /// multiply entire Polynomial by a term scaled by a Coefficient
  Polynomial &multiply(const Term &T, const Coefficient &c) {
    if (c == Coefficient(0.))
      clear();
    else if (!(T.isOne())) {
      Terms new_terms;
      for (auto &pair : terms_) {
        new_terms[pair.first * T] = c * pair.second;
      }
      std::swap(new_terms, terms_);
    } else {
      scale(c);
    }
    return *this;
  }
  /// divide entire Polynomial by a term scaled by a Coefficient
  Polynomial &divide(const Term &T, const Coefficient &c) {
    assert(c != Coefficient(0.));
    return multiply(!T, Coefficient(1.0) / c);
  }

  /// add to Polynomial a term scaled by a Coefficient
  Polynomial &add(const Term &T, const Coefficient &c) {
    Coefficient &coef = get(T);
    coef += c;
    if (isNearEq(coef, Coefficient(0.)))
      terms_.erase(T);
    return *this;
  }

  /// subtract from Polynomial a term scaled by a Coefficient
  Polynomial &subtract(const Term &T, const Coefficient &c) {
    return add(T, -c);
  }

  Polynomial operator-() const {
    Polynomial out(*this);
    out.scale(Coefficient(-1.0));
    return out;
  }

  /// Invert the entire Polynomial
  /// if there is not a unique term, we can't convert it directly to
  /// a sum-over-products form. So instead, we convert this to a variable,
  /// then invert it as a single term.
  Polynomial operator!() const {
    Polynomial out(*m_, Coefficient(1.0));
    if (!hasUniqueTerm()) {
      // if there is not a unique term, we can't convert it directly to
      // a sum-over-products form. So instead, we convert this to a variable,
      // then invert it as a single term.
      Variable this_as_var = constructVariable();
      out.divide(Term(this_as_var), Coefficient(1.0));
    } else {
      out.divide(terms_.begin()->first, terms_.begin()->second);
    }
    return out;
  }

  Polynomial &operator*=(const Term &T) {
    return multiply(T, Coefficient(1.0));
  }

  Polynomial &operator/=(const Term &T) { return divide(T, Coefficient(1.0)); }

  Polynomial &operator+=(const Term &T) { return add(T, Coefficient(1.0)); }

  Polynomial &operator-=(const Term &T) {
    return subtract(T, Coefficient(1.0));
  }

  Polynomial &operator*=(const Polynomial &other) {
    Polynomial temp = (*this) * other;
    std::swap(terms_, temp.terms_);
    return *this;
  }

  Polynomial &operator/=(const Polynomial &other) {
    Polynomial temp = (*this) / other;
    std::swap(terms_, temp.terms_);
    return *this;
  }

  Polynomial &operator+=(const Polynomial &other) {
    for (auto const &pair : other.terms_) {
      add(pair.first, pair.second);
    }
    return *this;
  }

  Polynomial &operator-=(const Polynomial &other) { return operator+=(-other); }

  friend Polynomial operator*(const Polynomial &a, const Polynomial &b) {
    Polynomial out(*a.m_);
    for (auto const &pair_a : a.terms_) {
      for (auto const &pair_b : b.terms_) {
        out.add((pair_a.first) * (pair_b.first),
                (pair_a.second) * (pair_b.second));
      }
    }
    return out;
  }

  friend Polynomial operator/(const Polynomial &a, const Polynomial &b) {
    return a * (!b);
  }

  friend Polynomial operator+(const Polynomial &a, const Polynomial &b) {
    Polynomial out(a);
    out += b;
    return out;
  }

  friend Polynomial operator-(const Polynomial &a, const Polynomial &b) {
    Polynomial out(a);
    out += -b;
    return out;
  }

  /// NOTE! this is not numerical less-than, but rather a total order for
  ///  use in std containers
  friend bool operator<(const Polynomial &a, const Polynomial &b) {
    return a.terms_ < b.terms_;
  }

  std::string print() const {
    std::string out;
    bool rm_last = false;
    for (auto const &pair : terms_) {
      out += std::to_string(pair.second) + pair.first.print() + " + ";
      rm_last = true;
    }
    if (rm_last) {
      out.pop_back();
      out.pop_back();
      out.pop_back();
    }
    return out;
  }

  class Term {
  private:
    std::map<Variable, int> factors_;
    int &get(const Variable &V) {
      assert(!V.isNull());
      auto v_iter = factors_.find(V);
      if (v_iter == factors_.end())
        v_iter = factors_.insert(std::make_pair(V, 0)).first;
      return v_iter->second;
    }

  public:
    using iterator = typename std::map<Variable, int>::iterator;
    using const_iterator = typename std::map<Variable, int>::const_iterator;

    Term() = default;
    Term(const Term &) = default;
    Term(Term &&) = default;
    Term(const Variable &V, int p = 1) { get(V) = p; }

    ~Term() = default;

    iterator begin() { return factors_.begin(); }
    iterator end() { return factors_.end(); }
    const_iterator begin() const { return factors_.cbegin(); }
    const_iterator end() const { return factors_.cend(); }

    bool isOne() const { return factors_.empty(); }

    int operator[](Variable V) const {
      auto v_iter = factors_.find(V);
      if (v_iter == factors_.end())
        return 0;
      return v_iter->second;
    }

    Term operator!() const {
      Term out(*this);
      for (auto &pair : out.factors_) {
        pair.second = -pair.second;
        if (pair.second > 0)
          out.get(handleDivisionByZero(pair.first));
      }
      return out;
    }

    Term &multiply(Variable V, int i = 1) {
      int &power = get(V);
      bool was_pos = power >= 0;
      power += i;
      if (!isHandleDivisionByZero(V)) {
        if (power == 0)
          factors_.erase(V);
        if (power >= 0 && was_pos == (i < 0))
          get(handleDivisionByZero(V));
      }
      return *this;
    }

    Term &operator*=(const Variable &V) { return multiply(V, 1); }

    Term &operator/=(const Variable &V) { return multiply(V, -1); }

    Term &operator*=(const Term &T) {
      for (auto const &pair : T) {
        multiply(pair.first, pair.second);
      }
      return *this;
    }

    Term &operator/=(const Term &T) {
      operator*=(!T);
      return *this;
    }

    friend Term operator*(const Term &T1, const Term &T2) {
      Term out(T1);
      out *= T2;
      return out;
    }

    friend Term operator/(const Term &T1, const Term &T2) {
      Term out(T1);
      out /= T2;
      return out;
    }

    friend bool operator<(const Term &T1, const Term &T2) {
      return T1.factors_ < T2.factors_;
    }

    std::string print() const;

    /// constructVariable returns a member of the Variable class representing
    /// the product outcome for this object. This must be specialized for the
    /// given Variable class if Variable doesn't have Variable(Coefficient) and
    /// arithmetic overloads NOTE: this ignores handleDivsionByZero factors, as
    /// we assume these are handled by the Polynomial classes version of this
    /// function.
    Variable constructVariable(Module_t *m) const {
      // we assume we don't want '1 * <everthing else>' so we
      // start with the first element (if it exists)

      const_iterator iter = factors_.cbegin();
      assert(!(iter->first).isNull());
      Variable out(Coefficient(1.));
      while (iter != factors_.cend()) {
        if (iter->second != 0) {
          out = takePower(*m, iter->first, iter->second);
          ++iter;
          break;
        } else
          ++iter;
      }

      while (iter != factors_.cend()) {
        assert(!(iter->first).isNull());
        if (iter->second < 0)
          out /= takePower(*m, iter->first, -iter->second);
        else if (iter->second > 0)
          out *= takePower(*m, iter->first, iter->second);
        ++iter;
      }

      return out;
    }

    std::set<Variable> getHandleDivisionByZeros() const {
      std::set<Variable> out;
      for (auto const &pair : factors_) {
        if (pair.second == 0 && isHandleDivisionByZero(pair.first))
          out.insert(pair.first);
      }
      return out;
    }

    friend Polynomial;
  };
};

} // namespace aqcc
} // namespace llvm

#endif // LLVM_AQCC_QCSIMPLIFY_H