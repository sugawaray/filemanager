#ifndef __FM_TEST_FILEMANAGER_H__
#define __FM_TEST_FILEMANAGER_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fm {
namespace test {

extern Suite* create_filemanager_test_suite();
extern void run_filemanager_tests();

} // test
} // fm

#endif // __FM_TEST_FILEMANAGER_H__
