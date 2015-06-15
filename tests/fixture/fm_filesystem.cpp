#include <iostream>
#include <sstream>
#include <unistd.h>
#include <absolute_path.h>
#include <filesystem.h>
#include "fm_filesystem.h"

namespace test {

using std::ostringstream;
using std::string;
using fs::mkdir;

Fm_filesystem_fixture::Fm_filesystem_fixture()
{
	init("");
}

Fm_filesystem_fixture::Fm_filesystem_fixture(const string& relative_path)
{
	init("/" + relative_path);
}

Fm_filesystem_fixture::~Fm_filesystem_fixture()
{
}

void Fm_filesystem_fixture::init(const string& relative_path)
{
	ostringstream os;
	os << dir_fixture.get_path() << relative_path << "/dir_1";
	fm_parent = os.str();
	os << "/.fm";
	mkdir(os.str());

	os.clear();
	os.str("");
	os << dir_fixture.get_path() << relative_path << "/dir_2";
	branch_without_fm = os.str();
	mkdir(branch_without_fm);
}

} // test
