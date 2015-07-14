#include <cassert>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <nomagic.h>
#include <absolute_path.h>
#include <filesystem.h>
#include <filemanager.h>
#include <fm.h>
#include <fm_commands.h>
#include <string_utils.h>
#include <utils.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filemanager.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/fm_filesystem.h>
#include <tests/fixture/map.h>
#include <tests/fixture/process_dir.h>
#include <tests/fixture/utils.h>
#include "fm_commands.h"

namespace fm {
namespace test {

using std::cerr;

namespace {

using std::string;
using std::vector;
namespace gfs = ::fs;
using gfs::create_emptyfile;
using gfs::mkdir;
using ::test::fixture::get_stat_result;
using ::test::Process_dir_fixture;
typedef Process_dir_fixture F;

char* the_argv[100];

vector<char> strtovec(const string& s)
{
	vector<char> v(s.begin(), s.end());
	v.push_back('\0');
	return v;
}

string createfile(const string& path)
{
	create_emptyfile(path);
	return path;
}

string createdir(const string& path)
{
	mkdir(path);
	return path;
}

using std::ostringstream;
using utils::Auto_caller;

inline void reset_cerr()
{
	fm::fmcerr = &cerr;
}

template<class T, class U, class V>
inline void A(T t, U u, V v)
{
	::test::fixture::Assert(t, u, v);
}

typedef ::test::Fm_filesystem_fixture FmfsF;

class Nested_fmfs_fixture : private FmfsF {
public:
	Nested_fmfs_fixture()
		:	FmfsF("dir1/dir2/dir3"),
			cwd(get_fm_parent()) {
		chdir(cwd.to_filepath_string().c_str());
		create_emptyfile(cwd.child("file1"));
		mkdir(get_parent_fmdir());
	}

	const char* get_child_file1_path() const {
		return cwd.child("file1").to_filepath_string().c_str();
	}

	Absolute_path get_parent_fmdir() const {
		return cwd.parent().child(".fm");
	}

	Absolute_path get_fmdir() const {
		return Absolute_path(get_fm_path());
	}

