/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.util;

import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.Stack;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.lang3.ArrayUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.core.variables.VariablesPlugin;
import org.omnetpp.common.engine.Common;

public class StringUtils extends org.apache.commons.lang3.StringUtils {

    static {
        // run unit test
        testToInstanceName();
    }

    /**
     * For null it returns "", otherwise it returns the passed string itself.
     *
     * (Note: StringUtils.defaultString(string) does the same, but its name
     * is not too intuitive.)
     */
    public static String nullToEmpty(String str) {
        return str == null ? EMPTY : str;
    }

    /**
     * For null it returns the given fallback string, otherwise it returns the passed string itself.
     */
    public static String fallback(String str, String valueIfNull) {
        return str == null ? valueIfNull : str;
    }

    /**
     * Converts a java id string (camel case and '_' used as word separators)
     * to a display string (' ' used as word separator).
     */
    public static String toDisplayString(String javaIdString) {
        return join(splitCamelCaseString(capitalize(javaIdString), '_').iterator(), ' ');
    }

    /**
     * Converts the provided CamelCase type-name to instance name (starting with lower case)
     * RULES: TCP -> tcp; _TCP -> _tcp; TCPModule -> tcpModule; TCP_module -> tcp_module
     * TCP1 -> tcp1; helloWorld -> helloWorld; CamelCase -> camelCase; TCP_SACK -> tcp_sack
     * TCP_Reno -> tcp_Reno
     */
    public static String toInstanceName(String typeName) {
        if (typeName.matches("[A-Z_]+"))                   // TCP -> tcp; _TCP -> _tcp; TCP_SACK -> tcp_sack
            return typeName.toLowerCase();
        else if (typeName.matches("[A-Z_]+[A-Z][a-z].*"))  // TCPModule -> tcpModule
            return uncapitalize(typeName, typeName.replaceFirst("([A-Z_]+)[A-Z][a-z].*", "$1").length());
        else if (typeName.matches("[A-Z]+.*"))             // TCP1 -> tcp1; TCP_Reno -> tcp_Reno
            return uncapitalize(typeName, typeName.replaceFirst("([A-Z]+).*", "$1").length());
        else
            return uncapitalize(typeName); // helloWorld -> helloWorld; CamelCase -> camelCase;
    }

    private static void testToInstanceName() {
        Assert.isTrue(toInstanceName("TCPModule").equals("tcpModule"));
        Assert.isTrue(toInstanceName("TCP_Reno").equals("tcp_Reno"));
        Assert.isTrue(toInstanceName("TCP1").equals("tcp1"));
        Assert.isTrue(toInstanceName("DerivedBaseVecWidthSize").equals("derivedBaseVecWidthSize"));
        Assert.isTrue(toInstanceName("AServer").equals("aServer"));
        Assert.isTrue(toInstanceName("AHost").equals("aHost"));
        Assert.isTrue(toInstanceName("ALOHAnet").equals("alohAnet"));
        Assert.isTrue(toInstanceName("CamelCase").equals("camelCase"));
        Assert.isTrue(toInstanceName("TCP_SACK").equals("tcp_sack"));
        Assert.isTrue(toInstanceName("_TCP").equals("_tcp"));
    }

    /**
     * Tweaks the given string so that it becomes a valid C++ or Java identifier.
     * Removes spaces, assures it begins with letter or underscore, etc.
     */
    public static String makeValidIdentifier(String name) {
        name = name.replaceAll("\\s", "");
        name = name.replaceAll("[^A-Za-z0-9_]", "_");
        name = name.replaceAll("__+", "_");
        if (!name.matches("[a-zA-Z_].*"))
            name = "_" + name;
        return name;
    }

    /**
     * Make the first noOfChars lower-case
     */
    public static String uncapitalize(String value, int noOfChars) {
        if (value.length() < noOfChars)
            noOfChars = value.length();
        return value.substring(0,noOfChars).toLowerCase()+value.substring(noOfChars);

    }

