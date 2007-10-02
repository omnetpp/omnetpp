package org.omnetpp.inifile.editor.model;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.io.StringReader;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;

/**
 * Parses an ini file. Parse results are passed back via a callback.
 * @author Andras
 */
public class InifileParser {
	/** 
	 * Implement this interface to store ini file contents as it gets parsed. Comments are 
	 * passed back with leading "#" and preceding whitespace, or as "" if there's no comment
	 * on that line. Lines that end in backslash are passed to incompleteLine(), with all 
	 * content assigned to their completing (non-backslash) line.
	 */
	public interface ParserCallback {
		void blankOrCommentLine(int lineNumber, int numLines, String rawLine, String rawComment);
		void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment);
		void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment);
		void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String rawComment);
		void parseError(int lineNumber, int numLines, String message);
	}

	/**
	 * A ParserCallback with all methods defined to be empty.
	 */
	public static class ParserAdapter implements ParserCallback {
		public void blankOrCommentLine(int lineNumber, int numLines, String rawLine, String rawComment) {}
		public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {}
		public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment) {}
		public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String rawComment) {}
		public void parseError(int lineNumber, int numLines, String message) {}
	}

	/**
	 * A ParserCallback for debug purposes.
	 */
	public static class DebugParserAdapter implements ParserCallback {
		public void blankOrCommentLine(int lineNumber, int numLines, String rawLine, String rawComment) {
			System.out.println(lineNumber+": "+rawLine+" --> comment="+rawComment);
		}
		public void sectionHeadingLine(int lineNumber, int numLines, String rawLine, String sectionName, String rawComment) {
			System.out.println(lineNumber+": "+rawLine+" --> section '"+sectionName+"'  comment="+rawComment);
		}
		public void keyValueLine(int lineNumber, int numLines, String rawLine, String key, String value, String rawComment) {
			System.out.println(lineNumber+": "+rawLine+" --> key='"+key+"' value='"+value+"'  comment="+rawComment);
		}
		public void directiveLine(int lineNumber, int numLines, String rawLine, String directive, String args, String rawComment) {
			System.out.println(lineNumber+": "+rawLine+" --> directive='"+directive+"' args='"+args+"'  comment="+rawComment);
		}
		public void parseError(int lineNumber, int numLines, String message) {
			System.out.println(lineNumber+": PARSE ERROR: "+message);
		}
	}
	
	/**
	 * Parses an IFile.
	 */
	public void parse(IFile file, ParserCallback callback) throws CoreException, IOException, ParseException {
		parse(new InputStreamReader(file.getContents()), callback);
	}

	/**
	 * Parses a multi-line string.
	 */
	public void parse(String text, ParserCallback callback) throws IOException, ParseException {
		parse(new StringReader(text), callback);
	}

	/**
	 * Parses a stream.
	 */
	public void parse(Reader streamReader, ParserCallback callback) throws IOException {
		LineNumberReader reader = new LineNumberReader(streamReader);

		String rawLine;
		while ((rawLine=reader.readLine()) != null) {
			int lineNumber = reader.getLineNumber();
			int numLines = 1;

			// join continued lines
			String line = rawLine;
			if (rawLine.endsWith("\\")) {
				StringBuilder concatenatedLines = new StringBuilder();
				while (rawLine != null && rawLine.endsWith("\\")) {
					concatenatedLines.append(rawLine, 0, rawLine.length()-1);
					rawLine = reader.readLine();
					numLines++;
				}
				if (rawLine == null)
					callback.parseError(lineNumber, numLines, "Stray backslash at end of file");
				else 
					concatenatedLines.append(rawLine);
				line = concatenatedLines.toString();
			}
			
			// process the line
			line = line.trim();
			char lineStart = line.length()==0 ? 0 : line.charAt(0);
			if (line.length()==0) {
				// blank line
				callback.blankOrCommentLine(lineNumber, numLines, rawLine, "");
			}
			else if (lineStart=='#') {
				// comment line
				callback.blankOrCommentLine(lineNumber, numLines, rawLine, line);
			}
            else if (lineStart==';') {
                // obsolete comment line
                callback.parseError(lineNumber, numLines, "Semicolon is no longer a comment start character, please use hashmark ('#')");
            }
			else if (lineStart=='i' && line.matches("include\\s.*")) {
				// include directive
				String directive = "include";
				String rest = line.substring(directive.length());
				int endPos = findEndContent(rest, 0);
				if (endPos == -1) {
					callback.parseError(lineNumber, numLines, "Unterminated string constant");
					continue;
				}
				String args = rest.substring(0, endPos).trim();
				String rawComment = rest.substring(endPos);
				callback.directiveLine(lineNumber, numLines, rawLine, directive, args, rawComment);
			}
			else if (lineStart=='[') {
				// section heading
				Matcher m = Pattern.compile("\\[([^#\"]+)\\]\\s*?(\\s*#.*)?").matcher(line);
				if (!m.matches()) {
					callback.parseError(lineNumber, numLines, "Syntax error in section heading");
					continue;
				}
				String sectionName = m.group(1).trim();
				String rawComment = m.groupCount()>1 ? m.group(2) : "";
				if (rawComment == null) rawComment = "";
				callback.sectionHeadingLine(lineNumber, numLines, rawLine, sectionName, rawComment);
			}
			else {
				// key = value
				int endPos = findEndContent(line, 0);
				if (endPos == -1) {
					callback.parseError(lineNumber, numLines, "Unterminated string constant");
					continue;
				}
				String rawComment = line.substring(endPos);
				String keyValue = line.substring(0, endPos);
				int equalSignPos = keyValue.indexOf('=');
				if (equalSignPos == -1) {
					callback.parseError(lineNumber, numLines, "Line must be in the form key=value");
					continue;
				}
				String key = keyValue.substring(0, equalSignPos).trim();
				if (key.length()==0) {
					callback.parseError(lineNumber, numLines, "Line must be in the form key=value");
					continue;
				}
				String value = keyValue.substring(equalSignPos+1).trim();
				callback.keyValueLine(lineNumber, numLines, rawLine, key, value, rawComment);
			}
		}
	}

	/**
	 * Returns the position of the comment on the given line (i.e. the position of the
	 * # character), or line.length() if no comment is found. String literals
	 * are recognized and skipped properly.
	 * 
	 * Returns -1 if line contains an unterminated string literal.
	 */
	private static int findEndContent(String line, int fromPos) {
		int k = fromPos;
		while (k < line.length()) {
			switch (line.charAt(k)) {
			case '"':
				// string literal: skip it
				k++;
				while (k < line.length() && line.charAt(k) != '"') {
					if (line.charAt(k) == '\\')  // skip \", \\, etc.
						k++; 
					k++;  
				}
				if (k >= line.length())
					return -1; // meaning "unterminated string literal"
				k++;
				break;
			case '#':
				// comment
				while (k > 0 && Character.isWhitespace(line.charAt(k-1))) k--;
				return k;
			default:
				k++;
			}
		}
		return k;
	}
}
