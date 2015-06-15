#ifndef __HELPER_H__
#define __HELPER_H__

namespace fs {

template<class T, std::size_t N>
char (& array_size(const T (&)[N]))[N];

} // fs

#endif // __HELPER_H__
