package org.omnetpp.inifile.editor.model;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.LinkedHashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.resources.IResource;
import org.eclipse.core.runtime.CoreException;
import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.DocumentEvent;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentListener;
import org.eclipse.ui.texteditor.MarkerUtilities;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;

/**
 * Standard implementation of IInifileDocument. Setters change the 
 * underlying text document (IDocument). Parsing is lazy: changes on the 
 * text document cause a "changed" flag to be set here, and getters 
 * automatically reparse the text document if it's out of date.
 * 
 * @author Andras
 */
//XXX renameSection() missing
public class InifileDocument implements IInifileDocument {
	public static final String INIFILEPROBLEM_MARKER_ID = InifileEditorPlugin.PLUGIN_ID + ".inifileproblem";
	
	private IDocument document; // the document we are manipulating
	private IFile documentFile; // the file of the document
	private boolean changed; // whether changed since last parsed

	class Line {
		IFile file;
		int lineNumber; // 1-based
		int numLines;  // ==1 unless line continues on other lines (trailing backslash)
		String comment;
	};
	class SectionHeadingLine extends Line {
		String sectionName;
		int lastLine; // last line of section contents  XXX fill in
	}
	class KeyValueLine extends Line {
		String key;
		String value;
	}
	class IncludeLine extends Line {
		String includedFile;
	}

	class Section {
		ArrayList<SectionHeadingLine> headingLines = new ArrayList<SectionHeadingLine>();
		LinkedHashMap<String,KeyValueLine> entries = new LinkedHashMap<String, KeyValueLine>();
	}

	// primary data structure: sections, keys
	private LinkedHashMap<String,Section> sections = new LinkedHashMap<String,Section>();
	
	// reverse (linenumber-to-section/key) mapping
	private ArrayList<SectionHeadingLine> mainFileSectionHeadingLines = new ArrayList<SectionHeadingLine>(); 
	private ArrayList<KeyValueLine> mainFileKeyValueLines = new ArrayList<KeyValueLine>();
	
	// include directives
	private ArrayList<IncludeLine> topIncludes = new ArrayList<IncludeLine>();
	private ArrayList<IncludeLine> bottomIncludes = new ArrayList<IncludeLine>();

	// listeners
	private IDocumentListener listener; // we listen on IDocument
	private InifileChangeListenerList listeners = new InifileChangeListenerList(); // clients that listen on us


	public InifileDocument(IDocument document, IFile documentFile) {
		this.document = document;
		this.documentFile = documentFile;
		this.changed = true;

		// listen on changes so we know when we need to re-parse
		listener = new IDocumentListener() {
			public void documentAboutToBeChanged(DocumentEvent event) {}
			public void documentChanged(DocumentEvent event) {
				changed = true;
			}
		};
		document.addDocumentListener(listener);
	}

	/** 
	 * To be called from the editor!
	 */ 
	public void dispose() {
		document.removeDocumentListener(listener);
	}

	synchronized public void parseIfChanged() {
		if (changed)
			parse();
	}

