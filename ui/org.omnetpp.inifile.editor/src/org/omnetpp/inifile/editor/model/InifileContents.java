package org.omnetpp.inifile.editor.model;

import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Vector;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;

/**
 * Stores contents of an ini file in a parsed form. Allows modification
 * and regeneration of the inifile as well.
 * 
 * @author Andras
 */
public class InifileContents {
	/**
	 * Stores the lines of the file. The array is indexed by line number,
	 * starting at zero. 
	 */
	private Vector<InifileLine> lines = new Vector<InifileLine>();

	private InifileChangeListenerList listeners = new InifileChangeListenerList();
	
	/**
	 * Constructor.
	 */
	public InifileContents() {
	}

	/**
	 * Parses an IFile.
	 */
	public void parse(IFile file) throws CoreException, IOException, ParseException {
		parse(new InputStreamReader(file.getContents()));
	}

	/**
	 * Parses a multi-line string.
	 */
	public void parse(String text) throws IOException, ParseException {
		parse(new StringReader(text));
	}
	
	/**
	 * Parses a stream.
	 */
	public void parse(Reader streamReader) throws IOException, ParseException {
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

		// notify listeners
		fireModeChanged();
	}
	
	public void print(OutputStream stream) {
		PrintStream out = new PrintStream(stream);
		for (InifileLine line : lines) 
			if (line != null) {
				line.setComment(line.getComment()); // invalidate rawText
				out.println(line.getText());
			}
	}
	
	public InifileLine getLine(int k) {
		return lines.get(k);
	}
	
	public InifileLine[] getLines() {
		return (InifileLine[]) lines.subList(1, lines.size()).toArray(new InifileLine[lines.size()-1]);
	}
	
	public InifileLine[] getSections() {
		ArrayList<InifileLine> a = new ArrayList<InifileLine>();
		for (InifileLine line : lines)
			if (line instanceof SectionHeadingLine)
				a.add(line);
		return (InifileLine[]) a.toArray(new InifileLine[a.size()]);
	}

    /**
     * @return The listener list attached to this element 
     */
    public InifileChangeListenerList getListeners() {
        return listeners;
    }
	
	/**
     * Fires a model change event by notifying listeners.
     */
    public void fireModeChanged() {
        if(listeners == null || !getListeners().isEnabled())
            return;
        //forward to the listerList
        listeners.fireModelChanged();
    }
}
