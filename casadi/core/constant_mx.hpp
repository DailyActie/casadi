/*
 *    This file is part of CasADi.
 *
 *    CasADi -- A symbolic framework for dynamic optimization.
 *    Copyright (C) 2010-2014 Joel Andersson, Joris Gillis, Moritz Diehl,
 *                            K.U. Leuven. All rights reserved.
 *    Copyright (C) 2011-2014 Greg Horn
 *
 *    CasADi is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Lesser General Public
 *    License as published by the Free Software Foundation; either
 *    version 3 of the License, or (at your option) any later version.
 *
 *    CasADi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public
 *    License along with CasADi; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */


#ifndef CASADI_CONSTANT_MX_HPP
#define CASADI_CONSTANT_MX_HPP

#include "mx_node.hpp"
#include <iomanip>
#include <iostream>

/// \cond INTERNAL

namespace casadi {

/** \brief Represents an MX that is only composed of a constant.
        \author Joel Andersson
        \date 2010-2013

        A regular user is not supposed to work with this Node class.
        This user can call MX(double) directly, or even rely on implicit typecasting.
        \sa zeros , ones
*/
  class CASADI_EXPORT ConstantMX : public MXNode {
  public:
    /// Destructor
    explicit ConstantMX(const Sparsity& sp);

    /// Destructor
    ~ConstantMX() override = 0;

    // Creator (all values are the same integer)
    static ConstantMX* create(const Sparsity& sp, int val);

    // Creator (all values are the same floating point value)
    static ConstantMX* create(const Sparsity& sp, double val);

    // Creator (values may be different)
    static ConstantMX* create(const Matrix<double>& val);

    /// Evaluate the function numerically
    void eval(const double** arg, double** res, int* iw, double* w, int mem) const override = 0;

    /// Evaluate the function symbolically (SX)
    void eval_sx(const SXElem** arg, SXElem** res,
                         int* iw, SXElem* w, int mem) const override = 0;

    /** \brief  Evaluate symbolically (MX) */
    void eval_mx(const std::vector<MX>& arg, std::vector<MX>& res) const override;

    /** \brief Calculate forward mode directional derivatives */
    void eval_forward(const std::vector<std::vector<MX> >& fseed,
                         std::vector<std::vector<MX> >& fsens) const override;

    /** \brief Calculate reverse mode directional derivatives */
    void eval_reverse(const std::vector<std::vector<MX> >& aseed,
                         std::vector<std::vector<MX> >& asens) const override;

    /** \brief  Propagate sparsity forward */
    void sp_fwd(const bvec_t** arg, bvec_t** res, int* iw, bvec_t* w, int mem) const override;

    /** \brief  Propagate sparsity backwards */
    void sp_rev(bvec_t** arg, bvec_t** res, int* iw, bvec_t* w, int mem) const override;

    /** \brief Get the operation */
    int op() const override { return OP_CONST;}

    /// Get the value (only for scalar constant nodes)
    double to_double() const override = 0;

    /// Get the value (only for constant nodes)
    Matrix<double> getMatrixValue() const override = 0;

    /// Matrix multiplication
    //    virtual MX getMultiplication(const MX& y) const;

    /// Inner product
    MX getDot(const MX& y) const override;

    /// Return truth value of an MX
    bool __nonzero__() const override;

    /** \brief  Check if valid function input */
    bool is_valid_input() const override;

    /** \brief Get the number of symbolic primitives */
    int n_primitives() const override;

    /** \brief Get symbolic primitives */
    void primitives(std::vector<MX>::iterator& it) const override;

    /** \brief Split up an expression along symbolic primitives */
    void split_primitives(const MX& x, std::vector<MX>::iterator& it) const override;

    /** \brief Join an expression along symbolic primitives */
    MX join_primitives(std::vector<MX>::const_iterator& it) const override;

    /** \brief Detect duplicate symbolic expressions */
    bool has_duplicates() const override { return false;}

    /** \brief Reset the marker for an input expression */
    void reset_input() const override {}
  };

  /// A constant given as a DM
  class CASADI_EXPORT ConstantDM : public ConstantMX {
  public:

    /** \brief  Constructor */
    explicit ConstantDM(const Matrix<double>& x) : ConstantMX(x.sparsity()), x_(x) {}

    /// Destructor
    ~ConstantDM() override {}

    /** \brief  Print expression */
    std::string print(const std::vector<std::string>& arg) const override {
      return x_.getDescription();
    }

    /** \brief  Evaluate the function numerically */
    void eval(const double** arg, double** res, int* iw, double* w, int mem) const override {
      std::copy(x_->begin(), x_->end(), res[0]);
    }

