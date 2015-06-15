#ifndef __FM_FILEMANAGER_TEST_MOVE_COPY_FIXTURE_H__
#define __FM_FILEMANAGER_TEST_MOVE_COPY_FIXTURE_H__

#include <string>
#include <vector>
#include <absolute_path.h>
#include <filemanager.h>
#include <tests/fixture/fm_filesystem.h>

namespace fm {
namespace filemanager {
namespace test {

class Move_copy_fixture {
public:
	Move_copy_fixture();

	Filemanager& get_manager() {
		return manager;
	}

	Absolute_path get_root() const {
		return Absolute_path(fixture.get_fm_parent());
	}

	void add_categories(const std::string& fm_path);
	std::vector<std::string> get_categories_values();

protected:
	::test::Fm_filesystem_fixture fixture;
	Filemanager manager;
	std::vector<std::string> cat;
};

} // test
} // filemanager
} // fm

#endif // __FM_FILEMANAGER_TEST_MOVE_COPY_FIXTURE_H__
