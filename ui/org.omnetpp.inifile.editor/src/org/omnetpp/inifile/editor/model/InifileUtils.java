package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.CONFIG_;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.GENERAL;
import static org.omnetpp.inifile.editor.model.ConfigRegistry.SCENARIO_;

import java.util.ArrayList;

import org.apache.commons.lang.ArrayUtils;
import org.eclipse.core.resources.IFile;
import org.eclipse.core.resources.IMarker;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.CoreException;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.engine.PatternMatcher;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.InifileEditorPlugin;
import org.omnetpp.inifile.editor.model.IInifileDocument.LineInfo;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Various lookups in inifiles, making use of NED declarations as well.
 * This class cannot be instantiated, all functionality is provided via
 * static methods.
 *
 * @author Andras
 */
public class InifileUtils {
	/**
	 * Looks up a configuration value in the given section or its fallback sections.
	 * Returns null if not found.
	 */
	public static String lookupConfig(String section, String key, IInifileDocument doc) {
		String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
		return lookupConfig(sectionChain, key, doc);
	}

	/**
	 * Looks up a configuration value. Returns null if not found.
	 */
	public static String lookupConfig(String[] sectionChain, String key, IInifileDocument doc) {
		for (String section : sectionChain)
			if (doc.containsKey(section, key))
				return doc.getValue(section, key);
		return null;
	}

	/**
	 * Given a parameter's fullPath, returns the key of the matching
	 * inifile entry, or null the parameter matches nothing. If hasDefault
	 * is set, ".apply-default" entries are also considered.
	 */
	public static SectionKey lookupParameter(String paramFullPath, boolean hasNedDefault, String[] sectionChain, IInifileDocument doc) {
		//
		//XXX this issue is much more complicated, as there may be multiple possibly matching
		// inifile entries. For example, we have "net.node[*].power", and inifile contains
		// "*.node[0..4].power=...", "*.node[5..9].power=...", and "net.node[10..].power=...".
		// Current code would not match any (!!!), only "net.node[*].power=..." if it existed.
		// lookupParameter() should actually return multiple matches.
		//
		String paramApplyDefault = paramFullPath + ".apply-default";
		boolean considerApplyDefault = hasNedDefault;
		for (String section : sectionChain) {
			for (String key : doc.getKeys(section)) {
				if (new PatternMatcher(key, true, true, true).matches(paramFullPath))
					return new SectionKey(section, key);
				if (considerApplyDefault && new PatternMatcher(key, true, true, true).matches(paramApplyDefault))
					if (doc.getValue(section, key).equals("true"))
						return new SectionKey(section, key);
					else
						considerApplyDefault = false;
			}
		}
		return null;
	}

	/**
	 * Returns the submodule name. If vector, appends [*].
	 */
	public static String getSubmoduleFullName(SubmoduleNode submodule) {
		String submoduleName = submodule.getName();
		if (!StringUtils.isEmpty(submodule.getVectorSize())) //XXX what if parsed expressions are in use?
			submoduleName += "[*]"; //XXX
		return submoduleName;
	}

	/**
	 * Chops off potential "Config " or "Scenario " prefix from a sectionName.
	 */
	public static String removeSectionNamePrefix(String sectionName) {
		return sectionName.replaceFirst(".+ +", "");
	}

	/**
	 * Resolves the run-time type of a "like" submodule, using the parameter
	 * settings in the inifile. Returns null if the lookup is unsuccessful.
	 */
	public static String resolveLikeParam(String moduleFullPath, SubmoduleNode submodule, String activeSection, InifileAnalyzer analyzer, IInifileDocument doc) {
		// get like parameter name
		String likeParamName = submodule.getLikeParam();
		if (!likeParamName.matches("[A-Za-z0-9_]+"))
			return null;  // sorry, we are only prepared to resolve parent module parameters (but not expressions)

		// look up parameter value
		ParamResolution res = analyzer.getResolutionForModuleParam(moduleFullPath, likeParamName, activeSection);
		if (res == null)
			return null; // likely no such parameter
		String value = InifileAnalyzer.getParamValue(res, doc);
		if (value == null)
			return null; // likely unassigned
		try {
			value = Common.parseQuotedString(value);
		} catch (RuntimeException e) {
			return null; // something is wrong: value is not a string constant?
		}
		return value;
	}

