#include "map_impl.h"
#include "map_impl/set.h"
#include "map_impl/core.h"
#include "map_impl/copy.h"
#include "map_impl/get_categories.h"
#include "map_impl/get_values.h"
#include "map_impl/move.h"
#include "map_impl/remove.h"
#include "map_impl/remove_value_spec.h"

using ml::map_impl::test::create_add_tcase;
using namespace ml::test::map_impl;

Suite* create_map_impl_test_suite()
{
	Suite* suite = suite_create("map_impl");
	suite_add_tcase(suite, create_tcase_for_core());
	suite_add_tcase(suite, create_add_tcase());
	suite_add_tcase(suite, create_tcase_for_copy());
	suite_add_tcase(suite, create_tcase_for_get_categories());
	suite_add_tcase(suite, create_tcase_for_get_values());
	suite_add_tcase(suite, create_tcase_for_remove());
	suite_add_tcase(suite, create_tcase_for_remove_value_spec());
	suite_add_tcase(suite, create_tcase_for_move());
	return suite;
}