    /** \brief  Evaluate the function symbolically (SX) */
    void eval_sx(const SXElem** arg, SXElem** res,
                         int* iw, SXElem* w, int mem) const override {
      std::copy(x_->begin(), x_->end(), res[0]);
    }

    /** \brief Generate code for the operation */
    void generate(CodeGenerator& g, const std::string& mem,
                          const std::vector<int>& arg, const std::vector<int>& res) const override;

    /** \brief  Check if a particular integer value */
    bool is_zero() const override;
    bool is_one() const override;
    bool is_minus_one() const override;
    bool is_identity() const override;

    /// Get the value (only for scalar constant nodes)
    double to_double() const override {return x_.scalar();}

    /// Get the value (only for constant nodes)
    Matrix<double> getMatrixValue() const override { return x_;}

    /** \brief Check if two nodes are equivalent up to a given depth */
    bool is_equal(const MXNode* node, int depth) const override;

    /** \brief  data member */
    Matrix<double> x_;
  };

  /// A zero-by-zero matrix
  class CASADI_EXPORT ZeroByZero : public ConstantMX {
  private:
    /** \brief Private constructor (singleton design pattern) */
    explicit ZeroByZero() : ConstantMX(Sparsity(0, 0)) {
      initSingleton();
    }

  public:
    /** \brief Get a pointer to the singleton */
    static ZeroByZero* getInstance() {
      static ZeroByZero instance;
      return &instance;
    }

    /// Destructor
    ~ZeroByZero() override {
      destroySingleton();
    }

    /** \brief  Print expression */
    std::string print(const std::vector<std::string>& arg) const override;

    /** \brief  Evaluate the function numerically */
    /// Evaluate the function numerically
    void eval(const double** arg, double** res, int* iw, double* w, int mem) const override {}

    /// Evaluate the function symbolically (SX)
    void eval_sx(const SXElem** arg, SXElem** res,
                         int* iw, SXElem* w, int mem) const override {}

    /** \brief Generate code for the operation */
    void generate(CodeGenerator& g, const std::string& mem,
                      const std::vector<int>& arg, const std::vector<int>& res) const override {}

    /// Get the value (only for scalar constant nodes)
    double to_double() const override { return 0;}

    /// Get the value (only for constant nodes)
    DM getMatrixValue() const override { return DM(); }

    /// Get densification
    MX getProject(const Sparsity& sp) const override;

    /// Get the nonzeros of matrix
    MX getGetNonzeros(const Sparsity& sp, const std::vector<int>& nz) const override;

    /// Assign the nonzeros of a matrix to another matrix
    MX getSetNonzeros(const MX& y, const std::vector<int>& nz) const override;

    /// Transpose
    MX getTranspose() const override;

    /// Get a unary operation
    MX getUnary(int op) const override;

    /// Get a binary operation operation
    MX getBinary(int op, const MX& y, bool ScX, bool ScY) const override;

    /// Reshape
    MX getReshape(const Sparsity& sp) const override;

    /** \brief  Check if valid function input */
    bool is_valid_input() const override { return true;}

    /** \brief  Get the name */
    const std::string& name() const override {
      static std::string dummyname;
      return dummyname;
    }
  };

  /** \brief Constant known at runtime */
  template<typename T>
  struct RuntimeConst {
    const T value;
    RuntimeConst() {}
    RuntimeConst(T v) : value(v) {}
  };

  /** \brief  Constant known at compiletime */
  template<int v>
  struct CompiletimeConst {
    static const int value = v;
  };

  /// A constant with all entries identical
  template<typename Value>
  class CASADI_EXPORT Constant : public ConstantMX {
  public:

    /** \brief  Constructor */
    explicit Constant(const Sparsity& sp, Value v = Value()) : ConstantMX(sp), v_(v) {}

    /// Destructor
    ~Constant() override {}

    /** \brief  Print expression */
    std::string print(const std::vector<std::string>& arg) const override;

    /** \brief  Evaluate the function numerically */
    /// Evaluate the function numerically
    void eval(const double** arg, double** res, int* iw, double* w, int mem) const override;

    /// Evaluate the function symbolically (SX)
    void eval_sx(const SXElem** arg, SXElem** res, int* iw, SXElem* w, int mem) const override;

    /** \brief Generate code for the operation */
    void generate(CodeGenerator& g, const std::string& mem,
                          const std::vector<int>& arg, const std::vector<int>& res) const override;

    /** \brief  Check if a particular integer value */
    bool is_zero() const override { return v_.value==0;}
    bool is_one() const override { return v_.value==1;}
    bool is_identity() const override { return v_.value==1 && sparsity().is_diag();}
    bool isValue(double val) const override { return v_.value==val;}

