#ifndef __FM_FILEMANAGER_TEST_FIXTURE_H__
#define __FM_FILEMANAGER_TEST_FIXTURE_H__

#include <absolute_path.h>
#include <filemanager.h>
#include <tests/fixture/fm_filesystem.h>

namespace fm {
namespace filemanager {
namespace test {

class Fm_fixture : public ::test::Fm_filesystem_fixture {
public:
	Fm_fixture();

	Filemanager& get_manager() {
		return m;
	}

	Absolute_path get_root() const {
		return Absolute_path(get_fm_parent());
	}

	Absolute_path get_another_branch() const {
		return Absolute_path(get_branch_path_without_fm());
	}
private:
	Filemanager m;
};

} // test
} // filemanager
} // fm

#endif // __FM_FILEMANAGER_TEST_FIXTURE_H__
