package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Comparator;
import java.util.List;
import java.util.StringTokenizer;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.engine.Common;

public class StringUtils extends org.apache.commons.lang.StringUtils {

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
	 * Converts a java id string (camel case and '_' used as word separators)
	 * to a display string (' ' used as word separator).
	 */
	public static String toDisplayString(String javaIdString) {
		return join(splitCamelCaseString(capitalize(javaIdString), '_').iterator(), ' ');
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
     * Formats a NED comment as a one-line doc string. If it is longer than the
     * given max length, it gets truncated. Only the first sentence or the first paragraph
     * is returned.
     */
    public static String makeBriefDocu(String comment, int maxlen) {
        if (comment==null)
            return null;

        // BEWARE: Java's multiline regexp mode "(?m)" seems to be broken:
        // negative sets "[^...]", "\s" etc may consume newlines, so substitutions
        // may change the number of lines in the string! This corresponds to the
        // documentation, but is inconsistent with Perl!

        comment = comment.replaceAll("(?m)^\\s*//#.*$", "");      // remove '//#' lines
        comment = comment.replaceAll("(?m)^\\s*//", "");          // remove "//"'s
        comment = comment.replaceFirst("(?s)\n[ \t]*\n.*", "");   // keep only first paragraph, or:
        comment = comment.replaceFirst("(?s)\\..*", "");          // extract the first sentence only (up to the first period)
        comment = comment.replaceAll("<.*?>","");                 // throw out tags (including end tags)
        comment = comment.replaceAll("(?s)\\s+", " ");            // make it one line, and normalize whitespace
        if (comment.length() > maxlen)
            comment = comment.substring(0, maxlen)+"...";
        return comment.trim();
    }

    /**
     * Formats a NED comment as plain text. Basically, strips "//" from the lines,
     * any removes explicit HTML formatting from the string.
     */
    public static String makeTextDocu(String comment) {
        if (comment == null)
            return null;

        // BEWARE: Java's multiline mode "(?m)" seems to be broken, see above!

        comment = comment.replaceAll("(?m)^[ \t]*//#.*$", "");  // remove '//#' lines
        comment = comment.replaceAll("(?m)^[ \t]*//+ ?", "");   // remove "//"'s
        comment = comment.replaceAll("<.*?>","");             // throw out tags (including end tags)
        comment = comment.replaceAll("(?s)[ \t]+\n","\n");    // remove whitespace from end of lines
        comment = comment.replaceAll("(?s)\n\n\n+","\n\n");   // remove multiple blank lines
        return comment.trim();
    }

    /**
     * Converts documentation string to HTML format, and returns it.
     */
    // TODO <pre> and <nohtml> is not supported
    public static String makeHtmlDocu(String comment) {
        if (comment==null)
            return null;

        // BEWARE: Java's multiline mode "(?m)" seems to be broken, see above!

        // add sentries to facilitate processing
        comment = "\n\n"+comment+"\n\n";

        // remove '//#' lines (those are comments to be ignored by documentation generation)
        comment = comment.replaceAll("(?s)(?<=\n) *//#.*?\n", "");

        // remove '// ', '/// ' and '//////...' from beginning of lines
        comment = comment.replaceAll("(?s)\n[ \t]*//+ ?", "\n");

        comment = comment.trim();

        // TODO extract existing <pre> sections to prevent tampering inside them
        // comment = comment.replaceAll("(?s)&lt;pre&gt;(.*?)&lt;/pre&gt;", "$pre{++$ctr}=$1;"<pre$ctr>"");e;

        // a plain '-------' line outside <pre> is replaced by a divider (<hr> tag)
        comment = comment.replaceAll("(?s)\n[ \t]*------+[ \t]*\n", "\n<hr/>\n");

        // lines outside <pre> containing whitespace only count as blank
        comment = comment.replaceAll("(?s)\n[ \t]+\n", "\n\n");

        // insert blank line (for later processing) in front of lines beginning with '- ' or '-# '
        comment = comment.replaceAll("(?s)\n( *-#? )", "\n\n$1");

        // format @author, @date, @todo, @bug, @see, @since, @warning, @version
        comment = comment.replaceAll("(?s)\\@author\\b", "\n\n<b>Author:</b>");
        comment = comment.replaceAll("(?s)\\@date\\b", "\n\n<b>Date:</b>");
        comment = comment.replaceAll("(?s)\\@todo\\b", "\n\n<b>TODO:</b>");
        comment = comment.replaceAll("(?s)\\@bug\\b", "\n\n<b>BUG:</b>");
        comment = comment.replaceAll("(?s)\\@see\\b", "\n\n<b>See also:</b>");
        comment = comment.replaceAll("(?s)\\@since\\b", "\n\n<b>Since:</b>");
        comment = comment.replaceAll("(?s)\\@warning\\b", "\n\n<b>WARNING:</b>");
        comment = comment.replaceAll("(?s)\\@version\\b", "\n\n<b>Version:</b>");
        comment = comment.replaceAll("(?s)\n\n\n+", "\n\n");

        // wrap paragraphs NOT beginning with '-' into <p></p>.
        // well, we should write "paragraphs not beginning with '- ' or '-# '", but
        // how do you say that in a Perl regex?
        // (note: (?=...) and (?<=...) constructs are lookahead and lookbehind assertions,
        // see e.g. http://tlc.perlarchive.com/articles/perl/pm0001_perlretut.shtml).
        // FIXME
        comment = comment.replaceAll("(?s)(?<=\n\n)[ \t]*([^- \t\n].*?)(?=\n\n)", "<p>$1</p>");

        // wrap paragraphs beginning with '-' into <li></li> and <ul></ul>
        // every 3 spaces increase indent level by one.
        comment = comment.replaceAll("(?s)(?<=\n\n)          *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><ul><ul><li>$1</li></ul></ul></ul></ul>");;
        comment = comment.replaceAll("(?s)(?<=\n\n)       *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><ul><li>$1</li></ul></ul></ul>");;
        comment = comment.replaceAll("(?s)(?<=\n\n)    *-[ \t]+(.*?)(?=\n\n)", "  <ul><ul><li>$1</li></ul></ul>");;
        comment = comment.replaceAll("(?s)(?<=\n\n) *-[ \t]+(.*?)(?=\n\n)", "  <ul><li>$1</li></ul>");;
        for (int i=0; i<4; i++) {
             comment = comment.replaceAll("(?s)</ul>[ \t\n]*<ul>", "\n\n  ");;
        }

        // wrap paragraphs beginning with '-#' into <li></li> and <ol></ol>.
        // every 3 spaces increase indent level by one.
        comment = comment.replaceAll("(?s)(?<=\n\n)          *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><ol><ol><li>$1</li></ol></ol></ol></ol>");;
        comment = comment.replaceAll("(?s)(?<=\n\n)       *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><ol><li>$1</li></ol></ol></ol>");;
        comment = comment.replaceAll("(?s)(?<=\n\n)    *-#[ \t]+(.*?)(?=\n\n)", "  <ol><ol><li>$1</li></ol></ol>");;
        comment = comment.replaceAll("(?s)(?<=\n\n) *-#[ \t]+(.*?)(?=\n\n)", "  <ol><li>$1</li></ol>");;
        for (int i=0; i<4; i++) {
             comment = comment.replaceAll("(?s)</ol>[ \t\n]*<ol>", "\n\n  ");;
        }

        // TODO now we can put back <pre> regions
        // comment = comment.replaceAll("(?s)<pre(\d+)>", "'<pre>'.$pre{$1}.'</pre>'");e;

        // now we can trim excess blank lines
        comment = comment.replaceAll("^\n+", "");
        comment = comment.replaceAll("\n+$", "\n");

        // restore " from &quot; (important for attrs of html tags, see below)
        comment = comment.replaceAll("(?s)(?i)&quot;", "\"");

        // TODO extract <nohtml> sections to prevent substituting inside them;
        // also backslashed words to prevent putting hyperlinks on them
        // comment = comment.replaceAll("(?s)&lt;nohtml&gt;(.*?)&lt;/nohtml&gt;", "$nohtml{++$ctr}=$1;"<nohtml$ctr>"");ei;
        // comment = comment.replaceAll("(?s)(\\[a-z_]+)", "$nohtml{++$ctr}=$1;"<nohtml$ctr>"");ei;

        // decode certain HTML tags: <i>,<b>,<br>,...
        String tags="a|b|body|br|center|caption|code|dd|dfn|dl|dt|em|font|form|hr|h1|h2|h3|i|input|img|li|meta|multicol|ol|p|small|span|strong|sub|sup|table|td|th|tr|tt|kbd|u|ul|var";
        comment = comment.replaceAll("(?s)(?i)<(("+tags+")( [^\n]*?)?)>","<$1>");
        comment = comment.replaceAll("(?s)(?i)<(/("+tags+"))>", "<$1>");

        // TODO put back <nohtml> sections and backslashed words
        // comment = comment.replaceAll("(?s)\<nohtml(\d+)\>", "$nohtml{$1}");e;

        // remove backslashes; double backslashes become single ones
        comment = comment.replaceAll("(?s)\\\\(.)", "$1");;

    return comment;
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
     *   formatCounted(0, "foo") = "no foo"
     *   formatCounted(1, "foo") = "1 foo"
     *   formatCounted(2, "foo") = "2 foos"
     */
    public static String formatCounted(int count, String noun) {
    	Assert.isLegal(count >= 0);
    	if (count == 0)
    		return "no " + noun;
    	else if (count == 1)
    		return "1 " + noun;
    	else
    		return String.valueOf(count) + " " + plural(noun);
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
    	if (lastChar == 's')
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
    public static String indefiniteArticle(String noun) {
    	return startsWithVowel(noun) ? "an" : "a";
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
    	
    	return Common.strdictcmp(first, second);
    }
    
    /**
     * Comparator for sorting lists with {@link dictionaryCompare()}.
     */
    public static final Comparator<String> dictionaryComparator = new DictionaryComparator();
    
    private static final class DictionaryComparator implements Comparator<String> {
		public int compare(String first, String second) {
			return dictionaryCompare(first, second);
		}
    }
}
