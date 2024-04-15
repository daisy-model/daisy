#ifndef SOLVER_H
#define SOLVER_H

#include "model.h"
#include "block_model.h"

#include <boost/numeric/ublas/vector.hpp>

#ifdef USE_DENSE_MATRIX
#include <boost/numeric/ublas/matrix.hpp>
typedef boost::numeric::ublas::matrix<double> SMatrix_type;
#else  // compressed matrix
#include <boost/numeric/ublas/matrix_sparse.hpp>
typedef boost::numeric::ublas::compressed_matrix<double> SMatrix_type;
#endif // compressed matrix

class Solver : public Model {
public:
  static const char *const component;
  typedef boost::numeric::ublas::vector<double> Vector;
  
  struct Matrix : public SMatrix_type {
  Matrix(const size_t size)
    : SMatrix_type(size, size)
      {}
    using SMatrix_type::operator=;
    void resize (size_t size) {
      SMatrix_type::resize(size, size, false);
    }
  };
  
  // Use.
public:
  void solve (Matrix& A, const Vector& b, Vector& x) const {}

  // Create and Destroy.
private:
  Solver ();			// Disable
public:
  Solver (const BlockModel& al) {};
  ~Solver () {};
};

#endif
