#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <iterator>
#include <string>

namespace su {

template<template<class T, class A> class C, class A>
void split_string(const std::string& in, const std::string& sep,
	std::back_insert_iterator<C<std::string, A> > inserter)
{
	std::string::size_type p, q;
	for (p = 0, q = in.find(sep, p); q != std::string::npos;
		p = q + sep.length(), q = in.find(sep, p)) {
		*inserter = in.substr(p, q - p);
		++inserter;
	}
	*inserter = in.substr(p, q);
	++inserter;
}

} // su

#endif // __STRING_UTILS_H__