    /**
     * Splits up text into lines which including the new line character.
     * The concatenation of lines must be equal to text.
     */
    public static String[] splitToLines(String text) {
        ArrayList<String> result = new ArrayList<String>();
        int beginningOfLine = 0;
        int endOfLine = text.indexOf('\n');

        while (endOfLine != -1) {
            int beginningOfNextLine = endOfLine + 1;
            result.add(text.substring(beginningOfLine, beginningOfNextLine));

            beginningOfLine = beginningOfNextLine;
            endOfLine = text.indexOf('\n', beginningOfLine);
        }

        if (beginningOfLine != text.length() || text.length() == 0)
            result.add(text.substring(beginningOfLine, text.length()));

        return result.toArray(new String[0]);
    }


    /**
     * Return a line range from the text, from start (inclusive) to end (exclusive).
     * Line numbers are 0-based.
     */
    public static String getLines(String text, int start, int end) {
        StringBuffer result = new StringBuffer();
        String[] lines = splitToLines(text);

        for (int i = start; i < end; i++)
            result.append(lines[i]);

        return result.toString();
    }

    public static List<String> splitCamelCaseString(String str, char separator)
    {
        List<String> result = new ArrayList<String>();
        StringBuffer currentWord = new StringBuffer();

        int length = str.length();
        boolean lastIsLower = false;

        for (int index = 0; index < length; index++)
        {
            char curChar = str.charAt(index);
            if (Character.isUpperCase(curChar) || !lastIsLower && Character.isDigit(curChar) || curChar == separator)
            {
                if (lastIsLower || curChar == separator)
                {
                    result.add(currentWord.toString());
                    currentWord = new StringBuffer();
                }
                lastIsLower = false;
            }
            else
            {
                if (!lastIsLower)
                {
                    int currentWordLength = currentWord.length();
                    if (currentWordLength > 1)
                    {
                        char lastChar = currentWord.charAt(--currentWordLength);
                        currentWord.setLength(currentWordLength);
                        result.add(currentWord.toString());
                        currentWord = new StringBuffer();
                        currentWord.append(lastChar);
                    }
                }
                lastIsLower = true;
            }
            if (curChar != separator)
            {
                currentWord.append(curChar);
            }
        }

        result.add(currentWord.toString());
        return result;
    }

    /**
     * Inserts newlines into the string, performing rudimentary
     * line breaking. Good enough for long tooltip texts etc.
     */
    public static String breakLines(String text, int maxLineLength) {
        StringBuilder buf = new StringBuilder();
        StringTokenizer st = new StringTokenizer(text, " \t\n", true);
        int lineLength = 0;
        while (st.hasMoreTokens()) {
            String token = st.nextToken();
            if (token.contains("\n") || lineLength + token.length() > maxLineLength) {
                buf.append("\n"); //TODO should probably chop off trailing spaces of buf[] first
                token = token.replaceFirst("^[ \t]*\n?", "");
                buf.append(token);
                token = token.replaceFirst("^.*\n", "");
                lineLength = token.length();
            }
            else {
                buf.append(token);
                lineLength += token.length();
            }
        }
        return buf.toString();
    }

    /**
     * Returns the number of newline characters in the text
     */
    public static int getMaximumLineLength(String text) {
        int max = 0;
        for (String line : splitToLines(text))
            max = Math.max(max, line.length());
        return max;
    }

    /**
     * Returns the number of newline characters in the text
     */
    public static int countNewLines(String text) {
        int newlineCount = 0;
        int pos = text.indexOf('\n');
        while (pos != -1) {
            newlineCount++;
            pos = text.indexOf('\n', pos+1);
        }
        return newlineCount;
    }

    /**
     * Interprets the text as multi-line string, and discards all leading lines
     * that start with (whitespace plus) the given comment marker.
     */
    public static String stripLeadingCommentLines(String text, String commentMarker) {
        StringBuilder result = new StringBuilder();
        boolean atStart = true;
        for (String line : splitToLines(text)) {
            if (!atStart || !line.trim().startsWith(commentMarker)) {
                result.append(line);
                atStart = false;
            }
        }
        return result.toString();
    }

    /**
     * Returns true if string is null or "".
     */
    public static boolean isEmpty(String string) {
        return string==null || "".equals(string);
    }

    /**
     * Returns true if string parses to an integer.
     */
    public static boolean isInteger(String string) {
        try {
            Long.parseLong(string);
            return true;
        } catch (NumberFormatException ex) {
            return false;
        }
    }

