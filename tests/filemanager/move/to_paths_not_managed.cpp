#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <nomagic.h>
#include <absolute_path.h>
#include <filemanager.h>
#include <filesystem.h>
#include <tests/fixture/assert.h>
#include <tests/fixture/filesystem.h>
#include <tests/fixture/process_dir.h>
#include <tests/filemanager/move_copy_test.h>
#include "to_paths_not_managed.h"

namespace fm {
namespace filemanager {
namespace test {

using std::back_inserter;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using nomagic::run;
namespace gfs = ::fs;
using gfs::mkdir;

namespace {

template<class T, class U, class S>
inline void A(T t, U u, S s)
{
	::test::fixture::Assert(t, u, s);
}

using ::test::Process_dir_fixture;

class Fixture : public Process_dir_fixture {
public:
	Fixture()
		:	root(get_path() + "/dir1/dir2/dir3") {
		mkdir(get_fmdir_path());
		cat.push_back("catA");
	}

	const Absolute_path& get_parent() const {
		return root;
	}

	Absolute_path get_fmdir_path() const {
		return root.child(".fm");
	}

	void add_categories(Filemanager& manager, const string& value) {
		manager.get_map().set(value, cat.begin(), cat.end());
	}

	vector<string> get_categories_values(Filemanager& manager) {
		vector<string> r;
		manager.get_map().get(cat, back_inserter(r));
		return r;
	}
private:
	Absolute_path root;
	vector<string> cat;
};

class Test_fixture : public Fixture {
public:
	Test_fixture()
		:	manager(get_fmdir_path()) {
	}

	void add_categories(const string& value) {
		Fixture::add_categories(manager, value);
	}

	vector<string> get_categories_values() {
		return Fixture::get_categories_values(manager);
	}
protected:
	Filemanager manager;
	Absolute_path dest;
};
typedef Test_fixture F;

using ::test::fixture::get_stat_result;
using gfs::create_emptyfile;

class To_dir_test_fixture : protected Test_fixture {
public:
	To_dir_test_fixture() {
		dest = get_parent().parent().parent();
	}
};

class File_to_dir_test : protected To_dir_test_fixture {
public:
	File_to_dir_test()
		:	require_srccat(true) {
	}

	void test() {
		src = get_parent().child("srcfile1");
		create_emptyfile(src);
		if (require_srccat)
			add_categories("srcfile1");

		call_method(src, dest);

		verify();
	}

	void require_src_categories(bool flag) {
		require_srccat = flag;
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Absolute_path src;
	bool require_srccat;
};

class Copydir_file_to_dir_test : public File_to_dir_test {
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.copydir(src, dest);
	}
	void verify() {
		A(get_stat_result(
			dest.child("srcfile1").to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.size() == 1, __FILE__, __LINE__);
		A(r.at(0) == "srcfile1", __FILE__, __LINE__);
	}
};

class Move_file_to_dir_test : public File_to_dir_test {
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.move(src, dest);
	}
	void verify() {
		A(get_stat_result(
			dest.child("srcfile1").to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.empty(), __FILE__, __LINE__);
	}
};

class Dir_to_dir_test : protected To_dir_test_fixture {
public:
	void test() {
		Absolute_path src(get_parent().child("dir4"));
		mkdir(src);

		create_emptyfile(src.child("file1"));
		add_categories("dir4/file1");

		create_emptyfile(src.child("file2"));
		add_categories("dir4/file2");

		call_method(src, dest);

		verify();
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;
};

class Copy_dir_to_dir_test : public Dir_to_dir_test {
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.copydir(src, dest);
	}
	void verify() {
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.size() == 2, __FILE__, __LINE__);
		std::sort(r.begin(), r.end());
		A(r.at(0) == "dir4/file1", __FILE__, __LINE__);
		A(r.at(1) == "dir4/file2", __FILE__, __LINE__);
	}
};

class Move_dir_to_dir_test : public Dir_to_dir_test {
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.move(src, dest);
	}
	void verify() {
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.size() == 0, __FILE__, __LINE__);
	}
};

class To_path_test_fixture : protected Test_fixture {
public:
	To_path_test_fixture() {
		dest = get_parent().parent().child("dest1");
	}
};

class File_to_path_test : protected To_path_test_fixture {
public:
	void test() {
		src = get_parent().child("file1");
		create_emptyfile(src);
		add_categories("file1");

		call_method(src, dest);

		verify();
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Absolute_path src;
};

class Copy_file_to_path_test : public File_to_path_test {
public:
	typedef void (Filemanager::*Mp)(const Absolute_path&,
		const Absolute_path&);