    /// Get the value (only for scalar constant nodes)
    double to_double() const override {
      return v_.value;
    }

    /// Get the value (only for constant nodes)
    Matrix<double> getMatrixValue() const override {
      return Matrix<double>(sparsity(), v_.value, false);
    }

    /// Get densification
    MX getProject(const Sparsity& sp) const override;

    /// Get the nonzeros of matrix
    MX getGetNonzeros(const Sparsity& sp, const std::vector<int>& nz) const override;

    /// Assign the nonzeros of a matrix to another matrix
    MX getSetNonzeros(const MX& y, const std::vector<int>& nz) const override;

    /// Transpose
    MX getTranspose() const override;

    /// Get a unary operation
    MX getUnary(int op) const override;

    /// Get a binary operation operation
    MX getBinary(int op, const MX& y, bool ScX, bool ScY) const override;

    /// Reshape
    MX getReshape(const Sparsity& sp) const override;

    /// Create a horizontal concatenation node
    MX getHorzcat(const std::vector<MX>& x) const override;

    /// Create a vertical concatenation node (vectors only)
    MX getVertcat(const std::vector<MX>& x) const override;

    /** \brief Check if two nodes are equivalent up to a given depth */
    bool is_equal(const MXNode* node, int depth) const override;

    /** \brief The actual numerical value */
    Value v_;
  };

  template<typename Value>
  MX Constant<Value>::getHorzcat(const std::vector<MX>& x) const {
    // Check if all arguments have the same constant value
    for (auto&& i : x) {
      if (!i->isValue(v_.value)) {
        // Not all the same value, fall back to base class
        return ConstantMX::getHorzcat(x);
      }
    }

    // Assemble the sparsity pattern
    std::vector<Sparsity> sp;
    for (auto&& i : x) sp.push_back(i.sparsity());
    return MX(horzcat(sp), v_.value, false);
  }

  template<typename Value>
  MX Constant<Value>::getVertcat(const std::vector<MX>& x) const {
    // Check if all arguments have the same constant value
    for (auto&& i : x) {
      if (!i->isValue(v_.value)) {
        // Not all the same value, fall back to base class
        return ConstantMX::getVertcat(x);
      }
    }

    // Assemble the sparsity pattern
    std::vector<Sparsity> sp;
    for (auto&& i : x) sp.push_back(i.sparsity());
    return MX(vertcat(sp), v_.value, false);
  }

  template<typename Value>
  MX Constant<Value>::getReshape(const Sparsity& sp) const {
    return MX::create(new Constant<Value>(sp, v_));
  }

  template<typename Value>
  MX Constant<Value>::getTranspose() const {
    return MX::create(new Constant<Value>(sparsity().T(), v_));
  }

  template<typename Value>
  MX Constant<Value>::getUnary(int op) const {
    // Constant folding
    double ret(0);
    casadi_math<double>::fun(op, v_.value, 0.0, ret);
    if (operation_checker<F0XChecker>(op) || sparsity().is_dense()) {
      return MX(sparsity(), ret);
    } else {
      if (v_.value==0) {
        if (is_zero() && operation_checker<F0XChecker>(op)) {
          return MX(sparsity(), ret, false);
        } else {
          return repmat(MX(ret), size1(), size2());
        }
      }
      double ret2;
      casadi_math<double>::fun(op, 0, 0.0, ret2);
      return DM(sparsity(), ret, false)
        + DM(sparsity().pattern_inverse(), ret2, false);
    }
  }

