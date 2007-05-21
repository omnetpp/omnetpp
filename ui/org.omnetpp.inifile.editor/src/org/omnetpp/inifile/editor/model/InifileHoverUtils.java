package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.core.resources.IMarker;
import org.omnetpp.common.ui.HoverSupport;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.model.ex.ParamNodeEx;
import org.omnetpp.ned.model.pojo.ParamNode;
import org.omnetpp.ned.model.pojo.SubmoduleNode;

/**
 * Produces hover information for various Inifile parts.
 * 
 * @author Andras
 */
public class InifileHoverUtils {
	/**
	 * Generates tooltip for an inifile section.
	 * @param section  null is accepted
	 */
	public static String getSectionHoverText(String section, IInifileDocument doc, InifileAnalyzer analyzer) {
		if (section == null || !doc.containsSection(section))
			return null;

		// name and description
		String text = getProblemsHoverText(section, null, doc);
		text += "<b>"+section+"</b>";
		String description = doc.getValue(section, "description");
		if (description != null)
			text += " -- " + description;
		text += "<br>\n";

		// section chain
		String[] sectionChain = InifileUtils.resolveSectionChain(doc, section);
		if (sectionChain.length >= 2)
			text += "Fallback order: " + StringUtils.join(sectionChain, " &gt; ") + " <br>\n"; //XXX decide terminology: "Lookup order" or "Section fallback chain" ? also: "," or ">" ?

		// network
		String networkName = InifileUtils.lookupConfig(sectionChain, CFGID_NETWORK.getKey(), doc);
		text += "Network: " + (networkName==null ? "not set" : networkName) + " <br>\n";

		// unassigned parameters
		if (analyzer != null) {
			ParamResolution[] resList = analyzer.getUnassignedParams(section);
			if (resList.length==0) {
				if (networkName != null)
					text += "<br>\nThis section seems to contain no unassigned NED parameters.";
			}
			else {
				text += "<br>\nThis section does not seem to assign the following NED parameters:\n<ul>";
				for (ParamResolution res : resList)
					text += " <li>" + res.moduleFullPath + "." +res.paramDeclNode.getName() + "</li>\n";
				text += "</ul>";
			}
		}
		return HoverSupport.addHTMLStyleSheet(text);
	}

