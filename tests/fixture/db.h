#ifndef __TEST_FIXTURE_DB_H__
#define __TEST_FIXTURE_DB_H__

#include <string>

namespace test {

/* This class creates a directory with name 'test_dir' under the current
   working directory at the instantiation if it's not existed. */
class Db_fixture {
public:
	Db_fixture();

	/* Returns an absolute file path for a file under 'test_dir' with 
	   a name 'testdb###' where the '###' is the process id. */
	std::string get_dbfilepath();
private:
	static std::string generate_dbfilepath();

	std::string dbfilepath;
};

} // test

#endif // __TEST_FIXTURE_DB_H__
