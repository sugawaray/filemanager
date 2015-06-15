#ifndef __FS_TEST_COPY_H__
#define __FS_TEST_COPY_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fs {
namespace test {

extern TCase* create_copy_tcase();
extern TCase* create_copy_error_tcase();
extern void run_copy_tests();

} // test
} // fs

#endif // __FS_TEST_COPY_H__
