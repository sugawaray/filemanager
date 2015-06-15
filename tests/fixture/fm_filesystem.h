#ifndef __TEST_FIXTURE_FM_FILESYSTEM_H__
#define __TEST_FIXTURE_FM_FILESYSTEM_H__

#include "process_dir.h"

namespace test {

class Fm_filesystem_fixture {
public:
	Fm_filesystem_fixture();

	/* creates the dir_1 and dir_2 directories as children of the path
	   specified by the relative path from the process directory */
	Fm_filesystem_fixture(const std::string& relative_path);

	~Fm_filesystem_fixture();

	/* returns the path of the parent directory of .fm directory as
	   absolute path. */
	std::string get_fm_parent() const {
		return fm_parent;
	}
	std::string get_fm_path() const {
		return get_fm_parent() + "/.fm";
	}
	std::string get_branch_path_without_fm() const {
		return branch_without_fm;
	}
private:
	void init(const std::string& relative_path);

	Process_dir_fixture dir_fixture;
	std::string fm_parent;
	std::string branch_without_fm;
};

} // test

#endif // __TEST_FIXTURE_FM_FILESYSTEM_H__