	Copy_file_to_path_test()
		:	mp(&Filemanager::copy) {
	}

	Copy_file_to_path_test(Mp mp)
		:	mp(mp) {
	}
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		(manager.*mp)(src, dest);
	}
	void verify() {
		A(get_stat_result(src.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.size() == 1, __FILE__, __LINE__);
		A(r.at(0) == "file1", __FILE__, __LINE__);
	}
private:
	Mp mp;
};

class Move_file_to_path_test : public File_to_path_test {
public:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		manager.move(src, dest);
	}
	void verify() {
		A(get_stat_result(src.to_filepath_string()) == ENOENT,
			__FILE__, __LINE__);
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.empty(), __FILE__, __LINE__);
	}
};

class Dir_to_path_test : protected To_path_test_fixture {
public:
	void test() {
		src = get_parent().child("srcdir1");
		mkdir(src);
		create_emptyfile(src.child("file1"));
		add_categories("srcdir1/file1");

		call_method(src, dest);

		verify();
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Absolute_path src;
};

class Copy_dir_to_path_test : public Dir_to_path_test {
public:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		manager.copydir(src, dest);
	}

	void verify() {
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.size() == 1, __FILE__, __LINE__);
		A(r.at(0) == "srcdir1/file1", __FILE__, __LINE__);
	}
};

class Move_dir_to_path_test : public Dir_to_path_test {
public:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		manager.move(src, dest);
	}

	void verify() {
		A(get_stat_result(src.to_filepath_string()) == ENOENT,
			__FILE__, __LINE__);
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.empty(), __FILE__, __LINE__);
	}
};

class Dir_without_names_to_path_test : protected To_path_test_fixture {
public:
	void test() {
		src = get_parent().child("srcdir1");
		mkdir(src);
		create_emptyfile(src.child("file1"));

		manager.move(src, dest);

		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
	}

protected:
	Absolute_path src;
};

class Name_to_dir_test : protected To_dir_test_fixture {
public:
	void test() {
		src = get_parent().child("file1");
		add_categories("file1");

		mkdir(dest);

		call_method(src, dest);

		verify();
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Absolute_path src;
};

class Copy_name_to_dir_test : public Name_to_dir_test {
public:
	typedef void (Filemanager::*Mp)(const Absolute_path&,
		const Absolute_path&);

	Copy_name_to_dir_test()
		:	mp(&Filemanager::copy) {
	}

	Copy_name_to_dir_test(Mp mp)
		:	mp(mp) {
	}

protected:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		(manager.*mp)(src, dest);
	}

	void verify() {
		vector<string> r(get_categories_values());
		A(r.size() == 1, __FILE__, __LINE__);
		A(r.at(0) == "file1", __FILE__, __LINE__);
	}
private:
	Mp mp;
};

class Move_name_to_dir_test : public Name_to_dir_test {
protected:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		manager.move(src, dest);
	}

	void verify() {
		vector<string> r(get_categories_values());
		A(r.empty(), __FILE__, __LINE__);
	}
};

