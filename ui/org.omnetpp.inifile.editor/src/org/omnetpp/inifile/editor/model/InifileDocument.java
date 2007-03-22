package org.omnetpp.inifile.editor.model;

import java.util.LinkedHashMap;

import org.eclipse.core.resources.IFile;
import org.eclipse.jface.text.IRegion;

/**
 * Standard implementation of IInifileDocument.
 * @author Andras
 */
public class InifileDocument implements IInifileDocument {
	/** Setters change the underlying IDocument */
	public class IniLine {
		private IFile file;
		private IRegion region;
		private String comment;
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
	
	
	public String getValue(String section, String key) {
		return null; //TODO
	}

	public void setValue(String section, String key, String value) {
		//TODO
	}

	public void addEntry(String section, String key, String value, String comment, String beforeKey) {
		//TODO
	}
	
	public LineInfo getEntryLineDetails(String section, String key) {
		return null; //TODO
	} 

	public String getComment(String section, String key) {
		return null; //TODO
	}
	
	public void setComment(String section, String key, String comment) {
		//TODO
	}
	
	public void removeKey(String section, String key) {
		//TODO
	}
	
	public String[] getKeys(String section) {
		return null; //TODO
	}
	
	public String[] getSectionNames() {
		return null; //TODO
	}
	
	public void removeSection(String section) {
		//TODO
	}
	
	public void addSection(String sectionName, String beforeSection) {
		//TODO
	}
	
	public LineInfo getSectionLineDetails(String section) {
		return null; //TODO
	} 

	public String getSectionComment(String section) {
		return null; //TODO
	}

	public void setSectionComment(String section, String comment) {
		//TODO
	}
	
	public String[] getTopIncludes() {
		return null; //TODO
	}

	public void addTopInclude(String include, String before) {
		//TODO
	}

	public void removeTopInclude(String include) {
		//TODO
	}
	
	public String[] getBottomIncludes() {
		return null; //TODO
	}

	public void addBottomInclude(String include, String before) {
		//TODO
	}

	public void removeBottomInclude(String include) {
		//TODO
	}
	
}
