#ifndef __TEST_FIXTURE_UTILS_H__
#define __TEST_FIXTURE_UTILS_H__

namespace test {
namespace fixture {

template<class F, class I>
inline const I& return_param(F f, const I& p)
{
	f(p);
	return p;
}

} // fixture
} // test

#endif // __TEST_FIXTURE_UTILS_H__
