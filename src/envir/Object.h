#ifndef JB_OBJECT_H
#define JB_OBJECT_H

#include <iostream>
#include <map>
#include <string>

namespace JsonBox {
	class Value;

	/**
	 * Represents a JSON object. It's a map with added methods. So the JSON
	 * object type can be used the same way as a standard STL map of string and
	 * Value, but can be more easily output in a stream.
	 * @see JsonBox::Value
	 */
	typedef std::map<std::string, Value> Object;

	/**
	 * Output operator overload for the JSON object. Outputs in standard JSON
	 * format. Indents the output and escapes the minimum characters.
	 * @param output Output stream in which to write the object's JSON.
	 * @param o Object to output into the stream.
	 * @return Output stream filled with the JSON code.
	 */
	std::ostream& operator<<(std::ostream& output, const Object& o);
}

#endif
