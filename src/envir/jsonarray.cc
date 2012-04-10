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

#include "jsonarray.h"

#include "jsonvalue.h"
#include "jsongrammar.h"
#include "jsonoutputfilter.h"
#include "jsonindenter.h"

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