	/**
	 * Generate tooltip for an inifile entry.
	 * @param section  null is accepted
	 * @param key      null is accepted
	 */
	//XXX should tolerate analyzer==null
	public static String getEntryHoverText(String section, String key, IInifileDocument doc, InifileAnalyzer analyzer) {
		if (section == null || key == null || !doc.containsKey(section, key))
			return null;

		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.CONFIG) {
			// config key: display description
			return getConfigHoverText(section, key, doc);
		}
		else if (keyType == KeyType.PARAM || key.endsWith(".apply-default")) { //XXX hardcoded key name
			// parameter assignment: display which parameters it matches
			return getParamKeyHoverText(section, key, analyzer);
		}
		else if (keyType == KeyType.PER_OBJECT_CONFIG) {
			return null; // TODO display which modules it applies to, plus comment maybe?
		}
		else {
			return null; // should not happen (invalid key type)
		}
	}

	/**
	 * Generates tooltip for a config entry.
	 */
	public static String getConfigHoverText(String section, String key, IInifileDocument doc) {
		String text = getProblemsHoverText(section, key, doc);
		ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
		if (entry == null)
			return text;

		text += "<b>[General]"+(entry.isGlobal() ? "" : " or [Config X]")+" / "+entry.getKey();
		text += " = &lt;" + entry.getDataType().name().replaceFirst("CFG_", "");
		if (entry.getDefaultValue()!=null && !entry.getDefaultValue().equals(""))
			text += ", default: " + entry.getDefaultValue();
		if (entry.getUnit()!=null)
			text += ", unit: "+entry.getUnit();
		text += "&gt;</b><br>\n<br>\n";
		text += entry.getDescription() + "<br>\n";

		if (doc != null && entry!=CFGID_DESCRIPTION && entry!=CFGID_EXTENDS) {
			List<String> sectionList = new ArrayList<String>();
			for (String sec : doc.getSectionNames())
				if (doc.containsKey(sec, entry.getKey()))
					sectionList.add(sec);
			if (sectionList.size()==0)
				text += "<br>\nCurrently not set in any sections.<br>\n";
			else
				text += "<br>\nSet in the following sections: <ul><li>"+StringUtils.join(sectionList.toArray(), "</li><li>")+"</li></ul><br>\n";
		}

		return HoverSupport.addHTMLStyleSheet(text);
	}

	/**
	 * Generate tooltip for a param key entry
	 */
	public static String getParamKeyHoverText(String section, String key, InifileAnalyzer analyzer) {
		//XXX somehow merge similar entries? (i.e. where pathModules[] and paramValueNode/paramDeclNode are the same)
		String text = getProblemsHoverText(section, key, analyzer.getDocument());
		text += "<b>[" + section + "] / " + key + "</b><br>\n";
		ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
		if (resList.length==0) {
			text += "Does not match any module parameters.";
			return HoverSupport.addHTMLStyleSheet(text);
		}

		// merge similar entries
		Set<ParamNode> paramDeclNodes = new LinkedHashSet<ParamNode>();
		for (ParamResolution res : resList)
			paramDeclNodes.add(res.paramDeclNode);

		text += "Applies to the following module parameters: <br>\n";
		for (ParamNode paramDeclNode : paramDeclNodes) {
			String paramName = paramDeclNode.getName();
			String paramType = paramDeclNode.getAttribute(ParamNode.ATT_TYPE);
			String paramDeclaredOn = ((ParamNodeEx)paramDeclNode).getContainerNEDTypeInfo().getName();
			String comment = StringUtils.makeBriefDocu(paramDeclNode.getComment(), 60);
			String optComment = comment==null ? "" : (" -- \"" + comment + "\"");

			text += "<br>"+paramDeclaredOn + "." + paramName + " : "+ paramType + optComment + "\n<ul>\n";

			for (ParamResolution res : resList)
				if (res.paramDeclNode == paramDeclNode)
					text +=	" <li>" + res.moduleFullPath + (section.equals(res.activeSection) ? "" : ", for sub-config ["+res.activeSection+"]") + "</li>\n";
			text += "</ul>";
		}
		return HoverSupport.addHTMLStyleSheet(text);
	}

	public static String getProblemsHoverText(String section, String key, IInifileDocument doc) {
		IMarker[] markers = InifileUtils.getProblemMarkersFor(section, key, doc);
		if (markers.length==0) 
			return "";
		
		String text = "";
		for (IMarker marker : markers) {
			String severity = "";
			switch (marker.getAttribute(IMarker.SEVERITY, -1)) {
				case IMarker.SEVERITY_ERROR: severity = "Error"; break;
				case IMarker.SEVERITY_WARNING: severity = "Warning"; break;
				case IMarker.SEVERITY_INFO: severity = "Info"; break;
			}
			text += "<i>"+severity+": " + marker.getAttribute(IMarker.MESSAGE, "") + "</i><br/>\n";
		}
		return text+"<br/>";
	}
	
	/**
	 * Generate tooltip for a NED parameter
	 */
	public static String getParamHoverText(SubmoduleNode[] pathModules, ParamNode paramDeclNode, ParamNode paramValueNode) {
		String paramName = paramDeclNode.getName();
		
		String paramType = paramDeclNode.getAttribute(ParamNode.ATT_TYPE);
		String paramDeclaredOn = ((ParamNodeEx)paramDeclNode).getContainerNEDTypeInfo().getName();
		String comment = StringUtils.makeBriefDocu(paramDeclNode.getComment(), 60);
		String optComment = comment==null ? "" : (" -- \"" + comment + "\"");

		String text = ""; //TODO
		text += paramDeclaredOn + "." + paramName + " : "+ paramType + optComment + "\n";
		//XXX more info.....
		return HoverSupport.addHTMLStyleSheet(text);
	}
	
}
