//
// Copyright (c) 2011 Anhero Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include "jsonobject.h"

#include "jsonvalue.h"
#include "jsongrammar.h"
#include "jsonoutputfilter.h"
#include "jsonindenter.h"

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
