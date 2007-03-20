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
	/**
	 * Stores the lines of the file. The array is indexed by line number,
	 * starting at zero. 
	 */
	private Vector<InifileLine> lines = new Vector<InifileLine>();

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
		long startTime = System.currentTimeMillis();
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
		System.out.println("Inifile parsing: "+(System.currentTimeMillis()-startTime)+"ms");
	}
	
	public void print(OutputStream stream) {
		PrintStream out = new PrintStream(stream);
		for (InifileLine line : lines) 
			if (line != null)
				out.println(line.getRawText());
	}
	
	public InifileLine getLine(int k) {
		return lines.get(k);
	}
}
