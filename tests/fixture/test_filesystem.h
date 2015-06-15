#ifndef __TEST_FIXTURE_TEST_FILESYSTEM_H__
#define __TEST_FIXTURE_TEST_FILESYSTEM_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace test {
namespace fixture {

extern Suite* create_filesystem_fixture_suite();
extern void run_filesystem_tests();

} // fixture
} // test

#endif // __TEST_FIXTURE_TEST_FILESYSTEM_H__