    /**
     * Returns true if string parses to a double.
     */
    public static boolean isDouble(String string) {
        try {
            Double.parseDouble(string);
            return true;
        } catch (NumberFormatException ex) {
            return false;
        }
    }

    private static final Pattern QUANTITY_PATTERN = Pattern.compile("-?(\\d*\\.?\\d+([eE][+-]?\\d+)?\\s*[a-zA-Z]+\\s*)+");

    /**
     * Returns true if string syntactically looks like a quantity literal
     * (allowing unit mismatches).
     */
    public static boolean isQuantityLike(String string) {
        return QUANTITY_PATTERN.matcher(string).matches();
    }

    /**
     * Interface for replaceMatches().
     */
    public static interface IRegexpReplacementProvider {
        /**
         * Returns the replacement string of the currently matched input or
         * {@code null} if the matching input should be left as is.
         */
        public String getReplacement(Matcher matcher);
    }

    /**
     * Performs regex substitution on the given string, where replacements are
     * provided by the given IRegexpReplacementProvider instance.
     */
    public static String replaceMatches(String text, String regexp, IRegexpReplacementProvider provider) {
        return replaceMatches(text, Pattern.compile(regexp), provider);
    }

    /**
     * Performs regex substitution on the given string, where replacements are
     * provided by the given IRegexpReplacementProvider instance.
     */
    public static String replaceMatches(String text, Pattern regexp, IRegexpReplacementProvider provider) {
        Matcher matcher = regexp.matcher(text);
        StringBuffer buffer = new StringBuffer();

        while (matcher.find()) {
            String replacement = provider.getReplacement(matcher);
            if (replacement != null)
                matcher.appendReplacement(buffer, replacement.replace("\\", "\\\\").replace("$", "\\$"));
        }

        matcher.appendTail(buffer);

        return buffer.toString();
    }

    /**
     * Prepares plain text for inclusion into HTML "pre" tag: replaces "<", ">"
     * with "&lt;", "&gt;", etc.
     */
    public static String quoteForHtml(String text) {
        return text.replace("<", "&lt;").replace(">", "&gt;");
    }

    /**
     * Returns the <numeral> <noun> clause.
     * Examples:
     *   formatCounted(0, "foo") = "0 foo"
     *   formatCounted(1, "foo") = "1 foo"
     *   formatCounted(2, "foo") = "2 foos"
     */
    public static String formatCounted(int count, String noun) {
        Assert.isLegal(count >= 0);
        if (count == 0)
            return "0 " + noun;
        else if (count == 1)
            return "1 " + noun;
        else if (count < 1000)
            return String.valueOf(count) + " " + plural(noun);
        else {
            NumberFormat format = NumberFormat.getIntegerInstance();
            format.setGroupingUsed(true);
            return format.format(count) + " " + plural(noun);
        }
    }

    /**
     * Returns the plural of an English <code>noun</code> (approximately).
     */
    public static String plural(String noun) {
        if (isEmpty(noun))
            return noun;
        int len = noun.length();
        char lastChar = noun.charAt(len - 1);
        if (lastChar == 'y')
            return noun.substring(0, len-1) + "ies";
        if (lastChar == 's' || (lastChar == 'h' && !noun.endsWith("th")))
            return noun + "es";
        else
            return noun + "s";
    }

    /**
     * Useful for choosing indefinite article ("a" or "an") for a noun.
     */
    public static boolean startsWithVowel(String word) {
        final String vowels = "aeiou";
        return vowels.contains(word.substring(0,1).toLowerCase());
    }

    /**
     * Useful for choosing indefinite article ("a" or "an") for a noun.
     */
    public static String indefiniteArticle(String noun, boolean capitalized) {
        if (capitalized)
            return startsWithVowel(noun) ? "An" : "A";
        else
            return startsWithVowel(noun) ? "an" : "a";
    }

    /**
     * Prefixes the noun with an indefinite article ("a" or "an").
     */
    public static String addIndefiniteArticle(String noun, boolean capitalized) {
        return indefiniteArticle(noun, capitalized) + " " + noun;
    }

