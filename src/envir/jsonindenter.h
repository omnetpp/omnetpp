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

#ifndef JB_INDENTER_H
#define JB_INDENTER_H

#include <streambuf>

namespace JsonBox {
    /**
     * Adds a level of indentation to a streambuf.
     * @see JsonBox::OutputFilter
     */
    class Indenter {
    public:
        /**
         * Default constructor.
         */
        Indenter();

        /**
         * Inserts a tab character at the start of each line.
         * @param destination Streambuf in which to insert the tab character.
         * @param character Character to insert in the streambuf.
         * @return Unspecified value not equal to traits::eof() on success,
         * traits::eof() on failure.
         */
        std::streambuf::int_type operator()(std::streambuf &destination,
                                            std::streambuf::int_type character);
    private:
        /// Used to indicate if we are at the start of a new line.
        bool atStartOfLine;
    };
}

#endif
