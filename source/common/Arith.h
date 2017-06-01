#pragma once

#include <type_traits>

struct Arith {};

template < typename T1, typename T2 >
auto operator*(T1 x1, T2 const & x2) -> std::enable_if_t<std::is_base_of<Arith, T1>::value, T1>
{
	x1 *= x2;
	return x1;
}

template < typename T1, typename T2 >
auto operator+(T1 x1, T2 const & x2) -> std::enable_if_t<std::is_base_of<Arith, T1>::value, T1>
{
	x1 += x2;
	return x1;
}

template < typename T1, typename T2 >
auto operator-(T1 x1, T2 const & x2) -> std::enable_if_t<std::is_base_of<Arith, T1>::value, T1>
{
	x1 -= x2;
	return x1;
}