class Names_to_dir_test : protected To_dir_test_fixture {
public:
	void test() {
		src = get_parent().child("srcdir");

		add_categories("srcdir/file1");
		add_categories("srcdir/file2");
		mkdir(dest.child("srcdir"));
		create_emptyfile(dest.child("srcdir").child("file1"));
		create_emptyfile(dest.child("srcdir").child("file2"));

		call_method(src, dest);

		verify();
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Absolute_path src;
};

class Copy_names_to_dir_test : public Names_to_dir_test {
protected:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		manager.copydir(src, dest);
	}

	void verify() {
		vector<string> r(get_categories_values());
		A(r.size() == 2, __FILE__, __LINE__);
		sort(r.begin(), r.end());
		A(r.at(0) == "srcdir/file1", __FILE__, __LINE__);
		A(r.at(1) == "srcdir/file2", __FILE__, __LINE__);
	}
};

class Move_names_to_dir_test : public Names_to_dir_test {
protected:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		manager.move(src, dest);
	}

	void verify() {
		vector<string> r(get_categories_values());
		A(r.empty(), __FILE__, __LINE__);
	}
};

using ::test::fixture::equal_files;
using ::test::fixture::write_to_file;

class File_to_file_test : protected Test_fixture {
public:
	File_to_file_test()
		:	require_srccat(true) {
	}

	void test() {
		src = get_parent().child("srcfile1");
		create_fileA(src);
		if (require_srccat)
			add_categories("srcfile1");

		dest = get_parent().parent().child("destfile1");
		create_fileB(dest);

		call_method(src, dest);

		verify();
	}

	void require_src_categories(bool flag) {
		require_srccat = flag;
	}
protected:
	virtual void call_method(const Absolute_path& src,
		const Absolute_path& dest) = 0;
	virtual void verify() = 0;

	Absolute_path src;
private:
	void create_fileA(const Absolute_path& path) {
		vector<char> v(2, 'A');
		write_to_file(path.to_filepath_string(), v);
	}
	void create_fileB(const Absolute_path& path) {
		vector<char> v(2, 'B');
		write_to_file(path.to_filepath_string(), v);
	}

	bool require_srccat;
};

class Copy_file_to_file_test : public File_to_file_test {
public:
	typedef void (Filemanager::*Mp)(const Absolute_path&,
		const Absolute_path&);

	Copy_file_to_file_test()
		:	mp(&Filemanager::copy) {
	}

	Copy_file_to_file_test(Mp mp)
		:	mp(mp) {
	}
protected:
	void call_method(const Absolute_path& src,
		const Absolute_path& dest) {
		(manager.*mp)(src, dest);
	}

	void verify() {
		A(equal_files(src.to_filepath_string(),
			dest.to_filepath_string()), __FILE__, __LINE__);
		A(get_stat_result(src.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.size() == 1, __FILE__, __LINE__);
		A(r.at(0) == "srcfile1", __FILE__, __LINE__);
	}
private:
	Mp mp;
};

class Move_file_to_file_test : public File_to_file_test {
protected:
	void call_method(const Absolute_path& src, const Absolute_path& dest) {
		manager.move(src, dest);
	}

