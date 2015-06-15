#ifndef __ML_RECEIVER_H__
#define __ML_RECEIVER_H__

namespace ml {

template<class T>
class Receiver {
public:
	virtual ~Receiver() throw() {
	}
	virtual bool receive(const T& value) = 0;
};

} // ml

#endif // __ML_RECEIVER_H__
