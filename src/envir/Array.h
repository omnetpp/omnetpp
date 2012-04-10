#ifndef JB_ARRAY_H
#define JB_ARRAY_H

#include <iostream>
#include <deque>

namespace JsonBox {
	class Value;

	/**
	 * Represents an array of values in JSON. It's a deque with added methods.
	 * So it can be used the same way as a standard STL deque, but can be more
	 * easily output in a stream.
	 * @see JsonBox::Value
	 */
	typedef std::deque<Value> Array;

	/**
	 * Output operator overload for the JSON array. Outputs in standard JSON
	 * format. Indents the output and esapes the minimum characters.
	 * @param output Output stream in which to write the array's JSON.
	 * @param a Array to output into the stream.
	 * @return Output stream filled with the JSON code.
	 */
	std::ostream& operator<<(std::ostream& output, const Array& a);
}

#endif
