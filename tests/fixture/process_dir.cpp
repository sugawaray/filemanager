#include <sstream>
#include <unistd.h>
#include <absolute_path.h>
#include <filesystem.h>
#include "process_dir.h"

using namespace std;
using fs::get_abs_path;
using fs::mkdir;

namespace test {

Process_dir_fixture::Process_dir_fixture()
{
	ostringstream os;
	os << "./test_dir/" << getpid();
	path = get_abs_path(os.str());
	mkdir(path);
}

std::string Process_dir_fixture::get_path() const throw()
{
	return path.to_filepath_string();
}

} // test
