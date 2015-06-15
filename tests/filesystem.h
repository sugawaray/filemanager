#ifndef __TEST_FILESYSTEM_H__
#define __TEST_FILESYSTEM_H__

extern "C" {

#include <check.h>

} // extern "C"

extern Suite* create_filesystem_test_suite();

namespace fs {
namespace test {

extern void run_filesystem_tests();

} // test
} // fs

#endif // __TEST_FILESYSTEM_H__
