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
	 * Implement this interface to store ini file contents as it gets parsed. The comment
	 * arg may be null (if there is no comment on that line). Lines that end in backslash
	 * are passed to incompleteLine(), with all content assigned to their completing
	 * (non-backslash) line.
	 */
	public interface ParserCallback {
		void blankOrCommentLine(int lineNumber, String rawLine, String comment);
		void sectionHeadingLine(int lineNumber, String rawLine, String sectionName, String comment);
		void keyValueLine(int lineNumber, String rawLine, String key, String value, String comment);
		void directiveLine(int lineNumber, String rawLine, String directive, String args, String comment);
		void incompleteLine(int lineNumber, String rawLine);
	}

	/**
	 * A ParserCallback with all methods defined to be empty.
	 */
	public static class ParserAdapter implements ParserCallback {
		public void blankOrCommentLine(int lineNumber, String rawLine, String comment) {}
		public void sectionHeadingLine(int lineNumber, String rawLine, String sectionName, String comment) {}
		public void keyValueLine(int lineNumber, String rawLine, String key, String value, String comment) {}
		public void directiveLine(int lineNumber, String rawLine, String directive, String args, String comment) {}
		public void incompleteLine(int lineNumber, String rawLine) {}
	}

	/**
	 * A ParserCallback for debug purposes.
	 */
	public static class DebugParserAdapter implements ParserCallback {
		public void blankOrCommentLine(int lineNumber, String rawLine, String comment) {
			System.out.println(lineNumber+": "+rawLine+" --> comment="+comment);
		}
		public void sectionHeadingLine(int lineNumber, String rawLine, String sectionName, String comment) {
			System.out.println(lineNumber+": "+rawLine+" --> section '"+sectionName+"'  comment="+comment);
		}
		public void keyValueLine(int lineNumber, String rawLine, String key, String value, String comment) {
			System.out.println(lineNumber+": "+rawLine+" --> key='"+key+"' value='"+value+"'  comment="+comment);
		}
		public void directiveLine(int lineNumber, String rawLine, String directive, String args, String comment) {
			System.out.println(lineNumber+": "+rawLine+" --> directive='"+directive+"' args='"+args+"'  comment="+comment);
		}
		public void incompleteLine(int lineNumber, String rawLine) {
			System.out.println(lineNumber+": "+rawLine+" --> incomplete line");
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
	public void parse(Reader streamReader, ParserCallback callback) throws IOException, ParseException {
		LineNumberReader reader = new LineNumberReader(streamReader);

		String rawLine;
		while ((rawLine=reader.readLine()) != null) {
			// join continued lines
			String line = rawLine;
			if (rawLine.endsWith("\\")) {
				StringBuilder concatenatedLines = new StringBuilder();
				while (rawLine != null && rawLine.endsWith("\\")) {
					callback.incompleteLine(reader.getLineNumber(), rawLine);
					concatenatedLines.append(rawLine, 0, rawLine.length()-1);
					rawLine = reader.readLine();
				}
				if (rawLine == null)
					throw new ParseException("stray backslash at end of file", reader.getLineNumber());
				concatenatedLines.append(rawLine);
				line = concatenatedLines.toString();
			}

			// process the line
			line = line.trim();
			char lineStart = line.length()==0 ? 0 : line.charAt(0);
			if (line.length()==0) {
				// blank line
				callback.blankOrCommentLine(reader.getLineNumber(), rawLine, null);
			}
			else if (lineStart=='#' || lineStart==';') {
				// comment line
				callback.blankOrCommentLine(reader.getLineNumber(), rawLine, line.trim());
			}
			else if (lineStart=='i' && line.matches("include\\s.*")) {
				// include directive
				String directive = "include";
				String rest = line.substring(directive.length());
				int endPos = findEndContent(rest, 0, reader.getLineNumber());
				String args = rest.substring(0, endPos).trim();
				String comment = (endPos==rest.length()) ? null : rest.substring(endPos);
				callback.directiveLine(reader.getLineNumber(), rawLine, directive, args, comment);
			}
			else if (lineStart=='[') {
				// section heading
				Matcher m = Pattern.compile("\\[([^#;\"]+)\\]\\s*([#;].*)?").matcher(line);
				if (!m.matches())
					throw new ParseException("syntax error in section heading", reader.getLineNumber());
				String sectionName = m.group(1).trim();
				String comment = m.groupCount()>1 ? m.group(2) : null; 
				callback.sectionHeadingLine(reader.getLineNumber(), rawLine, sectionName, comment);
			}
			else {
				// key = value
				int endPos = findEndContent(line, 0, reader.getLineNumber());
				String comment = (endPos==line.length()) ? null : line.substring(endPos);
				String keyValue = line.substring(0, endPos);
				int equalSignPos = keyValue.indexOf('=');
				if (equalSignPos == -1)
					throw new ParseException("line must be in the form key=value", reader.getLineNumber());
				String key = keyValue.substring(0, equalSignPos).trim();
				if (key.length()==0)
					throw new ParseException("line must be in the form key=value", reader.getLineNumber());
				String value = keyValue.substring(equalSignPos+1).trim();
				callback.keyValueLine(reader.getLineNumber(), rawLine, key, value, comment);
			}
		}
	}

	/**
	 * Returns the position of the comment on the given line (i.e. the position of the
	 * # or ; character), or line.length() if no comment is found. String literals
	 * are recognized and skipped properly. 
	 */
	private static int findEndContent(String line, int fromPos, int currentLineNumber) throws ParseException {
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
					throw new ParseException("unterminated string literal", currentLineNumber);
				k++;
				break;
			case '#': case ';':
				// comment
				return k;
			default:
				k++;
			}
		}
		return k;
	}

}
