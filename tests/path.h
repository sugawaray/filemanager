#ifndef __FS_TEST_PATH_H__
#define __FS_TEST_PATH_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fs {
namespace test {

extern Suite* create_path_test_suite();

} // test
} // fs

#endif // __FS_TEST_PATH_H__
