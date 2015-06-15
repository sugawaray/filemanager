#include <cerrno>
#include <algorithm>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include "filesystem.h"

namespace test {
namespace fixture {

using std::string;

int get_stat_result(const string& path)
{
	errno = 0;
	struct stat stat_obj = { 0 };
	int r(stat(path.c_str(), &stat_obj));
	if (r != 0)
		r = errno;
	return r;
}

using std::vector;

bool equal_files(const string& op1, const string& op2)
{
	vector<char> v1;
	read_file(op1, v1);

	vector<char> v2;
	read_file(op2, v2);

	if (v1.size() != v2.size())
		return false;
	else
		return std::equal(v1.begin(), v1.end(), v2.begin());
}

using std::ifstream;
using std::ios_base;
using std::ofstream;

void write_to_file(const string& path, const vector<char>& content)
{
	ofstream ofs(path.c_str(), ios_base::out | ios_base::binary);
	ofs.write(&content.at(0), content.size());
	ofs.flush();
}

void read_file(const string& path, vector<char>& buffer)
{
	buffer.clear();
	ifstream ifs(path.c_str(), ios_base::in | ios_base::binary);
	typedef ifstream::traits_type Traits;
	for (auto v = ifs.get(); v != Traits::eof(); v = ifs.get())
		buffer.push_back(v);
}

} // fixture
} // test

