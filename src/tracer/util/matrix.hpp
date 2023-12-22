#ifndef UTIL_MATRIX_H
#define UTIL_MATRIX_H

#include<initializer_list>
#include<ostream>
#include<sstream>
#include<iterator>
#include<optional>
#include<cmath>
#include<limits>
#include<cstddef>
#include<stdexcept>
#include<span>


template<std::size_t N, std::size_t M>
class Matrix{
public:
  static constexpr auto ROWS = N;
  static constexpr auto COLUMNS = M;

  // data & data access
  std::array<std::array<double,COLUMNS>, ROWS> v;
  std::array<double,COLUMNS>& operator [](const std::size_t& idx){ return v[idx]; }
  auto operator [](const std::size_t& idx) const -> decltype(auto) {
    return std::span<const double>(v[idx]);
  }
  

  Matrix() : v{} {}
  explicit Matrix(std::initializer_list<double> li) : Matrix(){
    auto it = li.begin();
    for(std::size_t i=0;i<ROWS;++i)
      for(std::size_t j=0;j<COLUMNS;++j){
	if(it == li.end()) break;
	v[i][j] = *it++;
      }
  }
  

  bool operator == (const Matrix<ROWS, COLUMNS>& rhs) const{
    static constexpr auto EPS = 1e-6;
    for(std::size_t i=0;i<ROWS;++i)
      for(std::size_t j=0;j<COLUMNS;++j)
	if(fabs(v[i][j]-rhs.v[i][j]) > EPS) return false;
    return true;
  }
  bool operator != (const Matrix<ROWS, COLUMNS>& rhs) const{ return !(*this == rhs); }

  // row operations
  auto RowExchange(std::size_t row1, std::size_t row2) -> decltype(auto) {
    std::swap(v[row1], v[row2]);
    return *this;
  }
  auto RowAdd(std::size_t addee, std::size_t adder, double multiplier) -> decltype(auto) {
    for(std::size_t i=0;i<COLUMNS;++i)
      v[addee][i] += v[adder][i] * multiplier;
    return *this;
  }
  auto RowScale(std::size_t rowidx, double multiplier) ->decltype(auto) {
    for(std::size_t i=0;i<COLUMNS;++i)
      v[rowidx][i] *= multiplier;
    return *this;
  }


public:
  template<std::size_t K>
  Matrix<ROWS,K> operator * (const Matrix<COLUMNS,K>& rhs) const {
    Matrix<ROWS,K> ans;
    for(std::size_t i=0;i<ROWS;++i)
      for(std::size_t j=0;j<COLUMNS;++j)
	for(std::size_t k=0;k<K;++k)
	  ans.v[i][k] += v[i][j] * rhs.v[j][k];
    return ans;
  }
  template<std::size_t K>
  Matrix<ROWS,K>& operator *=(const Matrix<COLUMNS,K>& rhs){
    return *this = *this * rhs;
  }

  
public:
  /**
   * @brief Transforms the matrix into its row-reduced echelon form (RREF).
   *
   * This method applies a series of elementary row operations to convert 
   * the matrix into its row-reduced echelon form. The process involves two
   * main phases: a forward phase that transforms the matrix into an upper
   * triangular form, and a backward phase that normalizes each pivot row
   * and zeroes out elements above each pivot.
   *
   * @return Matrix<ROWS, COLUMNS> The matrix in its row-reduced echelon form.
   */
  Matrix<ROWS,COLUMNS> rref() const{ return Matrix<ROWS,COLUMNS>::rref(*this); }
  static auto rref(Matrix<ROWS, COLUMNS> mat) -> decltype(auto) {
    do_rref_fwd_phase(mat);
    do_rref_bkwd_phase(mat);
    return mat;    
  }
private:			// helper functions for rref()
  // Forward phase: Transforms the matrix into an upper triangular form.
  // It iterates through each column, finds the first non-zero element 
  // as the pivot, then zeroes out all elements below the pivot.
  static void do_rref_fwd_phase(Matrix<ROWS, COLUMNS>& mat){
    std::size_t toprow = 0;
    for(std::size_t pivcol=0;pivcol < COLUMNS; ++pivcol){
      auto search_pivot_elem = [&](){
	static constexpr auto EPS = 1e-8;
	for(std::size_t i=toprow;i<ROWS;++i)
	  if(fabs(mat.v[i][pivcol]) > EPS) // the non-zero element
	    return std::optional<std::size_t>{i};
	return std::optional<std::size_t>{};
      };
      std::optional<std::size_t> pivrow = search_pivot_elem();

      // If no pivot is found in this column, continue to the next column
      if(!pivrow.has_value()) continue;
      
      mat.RowExchange(pivrow.value(), toprow);
      zero_out_pivcol(mat, toprow+1,ROWS, toprow, pivcol);
      
      ++toprow;
    }
  }
  // Backward phase: Normalizes each pivot row and zeroes out elements above the pivot.
  static void do_rref_bkwd_phase(Matrix<ROWS,COLUMNS>& mat){
    for(int i=ROWS-1;i>=0;--i){
      static constexpr auto EPS = 1e-8;
      for(std::size_t j=0;j<COLUMNS;++j)
	if(fabs(mat.v[i][j]) > EPS){
	  mat.RowScale(i, 1.0/mat.v[i][j]);
	  zero_out_pivcol(mat, 0,i, i,j);
	  break;
	}
    }
  }
  // Zeroes out elements in a specified pivot column.
  // For each row in the specified range, it subtracts an appropriate multiple of the 
  // pivot row from it, effectively setting the column element in that row to zero.
  static void zero_out_pivcol(Matrix<ROWS,COLUMNS>& mat, std::size_t fr, std::size_t to, std::size_t pivrow, std::size_t pivcol){
    for(auto it=fr; it<to; ++it)
      mat.RowAdd(it, pivrow, -mat.v[it][pivcol]/mat.v[pivrow][pivcol]);
  }


  
public:
  /**
   * @brief Computes the transpose of the matrix.
   *
   * @return Matrix<COLUMNS, ROWS> - A new object which is the transpose of the original matrix.
   *
   * Example:
   *   Matrix<3,2> mat; // Original matrix of size 3x2.
   *   auto transposedMat = mat.T(); // Transposed matrix of size 2x3.
   */
  Matrix<COLUMNS,ROWS> T(void) const{
    Matrix<COLUMNS,ROWS> ret;
    for(std::size_t i=0;i<ROWS;++i)
      for(std::size_t j=0;j<COLUMNS;++j)
	ret.v[j][i] = v[i][j];
    return ret;
  }

public:
  /**
   * @brief Appends another matrixs to the right side of the current matrix.
   * 
   * @return Matrix<ROWS, COLUMNS + K> - a new object that is the result of appending
   * 'mat' to the right side of the current matrix.
   */
  template<std::size_t K>
  auto AppendRight(const Matrix<ROWS,K>& mat) const -> decltype(auto) {
    using result_t = Matrix<ROWS, COLUMNS+K>;
    result_t res;
    for(std::size_t i=0;i<ROWS;++i){
      std::copy(v[i].cbegin(), v[i].cend(), res.v[i].begin());
      std::copy(mat.v[i].cbegin(), mat.v[i].cend(), res.v[i].begin()+COLUMNS);
    }
    return res;
  }
  