	void verify() {
		A(get_stat_result(src.to_filepath_string()) == ENOENT,
			__FILE__, __LINE__);
		A(get_stat_result(dest.to_filepath_string()) == 0,
			__FILE__, __LINE__);
		vector<string> r(get_categories_values());
		A(r.empty(), __FILE__, __LINE__);
	}
};

namespace ncp {

void t1()
{
	Copy_test test;
	test.move_files_to_paths_it_does_not_manage();
}

void t2()
{
	Copy_dir_to_dir_test t;
	t.test();
}

void t3()
{
	Copy_file_to_path_test t;
	t.test();
}

void t4()
{
	Copy_name_to_dir_test t;
	t.test();
}

void t5()
{
	Copy_file_to_file_test t;
	t.test();
}

void run_copy_tests()
{
	cerr << "fm::Filemanager::copy" << endl;

	run(	"It copies files to a directory this tool does not manage. "
		"But it does not copy mappings they have.", t1);

	run(	"It copies directories to paths this tool does not manage. "
		"But it does not copy mappings they have.", t2);

	run(	"It copies a file to path this tool does not manage. "
		"But it does not copy mappings they have.", t3);

	run(	"It does not copy mappings when it does not manage the "
		"destination directory.", t4);

	run(	"It overwrites the destination file by the source file. "
		"But it does not copy mappings if the destination file "
		"is in the directory this tool does not manage.", t5);
}

} // ncp

namespace ncpd {

void t1()
{
	Copy_name_to_dir_test t(&Filemanager::copydir);
	t.test();
}

void t2()
{
	Copy_names_to_dir_test t;
	t.test();
}

void t3()
{
	Copy_dir_to_path_test t;
	t.test();
}

void t4()
{
	Copy_file_to_file_test t(&Filemanager::copydir);
	t.test();
}

void t5()
{
	Copy_file_to_path_test t(&Filemanager::copydir);
	t.test();
}

void t6()
{
	Copydir_file_to_dir_test t;
	t.test();
}

void run_copydir_tests()
{
	cerr << "fm::Filemanager::copydir" << endl;

	run(	"It does not copy mappings when it does not manage the "
		"destination directory.", t1);

	run(	"It does not copy a directory relation when it does not "
		"manage the destination directory.", t2);

	run(	"It does not copy names in a source directory when "
		"it does not manage the destination path.", t3);

	run(	"copydir: It overwrites the destination file by "
		"the source file. But it does not copy mappings "
		"if the destination file is in the directory this "
		"tool does not manage.", t4);

	run(	"It copies a file to path this tool does not manage. "
		"But it does not copy mappings they have.", t5);

	run(	"It copies files to a directory this tool does not manage. "
		"But it does not copy mappings they have.", t6);
}

} // ncpd

namespace nmv {

void t1()
{
	Move_test test;
	test.move_files_to_paths_it_does_not_manage();
}

void t2()
{
	Move_dir_to_dir_test t;
	t.test();
}

void t3()
{
	Move_file_to_path_test t;
	t.test();
}

void t4()
{
	Move_name_to_dir_test t;
	t.test();
}

void t5()
{
	Move_names_to_dir_test t;
	t.test();
}

void t6()
{
	Move_dir_to_path_test t;
	t.test();
}

void t7()
{
	Dir_without_names_to_path_test t;
	t.test();
}

void t8()
{
	Move_file_to_file_test t;
	t.test();
}

void t9()
{
	Move_file_to_file_test t;
	t.require_src_categories(false);
	t.test();
}

void t10()
{
	Move_file_to_dir_test t;
	t.require_src_categories(false);
	t.test();
}

void run_move_tests()
{
	cerr << "fm::Filemanager::move" << endl;

	run(	"It moves files to paths it does not manage, but it removes "
		"all mappings they have.", t1);

	run(	"It moves directories to paths it does not manage, but it "
		"removes all mappings they have.", t2);

	run(	"When it moves a file to a path, it removes all mapping it "
		"has.", t3);

	run(	"When it moves a name to a directory, it is removed.", t4);

	run(	"It does not move a directory relation when it does not "
		"manage the destination directory.", t5);

	run(	"It removes names in the source directory when it "
		"does not manage the destination path.", t6);

	run(	"It is not an error to move a source directory to "
		"the destination path which this tool does not manage, "
		"when the source directory does not have any names. ", t7);

	run(	"It overwrites the destination file by the source file. "
		"It removes name of the source file. ", t8);

	run(	"It moves the source file without a name to the destination "
		"file which this tool does not manage.", t9);

	run(	"When it moves a file without any mappings to a directory, "
		"it removes all mapping it has.", t10);
}

} // nmv

} // unnamed

void run_to_paths_not_managed_tests()
{
	ncp::run_copy_tests();
	ncpd::run_copydir_tests();
	nmv::run_move_tests();
}

} // test
} // filemanager
} // fm
