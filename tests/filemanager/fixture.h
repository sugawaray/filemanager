#ifndef __FM_FILEMANAGER_FIXTURE_H__
#define __FM_FILEMANAGER_FIXTURE_H__

#include <string>
#include <vector>
#include <tests/fixture/fm_filesystem.h>

namespace fm {
	class Filemanager;
	class Fm_map_impl;

namespace filemanager {
namespace test {

class Fixture : public ::test::Fm_filesystem_fixture {
public:
	Fixture()
		:	manager(get_fm_path()) {
		categories.push_back("catA");
	}

	Filemanager& get_manager() {
		return manager;
	}

	Fm_map_impl& get_map() {
		return manager.get_map();
	}

	std::vector<std::string>& get_categories() {
		return categories;
	}
private:
	Filemanager manager;
	std::vector<std::string> categories;
};

} // test
} // filemanager
} // fm

#endif // __FM_FILEMANAGER_FIXTURE_H__
