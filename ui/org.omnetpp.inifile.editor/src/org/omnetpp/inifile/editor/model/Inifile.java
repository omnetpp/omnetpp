package org.omnetpp.inifile.editor.model;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.Reader;
import java.io.StringReader;
import java.util.Vector;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
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
		INCOMPLETE  // continued from previous line (which ended in backslash)
	};

	class Line {
		private LineType type;
		private int lineNumber;
		private String rawText;
		private String comment; // with "#"

		public Line(LineType type, int lineNumber, String rawText, String comment) {
			this.type = type;
			this.lineNumber = lineNumber;
			this.rawText = rawText;
			this.comment = comment;
		}

		public String getComment() {
			return comment;
		}

		public int getLineNumber() {
			return lineNumber;
		}

		public String getRawText() {
			return rawText;
		}

		public LineType getType() {
			return type;
		}
	}

	class CommentLine extends Line {
		public CommentLine(int lineNumber, String rawText, String comment) {
			super(LineType.COMMENT, lineNumber, rawText, comment);
		}
	}

	class SectionHeadingLine extends Line {
		private String sectionName;

		public SectionHeadingLine(int lineNumber, String rawText, String sectionName, String comment) {
			super(LineType.HEADING, lineNumber, rawText, comment);
			this.sectionName = sectionName;
		}

		public String getSectionName() {
			return sectionName;
		}
	}

	class KeyValueLine extends Line {
		private String key;
		private String value;
		
		public KeyValueLine(int lineNumber, String rawText, String key, String value, String comment) {
			super(LineType.KEYVALUE, lineNumber, rawText, comment);
			this.key = key;
			this.value = value;
		}

		public String getKey() {
			return key;
		}

		public String getValue() {
			return value;
		}
	}

	class DirectiveLine extends Line {
		private String directive;
		private String args;
		
		public DirectiveLine(int lineNumber, String rawText, String directive, String args, String comment) {
			super(LineType.DIRECTIVE, lineNumber, rawText, comment);
			this.directive = directive;
			this.args = args;
		}

		public String getArgs() {
			return args;
		}

		public String getDirective() {
			return directive;
		}
	}

	class IncompleteLine extends Line {
		public IncompleteLine(int lineNumber, String rawText) {
			super(LineType.INCOMPLETE, lineNumber, rawText, null);
		}
	}


	/**
	 * Stores the lines of the file. The array is indexed by line number,
	 * starting at zero. 
	 */
	private Vector<Line> lines = new Vector<Line>();

	/**
	 * Parses an IFile.
	 */
	public Inifile(IFile file) throws CoreException, IOException, ParseException {
		this(new InputStreamReader(file.getContents()));
	}

	/**
	 * Parses a multi-line string.
	 */
	public Inifile(String text) throws IOException, ParseException {
		this(new StringReader(text));
	}
	
	/**
	 * Parses a stream.
	 */
	public Inifile(Reader streamReader) throws IOException, ParseException {
		lines.clear();
		lines.add(null);
		new InifileParser().parse(streamReader, new InifileParser.ParserCallback() {
			public void blankOrCommentLine(int lineNumber, String rawLine, String comment) {
				Assert.isTrue(lines.size()==lineNumber);
				lines.add(lineNumber, new CommentLine(lineNumber, rawLine, comment));
			}
			public void directiveLine(int lineNumber, String rawLine, String directive, String args, String comment) {
				Assert.isTrue(lines.size()==lineNumber);
				lines.add(lineNumber, new DirectiveLine(lineNumber, rawLine, directive, args, comment));
			}
			public void incompleteLine(int lineNumber, String rawLine) {
				Assert.isTrue(lines.size()==lineNumber);
				lines.add(lineNumber, new IncompleteLine(lineNumber, rawLine));
			}
			public void keyValueLine(int lineNumber, String rawLine, String key, String value, String comment) {
				Assert.isTrue(lines.size()==lineNumber);
				lines.add(lineNumber, new KeyValueLine(lineNumber, rawLine, key, value, comment));
			}
			public void sectionHeadingLine(int lineNumber, String rawLine, String sectionName, String comment) {
				Assert.isTrue(lines.size()==lineNumber);
				lines.add(lineNumber, new SectionHeadingLine(lineNumber, rawLine, sectionName, comment));
			}
		});
	}
	
	public void print(OutputStream stream) {
		PrintStream out = new PrintStream(stream);
		for (Line line : lines) 
			if (line != null)
				out.println(line.getRawText());
	}
	
	public Line getLine(int k) {
		return lines.get(k);
	}
}
