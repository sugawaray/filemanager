#ifndef __TEST_FILESYSTEM_MKDIR_H__
#define __TEST_FILESYSTEM_MKDIR_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace test {
namespace fs {

extern TCase* create_tcase_for_mkdir();

} // fs
} // test

#endif // __TEST_FILESYSTEM_MKDIR_H__
