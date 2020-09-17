/*
 * Copyright 2015-2017 YOSHIOKA Takuma
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// Version 1.0

/*
 * Modified in 2020 by the OMNeT++ developers:
 *  - Added bright color variants
 *  - Added some comments and grouping
 *  - Added "faint" and "regular", removed "blink" and "negative"
 *  - Added macros for 8-bit and 24-bit foreground and background colors
 */

#pragma once
#ifndef INCLUDED__LO48576__SGR_MACRO_H_
#define INCLUDED__LO48576__SGR_MACRO_H_

#define SGR__CAT(a, b)          a ## b
#define SGR__CAT2(a, b)         SGR__CAT(a, b)
#define SGR__SECOND(a, b, ...)  b
#define SGR__CALL_SECOND(...)   SGR__SECOND(__VA_ARGS__,,)

#define SGR__SINGLE(x)          SGR_ ## x
#define SGR__EMPTY()
#define SGR__EVAL1(...)     __VA_ARGS__
#define SGR__EVAL2(...)     SGR__EVAL1(SGR__EVAL1(__VA_ARGS__))
#define SGR__EVAL4(...)     SGR__EVAL2(SGR__EVAL2(__VA_ARGS__))
#define SGR__EVAL8(...)     SGR__EVAL4(SGR__EVAL4(__VA_ARGS__))
#define SGR__EVAL16(...)    SGR__EVAL8(SGR__EVAL8(__VA_ARGS__))
#define SGR__EVAL32(...)    SGR__EVAL16(SGR__EVAL16(__VA_ARGS__))
#define SGR__EVAL64(...)    SGR__EVAL32(SGR__EVAL32(__VA_ARGS__))
#define SGR__EVAL128(...)   SGR__EVAL64(SGR__EVAL64(__VA_ARGS__))
#define SGR__DEFER3(m)      m SGR__EMPTY SGR__EMPTY SGR__EMPTY()()()
// Returns `SGR__SEQ_CONCAT_` if the arguments are given, returns `SGR__IGNORE` if none was given.
#define SGR__ARGCHECK(first, ...)   SGR__CALL_SECOND(SGR__CAT(SGR__ARGCHECK_, first) , SGR__DEFER3(SGR__SEQ_CONCAT_)()) (first, __VA_ARGS__)
#define SGR__ARGCHECK_      ~, SGR__IGNORE
#define SGR__IGNORE(...)
#define SGR__SEQ(first, ...)    SGR__SINGLE(first) SGR__ARGCHECK(__VA_ARGS__,)
#define SGR__SEQ_CONCAT(first, ...) ";" SGR__SINGLE(first) SGR__ARGCHECK(__VA_ARGS__,)
#define SGR__SEQ_CONCAT_()  SGR__SEQ_CONCAT


/*
 * Macros for user
 */
// SGR (Select Graphic Rendition) begin with CSI (Control Sequence Introducer).
#define     SGR(...)        SGR_BEGIN__ SGR__EVAL128(SGR__SEQ(__VA_ARGS__,)) SGR_END__

/*
 * User can define other SGR macro.
 * For example, if you would like to use italic font, define as below:
 *     #define SGR_ITALIC   "3"
 * now you can use ITALIC in SGR() macro, like SGR(RESET, ITALIC, FG_RED) .
 */
/*
 * For detail, see <https://en.wikipedia.org/wiki/ANSI_escape_code#SGR_(Select_Graphic_Rendition)_parameters>,
 * "SGR (Select Graphic Rendition) parameters" table (2017-12-07).
 */

// SGR_BEGIN__ is CSI.
#define     SGR_BEGIN__         "\033["
#define     SGR_END__           "m"

// foreground (text) colors
#define     SGR_FG_BLACK        "30"
#define     SGR_FG_RED          "31"
#define     SGR_FG_GREEN        "32"
#define     SGR_FG_YELLOW       "33"
#define     SGR_FG_BLUE         "34"
#define     SGR_FG_MAGENTA      "35"
#define     SGR_FG_CYAN         "36"
#define     SGR_FG_WHITE        "37" // is usually actually light gray

#define     SGR_FG_BRIGHT_BLACK        "90" // a.k.a. dark gray
#define     SGR_FG_BRIGHT_RED          "91"
#define     SGR_FG_BRIGHT_GREEN        "92"
#define     SGR_FG_BRIGHT_YELLOW       "93"
#define     SGR_FG_BRIGHT_BLUE         "94"
#define     SGR_FG_BRIGHT_MAGENTA      "95"
#define     SGR_FG_BRIGHT_CYAN         "96"
#define     SGR_FG_BRIGHT_WHITE        "97" // is usually the proper white
// there is no "98"

#define SGR_FG_8BIT(COL) "38;5;" #COL // index into a fixed 256-color palette, usage: SGR(FG_8BIT(5))
#define SGR_FG_24BIT(R, G, B) "38;2;" #R ";" #G ";" #B // set arbitrary color, usage: SGR(FG_24BIT(167, 34, 210))

#define     SGR_FG_DEFAULT      "39"

// background colors
#define     SGR_BG_BLACK        "40"
#define     SGR_BG_RED          "41"
#define     SGR_BG_GREEN        "42"
#define     SGR_BG_YELLOW       "43"
#define     SGR_BG_BLUE         "44"
#define     SGR_BG_MAGENTA      "45"
#define     SGR_BG_CYAN         "46"
#define     SGR_BG_WHITE        "47" // is usually actually light gray

#define     SGR_BG_BRIGHT_BLACK        "100" // a.k.a. dark gray
#define     SGR_BG_BRIGHT_RED          "101"
#define     SGR_BG_BRIGHT_GREEN        "102"
#define     SGR_BG_BRIGHT_YELLOW       "103"
#define     SGR_BG_BRIGHT_BLUE         "104"
#define     SGR_BG_BRIGHT_MAGENTA      "105"
#define     SGR_BG_BRIGHT_CYAN         "106"
#define     SGR_BG_BRIGHT_WHITE        "107" // is usually the proper white

#define SGR_BG_8BIT(COL) "48;5;" #COL // index into a fixed 256-color palette, usage: SGR(BG_8BIT(5))
#define SGR_BG_24BIT(R, G, B) "48;2;" #R ";" #G ";" #B // set arbitrary color, usage: SGR(BG_24BIT(167, 34, 210))

#define     SGR_BG_DEFAULT      "49"

// styling
#define     SGR_RESET           "0" // clears everything to full defaults, including colors
#define     SGR_BOLD            "1"
#define     SGR_FAINT           "2"
#define     SGR_REGULAR         "22" // not bold nor faint
#define     SGR_ITALIC          "3"
#define     SGR_NO_ITALIC       "23"
#define     SGR_UNDERLINE       "4"
#define     SGR_NO_UNDERLINE    "24"
// we don't support "blink" nor "negative" in Qtenv

/*
 * Usage:
 * std::cout << "red->" SGR(FG_RED)
                "and bold->" SGR(BOLD)
                "underline->" SGR(UNDERLINE)
                "reset->" SGR(RESET) "sample end";
 * or:
 * printf("red->" SGR(FG_RED)
          "and bold->" SGR(BOLD)
          "underline->" SGR(UNDERLINE)
          "reset->" SGR(RESET) "sample end");
 */

#endif  // ifndef INCLUDED__LO48576__SGR_MACRO_H_
