#include <JsonBox/Array.h>

#include <JsonBox/Value.h>
#include <JsonBox/Grammar.h>
#include <JsonBox/OutputFilter.h>
#include <JsonBox/Indenter.h>

namespace JsonBox {
	std::ostream &operator<<(std::ostream &output, const Array &a) {
		if (a.empty()) {
			output << Structural::BEGIN_ARRAY << Structural::END_ARRAY;

		} else {
			output << Structural::BEGIN_ARRAY << std::endl;
			OutputFilter<Indenter> indent(output.rdbuf());
			output.rdbuf(&indent);

			for (Array::const_iterator i = a.begin(); i != a.end(); ++i) {
				if (i != a.begin()) {
					output << Structural::VALUE_SEPARATOR << std::endl;
				}

				output << *i;
			}

			output.rdbuf(indent.getDestination());

			output << std::endl << Structural::END_ARRAY;
		}

		return output;
	}
}