  template<typename Value>
  MX Constant<Value>::getBinary(int op, const MX& y, bool ScX, bool ScY) const {
    casadi_assert(sparsity()==y.sparsity() || ScX || ScY);

    if (ScX && !operation_checker<FX0Checker>(op)) {
      double ret;
      casadi_math<double>::fun(op, nnz()> 0 ? v_.value: 0, 0, ret);

      if (ret!=0) {
        Sparsity f = Sparsity::dense(y.size1(), y.size2());
        MX yy = project(y, f);
        return MX(f, shared_from_this<MX>())->getBinary(op, yy, false, false);
      }
    } else if (ScY && !operation_checker<F0XChecker>(op)) {
      bool grow = true;
      if (y->op()==OP_CONST && dynamic_cast<const ConstantDM*>(y.get())==0) {
        double ret;
        casadi_math<double>::fun(op, 0, y.nnz()>0 ? y->to_double() : 0, ret);
        grow = ret!=0;
      }
      if (grow) {
        Sparsity f = Sparsity::dense(size1(), size2());
        MX xx = project(shared_from_this<MX>(), f);
        return xx->getBinary(op, MX(f, y), false, false);
      }
    }

    switch (op) {
    case OP_ADD:
      if (v_.value==0) return ScY && !y->is_zero() ? repmat(y, size1(), size2()) : y;
      break;
    case OP_SUB:
      if (v_.value==0) return ScY && !y->is_zero() ? repmat(-y, size1(), size2()) : -y;
      break;
    case OP_MUL:
      if (v_.value==1) return y;
      if (v_.value==-1) return -y;
      if (v_.value==2) return y->getUnary(OP_TWICE);
      break;
    case OP_DIV:
      if (v_.value==1) return y->getUnary(OP_INV);
      if (v_.value==-1) return -y->getUnary(OP_INV);
      break;
    case OP_POW:
      if (v_.value==0) return MX::zeros(y.sparsity());
      if (v_.value==1) return MX::ones(y.sparsity());
      if (v_.value==std::exp(1.0)) return y->getUnary(OP_EXP);
      break;
    default: break; //no rule
    }

    // Constant folding
    // NOTE: ugly, should use a function instead of a cast
    if (y->op()==OP_CONST && dynamic_cast<const ConstantDM*>(y.get())==0) {
      double y_value = y.nnz()>0 ? y->to_double() : 0;
      double ret;
      casadi_math<double>::fun(op, nnz()> 0 ? v_.value: 0, y_value, ret);

      return MX(y.sparsity(), ret, false);
    }

    // Fallback
    return MXNode::getBinary(op, y, ScX, ScY);
  }

  template<typename Value>
  void Constant<Value>::eval(const double** arg, double** res, int* iw, double* w, int mem) const {
    std::fill(res[0], res[0]+nnz(), static_cast<double>(v_.value));
  }

  template<typename Value>
  void Constant<Value>::
  eval_sx(const SXElem** arg, SXElem** res, int* iw, SXElem* w, int mem) const {
    std::fill(res[0], res[0]+nnz(), SXElem(v_.value));
  }

  template<typename Value>
  void Constant<Value>::generate(CodeGenerator& g, const std::string& mem,
                                 const std::vector<int>& arg, const std::vector<int>& res) const {
    if (nnz()==0) {
      // Quick return
    } else if (nnz()==1) {
      g << g.workel(res[0]) << " = " << g.constant(v_.value) << ";\n";
    } else {
      g << g.fill(g.work(res[0], nnz()), nnz(), g.constant(v_.value)) << '\n';
    }
  }

  template<typename Value>
  MX Constant<Value>::getGetNonzeros(const Sparsity& sp, const std::vector<int>& nz) const {
    if (v_.value!=0) {
      // Check if any "holes"
      for (std::vector<int>::const_iterator k=nz.begin(); k!=nz.end(); ++k) {
        if (*k<0) {
          // Do not simplify
          return MXNode::getGetNonzeros(sp, nz);
        }
      }
    }
    return MX::create(new Constant<Value>(sp, v_));
  }

  template<typename Value>
  MX Constant<Value>::getSetNonzeros(const MX& y, const std::vector<int>& nz) const {
    if (y.is_constant() && y->is_zero() && v_.value==0) {
      return y;
    }

    // Fall-back
    return MXNode::getSetNonzeros(y, nz);
  }

  template<typename Value>
  MX Constant<Value>::getProject(const Sparsity& sp) const {
    if (is_zero()) {
      return MX::create(new Constant<Value>(sp, v_));
    } else if (sp.is_dense()) {
      return densify(getMatrixValue());
    } else {
      return MXNode::getProject(sp);
    }
  }

  template<typename Value>
  std::string
  Constant<Value>::print(const std::vector<std::string>& arg) const {
    std::stringstream ss;
    if (sparsity().is_scalar()) {
      // Print scalar
      if (sparsity().nnz()==0) {
        ss << "00";
      } else {
        ss << v_.value;
      }
    } else if (sparsity().is_empty()) {
      // Print empty
      sparsity().print_compact(ss);
    } else {
      // Print value
      if (v_.value==0) {
        ss << "zeros(";
      } else if (v_.value==1) {
        ss << "ones(";
      } else if (v_.value!=v_.value) {
        ss << "nan(";
      } else if (v_.value==std::numeric_limits<double>::infinity()) {
        ss << "inf(";
      } else if (v_.value==-std::numeric_limits<double>::infinity()) {
        ss << "-inf(";
      } else {
        ss << "all_" << v_.value << "(";
      }

      // Print sparsity
      sparsity().print_compact(ss);
      ss << ")";
    }
    return ss.str();
  }

  template<typename Value>
  bool Constant<Value>::is_equal(const MXNode* node, int depth) const {
    return node->isValue(v_.value) && sparsity()==node->sparsity();
  }

} // namespace casadi
/// \endcond


#endif // CASADI_CONSTANT_MX_HPP
