#ifndef UTIL_MATRIX_H
#define UTIL_MATRIX_H

#include<initializer_list>
#include<ostream>

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
  const double* operator [](const int& idx)const{ return v + idx*4; }

  
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
    os<<'[';
    for(int i=0;i<4;++i){
      os<<'[';
      os << m.v[i*4] << ',' << m.v[i*4+1] << ',' << m.v[i*4+2] << ',' << m.v[i*4+3];
      os<<']';
    }
    os<<']';
    return os;
  }
};

#endif
