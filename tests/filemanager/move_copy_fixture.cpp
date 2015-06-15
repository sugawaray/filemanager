#include <iterator>
#include "move_copy_fixture.h"

namespace fm {
namespace filemanager {
namespace test {

using std::string;
using std::vector;

Move_copy_fixture::Move_copy_fixture()
:	manager(fixture.get_fm_path())
{
	cat.push_back("catA");
}

void Move_copy_fixture::add_categories(const string& fm_path)
{
	manager.get_map().set(fm_path, cat.begin(), cat.end());
}

vector<string> Move_copy_fixture::get_categories_values()
{
	vector<string> r;
	manager.get_map().get(cat, std::back_inserter(r));
	return r;
}

} // test
} // filemanager
} // fm