	/**
	 * Returns the submodule type name. If it uses "like", returns the "like" name.
	 */
	public static String getSubmoduleType(SubmoduleNode submodule) {
		//XXX should try to evaluate "like" expression and use result as type (if possible)
		String submoduleType = submodule.getType();
		if (StringUtils.isEmpty(submoduleType))
			submoduleType = submodule.getLikeType();
		return submoduleType;
	}

	/**
	 * Follows the section "extends" chain back to the [General] section, and
	 * returns the list of section names (including the given section and
	 * [General] as well).
	 *
	 * Errors (such as nonexistent section, cycle in the fallback chain, etc)
	 * are handled in a forgiving way, and a reasonably complete section chain
	 * is returned without throwing an exception -- so this method may be safely
	 * called during any calculation.
	 */
	public static String[] resolveSectionChain(IInifileDocument doc, String section) {
		ArrayList<String> sectionChain = new ArrayList<String>();
		if (!section.equals(GENERAL)) {
			String currentSection = section;
		    while (true) {
		    	if (!doc.containsSection(currentSection))
		            break; // error: nonexistent section
		        if (sectionChain.contains(currentSection))
		            break; // error: circle in the fallback chain
		        sectionChain.add(currentSection);
		        String extendsName = doc.getValue(currentSection, EXTENDS);
		        if (extendsName==null)
		        	break; // done
		        if (currentSection.startsWith(SCENARIO_) && doc.containsSection(SCENARIO_+extendsName))
			        currentSection = SCENARIO_+extendsName;
		        else
		        	currentSection = CONFIG_+extendsName;
		    }
		}
	    if (doc.containsSection(GENERAL))
	        sectionChain.add(GENERAL);
	    return sectionChain.toArray(new String[] {});
	}

	/**
	 * Whether the section chain contains the given section. Useful for detecting
	 * circles in the "extends" hierarchy.
	 */
	public static boolean sectionChainContains(IInifileDocument doc, String chainStartSection, String section) {
		String[] sectionChain = resolveSectionChain(doc, chainStartSection);
		return ArrayUtils.indexOf(sectionChain, section) >= 0;
	}

	/**
	 * Useful as a basis for an IInputValidator, e.g. for a cell editor or InputDialog.
	 */
	public static String validateParameterKey(IInifileDocument doc, String section, String key) {
		if (key==null || key.equals(""))
			return "Key cannot be empty";
		if (!key.contains("."))
			return "Parameter keys must contain at least one dot";
		if (doc.containsKey(section, key))
			return "Key "+key+" already exists in section ["+section+"]";
		return null;
	}

	/**
	 * Parse a boolean config value. Anything not recognized also counts as false.
	 */
	public static boolean parseAsBool(String value) {
		if (value.equals("yes") || value.equals("true") || value.equals("on") || value.equals("1"))
	    	return true;
	    else if (value.equals("no") || value.equals("false") || value.equals("off") || value.equals("0"))
	    	return false;
	    else
	    	return false; // unrecognized
	}

	/**
	 * Insert a section at the right place in the file.
	 * [General] at top, and other sections ordered alphabetically.
	 */
	public static void addSection(IInifileDocument doc, String newSection) {
		if (doc.containsSection(newSection))
			return;
		String[] sections = doc.getSectionNames();
		if (newSection.equals(GENERAL)) {
			doc.addSection(newSection, sections.length==0 ? null : sections[0]);
			return;
		}
		for (String section : sections) {
			if (section.compareToIgnoreCase(newSection) > 0 && !section.equals(GENERAL)) {
				doc.addSection(newSection, section);
				return;
			}
		}
		doc.addSection(newSection, null);
	}

