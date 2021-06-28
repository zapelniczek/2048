#pragma once
#include <iterator>
#include <type_traits>
template <typename Iter>
struct is_reverse_iterator : std::false_type
{};

template <typename Iter>
struct is_reverse_iterator <std::reverse_iterator<Iter>>
	: std::integral_constant<bool, !is_reverse_iterator<Iter>::value>
{

};

template <typename Iter>
inline constexpr bool is_reverse_iterator_v = is_reverse_iterator<Iter>::value;