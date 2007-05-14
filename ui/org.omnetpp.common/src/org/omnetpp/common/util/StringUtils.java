package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.StringTokenizer;

import org.eclipse.core.runtime.Assert;

public class StringUtils extends org.apache.commons.lang.StringUtils {
	
	/**
	 * Converts a java id string (camel case and '_' used as word separators)
	 * to a display string (' ' used as word separator).
	 */
	public static String toDisplayString(String javaIdString) {
		return join(splitCamelCaseString(capitalize(javaIdString), '_').iterator(), ' ');
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
			if (Character.isUpperCase(curChar) || (!lastIsLower && Character.isDigit(curChar)) || curChar == separator)
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
     * 
	 * @param text
	 * @return The number of new line chars in the text
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
	 * Returns true if string is null or "".
	 */
	public static boolean isEmpty(String string) {
		return string==null || "".equals(string);
	}

    /**
     * Formats a NED comment as a one-line doc string. If it is longer than the
     * given max length, it gets truncated.
     */
    public static String makeBriefDocu(String comment, int maxlen) {
        if (comment==null)
            return null;
        comment = comment.replaceAll("(?m)^\\s*//", "").trim(); // remove "//"'s
        comment = comment.replaceFirst("(?s)\n[ \t]*\n.*", "").trim(); // keep only first paragraph
        comment = comment.replaceAll("(?s)\\s+", " "); // make it one line, and normalize whitespace
        if (comment.length() > maxlen)
            comment = comment.substring(0, maxlen)+"...";
        return comment;
    }

    /**
     * Convert it to HTML format 
     * @param comment
     * @return
     */
    public static String makeHtmlDocu(String comment) {
        if (comment==null)
            return null;
        comment = comment.replaceAll("(?m)^\\s*//", "").trim(); // remove "//"'s
        comment = comment.replace("\n", "<br/>");
        return comment;
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
     * Returns the plural of an english <code>noun</code> (approximately). 
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
}
