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


#include "symbolic_mx.hpp"
#include "std_vector_tools.hpp"

using namespace std;

namespace casadi {

  SymbolicMX::SymbolicMX(const std::string& name, int nrow, int ncol) : name_(name) {
    setSparsity(Sparsity::dense(nrow, ncol));
  }

  SymbolicMX::SymbolicMX(const std::string& name, const Sparsity & sp) : name_(name) {
    setSparsity(sp);
  }

  std::string SymbolicMX::print(const std::vector<std::string>& arg) const {
    return name_;
  }

  void SymbolicMX::eval(const double** arg, double** res, int* iw, double* w, int mem) const {
  }

  void SymbolicMX::eval_sx(const SXElem** arg, SXElem** res, int* iw, SXElem* w, int mem) const {
  }

  void SymbolicMX::eval_mx(const std::vector<MX>& arg, std::vector<MX>& res) const {
  }

  void SymbolicMX::eval_forward(const std::vector<std::vector<MX> >& fseed,
                           std::vector<std::vector<MX> >& fsens) const {
  }

  void SymbolicMX::eval_reverse(const std::vector<std::vector<MX> >& aseed,
                           std::vector<std::vector<MX> >& asens) const {
  }

  const std::string& SymbolicMX::name() const {
    return name_;
  }

  void SymbolicMX::sp_fwd(const bvec_t** arg, bvec_t** res, int* iw, bvec_t* w, int mem) const {
    fill_n(res[0], nnz(), 0);
  }

  void SymbolicMX::sp_rev(bvec_t** arg, bvec_t** res, int* iw, bvec_t* w, int mem) const {
    fill_n(res[0], nnz(), 0);
  }

  bool SymbolicMX::has_duplicates() const {
    if (this->temp!=0) {
      userOut<true, PL_WARN>() << "Duplicate expression: " << name() << endl;
      return true;
    } else {
      this->temp = 1;
      return false;
    }
  }

  void SymbolicMX::reset_input() const {
    this->temp = 0;
  }

} // namespace casadi
