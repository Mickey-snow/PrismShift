#ifndef UTILITY_VECMATH_H
#define UTILITY_VECMATH_H

#include<cmath>
#include<tuple>
#include<array>
#include<utility>
#include<ostream>
#include<cassert>
#include<cstddef>
#include<type_traits>

template<typename value_t, std::size_t N>
class basic_vector{
public:
  static constexpr double EPS = 1e-5;
  using value_type = value_t;
  static constexpr std::size_t dimension = N;

  basic_vector() : v(std::array<value_type, dimension>()) {}
  basic_vector(std::initializer_list<value_type> li){
    assert(li.size() == N);
    std::copy(li.begin(), li.end(), v.begin());
  }
  template<typename... Ts> requires (((std::is_convertible_v<Ts, value_type>) && ...) && (sizeof...(Ts)==N))
  basic_vector(Ts... param) : basic_vector() {
    assign_impl(std::make_index_sequence<sizeof...(Ts)>{},
		(std::make_tuple(std::forward<Ts>(param)...)));
  }
  basic_vector(const std::input_iterator auto& ibegin,
	       const std::input_iterator auto& iend) : basic_vector() {
    assert(std::distance(ibegin, iend) == N);
    std::copy(ibegin, iend, v.begin());
  }
  basic_vector(const basic_vector<value_type, dimension>& it) : basic_vector() {
    std::copy(it.v.cbegin(), it.v.cend(), v.begin());
  }
  basic_vector(basic_vector<value_type, dimension>&& it){
    v = std::move(it.v);
  }
  
  ~basic_vector() = default;

  basic_vector& operator = (const basic_vector<value_type, dimension>& it){
    basic_vector<value_type, dimension> tmp(it);
    this->swap(tmp);
    return *this;
  }
  basic_vector& operator = (basic_vector<value_type, dimension>&& it){
    basic_vector<value_type, dimension> tmp(std::move(it));
    this->swap(tmp);
    return *this;
  }

  template<typename T>
  void swap(T& it){ std::swap(v, it.v); }

  // data access
  value_type x() const{ return v[0]; }
  value_type& x(){ return v[0]; }
  value_type y() const requires (dimension>=2) { return v[1]; }
  value_type& y() requires (dimension>=2) { return v[1]; }
  value_type z() const requires (dimension>=3) { return v[2]; }
  value_type& z() requires (dimension>=3) { return v[2]; }
  value_type operator [](const int& idx)const { return v[idx]; }
  value_type& operator [](const int& idx){ return v[idx]; }

  bool Near_Zero(void) const{
    for(std::size_t i=0; i<dimension; ++i)
      if(fabs(v[i]) > EPS) return false;
    return true;
  }
  bool operator == (const basic_vector<value_type, dimension>& rhs) const {
    for(std::size_t i=0;i<dimension; ++i)
      if(fabs(v[i]-rhs.v[i]) > EPS) return false;
    return true;
  }
  bool operator != (const basic_vector<value_type, dimension>& rhs) const {
    return !(*this == rhs);
  }
  
  value_type Length(void) const{ return sqrt(Length_squared()); }
  value_type Length_squared(void) const{
    value_type ans{};
    for(std::size_t i=0;i<dimension;++i) ans += v[i] * v[i];
    return ans;
  }
  basic_vector<value_type,dimension> Normalized(void) const{
    auto ret = *this;
    auto len = Length();
    for(auto& it : ret.v) it /= len;
    return ret;
  }
  
  auto Dot(const basic_vector<value_type, dimension>& rhs) const -> decltype(auto){
    using return_type = decltype(std::declval<value_type>() * std::declval<value_type>());
    return_type ret{};
    for(std::size_t i=0;i<dimension;++i) ret += v[i] * rhs.v[i];
    return ret;
  }
    
  std::array<value_type, dimension> v;
  
private:
  template<std::size_t... I, typename... Ts>
  void assign_impl(std::index_sequence<I...> unused,
		   std::tuple<Ts...> vals){
    ((v[I] = std::get<I>(vals)), ...);
  }
};

template<typename, typename = std::void_t<>>
struct has_vector_traits : std::false_type {};
template<typename T, size_t N>
struct has_vector_traits<basic_vector<T,N>> : std::true_type {};
template<typename T>
struct has_vector_traits<T, std::void_t<decltype(dynamic_cast<const basic_vector<typename T::value_type, T::dimension>*>(std::declval<T*>()))>> : std::true_type {};

template<typename T>
struct is_vector_like : has_vector_traits<std::decay_t<T>> {};

template<typename T>
concept vector_like = is_vector_like<T>::value;



template<vector_like T>
auto operator - (const T& it) -> T {
  T ret;
  for(std::size_t i=0;i<T::dimension;++i) ret.v[i] = -it.v[i];
  return ret;
}

template<vector_like T, vector_like U, class F>
auto vector_arithmetic(const T& lhs, const U& rhs, const F& op) -> decltype(auto)
  requires (T::dimension == U::dimension){
  using return_type = basic_vector<decltype(op(std::declval<typename T::value_type>(),
					       std::declval<typename U::value_type>())),
				   T::dimension>;
  return_type ret;
  for(std::size_t i=0;i<return_type::dimension;++i) ret.v[i] = op(lhs.v[i], rhs.v[i]);
  return ret;
}

template<vector_like T>
auto operator -(const T& lhs, const T& rhs) -> T {
  return (T)vector_arithmetic(lhs, rhs, std::minus<>{});
}
template<vector_like T, vector_like U>
auto& operator -=(T& lhs, const U& rhs){ return lhs = lhs - rhs; }

template<vector_like T>
auto operator +(const T& lhs, const T& rhs) -> T{
  return (T)vector_arithmetic(lhs, rhs, std::plus<>{});
}
template<vector_like T, vector_like U>
auto& operator +=(T& lhs, const U& rhs){ return lhs = lhs + rhs; }

template<vector_like T>
auto operator *(const T& lhs, const T& rhs) -> T {
  return (T)vector_arithmetic(lhs, rhs, std::multiplies<>{});
}
template<vector_like T, typename U>
auto operator *(const T& lhs, const U& rhs) -> T
  requires std::is_arithmetic_v<U>{
  T ret;
  for(std::size_t i=0;i<T::dimension;++i) ret.v[i] = lhs.v[i] * rhs;
  return ret;
}
template<typename T, vector_like U>
auto operator *(const T& lhs, const U& rhs) -> U
  requires std::is_arithmetic_v<T> { return rhs * lhs; }
template<typename T, typename U>
auto& operator *=(T& lhs, const U& rhs)
  requires vector_like<T> || vector_like<U> { return lhs = lhs*rhs; }

template<vector_like T>
auto operator /(const T& lhs, const T& rhs) -> T{
  return (T)vector_arithmetic(lhs,rhs,std::divides<>{});
}
template<vector_like T, typename U>
auto operator /(const T& lhs, const U& rhs) -> T
  requires std::is_arithmetic_v<U> {
  T ret;
  for(std::size_t i=0;i<T::dimension;++i) ret.v[i] = lhs.v[i] / rhs;
  return ret;
}
template<vector_like T, typename U>
auto& operator /=(T& lhs, const U& rhs){ return lhs = lhs / rhs; }


std::ostream& operator << (std::ostream& os, const vector_like auto& it){
  os << '{' << it.v[0];
  for(std::size_t i=1;i<it.dimension;++i) os << ',' << it.v[i];
  os << '}';
  return os;
}



#endif
