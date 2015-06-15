#include <sys/types.h>
#include <sys/stat.h>
#include <filesystem.h>
#include "get_umask.h"

using fs::get_umask;

START_TEST(should_get_umask)
{
	mode_t m(get_umask());
	mode_t expected(umask(0));
	fail_unless(m == expected, "value 1");
	umask(expected);

	get_umask();
	m = get_umask();
	fail_unless(m == expected, "value 2");
}
END_TEST

namespace fs {
namespace test {

TCase* create_get_umask_tcase()
{
	TCase* tcase(tcase_create("fs::get_umask"));
	tcase_add_test(tcase, should_get_umask);
	return tcase;
}

} // test
} // fs
