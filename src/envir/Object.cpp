#include <JsonBox/Object.h>

#include <JsonBox/Value.h>
#include <JsonBox/Grammar.h>
#include <JsonBox/OutputFilter.h>
#include <JsonBox/Indenter.h>

namespace JsonBox {
	std::ostream &operator<<(std::ostream &output, const Object &o) {
		// If the object is empty, we simply write "{}".
		if (o.empty()) {
			output << Structural::BEGIN_OBJECT << Structural::END_OBJECT;

		} else {
			output << Structural::BEGIN_OBJECT << std::endl;
			OutputFilter<Indenter> indent(output.rdbuf());
			output.rdbuf(&indent);

			// For each item in the object.
			for (Object::const_iterator i = o.begin(); i != o.end(); ++i) {
				if (i != o.begin()) {
					output << Structural::VALUE_SEPARATOR << std::endl;
				}

				// We print the name of the attribute and its value.
				output << Structural::BEGIN_END_STRING << Value::escapeMinimumCharacters(i->first) << Structural::BEGIN_END_STRING << Whitespace::SPACE << Structural::NAME_SEPARATOR << Whitespace::SPACE << i->second;
			}

			output.rdbuf(indent.getDestination());

			output << std::endl << Structural::END_OBJECT;
		}

		return output;
	}
}
