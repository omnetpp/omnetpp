package org.omnetpp.inifile.editor.model;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.CoreException;

/**
 * Represents an ini file.
 * 
 * @author Andras
 */
public class Inifile {
	public enum LineType {
		COMMENT, // blank or comment line (optional whitespace + optional comment) 
		HEADING, // section heading (plus optional comment) 
		KEYVALUE,   // key = value (plus optional comment)
		DIRECTIVE, // like "include foo.ini" (plus optional comment)  
		CONTINUATION  // continued from previous line (which ended in backslash)
	};

	class Line {
		private LineType type;
		private String rawText;
		private String comment; // without "#" plus leading whitespace
	}

	class CommentLine extends Line {
	}

	class HeadingLine extends Line {
		private String heading;
	}

	class KeyValueLine extends Line {
		private String key;
		private String value;
	}

	class DirectiveLine extends Line {
		private String directive;
		private String argument;
	}

	class ContinuationLine extends Line {
	}


	/**
	 * Stores the lines of the file. The array is indexed by line number,
	 * starting at zero. 
	 */
	private List<Line> lines = new ArrayList<Line>();

	public Inifile() {
	}

	public void parse(IFile file) throws CoreException, IOException {
		parse(new InputStreamReader(file.getContents()));
	}

	public void parse(String text) throws CoreException, IOException {
		parse(new StringReader(text));
	}

	public void parse(Reader streamReader) throws CoreException, IOException {
		LineNumberReader reader = new LineNumberReader(streamReader);

		String line;
		while ((line=reader.readLine()) != null) {
			System.out.print(reader.getLineNumber()+": "+line+" >>>> ");

			// join continued lines
			while (line.endsWith("\\")) {
				String cont = reader.readLine();
				if (cont == null)
					break; //XXX error? (missing continuation line after "\"-terminated line)
				line = line.substring(0, line.length()-1) + cont;
			}

			// process the line
			line = line.trim();
			char lineStart = line.length()==0 ? 0 : line.charAt(0);
			if (line.length()==0) {
				// blank line
				System.out.println("blank");
			}
			else if (lineStart=='#' || lineStart==';') {
				// comment line
				System.out.println("comment");
			}
			else if (line.startsWith("include ") || line.startsWith("include\t")) {
				// include directive
			}
			else if (lineStart=='[') {
				// section heading
				Matcher m = Pattern.compile("\\[([^#;\"]+)\\]\\s*([#;].*)?").matcher(line);
				if (!m.matches())
					throw new RuntimeException("syntax error");
				String sectionName = m.group(1).trim();
				String comment = m.groupCount()>1 ? m.group(2) : null; 
				System.out.println("section "+sectionName+"  comment="+comment);
			}
			else {
				// key = value
				int equalSignPos = line.indexOf('=');
				if (equalSignPos == -1)
					throw new RuntimeException("line must be in the form key=value");
				String key = line.substring(0, equalSignPos).trim();
				if (key.length()==0 || key.indexOf('#')!=-1 || key.indexOf(';')!=-1)
					throw new RuntimeException("line must be in the form key=value");
				String rest = line.substring(equalSignPos+1).trim();
				int k = 0;
				loop: while (k < rest.length()) {
					switch (rest.charAt(k)) {
					case '"':
						// find end of string literal
						k++;
						while (k < rest.length() && rest.charAt(k) != '"')
							k++;  //XXX handle \", \\, etc.
//						System.out.println("EOS:k="+k+" len="+rest.length());
						if (k == rest.length())
							throw new RuntimeException("unterminated string literal");
						break;
					case '#': case ';':
						break loop;
					}
					k++;
				}
				String rawValue = rest.substring(0, k).trim();
				String comment = rest.substring(k);
				System.out.println("key-value: ``"+key+"'' = ``"+rawValue+"''  comment="+comment);
			}
		}
	}
}
