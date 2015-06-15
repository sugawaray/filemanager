#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "absolute_path.h"
#include "except.h"
#include "filesystem.h"
#include "path.h"
#include "string_utils.h"

using namespace std;

namespace {

int mkdir_impl(const string& path)
{
	errno = 0;
	mode_t mode(S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
	return mkdir(path.c_str(), mode);
}

} // unnamed

namespace fs {

int create_emptyfile(const Absolute_path& path)
{
	errno = 0;
	int r = open(path.to_filepath_string().c_str(),
		O_WRONLY | O_CREAT | O_SYNC, S_IRUSR | S_IWUSR);
	if (r >= 0) {
		close(r);
		return 0;
	} else {
		return errno == EEXIST ? 0 : -1;
	}
}

Absolute_path get_abs_path(const string& path,
	char* (*getcwdfn)(char*, size_t))
{
	if (path.length() == 0)
		throw invalid_argument("empty path not allowed");
	Path p(path);
	if (p.is_absolute())
		return Absolute_path(path);
	pair<bool, Absolute_path> cwdinfo(getcwd(getcwdfn));
	return p.to_absolute_path(cwdinfo.second);
}

pair<bool, Absolute_path> getcwd(char* (*fn)(char*, size_t))
{
	vector<char> buffer(256);
	for (errno = 0; fn(&buffer.at(0), buffer.size()) == 0 &&
		errno == ERANGE; errno = 0)
		buffer.resize(buffer.size() * 2);
	if (errno == 0)
		return make_pair(true, Absolute_path(&buffer.at(0)));
	else
		return make_pair(false, Absolute_path());
}

int mkdir(const Absolute_path& path)
{
	vector<string> comps;
	path.get_components(back_inserter(comps));
	if (comps.size() == 1)
		throw invalid_argument("path");
	string s("");
	int r = 0;
	for (auto i = comps.begin() + 1; i != comps.end(); ++i) {
		s += "/" + *i;
		r = mkdir_impl(s);
	}
	return r;
}

namespace {

template<class Inserter>
inline void collect_children(DIR* d, Inserter ins)
{
	while (struct dirent* e = readdir(d)) {
		if (strcmp(e->d_name, ".") == 0)
			continue;
		if (strcmp(e->d_name, "..") == 0)
			continue;
		*ins++ = e->d_name;
	}
}

class Rmdirtree {
public:
	Rmdirtree(const Absolute_path& path) : path(path) { }
	void operator()(const string& name) const {
		rmdirtree(path.child(name));
	}
private:
	const Absolute_path& path;
};

inline int remove_children(const Absolute_path& path)
{
	DIR* d(opendir(path.to_filepath_string().c_str()));
	if (d == 0)
		return Failure;
	vector<string> e;
	collect_children(d, back_inserter(e));
	closedir(d);
	for_each(e.begin(), e.end(), Rmdirtree(path));
	return Success;
}

} // unnamed

/* Fix me.
   * It is not tested that it returns Failure when opendir failed.
     I am not sure whether it is better to make it possible to replace
     apis with their stubs.
*/
int rmdirtree(const Absolute_path& path)
{
	pair<int, bool> itsdir(is_dir(path));
	if (itsdir.first == Not_exist)
		return Not_exist;
	else if (itsdir.first != Success)
		return Failure;
	else if (itsdir.first == Success && itsdir.second == false) {
		std::remove(path.to_filepath_string().c_str());
		return Success;
	}
	int r(remove_children(path));
	if (r != Success)
		return r;
	else if (rmdir(path.to_filepath_string().c_str()) == 0)
		return Success;
	else
		return Failure;
}

pair<int, bool> is_dir(const Absolute_path& path)
{
	struct stat statbuf = { 0 };
	errno = 0;
	int r = stat(path.to_filepath_string().c_str(), &statbuf);
	if (r != 0) {
		if (errno == ENOENT)
			return make_pair(Not_exist, false);
		else
			return make_pair(Failure, false);
	} else if (S_ISDIR(statbuf.st_mode) != 0)
		return make_pair(Success, true);
	else
		return make_pair(Success, false);
}

/* Fix me.
   This function is not tested. */
int rename(const Absolute_path& src_path, const Absolute_path& dest_path)
{
	string src(src_path.to_filepath_string());
	string dest(dest_path.to_filepath_string());
	errno = 0;
	int r(std::rename(src.c_str(), dest.c_str()));
	if (r == -1) {
		if (mkdir(dest_path.parent()) == 0) {
			errno = 0;
			r = std::rename(src.c_str(), dest.c_str());
		}
	}
	return r;
}

int copy(const Absolute_path& src, const Absolute_path& dest)
{
	Io io;
	string srep(src.to_filepath_string());
	string drep(dest.to_filepath_string());
	int r(copy(io, srep.c_str(), drep.c_str()));
	if (r != 0) {
		if (mkdir(dest.parent()) == 0)
			r = copy(io, srep.c_str(), drep.c_str());
	}
	return r;
}

pair<int, mode_t> get_mode(int fd)
{
	struct stat stat_obj = { 0 };
	if (fstat(fd, &stat_obj) == 0)
		return make_pair(Success, stat_obj.st_mode);
	else
		return make_pair(Failure, 0);
}

pair<int, mode_t> get_mode(const char* path)
{
	struct stat stat_obj = { 0 };
	if (stat(path, &stat_obj) == 0)
		return make_pair(Success, stat_obj.st_mode);
	else
		return make_pair(Failure, 0);
}

mode_t get_umask()
{
	mode_t old(umask(0));
	umask(old);
	return old;
}

} // fs
