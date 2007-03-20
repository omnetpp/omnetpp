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
	
	public void parse(IFile file, ParserCallback callback) throws CoreException, IOException, ParseException {
		parse(new InputStreamReader(file.getContents()), callback);
	}

	public void parse(String text, ParserCallback callback) throws IOException, ParseException {
		parse(new StringReader(text), callback);
	}

	public void parse(Reader streamReader, ParserCallback callback) throws IOException, ParseException {
		LineNumberReader reader = new LineNumberReader(streamReader);

		String line;
		while ((line=reader.readLine()) != null) {
			System.out.print(reader.getLineNumber()+": "+line+" >>>> ");

			// join continued lines
			if (line.endsWith("\\")) {
				callback.incompleteLine(reader.getLineNumber(), line);
				while (line.endsWith("\\")) {
					String cont = reader.readLine();
					if (cont == null)
						break; //XXX error? (missing continuation line after "\"-terminated line)
					callback.incompleteLine(reader.getLineNumber(), cont);
					line = line.substring(0, line.length()-1) + cont;
				}
			}

			// process the line
			line = line.trim();
			char lineStart = line.length()==0 ? 0 : line.charAt(0);
			if (line.length()==0) {
				// blank line
				callback.blankOrCommentLine(reader.getLineNumber(), line, null);
			}
			else if (lineStart=='#' || lineStart==';') {
				// comment line
				callback.blankOrCommentLine(reader.getLineNumber(), line, line.trim());
			}
			else if (line.startsWith("include ") || line.startsWith("include\t")) {
				// include directive
				callback.directiveLine(reader.getLineNumber(), line, "include", "TODO XXX", "TODO XXX");
			}
			else if (lineStart=='[') {
				// section heading
				Matcher m = Pattern.compile("\\[([^#;\"]+)\\]\\s*([#;].*)?").matcher(line);
				if (!m.matches())
					throw new ParseException("syntax error", reader.getLineNumber());
				String sectionName = m.group(1).trim();
				String comment = m.groupCount()>1 ? m.group(2) : null; 
				//System.out.println("section "+sectionName+"  comment="+comment);
				callback.sectionHeadingLine(reader.getLineNumber(), line, sectionName, comment);
			}
			else {
				// key = value
				int equalSignPos = line.indexOf('=');
				if (equalSignPos == -1)
					throw new ParseException("line must be in the form key=value", reader.getLineNumber());
				String key = line.substring(0, equalSignPos).trim();
				if (key.length()==0 || key.indexOf('#')!=-1 || key.indexOf(';')!=-1)
					throw new ParseException("line must be in the form key=value", reader.getLineNumber());
				String rest = line.substring(equalSignPos+1).trim();
				int k = 0;
				loop: while (k < rest.length()) {
					switch (rest.charAt(k)) {
					case '"':
						// string literal: skip it
						k++;
						while (k < rest.length() && rest.charAt(k) != '"') {
							if (rest.charAt(k) == '\\')  // skip \", \\, etc.
								k++; 
							k++;  
						}
						if (k >= rest.length())
							throw new ParseException("unterminated string literal", reader.getLineNumber());
						break;
					case '#': case ';':
						// comment
						break loop;
					}
					k++;
				}
				String value = rest.substring(0, k).trim();
				String comment = (k==rest.length()) ? null : rest.substring(k);
				//System.out.println("key-value: ``"+key+"'' = ``"+rawValue+"''  comment="+comment);
				callback.keyValueLine(reader.getLineNumber(), line, key, value, comment);
			}
		}
	}

}
