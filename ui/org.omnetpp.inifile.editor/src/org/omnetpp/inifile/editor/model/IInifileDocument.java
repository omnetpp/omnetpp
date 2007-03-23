package org.omnetpp.inifile.editor.model;

import org.eclipse.core.resources.IFile;

/**
 * An interface for high-level manipulation of the contents of an ini file 
 * (as sections and entries).
 * 
 * This interface does not try to make sense of section names, key names
 * and values -- they are just treated as strings. Additional layer(s) 
 * may be needed to deal with these details.
 *
 * This interface assumes that within a section, keys are unique (i.e. multiple 
 * appearances of the same key should be flagged as an error during parsing.)
 * If there're non-unique keys, behaviour is undefined: this interface
 * should not be used until this condition gets resolved. 
 * 
 * Note: it is not possible to distinguish multiple sections of the same name
 * (ie a non-contiguous section): this interface presents them as a single section.
 * 
 * @author Andras
 */
public interface IInifileDocument {
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

	/**
	 * Returns the value of the given entry. Returns null if section,
	 * or key in it does not exist. 
	 */
	String getValue(String section, String key);

	/** 
	 * Sets the value of the given entry. Throws error if key does not exist 
	 * in that section, or it is readonly. 
	 */
	void setValue(String section, String key, String value);

	/**
	 * Creates a new entry. Throws error if already exists, or section or beforeKey does not exist. 
	 * @param comment may be null
	 * @param beforeKey may be null (meaning append) 
	 */
	void addEntry(String section, String key, String value, String comment, String beforeKey);

	/**
	 * Returns immutable value object with file/line/isreadonly info.
	 * Returns null if section, or key in it does not exist.  
	 */
	LineInfo getEntryLineDetails(String section, String key); 

	/** 
	 * Returns comment for the given key, or null if there's no comment.
	 * Throws error if key doesn't exist.
	 */
	String getComment(String section, String key);

	/** 
	 * Sets the comment for an entry. Throws error if key doesn't exist.
	 */
	void setComment(String section, String key, String comment);

	/** 
	 * Removes the given key from the given section. Nothing happens if it's not there.
	 */
	void removeKey(String section, String key);

	/** 
	 * Returns keys in the given section, in the order they appear. 
	 * Returns null if section does not exist, and zero-length array if it 
	 * does not contain entries.
	 */
	String[] getKeys(String section);

	/** 
	 * Returns list of unique section names. 
	 */
	String[] getSectionNames();

	/** 
	 * Removes all sections with that name, except the parts in readonly files.
	 * If there's no such section, nothing happens. 
	 */
	void removeSection(String section);
	
	/** 
	 * Adds a section. Throws an error if such section already exists. 
	 * (This effectively means that this interface does not support creating 
	 * and populating non-contiguous sections). 
	 */
	void addSection(String sectionName, String beforeSection);

	/** 
	 * Returns immutable value object with file/line/isreadonly info.
	 * Returns null if section does not exist.  
	 */
	LineInfo getSectionLineDetails(String section); 
	
	/**
	 * Returns the comment on the section heading's line. Throws error if section 
	 * doesn't exist; returns comment of the first heading if there're more than one. 
	 */
	String getSectionComment(String section);

	/**
	 * Sets the comment on the section heading's line. Throws error if section 
	 * doesn't exist; sets the comment of the first heading if there're more than one. 
	 */
	void setSectionComment(String section, String comment);

	/**
	 * Returns the included files at the top of the primary (edited) file.
	 * Other includes are not returned.
	 */ 
	String[] getTopIncludes();
	
	/**
	 * Adds an include at the top of the file.
	 * @param beforeInclude specify null to append
	 */
	void addTopInclude(String include, String beforeInclude);
	
	/**
	 * Removes the given include. Throws error if include does not exist.
	 */
	void removeTopInclude(String include);

	/**
	 * Returns the included files at the bottom of the primary (edited) file.
	 * Other includes are not returned.
	 */ 
	String[] getBottomIncludes();

	/**
	 * Adds an include at the bottom of the file.
	 * @param beforeInclude specify null to append
	 */
	void addBottomInclude(String include, String before);

	/**
	 * Removes the given include. Throws error if include does not exist.
	 */
	void removeBottomInclude(String include);
	
    /**
     * Return the listener list for adding/removing listeners. 
     */
	InifileChangeListenerList getListeners();
}
