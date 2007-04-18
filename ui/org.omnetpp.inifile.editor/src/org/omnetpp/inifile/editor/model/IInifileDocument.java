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
 * Some entries may not be editable (i.e. they are readonly), because they
 * come from included ini files, or for some other reason. It it an error 
 * to try to modify or remove these entries.
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
	 * Returns true iff section and key exists.
	 */
	boolean containsKey(String section, String key);

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
	 * Sets the comment for an entry. Throws error if key doesn't exist,
	 * or the entry is readonly.
	 */
	void setComment(String section, String key, String comment);

	/** 
	 * Renames the given key. Throws error if key doesn't exist, or the entry is readonly.
	 */
	void changeKey(String section, String oldKey, String newKey);
	
	/** 
	 * Removes the given key from the given section. Nothing happens if it's not there.
	 * Throws an error if this entry is not editable (readonly).
	 */
	void removeKey(String section, String key);

	/**
	 * Moves the given key before the given one. Throws error if the entry does 
	 * not exist, or beforeKey does not exist. 
	 * @param comment may be null
	 * @param beforeKey may be null (meaning append) 
	 */
	void moveKey(String section, String key, String beforeKey);
	
	/** 
	 * Returns keys in the given section, in the order they appear. 
	 * Returns null if section does not exist, and zero-length array if it 
	 * does not contain entries.
	 */
	String[] getKeys(String section);

	/** 
	 * Returns keys in the given section that match the given regex. Keys are 
	 * returned in the order they appear. Returns null if section does not exist, 
	 * and zero-length array if it does not contain entries or no entries matched
	 * the regex.
	 */
	String[] getMatchingKeys(String section, String regex);

	/** 
	 * Returns list of unique section names. 
	 */
	String[] getSectionNames();

	/**
	 * Returns true iff section exists.
	 */
	boolean containsSection(String section);

	/** 
	 * Removes all sections with that name, except the parts in readonly files.
	 * If there's no such section, nothing happens.  
	 * Throws an error if this entry is not editable (readonly).
	 * XXX what should happen if some is readonly and some is not?
	 */
	void removeSection(String section);
	
	/** 
	 * Adds a section. Throws an error if such section already exists. 
	 * (This effectively means that this interface does not support creating 
	 * and populating non-contiguous sections).  beforeSection==null means
	 * append.
	 */
	void addSection(String sectionName, String beforeSection);

	/** 
	 * Renames a section. Throws an error if a section with the new name 
	 * already exists. 
	 */
	void renameSection(String sectionName, String newName);

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
	 * Sets the comment on the section heading's line. 
	 * Sets the comment of the first heading if there're more than one. 
	 * Throws error if section doesn't exist, or if the section heading line 
	 * is readonly.
	 */
	void setSectionComment(String section, String comment);

	/**
	 * Interprets lineNumber as a position into the primary (edited) file,
	 * and returns which section it is part of; null is returned for 
	 * positions above the first section heading.
	 */
	String getSectionForLine(int lineNumber);
	
	/**
	 * Interprets lineNumber as a position into the primary (edited) file,
	 * and returns the key on which the cursor is positioned, or null if
	 * it is not a key-value line.
	 */
	String getKeyForLine(int lineNumber);

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
	 * Get user data object attached to the given key.
	 */
	public Object getKeyData(String section, String key);

	/**
	 * Attach a (single, unnamed) user data object to the given key.
	 */
	void setKeyData(String section, String key, Object data);

	/**
	 * Get user data object attached to the given section.
	 */
	Object getSectionData(String section);

	/**
	 * Attach a (single, unnamed) user data object to the given section.
	 */
	void setSectionData(String section, Object data);
	
	/**
     * Adds a listener to this document 
     */
	void addInifileChangeListener(IInifileChangeListener listener);

	/**
     * Removes a listener from this document 
     */
	void removeInifileChangeListener(IInifileChangeListener listener);

	/**
	 * Returns the main (edited) inifile.
	 */
	IFile getDocumentFile();
	
}