    /**
     * Returns "1st", "2nd", "3rd" etc.
     */
    public static String ordinal(int k) {
        String suffix;
        switch (k%10) {
            case 1: suffix = "st"; break;
            case 2: suffix = "nd"; break;
            case 3: suffix = "rd"; break;
            default: suffix = "th"; break;
        }
        return k + suffix;
    }

    /**
     * Joins the elements of a {@code collection} separated by {@code separator} and
     * {@code lastSeparator}.
     * Example:
     *   join(["foo","bar","baz"], ", ", " and ") = "foo, bar and baz"
     */
    public static String join(Collection<String> collection, String separator, String lastSeparator) {
        int size = collection.size();
        StringBuffer sb = new StringBuffer();
        int i = 0;
        for (String item : collection) {
            if (i > 0)
                sb.append(i == size-1 ? lastSeparator : separator);
            sb.append(item);
            ++i;
        }
        return sb.toString();
    }

    /**
     * Splits {@code string} to consecutive substrings each having at
     * most {@code maxLength} length.
     *
     * @param string the string to be split
     * @param maxLength the maximum length of the returned strings
     * @return the array of substrings or {@code null} if {@code string} is {@code null}
     */
    public static String[] split(String string, int maxLength) {
        if (string == null)
            return null;

        int len = string.length();
        if (len <= maxLength)
            return new String[] {string};

        int count = ((len + maxLength - 1) / maxLength);
        String[] result = new String[count];
        for (int i = 0, start = 0; i < count; ++i, start += maxLength) {
            int end = Math.min(start+maxLength, len);
            result[i] = string.substring(start, end);
        }
        return result;
    }

    /**
     * Dictionary-compare two strings, the main difference from String.compare()
     * being that integers embedded in the strings are compared in
     * numerical order. {@code null} values are ordered after other values.
     */
    public static int dictionaryCompare(String first, String second) {
        if (first == second)
            return 0;
        if (first == null)
            return 1;
        if (second == null)
            return -1;

        return Common.opp_strdictcmp(first, second);
    }

    /**
     * Comparator for sorting lists with {@link dictionaryCompare()}.
     */
    public static final Comparator<String> dictionaryComparator = new DictionaryComparator();

    public static final class DictionaryComparator implements Comparator<String> {
        public int compare(String first, String second) {
            return dictionaryCompare(first, second);
        }
    }

    /**
     * Tests whether two strings are equal after aggressive whitespace normalization.
     * @see normalizeWhiteSpace()
     */
    public static boolean areEqualIgnoringWhiteSpace(String text1, String text2) {
        return normalizeWhiteSpace(text1).equals(normalizeWhiteSpace(text2));
    }

    public static String normalizeWhiteSpace(String text) {
        // zap indentation, end-line whitespaces and blank lines: any whitespace
        // sequence containing "\n" becomes a single "\n"
        text = text.replaceAll("(?s)\\s*\\n\\s*", "\n");
        // replace remaining whitespace sequences with a space each
        text = text.replaceAll("(?s)[ \t\f]+", " ");
        // throw out leading/trailing whitespace altogether
        text = text.replaceAll("(?s)^\\s*", "");
        text = text.replaceAll("(?s)\\s*$", "");
        return text;
    }

    public static String indentLines(String text, String indent) {
        return indent + StringUtils.removeEnd(text.replace("\n", "\n" + indent), indent);
    }

    /**
     * Surround the given string with "quotes", also escape with backslash
     * where needed.
     */
    public static String quoteString(String text) {
        return Common.quoteString(text);
    }

    /**
     * Returns true if the string contains space, backslash, quote, or anything
     * else that would make quoting ({@link #quoteString(String)}) necessary before writing
     * it into a data file.
     */
    public static boolean needsQuotes(String text) {
        return Common.needsQuotes(text);
    }

    /**
     * Returns true if string is a quoted string, e.g. "\"Hello world\"".
     */
    public static boolean isQuotedString(String string) {
        try {
            Common.parseQuotedString(string);
            return true;
        } catch (RuntimeException ex) {
            return false;
        }
    }

    /**
     * Combines {{@link #needsQuotes(String)} and {{@link #quoteString(String)}.
     */
    public static String quoteStringIfNeeded(String text) {
        return Common.quoteStringIfNeeded(text);
    }

