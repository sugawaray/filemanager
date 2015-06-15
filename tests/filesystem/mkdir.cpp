#include <stdexcept>
#include <absolute_path.h>
#include <filesystem.h>
#include "mkdir.h"

using namespace std;

START_TEST(should_throw_an_exception_given_the_root)
{
	try {
		fs::mkdir("/");
		throw logic_error("should not reach here.");
	}
	catch (const invalid_argument&) {
	}
}
END_TEST

namespace test {
namespace fs {

TCase* create_tcase_for_mkdir()
{
	TCase* tcase = tcase_create("mkdir");
	tcase_add_test(tcase, should_throw_an_exception_given_the_root);
	return tcase;
}

} // fs
} // test
