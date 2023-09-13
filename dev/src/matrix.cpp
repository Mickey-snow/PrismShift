#include "matrix.hpp"

#include<stdexcept>
#include<cmath>

bool Matrix4::operator == (const Matrix4& rhs) const{
  for(int i=0;i<16;++i)
      if(fabs(v[i]-rhs.v[i]) > 1e-6) return false;
  return true;
}

double Matrix4::Det(const Matrix4& m){
  auto Det3x3 = [](const int* a){
    return a[0]*(a[4]*a[8]-a[5]*a[7])
      - a[1]*(a[3]*a[8]-a[5]*a[6])
      + a[2]*(a[3]*a[7]-a[4]*a[6]);
  };
  auto getCofactorij = [](const Matrix4& m, const int row,const int col, int* a){
    for(int i=0;i<4;++i){
      if(i == row) continue;
      for(int j=0;j<4;++j){
	if(j == col) continue;
	a[(i-(i>row))*3+j-(j>col)] = m[i][j];
      }
    }
  };

  int m_cofactor[9];
  double ans = 0;
  for(int i=0;i<4;++i){
    getCofactorij(m,0,i,m_cofactor);
    ans += (((i&1)==0)?(1):(-1)) * m[0][i] * Det3x3(m_cofactor);
  }
  
  return ans;
}
double Matrix4::Det(void) const { return Matrix4::Det(*this); }


Matrix4 Matrix4::Transpose(const Matrix4& m){
  return Matrix4{
    m[0][0],m[1][0],m[2][0],m[3][0],
    m[0][1],m[1][1],m[2][1],m[3][1],
    m[0][2],m[1][2],m[2][2],m[3][2],
    m[0][3],m[1][3],m[2][3],m[3][3]
  };
}
Matrix4& Matrix4::Transpose(void){ return *this = Matrix4::Transpose(*this); }


Matrix4 Matrix4::Inverse(const Matrix4& m){
  int indxc[4], indxr[4];
  int ipiv[4] = {0, 0, 0, 0};
  double minv[4][4];
  for(int i=0;i<4;++i)for(int j=0;j<4;++j) minv[i][j] = m[i][j];
  
  for (int i = 0; i < 4; i++) {
    int irow = 0, icol = 0;
    double big = 0.f;
    // Choose pivot
    for (int j = 0; j < 4; j++) {
      if (ipiv[j] != 1) {
	for (int k = 0; k < 4; k++) {
	  if (ipiv[k] == 0) {
	    if (std::abs(minv[j][k]) >= big) {
	      big = std::fabs(minv[j][k]);
	      irow = j;
	      icol = k;
	    }
	  } else if (ipiv[k] > 1)
	    throw std::runtime_error("Singular matrix in MatrixInvert");
	}
      }
    }
    ++ipiv[icol];
    // Swap rows _irow_ and _icol_ for pivot
    if (irow != icol) {
      for (int k = 0; k < 4; ++k) std::swap(minv[irow][k], minv[icol][k]);
    }
    indxr[i] = irow;
    indxc[i] = icol;
    if (minv[icol][icol] == 0.f)
      throw std::runtime_error("Singular matrix in MatrixInvert");

    // Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
    double pivinv = 1. / minv[icol][icol];
    minv[icol][icol] = 1.;
    for (int j = 0; j < 4; j++) minv[icol][j] *= pivinv;

    // Subtract this row from others to zero out their columns
    for (int j = 0; j < 4; j++) {
      if (j != icol) {
	double save = minv[j][icol];
	minv[j][icol] = 0;
	for (int k = 0; k < 4; k++) minv[j][k] -= minv[icol][k] * save;
      }
    }
  }
  // Swap columns to reflect permutation
  for (int j = 3; j >= 0; j--) {
    if (indxr[j] != indxc[j]) {
      for (int k = 0; k < 4; k++)
	std::swap(minv[k][indxr[j]], minv[k][indxc[j]]);
    }
  }
  
  return Matrix4(minv);
}

Matrix4& Matrix4::Inverse(void){ return *this = Matrix4::Inverse(*this); }


bool Matrix4::hasnans(void) const{
  for(int i=0;i<16;++i)
    if(std::isnan(v[i])) return true;
  return false;
}
bool Matrix4::isvalid(void) const{ return !hasnans(); }
