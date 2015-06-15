#ifndef __FM_FILEMANAGER_TEST_MOVE_COPY_TEST_H__
#define __FM_FILEMANAGER_TEST_MOVE_COPY_TEST_H__

#include <string>
#include <vector>
#include <absolute_path.h>
#include <filemanager.h>
#include <tests/fixture/fm_filesystem.h>
#include "move_copy_fixture.h"

namespace fm {
namespace filemanager {
namespace test {

class Move_copy_test : public Move_copy_fixture {
public:
	Move_copy_test();

	void test_should_move_or_copy_a_file_to_a_file();
	void test_should_move_a_file_to_a_filepath();
	void test_should_move_a_file_to_a_directory();
	void test_should_move_a_directory_to_a_directory();
	void test_should_not_move_similar_files();
	void test_should_not_move_a_file_when_it_looks_like_a_dir();
	void test_should_not_move_a_dir_when_it_looks_like_a_file();
	void test_move_a_file_mapping_without_its_real_file();
	void test_move_a_dir_mapping_without_its_real_dir();
	void test_should_not_move_a_dir_to_a_dir_which_does_not_exist();
	void test_move_a_dir_to_a_dirpath_where_there_is_a_dir();
	void test_should_not_move_a_file_to_a_dest_file_which_does_not_exist();
	void test_move_a_file_to_a_path_where_there_is_a_file();
	void test_should_not_move_a_file_to_a_dir_which_does_not_exist();
	void test_should_not_move_a_file_to_a_dir_when_a_dir_exists();
	void test_should_move_a_directory_to_a_directory_path();
	void test_should_not_move_a_file_to_an_impossible_path();
	void test_should_not_move_a_directory_to_an_impossible_path();
	void test_should_not_move_a_directory_to_its_an_ancestor();
	void test_move_a_file_to_a_dir_which_does_not_have_any_mappings();
	void move_files_to_paths_it_does_not_manage();

protected:
	std::vector<std::string>& get_categories1() {
		return categories1;
	}

	std::vector<std::string>& get_categories2() {
		return categories2;
	}

private:
	virtual void call_method(const Absolute_path& source,
		const Absolute_path& destination) = 0;
	virtual void call_method(Filemanager& manager,
		const Absolute_path& source,
		const Absolute_path& destination) = 0;
	void add_category_file(const std::string& fm_path,
		const std::string& dir_fm_path,
		const std::vector<std::string>& categories);

	virtual void assert_should_move_or_copy_a_file_to_a_file(
		const std::string& src, const std::string& dest) = 0;
	virtual void assert_should_move_a_file_to_a_filepath() = 0;
	virtual void assert_should_move_a_file_to_a_directory(
		const std::string& src, const std::string& dest) = 0;
	virtual void assert_should_move_a_directory_to_a_directory(
		const std::string& src, const std::string& dest) = 0;
	virtual void assert_should_not_move_similar_files() = 0;
	virtual void assert_move_a_dir_mapping_without_its_real_dir() = 0;
	virtual void assert_should_move_a_directory_to_directory_path() = 0;
	virtual void
		assert_move_a_file_to_a_dir_which_does_not_have_any_mappings(
			const std::string& root) = 0;
	virtual void assert_move_a_file_mapping_without_its_real_file() = 0;
	virtual void assert_move_a_dir_to_a_dirpath_where_there_is_a_dir(
		const Absolute_path& src, const Absolute_path& dest) = 0;
	virtual void assert_move_a_file_to_a_path_where_there_is_a_file(
		const Absolute_path& src, const Absolute_path& dest) = 0;
	virtual void assert_move_files_to_paths_it_does_not_manage(
		Filemanager& m, const Absolute_path& src,
		const Absolute_path& dest) = 0;

	std::vector<std::string> categories1;
	std::vector<std::string> categories2;
};

class Move_test : public Move_copy_test {
public:
	void call_method(const Absolute_path& source,
		const Absolute_path& destination) {
		get_manager().move(source, destination);
	}
	void call_method(Filemanager& manager,
		const Absolute_path& source,
		const Absolute_path& destination) {
		manager.move(source, destination);
	}
private:
	void assert_should_move_or_copy_a_file_to_a_file(
		const std::string& src, const std::string& dest);
	void assert_should_move_a_file_to_a_filepath();
	void assert_should_move_a_file_to_a_directory(
		const std::string& src, const std::string& dest);
	void assert_should_move_a_directory_to_a_directory(
		const std::string& src, const std::string& dest);
	void assert_should_not_move_similar_files();
	void assert_move_a_dir_mapping_without_its_real_dir();
	void assert_should_move_a_directory_to_directory_path();
	void assert_move_a_file_to_a_dir_which_does_not_have_any_mappings(
		const std::string& root);
	void assert_move_a_file_mapping_without_its_real_file();
	void assert_move_a_dir_to_a_dirpath_where_there_is_a_dir(
		const Absolute_path& src, const Absolute_path& dest);
	void assert_move_a_file_to_a_path_where_there_is_a_file(
		const Absolute_path& src, const Absolute_path& dest);
	void assert_move_files_to_paths_it_does_not_manage(
		Filemanager& m, const Absolute_path& src,
		const Absolute_path& dest);

	std::vector<std::string> result1;
	std::vector<std::string> result2;
};

class Copy_test : public Move_copy_test {
public:
	Copy_test()
		:	for_dir(false) {
	}

	Copy_test(bool for_dir)
		:	for_dir(for_dir) {
	}

	void call_method(const Absolute_path& source,
		const Absolute_path& destination) {
		if (for_dir)
			get_manager().copydir(source, destination);
		else
			get_manager().copy(source, destination);
	}
	void call_method(Filemanager& manager,
		const Absolute_path& source,
		const Absolute_path& destination) {
		if (for_dir)
			manager.copydir(source, destination);
		else
			manager.copy(source, destination);
	}
private:
	void assert_should_move_or_copy_a_file_to_a_file(
		const std::string& src, const std::string& dest);
	void assert_should_move_a_file_to_a_filepath();
	void assert_should_move_a_file_to_a_directory(
		const std::string& src, const std::string& dest);
	void assert_should_move_a_directory_to_a_directory(
		const std::string& src, const std::string& dest);
	void assert_should_not_move_similar_files();
	void assert_move_a_dir_mapping_without_its_real_dir();
	void assert_should_move_a_directory_to_directory_path();
	void assert_move_a_file_to_a_dir_which_does_not_have_any_mappings(
		const std::string& root);
	void assert_move_a_file_mapping_without_its_real_file();
	void assert_move_a_dir_to_a_dirpath_where_there_is_a_dir(
		const Absolute_path& src, const Absolute_path& dest);
	void assert_move_a_file_to_a_path_where_there_is_a_file(
		const Absolute_path& src, const Absolute_path& dest);
	void assert_move_files_to_paths_it_does_not_manage(
		Filemanager& m, const Absolute_path& src,
		const Absolute_path& dest);

	std::vector<std::string> result1;
	std::vector<std::string> result2;
	bool for_dir;
};

} // test
} // filemanager
} // fm

#endif // __FM_FILEMANAGER_TEST_MOVE_COPY_TEST_H__
