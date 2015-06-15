#ifndef __UTILS_ASSERT_H__
#define __UTILS_ASSERT_H__

/* Do not include this header in header files. This header is intended to
   be included in implementation files. */

#include <cassert>

namespace utils {

template<class Exception, class Expression>
struct Assert_default_ctor {
	void call_assert(Expression expression) {
		assert(expression);
		if (!expression)
			throw Exception();
	}
};

template<class Exception, class Expression>
struct Assert_string_ctor {
	void call_assert(Expression expression) {
		assert(expression);
		if (!expression)
			throw Exception("");
	}
};

template<class Exception, class Expression>
struct Assert_class : Assert_default_ctor<Exception, Expression> {
};

#ifdef DEFIND_ASSERT_CLASS_STRING_CTOR
#	error "should be happen."
#endif
#define DEFINE_ASSERT_CLASS_STRING_CTOR(class_name)			\
template<class Expression>						\
struct Assert_class<class_name, Expression>				\
	:	Assert_string_ctor<class_name, Expression> {		\
};									\
/**/
DEFINE_ASSERT_CLASS_STRING_CTOR(std::logic_error)

template<class Exception, class Expression>
inline void Assert(Expression expression)
{
	Assert_class<Exception, Expression>().call_assert(expression);
}

} // utils

using utils::Assert;

#endif // __UTILS_ASSERT_H__
