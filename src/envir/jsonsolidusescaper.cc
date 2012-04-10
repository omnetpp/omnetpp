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

#include "jsonsolidusescaper.h"

#include "jsongrammar.h"

namespace JsonBox {
    SolidusEscaper::SolidusEscaper() : afterBackSlash(false), inString(false) {
    }

    std::streambuf::int_type SolidusEscaper::operator()(std::streambuf &destination,
                                                        std::streambuf::int_type character) {
        bool notEscaped = true;
        std::streambuf::char_type tmpChar = std::streambuf::traits_type::to_char_type(character);

        // If we encounter a quotation mark.
        if (tmpChar == Strings::Json::Escape::QUOTATION_MARK) {
            // If we're not in a string, we change that. If we're in a string,
            // we change that only if we're not after an escape back slash.
            inString = !inString || (afterBackSlash);

        } else if (inString && !afterBackSlash) {
            // If we are in a string definition and we're not after a backslash
            // escape.
            if (tmpChar == Strings::Std::SOLIDUS) {
                destination.sputn(Strings::Json::SOLIDUS.c_str(), Strings::Json::SOLIDUS.size());
                notEscaped = false;

            }

        }

        // We determine if we start a backslash escape or not.
        afterBackSlash = inString && !afterBackSlash && (tmpChar == Strings::Json::Escape::BEGIN_ESCAPE);
        return (notEscaped) ? (destination.sputc(tmpChar)) : (0);
    }
}
