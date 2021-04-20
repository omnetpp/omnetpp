package org.omnetpp.inifile.editor.model;

import java.util.ArrayList;
import java.util.Collection;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.resources.IFile;
import org.eclipse.core.runtime.Assert;
import org.omnetpp.inifile.editor.model.InifileDocument.KeyValueLine;
import org.omnetpp.inifile.editor.model.InifileDocument.Line;
import org.omnetpp.inifile.editor.model.InifileDocument.Section;
import org.omnetpp.inifile.editor.model.InifileDocument.SectionHeadingLine;

/**
 * Immutable implementation of IReadonlyInifileDocument.
 * Used for creating snapshots of InifileDocuments.
 *
 * @author tomi
 */
final class ImmutableInifileDocument implements IReadonlyInifileDocument {

    private IFile documentFile; // the file of the document

    // primary data structure: sections, keys
    private LinkedHashMap<String,Section> sections;
    private Collection<Set<String>> sectionsCausingCycles;

    // reverse (linenumber-to-section/key) mapping
    private ArrayList<SectionHeadingLine> mainFileSectionHeadingLines;
    private ArrayList<KeyValueLine> mainFileKeyValueLines;

    // included files, including indirectly referenced ones
    private ArrayList<IFile> includedFiles;

    /**
     * Copies the content of doc into this immutable document.
     */
    @SuppressWarnings("unchecked")
    ImmutableInifileDocument(InifileDocument doc) {
        try {
            doc.parseIfChanged();
            this.documentFile = doc.documentFile;
            this.sections = (LinkedHashMap<String,Section>) doc.sections.clone();
            for (Map.Entry<String, Section> entry : this.sections.entrySet())
                entry.setValue(entry.getValue().clone());
            this.sectionsCausingCycles = new ArrayList<Set<String>>(doc.sectionsCausingCycles);
            this.mainFileSectionHeadingLines = (ArrayList<SectionHeadingLine>) doc.mainFileSectionHeadingLines.clone();
            for (ListIterator<SectionHeadingLine> line = this.mainFileSectionHeadingLines.listIterator(); line.hasNext(); )
                line.set(line.next().clone());
            this.mainFileKeyValueLines = (ArrayList<KeyValueLine>) doc.mainFileKeyValueLines.clone();
            for (ListIterator<KeyValueLine> line = this.mainFileKeyValueLines.listIterator(); line.hasNext(); )
                line.set(line.next().clone());
            this.includedFiles = (ArrayList<IFile>) doc.includedFiles.clone();
        }
        catch (CloneNotSupportedException e) {
            Assert.isTrue(false);
        }
    }

    public boolean containsKey(String section, String key) {
        return lookupEntry(section, key) != null;
    }

    private KeyValueLine lookupEntry(String sectionName, String key) {
        Section section = sections.get(sectionName);
        return section == null ? null : section.entries.get(key);
    }

    public String getValue(String section, String key) {
        KeyValueLine line = lookupEntry(section, key);
        return line == null ? null : line.value;
    }

    public String getRawValue(String section, String key) {
        KeyValueLine line = lookupEntry(section, key);
        return line == null ? null : line.rawValue;
    }

    public LineInfo getEntryLineDetails(String section, String key) {
        KeyValueLine line = lookupEntry(section, key);
        return line==null ? null : new LineInfo(line.file, line.lineNumber, line.numLines, true);
    }

    public String getComment(String section, String key) {
        return stripCommentPrefix(getRawComment(section, key));
    }

    private static String stripCommentPrefix(String rawComment) {
        // strip leading whitespace, "#" and one space if possible; return null if no "#"
        return rawComment.contains("#") ? rawComment.replaceFirst("^\\s*# ?", "") : null;
    }

    public String getRawComment(String section, String key) {
        return getEntry(section, key).rawComment;
    }

    private KeyValueLine getEntry(String sectionName, String key) {
        KeyValueLine line = lookupEntry(sectionName, key);
        if (line == null)
            throw new IllegalArgumentException("No such entry: ["+sectionName+"]/"+key);
        return line;
    }

    public Set<String> getKeys(String sectionName) {
        Section section = sections.get(sectionName);
        return section == null ? null : section.entries.keySet();
    }

    public List<String> getMatchingKeys(String sectionName, String regex) {
        Section section = sections.get(sectionName);
        if (section == null)
            return null;
        ArrayList<String> list = new ArrayList<String>();
        for (String key : section.entries.keySet())
            if (regex.matches(key))
                list.add(key);
        return list;
    }

    public String[] getSectionNames() {
        return sections.keySet().toArray(new String[0]);
    }

    public boolean containsSection(String section) {
        return sections.containsKey(section);
    }

    public LineInfo getSectionLineDetails(String sectionName) {
        SectionHeadingLine line = lookupPreferredSectionHeading(sectionName);
        if (line == null)
            return null;
        return new LineInfo(line.file, line.lineNumber, line.lastLine-line.lineNumber+1, true);
    }

    public String getSectionComment(String section) {
        return stripCommentPrefix(getRawSectionComment(section));
    }

    public String getRawSectionComment(String sectionName) {
        SectionHeadingLine line = lookupPreferredSectionHeading(sectionName);
        if (line == null)
            throw new IllegalArgumentException("Section does not exist: ["+sectionName+"]");
        return line.rawComment;
    }

    /**
     * Returns the first editable section heading, or if none are editable, the first one.
     * Returns null if there's no such section.
     */
    private SectionHeadingLine lookupPreferredSectionHeading(String sectionName) {
        Section section = sections.get(sectionName);
        if (section == null)
            return null;
        for (SectionHeadingLine line : section.headingLines)
            if (isEditable(line))
                return line;
        return section.headingLines.get(0);
    }

    private boolean isEditable(Line line) {
        return line.file == documentFile;
    }

    public Object getKeyData(String section, String key) {
        return getEntry(section, key).data;
    }

    public Object getSectionData(String sectionName) {
        Section section = sections.get(sectionName);
        if (section == null)
            throw new IllegalArgumentException("No such section: ["+sectionName+"]");
        return section.data;
    }

    public IFile getDocumentFile() {
        return documentFile;
    }

    public IFile[] getIncludedFiles() {
        return includedFiles.toArray(new IFile[]{});
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

        // find key in that section (note: linear search, optimize if needed)
        for (KeyValueLine line : mainFileKeyValueLines)
            if (line.lineNumber <= lineNumber && lineNumber < line.lineNumber+line.numLines)
                return line.key;
        return null;
    }

    public boolean containsSectionCycles() {
        return !sectionsCausingCycles.isEmpty();
    }

    public boolean isCausingCycle(String section) {
        for (Set<String> cycle : sectionsCausingCycles)
            if (cycle.contains(section))
                return true;
        return false;
    }

    public Collection<Set<String>> getSectionChainCycles() {
        return sectionsCausingCycles;
    }

    public String[] getSectionChain(String sectionName) {
        Section section = sections.get(sectionName);
        if (section == null)
            throw new IllegalArgumentException("No such section: ["+sectionName+"]");
        return section.sectionChain;
    }

    public String[] getConflictingSections(String sectionName) {
        Section section = sections.get(sectionName);
        if (section == null)
            throw new IllegalArgumentException("No such section: ["+sectionName+"]");
        return section.sectionChainConflict != null ? section.sectionChainConflict.toArray(new String[0]) : null;
    }
}
