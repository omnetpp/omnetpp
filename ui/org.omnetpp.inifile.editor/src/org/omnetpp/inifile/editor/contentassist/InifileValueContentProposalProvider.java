package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.*;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.Arrays;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.engine.Common;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Generate proposals for config values, parameter values, per-object config values etc.
 * @author Andras
 */
public class InifileValueContentProposalProvider implements	IContentProposalProvider {
	private String section;
	private String key;
	private IInifileDocument doc;
	private InifileAnalyzer analyzer;

	/**
	 * Value object that implements IContentProposal.
	 */
	static class ContentProposal implements IContentProposal, Comparable {
		private String content;
		private String label;
		private String description;
		private int cursorPosition;
		
		public ContentProposal(String content, String label, String description) {
			this(content, label, description, content.length());
		}

		public ContentProposal(String content, String label, String description, int cursorPosition) {
			this.content = content;
			this.label = label;
			this.description = description;
			this.cursorPosition = cursorPosition;
		}

		public String getContent() {
			return content;
		}

		public int getCursorPosition() {
			return cursorPosition;
		}

		public String getDescription() {
			return description;
		}

		public String getLabel() {
			return label;
		}

		public int compareTo(Object o) {
			return label.compareTo(((IContentProposal)o).getLabel());
		}
	}
	
	public InifileValueContentProposalProvider(String section, String key, IInifileDocument doc, InifileAnalyzer analyzer) {
		this.section = section;
		this.key = key;
		this.doc = doc;
		this.analyzer = analyzer;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.jface.fieldassist.IContentProposalProvider#getProposals(java.lang.String, int)
	 */
	public IContentProposal[] getProposals(String contents, int position) {
		ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();

		String prefix = contents.substring(0, position);
		String prefixToMatch = prefix.replaceFirst("^.*?([A-Za-z0-9_]*)$", "$1"); // the last incomplete word

		IContentProposal[] candidates = getCandidates(prefix);
		Arrays.sort(candidates);

		// collect those candidates that match the last incomplete word in the editor
		if (candidates!=null) {
			for (IContentProposal candidate : candidates) {
				String content = candidate.getContent();
				if (content.startsWith(prefixToMatch) && content.length()!= prefixToMatch.length()) {
					// from the content, drop the prefix that has already been typed by the user
					String modifiedContent = content.substring(prefixToMatch.length(), content.length());
					int modifiedCursorPosition = candidate.getCursorPosition() + modifiedContent.length() - content.length();
					result.add(new ContentProposal(modifiedContent, candidate.getLabel(), candidate.getDescription(), modifiedCursorPosition));
				}
			}
		}

		if (result.isEmpty()) {
			// returning an empty array or null apparently causes NPE in the framework, so return a blank proposal instead
			result.add(new ContentProposal("", "(no proposal)", null, 0));
		}
		return result.toArray(new IContentProposal[] {});
	}

	protected IContentProposal[] getCandidates(String prefix) {
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.CONFIG) {
			return getCandidatesForConfig();
		}
		else if (keyType == KeyType.PARAM) {
//			// parameter assignment: display which parameters it matches
//			ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
//			if (resList.length==0) 
//				return "Entry \"" + key + "\" does not match any module parameters ";
//			String text = "Entry \"" + key + "\" applies to the following module parameters: \n";
//			for (ParamResolution res : resList)
//				text += "  - " + res.moduleFullPath + "." +res.paramNode.getName() + "\n"; //XXX do we have module type, param type, maybe param doc etc?
//			return text;
			return null;
		}
		else if (keyType == KeyType.PER_OBJECT_CONFIG) {
			return null; // TODO for .apply-default, display parameters to which it applies
		}
		else {
			return null; // should not happen (invalid key type)
		}
	}

	protected IContentProposal[] getCandidatesForConfig() {
		ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
		if (entry == null)
			return null;

		if (entry==CFGID_EXTENDS)
			return toProposals(doc.getSectionNames()); //XXX strip "Config "
		if (entry==CFGID_NETWORK)
			return toProposals(NEDResourcesPlugin.getNEDResources().getModuleNames().toArray(new String[] {}));  //XXX use getNetworkNames()
		if (entry==CFGID_PRELOAD_NED_FILES)
			return toProposals(new String[] {"*.ned"});
		if (entry==CFGID_USER_INTERFACE)
			return toProposals(new String[] {"Cmdenv", "Tkenv"});
		if (entry.getType()==ConfigurationEntry.Type.CFG_BOOL)
			return toProposals(new String[] {"true", "false"});
		return null;
	}

	protected static IContentProposal[] toProposals(String[] strings) {
		IContentProposal[] p = new IContentProposal[strings.length];
		for (int i=0; i<p.length; i++)
			p[i] = new ContentProposal(strings[i], strings[i], null);
		return p;
	}
	
}