    /**
     * Find the end of a quoted string constant, obeying backslashed escapes.
     * The character at startPos specifies which character is used as
     * quotation mark. Returns the position of the matching quote character, or
     * throws an exception if the string ended without finding one.
     */
    public static int findCloseQuote(String txt, int startPos) {
        char quot = txt.charAt(startPos);
        int len = txt.length();
        int pos = startPos + 1;
        for (; pos < len && txt.charAt(pos) != quot; pos++)
            if (txt.charAt(pos) == '\\' && pos+1 < len)
                pos++;
        if (pos == len)
            throw new ParseException("Unterminated string constant");
        return pos;
    }

    public static int findCloseParen(String txt, int startPos) {
        return findCloseParen(txt, startPos, "(){}[]".toCharArray(), "'\"".toCharArray());
    }

    /**
     * Find the matching close paren in a string, observing nesting and quoted
     * string constants too. The character at startPos must be the open
     * parenthesis. Returns the position of the matching close paren. Exception
     * is thrown if it is not found, or nesting is broken.
     */
    public static int findCloseParen(String txt, int startPos, char[] parenChars, char[] quoteChars) {
        int pi  = ArrayUtils.indexOf(parenChars, txt.charAt(startPos));
        if (pi == -1 || pi % 2 != 0)
            throw new ParseException("Not at an open parenthesis");
        char closeParen = parenChars[pi+1];

        int pos = startPos;
        Stack<Character> parenStack = new Stack<>();
        while (pos < txt.length()) {
            char ch = txt.charAt(pos);

            if (ArrayUtils.contains(quoteChars, ch)) { // skip string constant
                pos = StringUtils.findCloseQuote(txt, pos);
            }
            else if ((pi = ArrayUtils.indexOf(parenChars, ch)) != -1) {  // paren
                if (pi % 2 == 0)  // open
                    parenStack.push(ch);
                else {  // close
                    if (parenStack.isEmpty())
                        throw new ParseException("Unmatched close parenthesis '" + ch + "'");
                    if (parenStack.peek() != parenChars[pi-1])
                        throw new ParseException("Mismatched parentheses '" + parenStack.peek() + "' and '" + ch + "'");
                    parenStack.pop();
                }
            }

            if (ch == closeParen && parenStack.isEmpty()) // close paren reached
                return pos;

            pos++;
        }

        throw new ParseException("Unclosed parenthesis '" + parenStack.peek() + "'");
    }

    /**
     * Joins two strings with a separator. Any of s1 and s2 can be null.
     */
    public static String joinWithSeparator(String s1, String separator, String s2) { //  Note: purposely not called "join()", to disambiguate from join() variants in Apache StringUtils.
        if (isEmpty(s1))
            return nullToEmpty(s2);
        if (isEmpty(s2))
            return nullToEmpty(s1);
        return s1+separator+s2;
    }

    /**
     * Performs template substitution. Constructs understood are:
     *  - {foo} gets replaced by (String)m.get("foo");
     *  - {bar?some text} gets replaced by "some text" if ((Boolean)m.get("bar"))==true*
     *  - {~bar?some text} gets replaced by "some text" if ((Boolean)m.get("bar"))==false*
     *  - {bar:} only keep the rest of the line if ((Boolean)m.get("bar"))==true*
     *  - {~bar:} only keep the rest of the line if ((Boolean)m.get("bar"))==false*
     *  - {@i1:list1,i2:list2,...} ... {i} ...{/@}  parallel iteration list1, list2 etc.
     *  *true/false is interpreted as: for a String, "" is false and everything else is true;
     *  instances of Boolean are also accepted.
     *
     * Newlines inside {...} are not permitted; this allows detecting errors caused
     * by misplaced braces. Also, nesting is not supported.
     *
     * Newlines in the template should be represented by "\n" and not "\r\n", so you
     * may need to preprocess the template by calling: template = template.replace("\r\n", "\n").
     *
     * Lines starting with ### treated as comments and will be removed from the output
     */
    public static String substituteIntoTemplate(String template, Map<String, Object> map) {
        return substituteIntoTemplate(template, map, "{", "}");
    }

