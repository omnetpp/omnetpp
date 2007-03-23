package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

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
	
	public static String breakLines(String text, int maxLineLength) {
		StringBuilder buf = new StringBuilder();
		StringTokenizer st = new StringTokenizer(text, " \t\n,;-", true);
		int lineLength = 0;
		while (st.hasMoreTokens()) {
			String token = st.nextToken();
			if (lineLength + token.length() > maxLineLength) {
				buf.append("\n");
				lineLength = 0;
				token = token.replaceFirst("^[ \t]+", "");
			}
			buf.append(token);
			lineLength += token.length();
		}
		return buf.toString();
	}

	public static int countNewLines(String text) {
		int newlineCount = 0;
		int pos = text.indexOf('\n');
		while (pos != -1) {
			newlineCount++;
			pos = text.indexOf('\n', pos+1);
		}
		return newlineCount;
	}
}
