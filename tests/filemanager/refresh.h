#ifndef __FM_TEST_FILEMANAGER_REFRESH_H__
#define __FM_TEST_FILEMANAGER_REFRESH_H__

extern "C" {

#include <check.h>

} // extern "C"

namespace fm {
namespace test {

extern TCase* create_refresh_core_tcase();
extern TCase* create_refresh_erase_tcase();

} // test
} // fm

#endif // __FM_TEST_FILEMANAGER_REFRESH_H__
