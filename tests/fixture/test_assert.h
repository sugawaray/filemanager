#ifndef __TEST_FIXTURE_ASSERT_TEST_H__
#define __TEST_FIXTURE_ASSERT_TEST_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace test {
namespace fixture {

extern Suite* create_assert_fixture_suite();

} // fixture
} // test

#endif // __TEST_FIXTURE_ASSERT_TEST_H__
