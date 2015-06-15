#ifndef __FS_TEST_GET_MODE_H__
#define __FS_TEST_GET_MODE_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fs {
namespace test {

extern TCase* create_get_mode_tcase();
extern TCase* create_get_mode_path_tcase();

} // test
} // fs

#endif // __FS_TEST_GET_MODE_H__