	/**
	 * Insert a key at the right place in the file. Config keys at top
	 * (extends= first, description= next, network= after, and the
	 * rest follows in alphabetical order), then parameters.
	 * If even the section is not present, it is added first.
	 * The entry MUST NOT exist yet.
	 */
	public static void addEntry(IInifileDocument doc, String section, String newKey, String value, String comment) {
		if (!doc.containsSection(section))
			addSection(doc, section);
		Assert.isTrue(!doc.containsKey(section, newKey));
		String keys[] = doc.getKeys(section);
		for (String key : keys) {
			if (precedesKey(newKey, key)) {
				doc.addEntry(section, newKey, value, comment, key);
				return;
			}
		}
		doc.addEntry(section, newKey, value, comment, null);
	}

	private static boolean precedesKey(String key1, String key2) {
		if (key1.equals(CFGID_EXTENDS.getKey())) return true;
		if (key2.equals(CFGID_EXTENDS.getKey())) return false;
		if (key1.equals(CFGID_DESCRIPTION.getKey())) return true;
		if (key2.equals(CFGID_DESCRIPTION.getKey())) return false;
		if (key1.equals(CFGID_NETWORK.getKey())) return true;
		if (key2.equals(CFGID_NETWORK.getKey())) return false;
		if (key1.contains(".")) return false;
		if (key2.contains(".")) return true;
		return key1.compareToIgnoreCase(key2) < 0;
	}

	public static void addOrSetOrRemoveEntry(IInifileDocument doc, String section, String key, String value) {
		if (value==null) {
			// remove
			if (doc.containsKey(section, key))
				doc.removeKey(section, key);
		}
		else {
			// set or add
			if (!doc.containsKey(section, key))
				addEntry(doc, section, key, value, null);
			else if (!value.equals(doc.getValue(section, key)))
				doc.setValue(section, key, value);
		}
	}

	/**
	 * Renames the given section. Also changes the extends= keys in other sections
	 * that refer to it.
	 */
	public static void renameSection(IInifileDocument doc, String oldSectionName, String newSectionName) {
		doc.renameSection(oldSectionName, newSectionName);

		// change referring extends= keys in other sections 
		String oldName = removeSectionNamePrefix(oldSectionName);
		String newName = removeSectionNamePrefix(newSectionName);
		for (String section : doc.getSectionNames())
			if (oldName.equals(doc.getValue(section, EXTENDS)))
				doc.setValue(section, EXTENDS, newName);
	}

	/**
	 * Returns the problem markers for a given inifile entry.
	 */
	public static IMarker[] getProblemMarkersFor(String section, String key, IInifileDocument doc) {
		try {
			LineInfo line = doc.getEntryLineDetails(section, key);
			if (line==null)
				return new IMarker[0];
			IFile file = line.getFile();
			IMarker[] markers = file.findMarkers(IMarker.PROBLEM, true, 0);
			ArrayList<IMarker> result = new ArrayList<IMarker>();
			for (IMarker marker : markers) {
				int lineNumber = marker.getAttribute(IMarker.LINE_NUMBER, -1);
				if (lineNumber == line.getLineNumber())
					result.add(marker);
			}
			return result.toArray(new IMarker[]{});
		}
		catch (CoreException e) {
			InifileEditorPlugin.logError(e);
			return new IMarker[0];
		}
	}

	/**
	 * Returns the maximum severity of the given markers, or -1.
	 * Complements getProblemMarkersFor().
	 */
	public static int getMaximumSeverity(IMarker[] markers) {
		int severity = -1;
		for (IMarker marker : markers)
			if (severity < marker.getAttribute(IMarker.SEVERITY, -1))
				severity = marker.getAttribute(IMarker.SEVERITY, -1);
		return severity;
	}
}