  /**
   * @brief Appends another matrix of the same number of columns to the bottom of the current matrix.
   * 
   * @return Matrix<ROWS + K, COLUMNS> - a new object that is the result of appending 'mat'
   *   to the bottom of the current matrix.
   */
  template<std::size_t K>
  auto AppendBottom(const Matrix<K,COLUMNS>& mat) const -> decltype(auto) {
    using result_t = Matrix<ROWS+K, COLUMNS>;
    result_t res;
    for(std::size_t i=0;i<ROWS;++i) std::copy(v[i].cbegin(), v[i].cend(), res.v[i].begin());
    for(std::size_t i=0;i<K;++i) std::copy(mat.v[i].cbegin(), mat.v[i].cend(), res.v[i+ROWS].begin());
    return res;
  }


public:
  /**
 * @brief Calculates the inverse of a square matrix.
 *
 * Computes the inverse of a square matrix using the
 * row-reduced echelon form (RREF) approach. It appends an identity matrix to 
 * the right of the original matrix and then applies the RREF algorithm. The 
 * right half of the resultant matrix is the inverse of the original matrix.
 *
 * @pre The matrix must be square (N x N). If the matrix is not square, the
 * method throws a std::runtime_error.
 *
 * @post If the matrix is singular (i.e., does not have an inverse), the method
 * throws a std::runtime_error indicating that the matrix is non-invertible.
 *
 * @return Matrix<N,N> The inverse of the input matrix.
 *
 * @throws std::runtime_error if the matrix is not square or is singular.
 *
 * @example
 * Matrix<3,3> matrix; // Initialize matrix
 * Matrix<3,3> inverseMatrix = matrix.inv();
 */
  auto inv() const -> decltype(auto){ return Matrix<ROWS,COLUMNS>::inv(*this); }
  static auto inv(const Matrix<ROWS,COLUMNS>& mat) -> decltype(auto) {
    if constexpr(ROWS != COLUMNS){
      throw std::runtime_error("MatrixInvert only works with square matrices");
    } else {
      static constexpr auto K = ROWS;
      auto I_n = Matrix<K,K>::I();
    
      auto augmat = mat.AppendRight(I_n);
      augmat = augmat.rref();

      auto lside_is_In = [](const Matrix<K,K+K>& mat){
	static constexpr auto EPS = 1e-8;
	for(std::size_t i=0;i<K;++i)
	  for(std::size_t j=0;j<K;++j){
	    double expect = i==j ? 1 : 0;
	    if(fabs(mat.v[i][j]-expect) > EPS) return false;
	  } return true;
      };
      if(not lside_is_In(augmat)) {
	std::ostringstream ss;
	ss << "Singular matrix in MatirxInvert";
	ss << ", with mat=" << mat;
	ss << " augmat=" << augmat;
	throw std::runtime_error(ss.str());
      }

      Matrix<K,K> ans;
      for(std::size_t i=0;i<K;++i)
	for(std::size_t j=0;j<K;++j)
	  ans.v[i][j] = augmat.v[i][j+K];
      return ans;
    }
  }

public:				// several useful factory methods
  static Matrix<ROWS,COLUMNS> I(void) noexcept{
    Matrix<ROWS,COLUMNS> In;
    for(std::size_t i=0;i<ROWS&&i<COLUMNS;++i)
      In.v[i][i] = 1;
    return In;
  }
    
  
public:
  friend std::ostream& operator << (std::ostream& os, const Matrix<ROWS,COLUMNS> m){
    os << '(';
    for(std::size_t i=0;i<ROWS;++i){
      os << '[';
      std::copy(m.v[i].cbegin(), m.v[i].cend(),
		std::ostream_iterator<double>(os, ","));
      os << "],";
    }
    os << ')';
    return os;
  }
};

using Matrix4 = Matrix<4,4>;

#endif
