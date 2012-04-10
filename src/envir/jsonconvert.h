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

#ifndef JB_CONVERTER_H
#define JB_CONVERTER_H


#include <string>
#include "intxtypes.h" //<stdint.h>

#include "envirdefs.h"

namespace JsonBox {

    typedef std::basic_string<int32_t> String32;
    /**
     * This class is used to encode/decode/transcode UTF8, 16 and 32.
     */
    class Convert {
    public:
        /**
         * Encode the given UTF32 string to a 8bit UTF8 one.
         * @param utf32String UTF32 string to convert to UTF8.
         * @return UTF8 string resulting from the conversion.
         */
        static std::string encodeToUTF8(const String32& utf32String);

        /**
         * Decode the given 8bit UTF8 string to an UTF32 string.
         * @param utf8String UTF8 string to convert to UTF32.
         * @return UTF32 string resulting from the conversion.
         */
        static String32 decodeUTF8(const std::string& utf8String);
    };
}

#endif
