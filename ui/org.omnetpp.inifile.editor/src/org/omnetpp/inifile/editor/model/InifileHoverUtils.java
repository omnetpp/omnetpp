package org.omnetpp.inifile.editor.model;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_DESCRIPTION;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.internal.text.html.BrowserInformationControl;
import org.eclipse.jface.internal.text.html.HTMLTextPresenter;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.editors.text.EditorsUI;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.model.ex.ParamNodeEx;
import org.omnetpp.ned.model.pojo.ParamNode;

/**
 * Produces hover information for various Inifile parts.
 * 
 * @author Andras
 */
public class InifileHoverUtils {
	// Note: this is a copy of the stylesheet used by JDT (we want to avoid depending on the JDT plugins)
	private static final String HTML_PROLOG = 
		"<html><head><style CHARSET=\"ISO-8859-1\" TYPE=\"text/css\">\n" +
		"/* Font definitions*/\n" +
		"html   { font-family: 'Tahoma',sans-serif; font-size: 8pt; font-style: normal; font-weight: normal; }\n" +
		"body, h1, h2, h3, h4, h5, h6, p, table, td, caption, th, ul, ol, dl, li, dd, dt { font-size:1em; }\n" +
		"pre    { font-family: monospace; }\n" +
		"/* Margins */\n" +
		"body   { overflow: auto; margin-top: 0px; margin-bottom: 0.5em; margin-left: 0.3em; margin-right: 0px; }\n" +
		"h1     { margin-top: 0.3em; margin-bottom: 0.04em; }\n" +
		"h2     { margin-top: 2em; margin-bottom: 0.25em; }\n" +
		"h3     { margin-top: 1.7em; margin-bottom: 0.25em; }\n" +
		"h4     { margin-top: 2em; margin-bottom: 0.3em; }\n" +
		"h5     { margin-top: 0px; margin-bottom: 0px; }\n" +
		"p      { margin-top: 1em; margin-bottom: 1em; }\n" +
		"pre    { margin-left: 0.6em; }\n" +
		"ul     { margin-top: 0px; margin-bottom: 1em; }\n" +
		"li     { margin-top: 0px; margin-bottom: 0px; }\n" +
		"li p   { margin-top: 0px; margin-bottom: 0px; }\n" +
		"ol     { margin-top: 0px; margin-bottom: 1em; }\n" +
		"dl     { margin-top: 0px; margin-bottom: 1em; }\n" +
		"dt     { margin-top: 0px; margin-bottom: 0px; font-weight: bold; }\n" +
		"dd     { margin-top: 0px; margin-bottom: 0px; }\n" +
		"/* Styles and colors */\n" +
		"a:link    { color: #0000FF; }\n" +
		"a:hover   { color: #000080; }\n" +
		"a:visited { text-decoration: underline; }\n" +
		"h4        { font-style: italic; }\n" +
		"strong    { font-weight: bold; }\n" +
		"em        { font-style: italic; }\n" +
		"var       { font-style: italic; }\n" +
		"th        { font-weight: bold; }\n" +
		"</style></head>\n" + 
		"<body text=\"#000000\" bgcolor=\"#ffffe1\">\n";
	private static final String HTML_EPILOG = 
		"</body></html>\n";

	/**
	 * Generates tooltip for an inifile section.
	 * @param section  null is accepted
	 */
	public static String getSectionTooltip(String section, IInifileDocument doc, InifileAnalyzer analyzer) {
		if (section == null || !doc.containsSection(section))
			return null;

		// name and description
		String text = "<b>"+section+"</b>";
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
				int n = Math.min(resList.length, 8);
				for (int i=0; i<n; i++) {
					ParamResolution res = resList[i];
					text += " <li>" + res.moduleFullPath + "." +res.paramDeclNode.getName() + "</li>\n";
				}
				if (resList.length > n)
					text += " <li>...</li>\n";
				text += "</ul>";
			}
		}
		return addHTMLStyleSheet(text);
	}

	/**
	 * Generate tooltip for an inifile entry.
	 * @param section  null is accepted
	 * @param key      null is accepted
	 */
	//XXX should tolerate analyzer==null
	public static String getEntryTooltip(String section, String key, IInifileDocument doc, InifileAnalyzer analyzer) {
		if (section == null || key == null || !doc.containsKey(section, key))
			return null;

		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.CONFIG) {
			// config key: display description
			ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
			if (entry == null)
				return null;
			return getConfigTooltip(entry, doc);
		}
		else if (keyType == KeyType.PARAM || key.endsWith(".apply-default")) { //XXX hardcoded key name
			// parameter assignment: display which parameters it matches
			return getParamKeyTooltip(section, key, analyzer);
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
	public static String getConfigTooltip(ConfigurationEntry entry, IInifileDocument doc) {
		String text = "<b>[General]"+(entry.isGlobal() ? "" : " or [Config X]")+" / "+entry.getKey();
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

		return addHTMLStyleSheet(text);
	}

	/**
	 * Generate config for a param key entry
	 */
	public static String getParamKeyTooltip(String section, String key, InifileAnalyzer analyzer) {
		//XXX somehow merge similar entries? (ie where pathModules[] and paramValueNode/paramDeclNode are the same)
		String text = "<b>[" + section + "] / " + key + "</b><br>\n";
		ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
		if (resList.length==0) {
			text += "Does not match any module parameters.";
			return addHTMLStyleSheet(text);
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

//			int count = 0;
			for (ParamResolution res : resList) {
				if (res.paramDeclNode == paramDeclNode) {
//					if (++count > 4) {
//						text += "   ...<br>\n";
//						break;
//					}
					text +=	" <li>" + res.moduleFullPath + (section.equals(res.activeSection) ? "" : ", for sub-config ["+res.activeSection+"]") + "</li>\n";
				}
			}
			text += "</ul>";
		}
		return addHTMLStyleSheet(text);

//		int n = Math.min(resList.length, 8);
//		for (int i=0; i<n; i++) {
//			ParamResolution res = resList[i];
//			String paramName = res.paramValueNode.getName();
//			String paramDeclaredOn = ((ParamNodeEx)res.paramDeclNode).getContainerNEDTypeInfo().getName();
//			String paramType = res.paramDeclNode.getAttribute(ParamNode.ATT_TYPE);
//			String comment = makeBriefDocu(res.paramDeclNode.getComment(), 40);
//			String optComment = comment==null ? "" : (" -- \"" + comment + "\"");
//			text += "  - " + res.moduleFullPath + "." +paramName;
//			text += " (" + paramDeclaredOn + "." + paramName + " : "+ paramType + optComment + ")";
//			text +=	(section.equals(res.activeSection) ? "" : ", for sub-config ["+res.activeSection+"]") + "<br>\n"; //XXX do we have module type, maybe param doc etc?
//		}
//		if (resList.length > n)
//			text += "    ...<br>\n";
//		return text;
	}

	@SuppressWarnings("restriction")
	private static String addHTMLStyleSheet(String htmlText) {
		return HTML_PROLOG + htmlText + HTML_EPILOG;
	}
	
}
