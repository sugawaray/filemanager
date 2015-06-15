#ifndef __FS_TEST_GET_UMASK_H__
#define __FS_TEST_GET_UMASK_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fs {
namespace test {

extern TCase* create_get_umask_tcase();

} // test
} // fs

#endif // __FS_TEST_GET_UMASK_H__