	synchronized public void parse() {
		sections.clear();
		mainFileKeyValueLines.clear();
		mainFileSectionHeadingLines.clear();
		topIncludes.clear();
		bottomIncludes.clear();
		long startTime = System.currentTimeMillis();
		Reader streamReader = new StringReader(document.get());
        
		try {
			documentFile.deleteMarkers(INIFILEPROBLEM_MARKER_ID, true, IResource.DEPTH_ZERO);
		} catch (CoreException e1) {
			e1.printStackTrace(); //XXX
		}

		try {
			new InifileParser().parse(streamReader, new InifileParser.ParserCallback() {
				Section currentSection = null;
				IFile currentFile = documentFile; 

				public void blankOrCommentLine(int lineNumber, String rawLine, String comment) {
					// ignore
				}

				public void directiveLine(int lineNumber, String rawLine, String directive, String args, String comment) {
					if (directive.equals("include")) {
						IncludeLine line = new IncludeLine();
						line.file = currentFile;
						line.lineNumber = lineNumber;
						line.numLines = 1; //XXX
						line.comment = comment;
						line.includedFile = args;
						// TODO: at this point, we should recursively parse that file as well
					}
				}

				public void incompleteLine(int lineNumber, String rawLine) {
					//XXX eliminate
				}

				public void keyValueLine(int lineNumber, String rawLine, String key, String value, String comment) {
					if (currentSection==null)
						throw new ParseException("entry occurs before first section heading", lineNumber);
					if (!currentSection.entries.containsKey(key)) {
						KeyValueLine line = new KeyValueLine();
						line.file = currentFile;
						line.lineNumber = lineNumber;
						line.numLines = 1; //XXX
						line.comment = comment;
						line.key = key;
						line.value = value;
						currentSection.entries.put(key, line);
						mainFileKeyValueLines.add(line);
					}
				}

				public void sectionHeadingLine(int lineNumber, String rawLine, String sectionName, String comment) {
					// add if such section not yet exists
					Section section = sections.get(sectionName);
					if (section == null) {
						section = new Section();
						sections.put(sectionName, section);
					}
					// add line
					SectionHeadingLine line = new SectionHeadingLine();
					line.file = currentFile;
					line.lineNumber = lineNumber;
					line.numLines = 1; //XXX include continued lines!
					line.comment = comment;
					line.sectionName = sectionName;
					section.headingLines.add(line);
					mainFileSectionHeadingLines.add(line);
					currentSection = section;
				}
			});
		} 
		catch (IOException e) {
			// cannot happen with string input
		} 
		catch (ParseException e) {
		    addMarker(documentFile, INIFILEPROBLEM_MARKER_ID, IMarker.SEVERITY_ERROR, e.getMessage(), e.getLineNumber());
			//System.err.println(e.getClass()+": "+e.getMessage());
		}
		System.out.println("Inifile parsing: "+(System.currentTimeMillis()-startTime)+"ms");
		
		// mark data structure as up to date (even if there was an error, because 
		// we don't want to keep re-parsing)
		changed = false;
		
		// notify listeners
		fireModelChanged();
	}

    @SuppressWarnings("unchecked")
    private static void addMarker(final IFile file, final String type, int severity, String message, int line) {
    	try {
    		HashMap map = new HashMap();
    		MarkerUtilities.setMessage(map, message);
    		MarkerUtilities.setLineNumber(map, line);
    		map.put(IMarker.SEVERITY, severity);
			MarkerUtilities.createMarker(file, map, type);
		} catch (CoreException e) {
			e.printStackTrace(); //XXX
		}
    }

    public void dump() {
    	for (String sectionName : sections.keySet()) {
    		System.out.println("Section "+sectionName);
    		Section section = sections.get(sectionName);
    		for (SectionHeadingLine line : section.headingLines) {
    			System.out.println("  headingLine: line="+line.lineNumber+"  sectionName="+line.sectionName);
    		}
    		for (String key : section.entries.keySet()) {
    			KeyValueLine line = section.entries.get(key);
    			System.out.println("  keyValueLine: line="+line.lineNumber+"  key="+line.key+" value="+line.value);
    		}
    	}
    	System.out.println("Includes:");
		for (IncludeLine line : topIncludes) {
			System.out.println("  topInclude: line="+line.lineNumber+"  file="+line.includedFile);
		}
		for (IncludeLine line : bottomIncludes) {
			System.out.println("  bottomInclude: line="+line.lineNumber+"  file="+line.includedFile);
		}
		System.out.println("num section heading lines: "+mainFileSectionHeadingLines.size());
		System.out.println("num key-value lines: "+mainFileKeyValueLines.size());
    }
    
    protected boolean isEditable(Line line) {
    	return line.file == documentFile;
    }

    protected static boolean nullSafeEquals(String first, String second) {
    	return first==null ? second == null : first.equals(second);
    }
    
	/**
	 * Adds a line to IDocument at the given lineNumber (1-based). Existing lineNumber
	 * will be shifted down. Line text is to be specified without the trailing newline.
	 */
    synchronized protected void addLineAt(int lineNumber, String text) {
		try {
			if (lineNumber==document.getNumberOfLines()+1) {
				// adding a line at the bottom
				document.replace(document.getLength(), 0, "\n");  // XXX doing this, we sometime create two blank lines
			}
			int offset = document.getLineOffset(lineNumber-1); //IDocument is 0-based
			document.replace(offset, 0, text+"\n");
		} 
		catch (BadLocationException e) {
			throw new RuntimeException("cannot insert line: bad location: "+e.getMessage());
		}
	}

	/**
	 * Replaces line content in IDocument, or if text==null, deletes the line.
	 * If the line numbers are NOT changed, suppresses re-parsing the document.
	 * @return true if line numbers have changed, false if not
	 */
    synchronized protected boolean replaceLine(Line line, String text) {
		try {
			int offset = document.getLineOffset(line.lineNumber-1);
			int length = document.getLineOffset(line.lineNumber-1+line.numLines) - offset;
			document.replace(offset, length, text==null ? "" : text+"\n");

			boolean lineNumberChange = (text==null) || (line.numLines != StringUtils.countNewLines(text)+1);
			changed = lineNumberChange; // force re-parsing because line numbers have shifted, and cancel changed=true setting from documentlistener 
			return lineNumberChange;
		} 
		catch (BadLocationException e) {
			throw new RuntimeException("cannot set value: bad location: "+e.getMessage());
		}
	}

