#ifndef __FS_TEST_IS_DIR_H__
#define __FS_TEST_IS_DIR_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fs {
namespace test {

extern TCase* create_tcase_for_is_dir();

} // test
} // fs

#endif // __FS_TEST_IS_DIR_H__
