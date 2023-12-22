#ifndef UTIL_MATRIX_H
#define UTIL_MATRIX_H

#include<initializer_list>
#include<ostream>
#include<iterator>
#include<optional>
#include<cmath>
#include<limits>
#include<cstddef>

class Matrix4{
public:
  Matrix4(){
    v = new double[16];
    for(int i=0;i<16;++i) v[i]=0;
  }
  ~Matrix4(){ delete[] v; }
  Matrix4(std::initializer_list<double> li) : Matrix4(){
    double* cursor = v;
    for(auto it=li.begin();it!=li.end();++it){
      *cursor++ = *it;
      if(cursor >= v+16) break;
    }
  }
  Matrix4(const double* const li):Matrix4() { for(int i=0;i<16;++i) v[i]=li[i]; }
  Matrix4(const double li[4][4]):Matrix4() {
    double* cursor = v;
    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	*cursor++ = li[i][j];
  }

  
  Matrix4(const Matrix4& cp) : Matrix4(cp.v) {}
  Matrix4& operator = (const Matrix4& cp){
    for(int i=0;i<16;++i) v[i] = cp.v[i];
    return *this;
  }
  Matrix4(Matrix4&& mv){ v=mv.v; mv.v=nullptr; }
  Matrix4& operator = (Matrix4&& mv){
    delete[] v;
    v = mv.v;
    mv.v = nullptr;
    return *this;
  }
  
  
  static Matrix4 I(void){
    return Matrix4{1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      0,0,0,1};
  }

  bool operator == (const Matrix4& rhs) const;
  bool operator != (const Matrix4& rhs) const { return !(*this == rhs); }

  double* operator [](const int& idx){ return v + idx*4; }
  double const* const operator [](const int& idx)const{ return v + idx*4; }

  
  static Matrix4 Mul(const Matrix4& lhs, const Matrix4& rhs){
    Matrix4 r;
    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	r[i][j] = lhs[i][0]*rhs[0][j] + lhs[i][1]*rhs[1][j] + lhs[i][2]*rhs[2][j] + lhs[i][3]*rhs[3][j];
    return r;
  }
  Matrix4 operator * (const Matrix4& rhs) const{ return Matrix4::Mul(*this, rhs); }
  Matrix4& operator *= (const Matrix4& rhs){ return *this = *this * rhs; }

  static double Det(const Matrix4&);
  double Det(void) const;
  static Matrix4 Transpose(const Matrix4&);
  Matrix4& Transpose(void);
  static Matrix4 Inverse(const Matrix4&);
  Matrix4& Inverse(void);

  bool isvalid(void) const;
  bool hasnans(void) const;

  double* begin(void){ return v; }
  double* end(void){ return v+16; }

private:
  // data
  double* v;

public:
  friend std::ostream& operator << (std::ostream& os, const Matrix4& m){
    os<<'(';
    for(int i=0;i<4;++i){
      os<<'[';
      os << m.v[i*4] << ',' << m.v[i*4+1] << ',' << m.v[i*4+2] << ',' << m.v[i*4+3];
      os<<']';
    }
    os<<')';
    return os;
  }
};
// deprecated code above




template<std::size_t N, std::size_t M>
class Matrix{
public:
  static constexpr auto ROWS = N;
  static constexpr auto COLUMNS = M;

  // data
  std::array<std::array<double,COLUMNS>, ROWS> v;

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
	static constexpr auto EPS = std::numeric_limits<double>::epsilon();
	for(std::size_t i=toprow;i<ROWS;++i)
	  if(fabs(mat.v[i][pivcol]) > EPS) // the non-zero element
	    return std::optional<std::size_t>{i};
	return std::optional<std::size_t>{};
      };
      std::optional<std::size_t> pivrow = search_pivot_elem();

      // If no pivot is found in this column, continue to the next column
      if(!pivrow.has_value()) continue;
      
      mat.RowExchange(pivrow.value(), toprow);
      zero_out_pivcol(mat, toprow+1,ROWS,pivrow.value(), pivcol);
      
      ++toprow;
    }
  }
  // Backward phase: Normalizes each pivot row and zeroes out elements above the pivot.
  static void do_rref_bkwd_phase(Matrix<ROWS,COLUMNS>& mat){
    for(int i=ROWS-1;i>=0;--i){
      for(std::size_t j=0;j<COLUMNS;++j)
	if(mat.v[i][j] != 0){
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
  auto AppendRight(const Matrix<ROWS,K>& mat) -> decltype(auto) {
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
  auto AppendBottom(const Matrix<K,COLUMNS>& mat) -> decltype(auto) {
    using result_t = Matrix<ROWS+K, COLUMNS>;
    result_t res;
    for(std::size_t i=0;i<ROWS;++i) std::copy(v[i].cbegin(), v[i].cend(), res.v[i].begin());
    for(std::size_t i=0;i<K;++i) std::copy(mat.v[i].cbegin(), mat.v[i].cend(), res.v[i+ROWS].begin());
    return res;
  }


public:
  auto inv() -> decltype(auto){ return Matrix<ROWS,COLUMNS>::inv(*this); }
  static auto inv(Matrix<ROWS,COLUMNS> mat) -> decltype(auto) {
    return mat;
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


#endif
