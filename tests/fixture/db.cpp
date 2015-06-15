#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <absolute_path.h>
#include <filesystem.h>
#include "db.h"

namespace test {

using namespace std;
using fs::get_abs_path;
using fs::mkdir;

Db_fixture::Db_fixture()
{
	mkdir(get_abs_path("./test_dir"));
	dbfilepath = generate_dbfilepath();
	remove(dbfilepath.c_str());
}

string Db_fixture::get_dbfilepath()
{
	return dbfilepath;
}

string Db_fixture::generate_dbfilepath()
{
	ostringstream os;
	os << "./test_dir/test_db" << getpid();
	return os.str();
}

} // test