	public boolean containsKey(String section, String key) {
		return lookupEntry(section, key) != null;
	}
  
    protected KeyValueLine lookupEntry(String sectionName, String key) {
    	parseIfChanged();
    	Section section = sections.get(sectionName);
    	return section == null ? null : section.entries.get(key);
    }
    
    protected KeyValueLine getEntry(String sectionName, String key) {
    	KeyValueLine line = lookupEntry(sectionName, key);
    	if (line == null)
    		throw new IllegalArgumentException("no such entry: ["+sectionName+"] "+key);
    	return line;
    }

    protected KeyValueLine getEditableEntry(String sectionName, String key) {
    	KeyValueLine line = getEntry(sectionName, key);
    	if (!isEditable(line))
    		throw new IllegalArgumentException("entry is in an included file which cannot be edited: ["+sectionName+"] "+key);
    	return line;
    }

    public String getValue(String section, String key) {
		KeyValueLine line = lookupEntry(section, key);
		return line == null ? null : line.value;
	}
	
	public void setValue(String section, String key, String value) {
		KeyValueLine line = getEditableEntry(section, key);
		if (!nullSafeEquals(line.value, value)) {
			line.value = value; 
			String text = line.key + " = " + line.value + (line.comment == null ? "" : " "+line.comment);
			replaceLine(line, text);
		}
	}

	public void addEntry(String section, String key, String value, String comment, String beforeKey) {
		if (lookupEntry(section, key) != null)
			throw new IllegalArgumentException("entry already exists: ["+section+"] "+key);

		// modify IDocument
		//FIXME this inserts NOT APPENDS!!!!!
		System.out.println("addEntry() is shit!");
		int atLine = beforeKey==null ? getFirstEditableSectionHeading(section).lineNumber+1 : getEditableEntry(section, beforeKey).lineNumber;
		String text = key + " = " + value + (comment == null ? "" : " "+comment);
		addLineAt(atLine, text);
	}

	public LineInfo getEntryLineDetails(String section, String key) {
		KeyValueLine line = lookupEntry(section, key);
		return line==null ? null : new LineInfo(line.file, line.lineNumber, !isEditable(line));
	} 

	public String getComment(String section, String key) {
		return getEntry(section, key).comment;
	}

	public void setComment(String section, String key, String comment) {
		KeyValueLine line = getEditableEntry(section, key);
		if (!nullSafeEquals(line.comment, comment)) {
			line.comment = comment; 
			String text = line.key + " = " + line.value + (line.comment == null ? "" : " "+line.comment);
			replaceLine(line, text);
		}
	}

	public void changeKey(String section, String oldKey, String newKey) {
		KeyValueLine line = getEditableEntry(section, oldKey);
		if (!nullSafeEquals(line.key, newKey)) {
			line.key = newKey; 
			String text = line.key + " = " + line.value + (line.comment == null ? "" : " "+line.comment);
			replaceLine(line, text);
			changed = true; // force re-parsing to update hash tables (replaceLine() may have suppressed re-parsing)
		}
	}

	public void removeKey(String section, String key) {
		KeyValueLine line = lookupEntry(section, key);
		if (line != null) { //XXX isEditable
			replaceLine(line, null); //XXX what if multi-line
		}
	}

	public void moveKey(String section, String key, String beforeKey) {
		KeyValueLine line = getEditableEntry(section, key);
		if (beforeKey != null) {
			if (beforeKey.equals(key))
				return; // moving it before itself == nop
			getEditableEntry(section, beforeKey); // just probe it, to make sure it's editable
		}
		removeKey(section, key);
		addEntry(section, key, line.value, line.comment, beforeKey);
	}

	public String[] getKeys(String sectionName) {
		parseIfChanged();
		Section section = sections.get(sectionName);
		return section == null ? null : section.entries.keySet().toArray(new String[0]);
	}

	public String[] getSectionNames() {
		parseIfChanged();
		return sections.keySet().toArray(new String[0]);
	}

	public boolean containsSection(String section) {
		return sections.containsKey(section);
	}
	
