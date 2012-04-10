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

#ifndef JB_ARRAY_H
#define JB_ARRAY_H

#include <iostream>
#include <vector>


namespace JsonBox {
    class Value;

    /**
     * Represents an array of values in JSON. It's a deque with added methods.
     * So it can be used the same way as a standard STL deque, but can be more
     * easily output in a stream.
     * @see JsonBox::Value
     */
    typedef std::vector<Value> Array;

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
