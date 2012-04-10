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

#include "jsonindentcanceller.h"

#include "jsongrammar.h"

namespace JsonBox {
    IndentCanceller::IndentCanceller() : afterBackSlash(false), inString(false) {
    }

    std::streambuf::int_type IndentCanceller::operator()(std::streambuf &destination,
                                                         std::streambuf::int_type character) {
        std::streambuf::char_type tmpChar = std::streambuf::traits_type::to_char_type(character);

        // If we encounter a quotation mark.
        if (tmpChar == Structural::BEGIN_END_STRING) {
            // If we're not in a string, we change that. If we're in a string,
            // we change that only if we're not after an escape back slash.
            inString = !inString || (afterBackSlash);
        }

        // We determine if we start a backslash escape or not.
        afterBackSlash = inString && !afterBackSlash && (tmpChar == Strings::Json::Escape::BEGIN_ESCAPE);

        return (tmpChar != Whitespace::NEW_LINE && tmpChar != Whitespace::HORIZONTAL_TAB && tmpChar != Whitespace::CARRIAGE_RETURN && (inString || tmpChar != Whitespace::SPACE)) ? (destination.sputc(tmpChar)) : (0);
    }
}
