#ifndef __RECEIVER_IMPLS_H__
#define __RECEIVER_IMPLS_H__

#include <iterator>
#include "receiver.h"

namespace ml {

template<class Inserter>
class Receiver_by_inserter
	:	public Receiver<
			typename Inserter::container_type::value_type> {
public:
	typedef typename Inserter::container_type::value_type Argument;
	Receiver_by_inserter(Inserter ins)
		:	ins(ins) {
	}
	bool receive(const Argument& arg) {
		*ins++ = arg;
		return true;
	}
private:
	Inserter ins;
};

template<class Inserter>
inline Receiver_by_inserter<Inserter> Receiver_fun(Inserter& ins)
{
	return Receiver_by_inserter<Inserter>(ins);
}

} // ml

#endif // __RECEIVER_IMPLS_H__
