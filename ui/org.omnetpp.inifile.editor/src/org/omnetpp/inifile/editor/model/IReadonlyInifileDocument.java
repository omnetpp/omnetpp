package org.omnetpp.inifile.editor.model;

import java.util.Collection;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IFile;

/**
 * Readonly part of IInifileDocument.
 *
 * @author tomi
 */
public interface IReadonlyInifileDocument {
    public class LineInfo {
        private IFile file;
        private int lineNumber;
        private int numLines;
        private boolean readOnly;

        public LineInfo(IFile file, int lineNumber, int numLines, boolean readOnly) {
            this.file = file;
            this.lineNumber = lineNumber;
            this.numLines = numLines;
            this.readOnly = readOnly;
        }

        public IFile getFile() {
            return file;
        }
        public int getLineNumber() {
            return lineNumber;
        }
        public int getNumLines() {
            return numLines;
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
     * Returns the value of the given entry. The value may be multi-line,
     * and does NOT contain comments. Returns null if section, or key
     * in it does not exist.
     */
    String getValue(String section, String key);

    /**
     * Returns the value of the given entry. The value may be multi-line,
     * and may include comments. Returns null if section, or key in it
     * does not exist.
     *
     * Fields editors should use this method instead of getValue().
     */
    String getRawValue(String section, String key);

    /**
     * Returns immutable value object with file/line/isReadonly info.
     * Returns null if section, or key in it does not exist.
     */
    LineInfo getEntryLineDetails(String section, String key);

    /**
     * Returns comment for the given key, or null if there's no comment.
     * The comment is returned without the leading "# " or "#".
     * Note: "" means empty comment ("#" followed by nothing.)
     * Throws error if key doesn't exist.
     */
    String getComment(String section, String key);

    /**
     * Returns the comment for the given key, including the leading "#" and
     * preceding whitespace. Returns "" if there is no comment (i.e. null
     * is never returned.)
     * Throws error if key doesn't exist.
     */
    String getRawComment(String section, String key);

    /**
     * Returns keys in the given section, in the order they appear.
     * Returns null if section does not exist, and zero-length array if it
     * does not contain entries.
     */
    Set<String> getKeys(String section);

    /**
     * Returns keys in the given section that match the given regex. Keys are
     * returned in the order they appear. Returns null if section does not exist,
     * and zero-length array if it does not contain entries or no entries matched
     * the regex.
     */
    List<String> getMatchingKeys(String section, String regex);

    /**
     * Returns list of unique section names.
     */
    String[] getSectionNames();

    /**
     * Returns true iff section exists.
     */
    boolean containsSection(String section);

    /**
     * Returns immutable value object with file/line/numLines/isReadonly info,
     * where the number of lines includes section content (not only the header).
     * Returns null if section does not exist.
     */
    LineInfo getSectionLineDetails(String section);

    /**
     * Returns comment on the section heading's line, or null if there's no comment.
     * The comment is returned without the leading "# " or "#".
     * Note: "" means empty comment ("#" followed by nothing.)
     * Throws error if section doesn't exist; returns comment of the first heading
     * if there're more than one.
     */
    String getSectionComment(String section);

    /**
     * Returns the comment on the section heading's line. Throws error if section
     * doesn't exist; returns comment of the first heading if there's more than one.
     * The comment is returned with the leading "#" and preceding whitespace.
     * Returns "" if there is no comment (i.e. null is never returned.)
     */
    String getRawSectionComment(String section);

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
     * Get user data object attached to the given key.
     */
    public Object getKeyData(String section, String key);

    /**
     * Get user data object attached to the given section.
     */
    Object getSectionData(String section);
    /**
     * Returns the main (edited) inifile.
     */
    IFile getDocumentFile();

    /**
     * Returns all included files, including indirectly referenced ones
     */
    IFile[] getIncludedFiles();

    /**
     * Returns true if there is any cycle in the section inheritance chains.
     */
    boolean containsSectionCycles();

    /**
     * Returns true if the given section is part of a cycle in the inheritance chain.
     */
    boolean isCausingCycle(String section);

    /**
     * Returns the collection of cycles found in the section inheritance chains.
     */
    Collection<Set<String>> getSectionChainCycles();

    /**
     * Follows the section "extends" chain back to the [General] section, and
     * returns the array of section names (including the given section and
     * [General] as well).
     *
     * The returned chain might by partial, if the section is part of a cycle
     * or the extended sections cannot be linearized due to a conflict.
     *
     * @return a non-empty array starting with {@code section}
     */
    String[] getSectionChain(String section);

    /**
     * Returns the names of conflicting sections found while computing the
     * section chain of the given section.
     */
    String[] getConflictingSections(String section);
}