    /**
     * Like the other substituteIntoTemplate() method, but arbitrary start/end tag can be specified.
     */
    public static String substituteIntoTemplate(String template, Map<String, Object> map, String startTag, String endTag) {
        Assert.isTrue(!startTag.equals(endTag), "template: startTag and endTag must be different");
        StringBuilder buf = new StringBuilder();

        template = template.replaceAll("\n[ \t]*###.*\n", "\n"); // remove whole-line comments
        template = template.replaceAll("[ \t]*###.*\n", "\n"); // remove end-line comments

        int startTagLen = startTag.length();
        int endTagLen = endTag.length();

        int current = 0;
        while (true) {
            int start = template.indexOf(startTag, current);
            if (start == -1)
                break;
            else {
                int end = template.indexOf(endTag, start);
                if (end != -1) {
                    end += endTagLen;
                    String tag = template.substring(start, end);
                    //Debug.println("processing " + tag);
                    String key = template.substring(start+startTagLen, end - endTagLen);
                    if (key.indexOf('\n') != -1)
                        throw new RuntimeException("template error: newline inside " + quoteString(tag) + " (misplaced start/end tag?)");
                    boolean isLoop = key.charAt(0) == '@';
                    if (isLoop)
                        key = key.substring(1);
                    boolean isNegated = key.charAt(0)=='~';
                    if (isNegated)
                        key = key.substring(1);  // drop "~"
                    boolean isOptLine = key.endsWith(":") && key.indexOf('?') == -1;
                    if (isOptLine)
                        key = key.substring(0, key.length()-1);  // drop trailing ":"
                    int questionmarkPos = key.indexOf('?');
                    String substringAfterQuestionmark = questionmarkPos == -1 ? null : key.substring(questionmarkPos+1);
                    if (questionmarkPos != -1)
                        key = key.substring(0, questionmarkPos); // drop "?..." from key

                    // determine replacement string, and possibly adjust start/end
                    String replacement = "";
                    if (isLoop) {
                        // basic loop syntax: {@i:list1,j:list2,...} ... {i} ... {/@}
                        // this is parallel iteration, not nested loops!
                        // first, find loop close tag {/@}
                        String loopEndTag = startTag + "/@" + endTag;
                        int balance = 1;
                        int pos = end-1; // because we'll start with +1
                        while (balance != 0) {
                            pos = indexOfEither(template, startTag+"@", loopEndTag, pos+1);
                            if (pos == -1)
                                throw new RuntimeException("template error: missing loop end marker " + loopEndTag);
                            boolean isStartTag = template.charAt(pos+startTagLen)=='@';
                            balance += isStartTag ? 1 : -1;
                        }
                        int loopEndPos = pos;
                        String loopBody = template.substring(end, loopEndPos);
                        end = loopEndPos + loopEndTag.length();
                        // parse loop spec: "i:list1,j:list2,..."
                        List<String> loopVars = new ArrayList<String>();
                        List<String> loopLists = new ArrayList<String>();
                        for (String loopSpec : key.split(",")) {
                            if (!loopSpec.matches(" *[a-zA-Z0-9_]+ *: *[a-zA-Z0-9_]+ *"))
                                throw new RuntimeException("template error: syntax error in loop tag " + tag + ", " + startTag + "@var1:list1,var2:list2,..." + endTag + " expected in body");
                            loopVars.add(StringUtils.substringBefore(loopSpec, ":").trim());
                            loopLists.add(StringUtils.substringAfter(loopSpec, ":").trim());
                        }
                        // execute loop: iterate in parallel on all loop variables
                        int length = getFromMapAsList(map, loopLists.get(0)).size();
                        for (int i=0; i<length; i++) {
                            for (int j=0; j<loopVars.size(); j++) {
                                String loopVarJ = loopVars.get(j);
                                List<String> loopListJ = getFromMapAsList(map, loopLists.get(j));
                                if (loopListJ.size() != length)
                                    throw new RuntimeException("template error: list lengths differ in " + tag);
                                map.put(loopVarJ, loopListJ.get(i));
                            }
                            replacement += substituteIntoTemplate(loopBody, map, startTag, endTag);
                        }
                        // remove loop variables
                        for (int j=0; j<loopVars.size(); j++)
                            map.remove(loopVars.get(j));
                    }
                    else if (isOptLine) {
                        // replacing a whole line
                        if (getFromMapAsBool(map, key) != isNegated) {
                            // put line in: all variables OK
                        }
                        else {
                            // omit line
                            int endLine = template.indexOf('\n', end);
                            if (endLine == -1)
                                endLine = template.length();
                            replacement = "";
                            end = endLine + 1;
                        }
                    }
                    else if (questionmarkPos != -1) {
                        // conditional
                        replacement = getFromMapAsBool(map, key)!=isNegated ? substringAfterQuestionmark : "";
                    }
                    else {
                        // plain replacement
                        if (isNegated)
                            throw new RuntimeException("template error: wrong syntax " + quoteString(tag) + " (possible missing \"?\")");
                        replacement = getFromMapAsString(map, key);
                    }

                    // do it: replace substring(start, end) with replacement, unless replacement==null
                    buf.append(template.substring(current, start));  // template code up to the {...}
                    buf.append(replacement);
                    current = end;
                }
            }
        }
        buf.append(template.substring(current));  // rest of the template
        String result = buf.toString();
        result = result.replaceAll(" +\n", "\n");  // remove spaces at line end
        result = result.replaceAll("\n\n\n+", "\n\n");  // remove multiple empty lines
        return result;
    }