	protected Section lookupSection(String sectionName) {
		parseIfChanged();
		Section section = sections.get(sectionName);
		if (section == null)
			throw new IllegalArgumentException("section does not exist: ["+sectionName+"]");
		return section;
	}

	/**
	 * Returns the first editable section heading, or if none are editable, the first one.
	 */
	protected SectionHeadingLine lookupPreferredSectionHeading(String sectionName) {
		parseIfChanged();
		Section section = sections.get(sectionName);
		if (section == null)
			throw new IllegalArgumentException("section does not exist: ["+sectionName+"]");
		for (SectionHeadingLine line : section.headingLines)
			if (isEditable(line))
				return line;
		return section.headingLines.get(0);
	}

	protected SectionHeadingLine getFirstEditableSectionHeading(String sectionName) {
		SectionHeadingLine line = lookupPreferredSectionHeading(sectionName);
		if (!isEditable(line))
			throw new IllegalArgumentException("section is in an included file: ["+sectionName+"]");
		return line;
	}
	
	public void removeSection(String sectionName) {
		parseIfChanged();
		Section section = sections.get(sectionName);
		if (section != null) {
			//TODO remove from IDocument
		}
	}

	public void addSection(String sectionName, String beforeSectionName) {
		parseIfChanged();
		if (sections.get(sectionName) != null)
			throw new IllegalArgumentException("section already exists: ["+sectionName+"]");

		// find insertion point
		int lineNumber;
		if (beforeSectionName==null)                                       
			lineNumber = bottomIncludes.isEmpty() ? document.getNumberOfLines()+1 : bottomIncludes.get(0).lineNumber;
		else
			lineNumber = getFirstEditableSectionHeading(beforeSectionName).lineNumber;

		// modify IDocument
		String text = "[" + sectionName + "]";
		addLineAt(lineNumber, "");  // leave blank
		addLineAt(lineNumber, text);
	}

	public LineInfo getSectionLineDetails(String sectionName) {
		SectionHeadingLine line = lookupPreferredSectionHeading(sectionName);
		return new LineInfo(line.file, line.lineNumber, !isEditable(line));
	} 

	public String getSectionComment(String sectionName) {
		return lookupPreferredSectionHeading(sectionName).comment;
	}

	public void setSectionComment(String sectionName, String comment) {
		SectionHeadingLine line = getFirstEditableSectionHeading(sectionName);
		if (!nullSafeEquals(line.comment, comment)) {
			line.comment = comment; 
			String text = "[" + line.sectionName + "]" + (line.comment == null ? "" : " "+line.comment);
			replaceLine(line, text);
		}
	}

	public String getSectionForLine(int lineNumber) {
		SectionHeadingLine line = findSectionHeadingLine(lineNumber);
		return line == null ? null : line.sectionName;
	}

	private SectionHeadingLine findSectionHeadingLine(int lineNumber) {
		int i = -1;
		while (i+1 < mainFileSectionHeadingLines.size() && lineNumber >= mainFileSectionHeadingLines.get(i+1).lineNumber)
			i++;
		return i==-1 ? null : mainFileSectionHeadingLines.get(i);
	}
	
	public String getKeyForLine(int lineNumber) {
		SectionHeadingLine sectionHeadingLine = findSectionHeadingLine(lineNumber);
		if (sectionHeadingLine == null)
			return null;

		// find key in that section
		//XXX use binary search, or map<int,Line>, etc
		for (KeyValueLine line : mainFileKeyValueLines) {
			if (line.lineNumber==lineNumber)
				return line.key;
		}
		return null;
	}
	
	public String[] getTopIncludes() {
		parseIfChanged();
		return null; //TODO
	}

	public void addTopInclude(String include, String before) {
		parseIfChanged();
		//TODO
	}

	public void removeTopInclude(String include) {
		parseIfChanged();
		//TODO
	}

	public String[] getBottomIncludes() {
		parseIfChanged();
		return null; //TODO
	}

	public void addBottomInclude(String include, String before) {
		parseIfChanged();
		//TODO
	}

	public void removeBottomInclude(String include) {
		parseIfChanged();
		//TODO
	}

    /**
     * Adds a listener to this document 
     */
	public void addInifileChangeListener(IInifileChangeListener listener) {
        listeners.add(listener);
    }

	/**
     * Adds a listener to this document 
     */
	public void removeInifileChangeListener(IInifileChangeListener listener) {
        listeners.remove(listener);
    }
	
	/**
     * Fires a model change event by notifying listeners.
     */
    public void fireModelChanged() {
        if (listeners != null && listeners.isEnabled())
        	listeners.fireModelChanged();
    }
}