	Absolute_path get_cwd() const {
		return cwd;
	}
private:
	Absolute_path cwd;
};

namespace ncp {

void t1()
{
	F f;
	string root(f.get_path());

	vector<char> v0(strtovec("fm-cp"));
	vector<char> v1(strtovec(createfile(root + "/file1")));
	vector<char> v2(strtovec(createfile(root + "/file2")));
	vector<char> v3(strtovec(createfile(root + "/file3")));
	vector<char> v4(strtovec(createdir(root + "/dir1")));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;
	cp(5, the_argv);

	assert(get_stat_result(root + "/dir1/file1") == 0);
	assert(get_stat_result(root + "/dir1/file2") == 0);
	assert(get_stat_result(root + "/dir1/file3") == 0);
}

void t2()
{
	F f;
	string root(f.get_path());

	vector<char> v0(strtovec("fm-cp"));
	vector<char> v1(strtovec("-R"));
	vector<char> v2(strtovec(createdir(root + "/dir1")));
	vector<char> v3(strtovec(createdir(root + "/dir2")));
	vector<char> v4(strtovec(createdir(root + "/dir3")));
	vector<char> v5(strtovec(createdir(root + "/target")));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = &v5.at(0);
	the_argv[6] = 0;
	cp(6, the_argv);

	assert(get_stat_result(root + "/target/dir1") == 0);
	assert(get_stat_result(root + "/target/dir2") == 0);
	assert(get_stat_result(root + "/target/dir3") == 0);
}

void t3()
{
	auto a(Auto_caller(reset_cerr));
	F f;
	string root(f.get_path());
	string dest(root + "/file2");

	vector<char> v0(strtovec("fm-cp"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	vector<char> v3(strtovec(createfile(root + "/file1")));
	vector<char> v4(strtovec(dest));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	ostringstream os;
	fmcerr = &os;

	int r(cp(5, the_argv));
	A(r == 1, __FILE__, __LINE__);
	A(get_stat_result(dest) == ENOENT, __FILE__, __LINE__);
}

void t4()
{
	auto a(Auto_caller(reset_cerr));
	Nested_fmfs_fixture f;
	string src(f.get_cwd().child("file1").to_filepath_string());

	vector<char> v0(strtovec("fm-cp"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	vector<char> v3(strtovec("-R"));
	vector<char> v4(strtovec(createfile(src)));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	ostringstream os;
	fmcerr = &os;

	int r(cp(5, the_argv));
	A(r == 1, __FILE__, __LINE__);
	A(os.str() != "", __FILE__, __LINE__);
}

} // ncp

namespace nrm {

void t1()
{
	auto a(Auto_caller(reset_cerr));
	F f;
	ostringstream os;
	fm::fmcerr = &os;
	string root(f.get_path());
	string d(createdir(root + "/dir1"));
	createfile(d + "/file1");

	vector<char> v0(strtovec("fm-rm"));
	vector<char> v1(strtovec(d));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = 0;

	rm(2, the_argv);

	assert(os.str() != "");
	assert(get_stat_result(d) == 0);
}

void t2()
{
	auto a(Auto_caller(reset_cerr));
	F f;
	ostringstream os;
	fm::fmcerr = &os;
	string root(f.get_path());
	string d(createdir(root + "/dir1"));
	createfile(d + "/file1");

	vector<char> v0(strtovec("fm-rm"));
	vector<char> v1(strtovec("-R"));
	vector<char> v2(strtovec(d));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = 0;

	rm(3, the_argv);

	assert(os.str() == "");
	assert(get_stat_result(d) == ENOENT);
}

void t3()
{
	F f;
	string root(f.get_path());
	string f1(createfile(root + "/file1"));
	string f2(createfile(root + "/file2"));
	string f3(createfile(root + "/file3"));

	vector<char> v0(strtovec("fm-rm"));
	vector<char> v1(strtovec(f1));
	vector<char> v2(strtovec(f2));
	vector<char> v3(strtovec(f3));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	rm(4, the_argv);

	assert(get_stat_result(f1) == ENOENT);
	assert(get_stat_result(f2) == ENOENT);
	assert(get_stat_result(f3) == ENOENT);
}

void t4()
{
	F f;
	string root(f.get_path());
	string d1(createdir(root + "/dir1"));
	string d2(createdir(root + "/dir2"));
	string d3(createdir(root + "/dir3"));

	vector<char> v0(strtovec("fm-rm"));
	vector<char> v1(strtovec("-R"));
	vector<char> v2(strtovec(d1));
	vector<char> v3(strtovec(d2));
	vector<char> v4(strtovec(d3));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	fm::rm(5, the_argv);

	assert(get_stat_result(d1) == ENOENT);
	assert(get_stat_result(d2) == ENOENT);
	assert(get_stat_result(d3) == ENOENT);
}

void t5()
{
	auto a(Auto_caller(reset_cerr));
	F f;
	string root(f.get_path());

	vector<char> v0(strtovec("fm-rm"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = 0;

	ostringstream os;
	fmcerr = &os;

	int r(fm::rm(3, the_argv));

	A(r == 1, __FILE__, __LINE__);
}

void t6()
{
	auto a(Auto_caller(reset_cerr));
	F f;
	string root(f.get_path());

	vector<char> v0(strtovec("fm-rm"));
	vector<char> v1(strtovec("-R"));
	vector<char> v2(strtovec("-b"));
	vector<char> v3(strtovec("./"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	ostringstream os;
	fmcerr = &os;

	int r(fm::rm(4, the_argv));

	A(r == 1, __FILE__, __LINE__);
}

} // nrm

using ::ml::test::fixture::get_values;
using ::ml::test::fixture::set_category;

inline void reset_cout()
{
	fmcout = &std::cout;
}

inline void reset_cin()
{
	fmcin = &std::cin;
}

using std::endl;
using std::back_inserter;
using su::split_string;
using std::stringstream;
using ::test::fixture::return_param;
using nomagic::loc;
using nomagic::Test;

class From_stdin_test {
public:
	From_stdin_test(stringstream& is)
		:	is(is) {
	}

	void test(const char* ms);
protected:
	virtual void prepare(Filemanager& manager) {
	}

	virtual void execute() = 0;

	virtual void verify(Test& t) = 0;

	::fm::filemanager::test::Fm_fixture f;
	ostringstream os;
	ostringstream oserr;
private:
	stringstream& is;
};

void From_stdin_test::test(const char* ms)
{
	using ::test::fixture::return_param;
	using nomagic::loc;

	nomagic::Test t(ms);
	auto aout(Auto_caller(reset_cout));
	auto ain(Auto_caller(reset_cin));
	auto aerr(Auto_caller(reset_cerr));
	chdir(f.get_root().to_filepath_string().c_str());
	auto& m(f.get_manager());

	prepare(m);

	fmcout = &os;
	fmcin = &is;
	fmcerr = &oserr;

	execute();
	verify(t);
}

namespace ngetcat {

void t1()
{
	auto a(Auto_caller(reset_cerr));
	FmfsF f("dir1/dir2");
	Absolute_path root(f.get_fm_parent());
	chdir(root.to_filepath_string().c_str());
	create_emptyfile(root.parent().child("file1"));
	
	vector<char> v0(strtovec("fm-getcat"));
	vector<char> v1(strtovec("../file1"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = 0;

	ostringstream os;
	fm::fmcerr = &os;

	int r(fm::getcat(2, the_argv));

	A(os.str() != "", __FILE__, __LINE__);
	A(r == 0, __FILE__, __LINE__);
}

void t2()
{
	auto a(Auto_caller(reset_cerr));
	Nested_fmfs_fixture f;

	vector<char> v0(strtovec("fm-getcat"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = 0;

	ostringstream os;
	fmcerr = &os;

	int r(fm::getcat(3, the_argv));

	A(r == 1, __FILE__, __LINE__);
	A(os.str() != "", __FILE__, __LINE__);
}

void t3()
{
	auto a(Auto_caller(reset_cout));
	Nested_fmfs_fixture f;

	Filemanager m1(f.get_fmdir());
	set_category(m1.get_map(), "file1", "catA");

	Filemanager m2(f.get_parent_fmdir());
	set_category(m2.get_map(), "dir_1/file1", "catB");

	vector<char> v0(strtovec("fm-getcat"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	vector<char> v3(strtovec("file1"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	ostringstream os;
	fmcout = &os;

	int r(fm::getcat(4, the_argv));

	A(r == 0, __FILE__, __LINE__);
	ostringstream expect;
	expect << "catB" << endl;
	A(os.str() == expect.str(), __FILE__, __LINE__);
}

class Getcat_from_stdin_test : public From_stdin_test {
public:
	Getcat_from_stdin_test(stringstream& is)
		:	From_stdin_test(is), r(-1) {
	}

protected:
	void prepare(Filemanager& manager);
	void execute();
	void verify(Test& test);

private:
	vector<char> v0;
	vector<char> v1;
	int r;
};

void Getcat_from_stdin_test::prepare(Filemanager& m)
{
	Absolute_path f1(return_param(create_emptyfile,
		f.get_root().child("f1")));
	set_category(m.get_map(), "f1", "catA");

	Absolute_path f2(return_param(create_emptyfile,
		f.get_root().child("f2")));
	set_category(m.get_map(), "f2", "catA", "catB");
}

void Getcat_from_stdin_test::execute()
{
	v0 = strtovec("fm-getcat");
	v1 = strtovec("-c");
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = 0;

	r = getcat(2, the_argv);
}

void Getcat_from_stdin_test::verify(Test& t)
{
	t.a(r == 0, loc(__FILE__, __LINE__));
	vector<string> v;
	split_string(os.str(), "\n", back_inserter(v));
	v.pop_back();
	std::sort(v.begin(), v.end());
	t.a(v.size() == 2, loc(__FILE__, __LINE__));
	t.a(v.at(0) == "catA", loc(__FILE__, __LINE__));
	t.a(v.at(1) == "catB", loc(__FILE__, __LINE__));
	t.a(oserr.str().empty(), loc(__FILE__, __LINE__));
}

void t4(const char* ms)
{
	stringstream is;
	is << "f1" << endl;
	is << "f2" << endl;
	Getcat_from_stdin_test t(is);
	t.test(ms);
}

void t5(const char* ms)
{
	stringstream is;
	is << "f1" << endl;
	is << " " << endl;
	is << " \t" << endl;
	is << "\t" << endl;
	is << "f2" << endl;
	Getcat_from_stdin_test t(is);
	t.test(ms);
}

} // ngetcat

namespace nset {

typedef Nested_fmfs_fixture F;

void t1()
{
	F f;

	vector<char> v0(strtovec("fm-set"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	vector<char> v3(strtovec(f.get_child_file1_path()));
	vector<char> v4(strtovec("catA"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	fm::set(5, the_argv);

	Filemanager m(f.get_parent_fmdir());
	vector<string> t;
	m.get_map().get_categories(back_inserter(t));
	A(t.size() == 1, __FILE__, __LINE__);
	A(t.at(0) == "catA", __FILE__, __LINE__);
}

void t2()
{
	auto a(Auto_caller(reset_cerr));
	F f;

	vector<char> v0(strtovec("fm-set"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	vector<char> v3(strtovec(f.get_child_file1_path()));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	ostringstream os;
	fm::fmcerr = &os;

	int r(fm::set(4, the_argv));
	A(os.str() != "", __FILE__, __LINE__);
	A(r == 1, __FILE__, __LINE__);
}

void t3()
{
	auto a(Auto_caller(reset_cerr));
	F f;

	vector<char> v0(strtovec("fm-set"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	vector<char> v3(strtovec(f.get_child_file1_path()));
	vector<char> v4(strtovec("catA"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	ostringstream os;
	fm::fmcerr = &os;

	int r(fm::set(5, the_argv));
	A(os.str() != "", __FILE__, __LINE__);
	A(r == 1, __FILE__, __LINE__);
}

} // nset

namespace nrefresh {

void t1()
{
	Nested_fmfs_fixture f;

	Filemanager m1(f.get_fmdir());
	set_category(m1.get_map(), "not_exising_file1", "catA");
	A(get_values(m1.get_map(), "catA").size() == 1, __FILE__, __LINE__);

	Filemanager m2(f.get_parent_fmdir());
	set_category(m2.get_map(), "dir_1/not_existing_file1", "catA");
	A(get_values(m2.get_map(), "catA").size() == 1, __FILE__, __LINE__);

	vector<char> v0(strtovec("fm-refresh"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = 0;

	fm::refresh(3, the_argv);

	A(get_values(m1.get_map(), "catA").size() == 1, __FILE__, __LINE__);
	A(get_values(m2.get_map(), "catA").empty(), __FILE__, __LINE__);
}

void t2()
{
	auto a(Auto_caller(reset_cerr));
	F f;

	ostringstream os;
	fmcerr = &os;

	vector<char> v0(strtovec("fm-refresh"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = 0;

	int r(fm::refresh(3, the_argv));
	A(r == 1, __FILE__, __LINE__);
}

void t3()
{
	auto a(Auto_caller(reset_cerr));
	F f;

	ostringstream os;
	fmcerr = &os;

	vector<char> v0(strtovec("fm-refresh"));
	vector<char> v1(strtovec("-b"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = 0;

	int r(fm::refresh(2, the_argv));
	A(r == 1, __FILE__, __LINE__);
}

} // nrefresh

namespace nget {

void t1()
{
	auto a(Auto_caller(reset_cout));
	Nested_fmfs_fixture f;

	Filemanager m1(f.get_parent_fmdir());
	set_category(m1.get_map(), "dir_1/file1", "catA");
	A(get_values(m1.get_map(), "catA").size() == 1, __FILE__, __LINE__);

	Filemanager m2(f.get_fmdir());
	set_category(m2.get_map(), "file1", "catB");
	A(get_values(m2.get_map(), "catB").size() == 1, __FILE__, __LINE__);

	vector<char> v0(strtovec("fm-get"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	vector<char> v3(strtovec("catA"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	ostringstream os;
	fmcout = &os;

	fm::get(4, the_argv);

	vector<string> v;
	split_string(os.str(), "\n", back_inserter(v));
	A(v.size() == 2, __FILE__, __LINE__);
	A(v.at(0) != "", __FILE__, __LINE__);
	A(v.at(1) == "", __FILE__, __LINE__);
}

void t2()
{
	auto a1(Auto_caller(reset_cout));
	auto a2(Auto_caller(reset_cerr));

	Nested_fmfs_fixture f;

	vector<char> v0(strtovec("fm-get"));
	vector<char> v1(strtovec("-b"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = 0;

	ostringstream os;
	fmcout = fmcerr = &os;

	int r(fm::get(2, the_argv));

	A(r == 1, __FILE__, __LINE__);
}

void t3()
{
	auto a1(Auto_caller(reset_cout));
	auto a2(Auto_caller(reset_cerr));

	Nested_fmfs_fixture f;

	vector<char> v0(strtovec("fm-get"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	vector<char> v3(strtovec("file1"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	ostringstream os;
	fmcout = fmcerr = &os;

	int r(fm::get(4, the_argv));

	A(r == 1, __FILE__, __LINE__);
}

void t4()
{
	auto a1(Auto_caller(reset_cout));
	auto a2(Auto_caller(reset_cerr));

	Nested_fmfs_fixture f;

	vector<char> v0(strtovec("fm-get"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = 0;

	ostringstream os;
	fmcout = fmcerr = &os;

	int r(fm::get(3, the_argv));

	A(r == 1, __FILE__, __LINE__);
}

class Get_from_stdin_test : public From_stdin_test {
public:
	Get_from_stdin_test(stringstream& is);

	void set_no_name(bool value);
protected:
	void prepare(Filemanager& manager);
	void execute();
	void verify(Test& test);
private:
	vector<char> v0;
	vector<char> v1;
	vector<char> v2;
	int r;
	bool no_name;
};

Get_from_stdin_test::Get_from_stdin_test(stringstream& is)
:	From_stdin_test(is), r(-1), no_name(false)
{
}

void Get_from_stdin_test::set_no_name(bool value)
{
	no_name = value;
}

void Get_from_stdin_test::prepare(Filemanager& m)
{
	Absolute_path f1(return_param(create_emptyfile,
		f.get_root().child("f1")));
	set_category(m.get_map(), "f1", "catA", "catB", "catC");

	Absolute_path f2(return_param(create_emptyfile,
		f.get_root().child("f2")));
	set_category(m.get_map(), "f2", "catA");
}

void Get_from_stdin_test::execute()
{
	v0 = strtovec("fm-get");
	v1 = strtovec("-c");
	v2 = strtovec("catC");
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	if (no_name)
		the_argv[2] = 0;
	else {
		the_argv[2] = &v2.at(0);
		the_argv[3] = 0;
	}

	r = get(no_name ? 2 : 3, the_argv);
}

void Get_from_stdin_test::verify(Test& t)
{
	t.a(r == 0, loc(__FILE__, __LINE__));
	vector<string> v;
	split_string(os.str(), "\n", back_inserter(v));
	v.pop_back();
	t.a(v.size() == 1, loc(__FILE__, __LINE__));
	std::size_t length(std::char_traits<char>::length("f1"));
	t.a(v.at(0).rfind("f1") == v.at(0).length() - length,
		loc(__FILE__, __LINE__));
}

void t5(const char* ms)
{
	stringstream s;
	s << "catA" << endl;
	s << "catB" << endl;
	Get_from_stdin_test t(s);
	t.test(ms);
}

void t6(const char* ms)
{
	stringstream s;
	s << "catA" << endl;
	s << "catB" << endl;
	s << "catC" << endl;
	Get_from_stdin_test t(s);
	t.set_no_name(true);
	t.test(ms);
}

void t7(const char* ms)
{
	auto a(Auto_caller(reset_cout));
	::fm::filemanager::test::Fm_fixture f;

	chdir(f.get_root().to_filepath_string().c_str());
	Filemanager m(f.get_fm_path());
	create_emptyfile(f.get_root().child("file1"));
	set_category(m.get_map(), "file1", "catA");

	create_emptyfile(f.get_root().child("file2"));
	set_category(m.get_map(), "file2", "catB");

	vector<char> v0(strtovec("fm-get"));
	vector<char> v1(strtovec("-o"));
	vector<char> v2(strtovec("catA"));
	vector<char> v3(strtovec("catB"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	ostringstream os;
	fmcout = &os;

	fm::get(4, the_argv);

	vector<string> v;
	split_string(os.str(), "\n", back_inserter(v));
	v.pop_back();
	A(v.size() == 2, __FILE__, __LINE__);
	A(v.at(0) != "", __FILE__, __LINE__);
	A(v.at(1) != "", __FILE__, __LINE__);
}

} // nget

namespace nmv {

void t1()
{
	Nested_fmfs_fixture f;

	Filemanager m(f.get_parent_fmdir());
	set_category(m.get_map(), "dir_1/file1", "catA");

	vector<char> v0(strtovec("fm-mv"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("../"));
	vector<char> v3(strtovec("file1"));
	vector<char> v4(strtovec("file2"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	int r(fm::mv(5, the_argv));

	A(r == 0, __FILE__, __LINE__);
	A(get_values(m.get_map(), "catA").size() == 1, __FILE__, __LINE__);
	A(get_values(m.get_map(), "catA").at(0) == "dir_1/file2",
		__FILE__, __LINE__);
}

void t2()
{
	auto a(Auto_caller(reset_cerr));
	Nested_fmfs_fixture f;

	vector<char> v0(strtovec("fm-mv"));
	vector<char> v1(strtovec("-b"));
	vector<char> v2(strtovec("--"));
	vector<char> v3(strtovec("file1"));
	vector<char> v4(strtovec("file2"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = &v4.at(0);
	the_argv[5] = 0;

	ostringstream os;
	fmcerr = &os;

	int r(fm::mv(5, the_argv));

	A(r == 1, __FILE__, __LINE__);
}

void t3()
{
	Nested_fmfs_fixture f;
	Absolute_path root(f.get_fmdir().parent());

	Absolute_path src1(root.child("file1"));
	createfile(src1.to_filepath_string());

	Absolute_path src2(root.child("file2"));
	createfile(src2.to_filepath_string());

	Absolute_path dest(root.child("destdir1"));
	createdir(dest.to_filepath_string());

	vector<char> v0(strtovec("fm-mv"));
	vector<char> v1(strtovec("file1"));
	vector<char> v2(strtovec("file2"));
	vector<char> v3(strtovec("destdir1"));
	the_argv[0] = &v0.at(0);
	the_argv[1] = &v1.at(0);
	the_argv[2] = &v2.at(0);
	the_argv[3] = &v3.at(0);
	the_argv[4] = 0;

	int r(fm::mv(4, the_argv));

	A(r == 0, __FILE__, __LINE__);
	A(get_stat_result(src1.to_filepath_string()) == ENOENT, __FILE__,
		__LINE__);
	A(get_stat_result(src2.to_filepath_string()) == ENOENT, __FILE__,
		__LINE__);
	A(get_stat_result(dest.child("file1").to_filepath_string()) == 0,
		__FILE__, __LINE__);
	A(get_stat_result(dest.child("file2").to_filepath_string()) == 0,
		__FILE__, __LINE__);
}

} // nmv

} // unnamed

using nomagic::run;

void cp_tests()
{
	using namespace ncp;

	run(	"The command should copy multiple source files to a target "
		"directory.", t1);

	run(	"The command should copy multiple directories to a target "
		"directory.", t2);

	run("It is an error if the object construction failed.", t3);

	run(	"It is an error if it is called without a source "
		"and a target.", t4);
}

void rm_tests()
{
	using namespace nrm;

	run(	"The command should output an error message when it is called "
		"without -R option and the target file is a directory. ", t1);

	run(	"The command should remove a directory without error message "
		"if it is called with -R option when the target file is a "
		"directory.", t2);

	run(	"The command should accept multiple targets.", t3);

	run(	"The command should accept multiple targets when it is called "
		"with the -R option.", t4);

	run("It is an error if the filemanager construction failed.", t5);

	run(	"It is an error if it lacks arguments after parsing "
		"options.", t6);
}

void getcat_tests()
{
	using namespace ngetcat;

	run(	"It returns exit status 1 and outputs a diagnostic message "
		"when the target file is not managed.", t1);

	run("It is an error if the filemanager construction failed.", t2);

	run("The base directory can be specified by the -b flag.", t3);

	run("It reads target list from standard input.", t4);

	run("It ignores invalid lines when it reads from standard input.", t5);
}

void set_tests()
{
	using namespace nset;

	run("The base directory can be specified by the -b flag.", t1);

	run(	"It is an error if it lacks arguments after parsing "
		"options.", t2);

	run("It is an error if the filemanager construction failed.", t3);
}

void refresh_tests()
{
	using namespace nrefresh;

	run("The base directory can be specified by the -b parameter.", t1);

	run("It is an error if the filemanager construction failed.", t2);

	run(	"It is an error when it is called with -b without an option "
		"value.", t3);
}

void get_tests()
{
	using namespace nget;

	run("The base directory can be specified by the -b parameter.", t1);

	run(	"It is an error when it is called with -b without an option "
		"value.", t2);

	run("It is an error if the filemanager construction failed.", t3);

	run("It is an error if any target file are not specified.", t4);

	run("It reads name list from the standard input.", t5);

	run(	"It is not an error if no names are specified "
		"on the command line but some names specified at "
		"the standard input.", t6);

	run("It uses OR condition if the -o flag is set.", t7);
}

void mv_tests()
{
	using namespace nmv;

	run("The base directory can be specified by the -b parameter.", t1);

	run("It is an error if the filemanager construction faile.", t2);

	run("It can move multiple files.", t3);
}

} // test
} // fm