    // for substituteIntoTemplate()
    private static int indexOfEither(String template, String substring1, String substring2, int from) {
        int index1 = template.indexOf(substring1, from);
        int index2 = template.indexOf(substring2, from);
        if (index1 == -1)
            return index2;
        if (index2 == -1)
            return index1;
        return index1 < index2 ? index1 : index2;
    }

    // for substituteIntoTemplate()
    private static String getFromMapAsString(Map<String, Object> map, String key) {
        Object object = map.get(key);
        if (object == null)
            throw new IllegalArgumentException("template error: undefined (or null) template parameter '" + key + "'");
        if (!(object instanceof String))
            throw new IllegalArgumentException("template error: template parameter '" + key + "' was expected to be a string, but it is " + object.getClass().toString());
        return (String)object;
    }

    // for substituteIntoTemplate()
    @SuppressWarnings("rawtypes")
    private static boolean getFromMapAsBool(Map<String, Object> map, String key) {
        Object object = map.get(key);
        if (object == null)
            throw new IllegalArgumentException("template error: undefined (or null) template parameter '" + key + "'");
        if (object instanceof Boolean)
            return (Boolean)object;
        else if (object instanceof String)
            return ((String)object).length() != 0;
        else if (object instanceof List)
            return !((List)object).isEmpty();
        else
            throw new IllegalArgumentException("template error: template parameter '" + key + "' was expected to be a string or boolean, but it is " + object.getClass().toString());
    }

    @SuppressWarnings("unchecked")
    private static List<String> getFromMapAsList(Map<String, Object> map, String key) {
        Object object = map.get(key);
        if (object == null)
            throw new IllegalArgumentException("template error: undefined (or null) template parameter '" + key + "'");
        if (object instanceof List)
            return (List<String>)object;
        else
            throw new IllegalArgumentException("template error: template parameter '" + key + "' was expected to be list, but it is " + object.getClass().toString());
    }

    public static String formatList(Collection<? extends Object> list, String elementFormat, String separator) {
        StringBuilder builder = new StringBuilder();
        boolean firstIteration = true;
        for (Object object : list) {
            if (!firstIteration)
                builder.append(separator);
            builder.append(String.format(elementFormat, object));
            firstIteration = false;
        }
        return builder.toString();
    }

    /**
     * Performs Eclipse variable substitution on the string. Unresolved macros and other errors cause CoreException.
     */
    public static String substituteVariables(String string) throws CoreException {
        return VariablesPlugin.getDefault().getStringVariableManager().performStringSubstitution(string, true);
    }

