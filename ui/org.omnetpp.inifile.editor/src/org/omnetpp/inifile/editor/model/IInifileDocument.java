package org.omnetpp.inifile.editor.model;

import java.util.LinkedHashMap;
import java.util.LinkedHashSet;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;

/**
 * An interface for high-level manipulation of the contents of an ini file 
 * (as sections and entries).
 * 
 * This interface does not try to make sense of section names, key names
 * and values -- they are just treated as strings. Additional layer(s) 
 * may be needed to deal with these details.
 *
 * NOTE 1: this interface assumes that within a section, keys are unique
 * (i.e. multiple appearances of the same key should be flagged as an error
 * during parsing.)
 * 
 * NOTE 2: It is also not possible to distinguish multiple sections 
 * of the same name: this interface presents them as a single section.
 * 
 * @author Andras
 */
//FIXME deal with include files; their contents should be visible but not editable?
public interface IInifileDocument { //XXX
	/** Setters change the underlying IDocument */
	public class IniLine {
		private IFile file;
		private IRegion region;
		//private String comment;
	};
	public class SectionHeadingLine extends IniLine {
		private String sectionName;
	}
	public class KeyValueLine extends IniLine {
		private String keyName;
		private String value;
	}
	public class IniSection {
		private SectionHeadingLine sectionHeading; // the first if there's more than one
		private LinkedHashMap<String,KeyValueLine> entries; // the first, if key occurs multiple times
	}
	
	public LinkedHashMap<String,IniSection> sections = new LinkedHashMap<String, IniSection>();
	
	//--------------------

	public class LineInfo {
		private IFile file;
		private int lineNumber;
		private boolean readOnly;

		public LineInfo(IFile file, int lineNumber, boolean readOnly) {
			this.file = file;
			this.lineNumber = lineNumber;
			this.readOnly = readOnly;
		}

		public IFile getFile() {
			return file;
		}
		public int getLineNumber() {
			return lineNumber;
		}
		public boolean isReadOnly() {
			return readOnly;
		}
	}
	
	// null if not exists, first one if there's more than one
	String getValue(String section, String key);

	// set if exists, error if not exists
	void setValue(String section, String key, String value);

	// error if exists, add to section if not exists, create section if section not exists;
	// comment may be null; beforeKey may be null (meaning append)
	void addEntry(String section, String key, String value, String comment, String beforeKey);

	// returns immutable value object with file/line/isreadonly info; 
	// of the first occurrence if there's more than one
	LineInfo getEntryLineDetails(String section, String key); 

	// error if key doesn't exist; of first one if key is not unique
	String getComment(String section, String key);

	// error if key doesn't exist; sets comment on first one if key is not unique
	void setComment(String section, String key, String comment);

	// removes all if there's more than one
	void removeKey(String section, String key);

	// returns unique keys, in order
	String[] getKeys(String section);

	// returns list of unique section names
	String[] getSectionNames();

	// removes all sections with that name, except the parts in readonly files
	void removeSection(String section);
	
	// adds section; error if already exists
	void addSection(String sectionName, String beforeSection);

	// returns immutable value object with file/line/isreadonly info; 
	// of the first occurrence if there's more than one
	LineInfo getSectionLineDetails(String section); 
	
	// error if section doesn't exist; of the first one if there're more than one
	String getSectionComment(String section);

	// error if section doesn't exist; sets comment on first one if there're more than one
	void setSectionComment(String section, String comment);

	// only includes the includes at the top of the primary (edited) file
	String[] getTopIncludes();
	void addTopInclude(String include, String before);
	void removeTopInclude(String include);

	// only includes the includes at the bottom of the primary (edited) file
	String[] getBottomIncludes();
	void addBottomInclude(String include, String before);
	void removeBottomInclude(String include);

}
