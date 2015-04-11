/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.model;


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
 * If there're non-unique keys, behavior is undefined: this interface
 * should not be used until this condition gets resolved.
 *
 * Note: it is not possible to distinguish multiple sections of the same name
 * (i.e. a non-contiguous section): this interface presents them as a single section.
 *
 * @author Andras
 */
public interface IInifileDocument extends IReadonlyInifileDocument {

    /**
     * Sets the value of the given entry. Throws error if key does not exist
     * in that section, or it is readonly.
     */
    void setValue(String section, String key, String value);

    /**
     * Creates a new entry. Throws error if already exists, or section or beforeKey does not exist.
     * @param comment should be "", or include the leading "#" and potential preceding whitespace
     * @param beforeKey may be null (meaning append)
     */
    void addEntry(String section, String key, String value, String rawComment, String beforeKey);

    /**
     * Creates several new entries in one operation. If any of the keys already exists,
     * the method throws an exception without changing the document. Also throws exception
     * when section or beforeKey does not exist, or any of the arrays are wrong (see below).
     * @param keys  may not be null or contain nulls
     * @param values may be null; when non-null, size must match keys[] and may contain nulls
     * @param rawComments may be null; when non-null, size must match keys[] and may contain nulls
     * @param beforeKey may be null (meaning append)
     */
    void addEntries(String section, String[] keys, String[] values, String[] rawComments, String beforeKey);

    /**
     * Sets the comment for an entry. Specify null to remove the comment.
     * The comment should be passed without the leading "# " or "#".
     * Note: "" means empty comment ("#" followed by nothing.)
     * Throws error if key doesn't exist, or the entry is readonly.
     */
    void setComment(String section, String key, String comment);

    /**
     * Sets the comment for an entry. The comment should be passed with
     * the leading "#" and preceding whitespace. Specify "" to
     * remove the comment (null is not accepted).
     * Throws error if key doesn't exist, the entry is readonly.
     */
    void setRawComment(String section, String key, String comment);

    /**
     * Renames the given key. Throws error if key doesn't exist, or the entry is readonly.
     */
    void renameKey(String section, String oldKey, String newKey);

    /**
     * Removes the given key from the given section. Nothing happens if it's not there.
     * Throws an error if this entry is not editable (readonly).
     */
    void removeKey(String section, String key);

    /**
     * Batch operation: removes the given entries in one go. The sections[] and keys[]
     * arrays must be the same size, and must not contain nulls.
     */
    void removeKeys(String[] sections, String keys[]);

    /**
     * Moves the given key before the given one. Throws error if the entry does
     * not exist, or beforeKey does not exist.
     * @param comment may be null
     * @param beforeKey may be null (meaning append)
     */
    void moveKey(String section, String key, String beforeKey);

    /**
     * Removes all sections with that name. If there's no such section, nothing happens.
     * If some parts are in included files (i.e. readonly parts), those parts are
     * not skipped, and an exception is thrown at the end of the operation.
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
     * Sets the comment on the section heading's line.
     * Sets the comment of the first heading if there're more than one.
     * The comment should be passed without the leading "# " or "#";
     * specify null to remove the comment.
     * Note: "" means empty comment ("#" followed by nothing.)
     * Throws error if section doesn't exist, or if the section heading line
     * is readonly.
     */
    void setSectionComment(String section, String comment);

    /**
     * Sets the comment on the section heading's line.
     * Sets the comment of the first heading if there're more than one.
     * The comment should be passed with the leading "#" and preceding whitespace.
     * Specify "" to remove the comment (null is not accepted.)
     * Throws error if section doesn't exist, or if the section heading line
     * is readonly.
     */
    void setRawSectionComment(String section, String comment);

    /**
     * Performs basic syntactic validation on the section name string
     * (not empty, contains no illegal characters, etc).
     * @return null if no problem, otherwise the description of the problem
     */
    String validateSectionName(String section);

    /**
     * Performs basic syntactic validation on the key string (not empty,
     * contains no illegal characters, etc).
     * @return null if no problem, otherwise the description of the problem
     */
    String validateKey(String key);

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
     * Attach a (single, unnamed) user data object to the given key.
     */
    void setKeyData(String section, String key, Object data);

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
     * Creates a read-only copy of this ini file.
     * The copy is cached and returned until this document has not been changed.
     */
    IReadonlyInifileDocument getImmutableCopy();

    /**
     * Checks if the given copy is the current one.
     */
    boolean isImmutableCopyUpToDate(IReadonlyInifileDocument copy);

    /**
     * Called when the editor is closed to allow freeing resources.
     */
    void dispose();

    /**
     * Force parsing the document now.
     */
    void parse();
}
