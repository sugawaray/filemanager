#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include "tree.h"
#include <tree.h>

using namespace std;
using namespace fm;

namespace {

class Path_collector : public Path_node_handler {
public:
	vector<const Path_node*>& get_nodes() throw() {
		return nodes;
	}

	bool handle(const Path_node& node) {
		nodes.push_back(&node);
		return true;
	}
private:
	vector<const Path_node*> nodes;
};

} // unnamed

START_TEST(should_not_callback_when_tree_is_empty)
{
	Path_tree tree;
	Path_collector collector;
	tree.walk(collector);
	fail_unless(collector.get_nodes().empty(), "should be empty");
}
END_TEST

START_TEST(should_callback_when_tree_is_not_empty)
{
	Path_tree tree;
	Path_collector collector;
	tree.insert("");
	tree.walk(collector);
	fail_unless(!collector.get_nodes().empty(), "should not be empty");
}
END_TEST

START_TEST(should_propagate_callback)
{
	Path_tree tree;
	Path_collector collector;
	tree.insert("");
	tree.insert("dir1");
	tree.insert("dir1/dir2");
	tree.walk(collector);
	fail_unless(collector.get_nodes().size() == 3, "count");
}
END_TEST

START_TEST(should_insert_direct_children)
{
	Path_node_allocator allocator;
	Path_node node(allocator, "");
	node.insert("dir1");

	Path_collector collector;
	node.walk(collector);

	auto& nodes(collector.get_nodes());
	fail_unless(nodes.size() == 2, "count");
	fail_unless(nodes.at(0)->get_value() == "", "value1");
	fail_unless(nodes.at(1)->get_value() == "dir1", "value2");
}
END_TEST

START_TEST(should_insert_a_grand_child)
{
	Path_node_allocator allocator;
	Path_node node(allocator, "");
	node.insert("dir1/dir2");

	Path_collector collector;
	node.walk(collector);

	auto& nodes(collector.get_nodes());
	fail_unless(nodes.size() == 3, "count");
	fail_unless(nodes.at(0)->get_value() == "", "value1");
	fail_unless(nodes.at(1)->get_value() == "dir1", "value2");
	fail_unless(nodes.at(2)->get_value() == "dir1/dir2", "value3");
}
END_TEST

START_TEST(should_insert_siblings)
{
	Path_node_allocator allocator;
	Path_node node(allocator, "");
	node.insert("dir1/dir2");
	node.insert("dir2/dir3");

	Path_collector collector;
	node.walk(collector);

	auto& nodes(collector.get_nodes());
	fail_unless(nodes.size() == 5, "count");
	fail_unless(nodes.at(0)->get_value() == "", "value1");
	fail_unless(nodes.at(1)->get_value() == "dir1", "value2");
	fail_unless(nodes.at(2)->get_value() == "dir1/dir2", "value3");
	fail_unless(nodes.at(3)->get_value() == "dir2", "value4");
	fail_unless(nodes.at(4)->get_value() == "dir2/dir3", "value5");
}
END_TEST

START_TEST(should_arrive_root_first_then_arrive_children)
{
	Path_node_allocator allocator;
	Path_node node(allocator, "");
	node.insert("dir1/dir2/dir3");
	node.insert("dir1");

	Path_collector collector;
	node.walk(collector);

	auto& nodes(collector.get_nodes());
	fail_unless(nodes.at(0)->get_value() == "", "1");
	fail_unless(nodes.at(1)->get_value() == "dir1", "2");
	fail_unless(nodes.at(2)->get_value() == "dir1/dir2", "3");
	fail_unless(nodes.at(3)->get_value() == "dir1/dir2/dir3", "4");
}
END_TEST

START_TEST(should_return_true_given_leaf_node_when_is_leaf_called)
{
	Path_node_allocator allocator;
	Path_node node(allocator, "");

	fail_unless(node.is_leaf() == true, "result");
}
END_TEST

START_TEST(should_return_false_given_internal_node_when_is_leaf_called)
{
	Path_node_allocator allocator;
	Path_node node(allocator, "");
	node.insert("dir1");

	fail_unless(node.is_leaf() == false, "result");
}
END_TEST

namespace fm {
namespace test {

TCase* create_tcase_for_tree()
{
	TCase* tcase(tcase_create("tree"));
	tcase_add_test(tcase, should_not_callback_when_tree_is_empty);
	tcase_add_test(tcase, should_callback_when_tree_is_not_empty);
	tcase_add_test(tcase, should_propagate_callback);
	return tcase;
}

TCase* create_tcase_for_tree_node()
{
	TCase* tcase(tcase_create("tree_node"));
	tcase_add_test(tcase, should_insert_direct_children);
	tcase_add_test(tcase, should_insert_a_grand_child);
	tcase_add_test(tcase, should_insert_siblings);
	tcase_add_test(tcase, should_arrive_root_first_then_arrive_children);
	tcase_add_test(tcase,
		should_return_true_given_leaf_node_when_is_leaf_called);
	tcase_add_test(tcase,
		should_return_false_given_internal_node_when_is_leaf_called);
	return tcase;
}

Suite* create_tree_test_suite()
{
	Suite* suite(suite_create("fm_tree"));
	suite_add_tcase(suite, create_tcase_for_tree());
	suite_add_tcase(suite, create_tcase_for_tree_node());
	return suite;
}

} // test
} // fm
