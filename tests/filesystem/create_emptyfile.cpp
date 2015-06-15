#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "create_emptyfile.h"
#include <absolute_path.h>
#include <filesystem.h>

using namespace std;
using fs::Absolute_path;
using fs::create_emptyfile;
using fs::get_abs_path;

namespace {

class Fixture {
public:
	Fixture() {
		ostringstream os;
		os << "./test_dir/emptyfile_" << getpid();
		path = get_abs_path(os.str());
		remove(path.to_filepath_string().c_str());
	}
	string get_path() const {
		return path.to_filepath_string();
	}
private:
	Absolute_path path;
};

} // unnamed

/* I use this test as a way to check the function manually, too. */
START_TEST(should_create_emptyfile)
{
	Fixture f;
	int r1, r2;
	r1 = create_emptyfile(f.get_path());
	{
		ofstream os(f.get_path());
	}
	r2 = create_emptyfile(f.get_path());
	fail_unless(r1 == 0, "should return 0. 1");
	fail_unless(r2 == 0, "should return 0. 2");
}
END_TEST

START_TEST(should_return_an_error_when_it_failed)
{
	Fixture f;
	int fd;
	int r;
	try {
		errno = 0;
		fd = open(f.get_path().c_str(),
			O_WRONLY | O_CREAT | O_SYNC, 0);
		if (fd < 0) {
			throw logic_error(strerror(errno));
		}
		struct flock fl = { 0 };
		fl.l_type = F_WRLCK;
		fl.l_whence = SEEK_SET;
		fl.l_pid = getppid();
		errno = 0;
		if (fcntl(fd, F_SETLK, &fl) == -1) {
			throw logic_error(strerror(errno));
		}
		close(fd);
		fd = -1;
		r = create_emptyfile(f.get_path());
	}
	catch (...) {
		if (fd >= 0)
			close(fd);
		throw;
	}
	fail_unless(r == -1, "should return an error.");
}
END_TEST

namespace test {
namespace fs {

TCase* create_tcase_for_create_emptyfile()
{
	TCase* tcase(tcase_create("create_emptyfile"));
	tcase_add_test(tcase, should_create_emptyfile);
	tcase_add_test(tcase, should_return_an_error_when_it_failed);
	return tcase;
}

} // fs
} // test
