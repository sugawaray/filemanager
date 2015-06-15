#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <dirent.h>
#include <sys/stat.h>
#include "absolute_path.h"
#include "fm.h"
#include "fm_filesystem.h"

using namespace std;

namespace {

int is_dir(const string& path)
{
	struct stat statbuf = { 0 };
	errno = 0;
	int r = stat(path.c_str(), &statbuf);
	if (r != 0) {
		cerr << "could not determine the file type. " <<
			strerror(errno) << endl;
		return -1;
	}
	return S_ISDIR(statbuf.st_mode) != 0 ? 1 : 0;
}

bool find_fm_in_dir(const string& path)
{
	DIR* dir;
	dir = opendir(path.c_str());
	if (dir == 0)
		return false;
	struct dirent* e;
	for (e = readdir(dir); e != 0; e = readdir(dir)) {
		if (".fm" != string(e->d_name))
			continue;
		if (is_dir(path + "/" + e->d_name) != 1)
			continue;
		break;
	}
	errno = 0;
	if (closedir(dir) != 0 && errno == EBADF)
		throw logic_error("find_fm_in_dir()");
	return e != 0;
}

string get_root_path(const string& fm_dirpath)
{
	string parent(fm_dirpath);
	string::size_type p(parent.rfind("/.fm"));
	if (p == string::npos ||
		p + char_traits<char>::length("/.fm") != parent.length())
		throw invalid_argument("get_fm_path");
	parent.erase(p);
	return parent;
}

} // unnamed

namespace fm {
namespace fs {

/*
 Fix me.
 can't test the case in which the root directory contains .fm dir. */
int find_fm_dir(const Absolute_path& original_path, int limit,
	Absolute_path& result)
{
	string path(original_path.to_filepath_string());
	for (; limit >= 0 && !path.empty() && !find_fm_in_dir(path); --limit) {
		path = path.substr(0, path.find_last_of('/'));
	}
	if (limit < 0)
		return Not_found;
	if (path.empty()) {
		if (!find_fm_in_dir("/"))
			return Not_found;
	}
	result = path + "/.fm";
	return Found;
}

string get_fm_path(const Absolute_path& fm_dirpath, const Absolute_path& path)
	throw(invalid_argument)
{
	string parent(get_root_path(fm_dirpath.to_filepath_string()));
	string copied(path.to_filepath_string());
	if (copied.find(parent) != 0)
		throw invalid_argument("get_fm_path");
	copied.erase(0, parent.length() + 1);
	return copied;
}

Absolute_path convert_fm_path(const Absolute_path& fm_dirpath,
	const string& fm_path) throw(invalid_argument)
{
	string parent(get_root_path(fm_dirpath.to_filepath_string()));
	return Absolute_path(parent + "/" + fm_path);
}

string Filesystem::get_dbfilepath() const
{
	return fm_dir.to_filepath_string() + "/map";
}

bool Filesystem::is_descendant(const Absolute_path& path) const
{
	string s(fm_dir.parent().to_string());
	if (path.to_filepath_string().find(s) == 0)
		return true;
	else
		return false;
}

} // fs
} // fm
