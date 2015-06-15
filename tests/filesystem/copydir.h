#ifndef __FS_TEST_COPYDIR_H__
#define __FS_TEST_COPYDIR_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fs {
namespace test {

extern TCase* create_copydir_tcase();

} // test
} // fs

#endif // __FS_TEST_COPYDIR_H__
