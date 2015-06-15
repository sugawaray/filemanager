#ifndef __TEST_FIXTURE_PROCESS_DIR_H__
#define __TEST_FIXTURE_PROCESS_DIR_H__

#include <string>
#include <absolute_path.h>

namespace test {

class Process_dir_fixture {
public:
	Process_dir_fixture();

	std::string get_path() const throw();
private:
	::fs::Absolute_path path;
};

} // test

#endif // __TEST_FIXTURE_PROCESS_DIR_H__
