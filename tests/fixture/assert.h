#ifndef __TEST_FIXTURE_ASSERT_H__
#define __TEST_FIXTURE_ASSERT_H__

#include <iostream>

namespace test {
namespace fixture {

using std::cerr;
using std::endl;
using std::ostream;

template<class Expr, class T, class U>
inline void Assert(Expr expr, T value1, U value2, ostream& os = cerr)
{
	if (!(expr)) {
		os << value1 << ":" << value2 << endl;
	}
}

} // fixture
} // test

#endif // __TEST_FIXTURE_ASSERT_H__
