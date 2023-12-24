#include "transform.hpp"
#include "geometry.hpp"
#include "ray.hpp"
#include "aabb.hpp"

#include<cmath>

Point3 Transformation::operator() (const Point3& p) const{
  double x=p.x(),y=p.y(),z=p.z();
  double xp = m[0][0]*x + m[0][1]*y + m[0][2]*z + m[0][3];
  double yp = m[1][0]*x + m[1][1]*y + m[1][2]*z + m[1][3];
  double zp = m[2][0]*x + m[2][1]*y + m[2][2]*z + m[2][3];
  double wp = m[3][0]*x + m[3][1]*y + m[3][2]*z + m[3][3];
  return Point3(xp,yp,zp) / wp;
}

Vector3 Transformation::operator() (const Vector3& p) const{
  double x=p.x(),y=p.y(),z=p.z();
  return Vector3(m[0][0]*x + m[0][1]*y + m[0][2]*z,
		 m[1][0]*x + m[1][1]*y + m[1][2]*z,
		 m[2][0]*x + m[2][1]*y + m[2][2]*z);
		 
}

Normal Transformation::operator() (const Normal& p) const{
  double x=p.x(),y=p.y(),z=p.z();
  double xp = minv[0][0]*x + minv[1][0]*y + minv[2][0]*z;
  double yp = minv[0][1]*x + minv[1][1]*y + minv[2][1]*z;
  double zp = minv[0][2]*x + minv[1][2]*y + minv[2][2]*z;
  return Normal(xp,yp,zp);
}

  

Transformation Transformation::Translate(const Vector3& p){
  return Transformation(Matrix4{1,0,0,p.x(),
			   0,1,0,p.y(),
			   0,0,1,p.z(),
			   0,0,0,1},
    Matrix4{1,0,0,-p.x(),
	    0,1,0,-p.y(),
	    0,0,1,-p.z(),
	    0,0,0,1});
}
Transformation Transformation::Translate(const double& dx, const double& dy, const double& dz){
  return Translate(Vector3{dx,dy,dz});
}



Transformation Transformation::Rotate(Vector3 axis, const double& costheta, const double& sintheta){
  axis = axis.Normalized();
    
  auto R = [](const Vector3& axis, const double& costheta, const double& sintheta){
    const double nx=axis.x(), ny=axis.y(), nz=axis.z();
    return Matrix4{
      nx*nx*(1-costheta)+costheta, nx*ny*(1-costheta)+nz*sintheta, nx*nz*(1-costheta)-ny*sintheta, 0,
      nx*ny*(1-costheta)-nz*sintheta, ny*ny*(1-costheta)+costheta, ny*nz*(1-costheta)+nx*sintheta, 0,
      nx*nz*(1-costheta)+ny*sintheta, ny*nz*(1-costheta)-nx*sintheta, nz*nz*(1-costheta)+costheta, 0,
      0,0,0,1
    };
  };

  auto RotateMat = R(axis, costheta, sintheta);
  auto RotateMatInv = R(axis, costheta, -sintheta);

  return Transformation(RotateMat, RotateMatInv);
}
Transformation Transformation::Rotate(Vector3 axis, const double& theta){
  double costheta = cos(theta);
  double sintheta = sin(theta);
  return Rotate(axis, costheta, sintheta);
}
Transformation Transformation::RotateX(const double& theta){ return Rotate(Vector3{1,0,0}, theta); }
Transformation Transformation::RotateX(const double& costheta, const double& sintheta){ return Rotate(Vector3{1,0,0}, costheta, sintheta); }
Transformation Transformation::RotateY(const double& theta){ return Rotate(Vector3{0,1,0}, theta); }
Transformation Transformation::RotateY(const double& costheta, const double& sintheta){ return Rotate(Vector3{0,1,0}, costheta, sintheta); }
Transformation Transformation::RotateZ(const double& theta){ return Rotate(Vector3{0,0,1}, theta); }
Transformation Transformation::RotateZ(const double& costheta, const double& sintheta){ return Rotate(Vector3{0,0,1}, costheta, sintheta); }

Transformation Transformation::RotateFrTo(const Vector3& fr, const Vector3& to){
  // requires fr and to to be both unit vectors
  assert(fr.isUnit() && to.isUnit());
  
  Vector3 refl;
  if(std::abs(fr.x())<0.72 && std::abs(to.x())<0.72) refl = Vector3{1,0,0};
  else if(std::abs(fr.y())<0.72 && std::abs(to.y())<0.72) refl = Vector3{0,1,0};
  else refl = Vector3{0,0,1};

  Vector3 u=refl-fr, v=refl-to;
  Matrix4 r = Matrix4::I();
  for(int i=0;i<3;++i)
    for(int j=0;j<3;++j)
      r[i][j] = ((i==j)?1:0) -
	2/u.Dot(u) * u[i] * u[j] -
	2/v.Dot(v) * v[i] * v[j] +
	4*u.Dot(v)/(u.Dot(u)*v.Dot(v)) * v[i] * u[j];
  return Transformation(r, r.T());
}



Transformation Transformation::Scale(const Vector3& n){
  auto ScaleMat = [](const double& kx, const double& ky, const double& kz){
    return Matrix4{kx,0,0,0,
		   0,ky,0,0,
		   0,0,kz,0,
		   0,0,0,1};
  };

  auto mat = ScaleMat(n.x(),n.y(),n.z());
  auto matinv = ScaleMat(1.0/n.x(), 1.0/n.y(), 1.0/n.z());

  return Transformation(mat, matinv);
}
Transformation Transformation::Scale(const Vector3& n, const double& k){
  auto ScaleMat = [](const Vector3& n, double k){
    const double nx=n.x(), ny=n.y(), nz=n.z();
    --k;
    return Matrix4{
      1+k*nx*nx, k*nx*ny, k*nx*nz, 0,
      k*nx*ny, 1+k*ny*ny, k*ny*nz,0,
      k*nx*nz, k*ny*nz, 1+k*nz*nz,0,
      0,0,0,1
    };
  };

  auto mat = ScaleMat(n,k);
  auto matinv = ScaleMat(n,1.0/k);

  return Transformation(mat, matinv);
}
