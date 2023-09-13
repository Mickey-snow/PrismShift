#ifndef UTIL_MATRIX_H
#define UTIL_MATRIX_H

#include<initializer_list>

class Matrix4{
public:
  Matrix4(){ for(int i=0;i<16;++i) v[i]=0; }
  Matrix4(std::initializer_list<double> li) : Matrix4(){
    double* cursor = v;
    for(auto it=li.begin();it!=li.end();++it){
      *cursor++ = *it;
      if(cursor >= v+16) break;
    }
  }
  static Matrix4 I(void){
    return Matrix4{1,0,0,0,
      0,1,0,0,
      0,0,1,0,
      0,0,0,1};
  }

  bool operator == (const Matrix4& rhs) const{
    for(int i=0;i<16;++i) if(v[i]!=rhs.v[i]) return false;
    return true;
  }
  bool operator != (const Matrix4& rhs) const { return !(*this == rhs); }

  double* operator [](const int& idx){ return v + idx*4; }
  double* operator [](const int& idx)const{ return v + idx*4; }

  
  static Matrix4 Mul(const Matrix4& lhs, const Matrix4& rhs){
    Matrix4 r;
    for(int i=0;i<4;++i)
      for(int j=0;j<4;++j)
	r[i][j] = lhs[i][0]*rhs[0][j] + lhs[i][1]*rhs[1][j] + lhs[i][2]*rhs[2][j] + lhs[i][3]*rhs[3][j];
    return r;
  }
  Matrix4 operator * (const Matrix4& rhs) const{ return Matrix4::Mul(*this, rhs); }
  Matrix4& operator *= (const Matrix4& rhs){ return *this = *this * rhs; }

  static Matrix4 Transpose(const Matrix4&);
  Matrix4& Transpose(void);
  static Matrix4 Inverse(const Matrix4&);
  Matrix4& Inverse(void);

  double v[16];
};

#endif