    /**
     * The returned list always begins with the text before the first separator
     * (may be empty string), then goes on with (separator, text) pairs until it
     * reaches the end of the text. The length of the output is always an odd number,
     * and the returned list of strings joined together should always exactly reproduce
     * the original input.
     */
    public static List<String> splitPreservingSeparators(String string, Pattern separator) {
        List<String> result = new ArrayList<String>();
        Matcher matcher = separator.matcher(string);
        int lastEnd = 0;
        while (matcher.find()) {
            result.add(string.substring(lastEnd, matcher.start()));
            result.add(string.substring(matcher.start(), matcher.end()));
            lastEnd = matcher.end();
        }
        result.add(string.substring(lastEnd));
        return result;
    }

    /**
     * Return the substring of {@code str} starting at the given position,
     * either to the end of the string or maximum {@code len} characters.
     * @throws IllegalArgumentException if {@code startIndex} is negative or out-of-bounds
     * @throws IllegalArgumentException if {@code len} is negative
     */
    public static String substring(String str, int startIndex, int len) {
        int strLen = str.length();
        if (startIndex < 0 || startIndex > strLen)
            throw new IllegalArgumentException("substring(): index out of bounds");
        if (len < 0)
            throw new IllegalArgumentException("substring(): length is negative");
        return str.substring(startIndex, Math.min(startIndex + len, strLen));
    }

    /**
     * Returns the last {@code len} character of {@code str},
     * or the full {@code str} if it is shorter than {@code len} characters.
     * @throws IllegalArgumentException if {@code len} is negative
     */
    public static String tail(String str, int len) {
        int strLen = str.length();
        if (len < 0)
            throw new IllegalArgumentException("tail(): length is negative");
        return len >= strLen ? str : str.substring(strLen - len);
    }

    /**
     * Interprets {@code str} as a space-separated list, and returns the item at the given index.
     * @throws IllegalArgumentException if {@code index} is negative or out-of-bounds
     */
    public static String choose(int index, String str) {
        if (index < 0)
            throw new IllegalArgumentException("choose(): negative index");
        StringTokenizer tokenizer = new StringTokenizer(str);
        for (int i=0; i<index && tokenizer.hasMoreTokens(); i++)
            tokenizer.nextToken();
        if (!tokenizer.hasMoreTokens())
            throw new IllegalArgumentException("choose(): index out of bounds: " + index);
        return tokenizer.nextToken();
    }

    /**
     * Replaces all occurrences of {@code substr} in {@code str} with the string {@code repl}.
     * Search begins from position {@code startIndex} in {@code str}.
     * @throws IllegalArgumentException if {@code startIndex} is negative or out-of-bounds
     */
    public static String replace(String str, String substr, String repl, int startIndex) {
        if (startIndex < 0)
            throw new IllegalArgumentException("replace(): start index is negative");
        if (startIndex > str.length())
            throw new IllegalArgumentException("replace(): start index out of bounds");

        int substrLen = substr.length();
        int replLen = repl.length();
        int index = startIndex;
        StringBuilder sb = new StringBuilder(str);
        while ((index = sb.indexOf(substr, index)) != -1) {
            sb.replace(index, index+substrLen, repl);
            index += replLen - substrLen + 1;
        }
        return sb.toString();
    }

    /**
     * Replaces the first occurrence of {@code substr} in {@code str} with the string {@code repl}.
     * Search begins from position {@code startIndex} in {@code str}.
     * @throws IllegalArgumentException if {@code startIndex} is negative or out-of-bounds
     */
    public static String replaceFirst(String str, String substr, String repl, int startIndex) {
        if (startIndex < 0)
            throw new IllegalArgumentException("replace(): start index is negative");
        if (startIndex > str.length())
            throw new IllegalArgumentException("replace(): start index out of bounds");

        int index = str.indexOf(substr, startIndex);
        if (index != -1) {
            StringBuilder sb = new StringBuilder(str);
            sb.replace(index, index+substr.length(), repl);
            return sb.toString();
        }
        else
            return str;
    }

    /**
     * Escape to bash (shell) script. Converts $ to $$ and ( and ) to \( and \) .
     */
    public static String escapeBash(String str) {
        return str.replace("$", "$$").replace("(", "\\(").replace(")", "\\)");
    }

    public static String removeOptionalPrefix(String str, String prefix) {
        if (str.startsWith(prefix))
            str = str.substring(prefix.length());
        return str;
    }

}
