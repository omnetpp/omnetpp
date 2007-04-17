package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_PRELOAD_NED_FILES;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_USER_INTERFACE;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.GENERAL;

import java.util.ArrayList;
import java.util.Arrays;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.inifile.editor.model.ConfigurationEntry;
import org.omnetpp.inifile.editor.model.ConfigurationRegistry;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.InifileUtils;
import org.omnetpp.inifile.editor.model.ParamResolution;
import org.omnetpp.inifile.editor.model.InifileAnalyzer.KeyType;
import org.omnetpp.ned.model.interfaces.INEDTypeInfo;
import org.omnetpp.ned.resources.NEDResourcesPlugin;

/**
 * Generate proposals for config values, parameter values, per-object config values etc.
 * 
 * Note: although IContentProposalProvider is for field editors, we use this class
 * in the text editor content assist too; we just re-wrap IContentProposals to 
 * ICompletionProposal. 
 * 
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

	public void setInifileEntry(String section, String key) {
		this.section = section;
		this.key = key;
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.jface.fieldassist.IContentProposalProvider#getProposals(java.lang.String, int)
	 */
	public IContentProposal[] getProposals(String contents, int position) {
		ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();

		String prefix = contents.substring(0, position);
		String prefixToMatch = prefix.replaceFirst("^.*?([A-Za-z0-9_]*)$", "$1"); // the last incomplete word

		IContentProposal[] candidates = getProposalCandidates(prefix);

		if (candidates!=null) {
			Arrays.sort(candidates);

			// check if any of the proposals has description. If they do, we set "(no description)" 
			// on the others as well. Reason: if left at null, previous tooltip will be shown, 
			// which is very confusing.
			boolean descriptionSeen = false;
			for (IContentProposal p : candidates)
				if (!StringUtils.isEmpty(p.getDescription()))
					descriptionSeen = true;

			// collect those candidates that match the last incomplete word in the editor
			for (IContentProposal candidate : candidates) {
				String content = candidate.getContent();
				if (content.startsWith(prefixToMatch) && content.length()!= prefixToMatch.length()) {
					// from the content, drop the prefix that has already been typed by the user
					String modifiedContent = content.substring(prefixToMatch.length(), content.length());
					int modifiedCursorPosition = candidate.getCursorPosition() + modifiedContent.length() - content.length();
					String description = (StringUtils.isEmpty(candidate.getDescription()) && descriptionSeen) ? "(no description)" : candidate.getDescription();
					System.out.println("desc seen="+descriptionSeen+" XXX = "+description);
					result.add(new ContentProposal(modifiedContent, candidate.getLabel(), description, modifiedCursorPosition));
				}
			}
		}

		if (result.isEmpty()) {
			// returning an empty array or null apparently causes NPE in the framework, so return a blank proposal instead
			result.add(new ContentProposal("", "(no proposal)", null, 0));
		}
		return result.toArray(new IContentProposal[] {});
	}

	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before presenting them to the user.
	 */
	protected IContentProposal[] getProposalCandidates(String prefix) {
		KeyType keyType = (key == null) ? KeyType.CONFIG : InifileAnalyzer.getKeyType(key);
		if (keyType==KeyType.CONFIG) {
			return getCandidatesForConfig();
		}
		else if (keyType == KeyType.PARAM) {
			return getCandidatesForParam();
		}
		else if (keyType == KeyType.PER_OBJECT_CONFIG) {
			return getCandidatesForPerObjectConfig();
		}
		return null; // should not happen (invalid key type)
	}

	/**
	 * Generate proposals for a config entry
	 */
	protected IContentProposal[] getCandidatesForConfig() {
		ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
		if (entry == null)
			return null;

		if (entry==CFGID_EXTENDS) {
			ArrayList<String> names = new ArrayList<String>();
			for (String section : doc.getSectionNames())
				if (!section.equals(GENERAL) && !InifileUtils.sectionChainContains(doc, section, this.section)) // prevent circles
					names.add(section.replaceFirst("^Config +", ""));
			return toProposals(names.toArray(new String[]{}));
		}
		if (entry==CFGID_NETWORK) {
			ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
			for (INEDTypeInfo ned : NEDResourcesPlugin.getNEDResources().getNetworks())
				result.add(new ContentProposal(ned.getName(), ned.getName(), ned.getNEDElement().getComment()));
			return result.toArray(new IContentProposal[] {});
		}
		if (entry==CFGID_PRELOAD_NED_FILES) {
			return toProposals(new String[] {"*.ned"});
		}
		if (entry==CFGID_USER_INTERFACE) {
			return toProposals(new String[] {"Cmdenv", "Tkenv"});
		}
		if (entry.getType()==ConfigurationEntry.Type.CFG_BOOL) {
			return toProposals(new String[] {"true", "false"});
		}
		return null;
	}

	/** 
	 * Generate proposals for a module parameter key
	 */
	protected IContentProposal[] getCandidatesForParam() {
		if (analyzer==null)
			return null; // sorry

		return toProposals(new String[] {"TODO", "FIXME", "XXX", section, key}); //XXX

//		ParamResolution[] resList = analyzer.getParamResolutionsForKey(section, key);
//		for (ParamResolution res : resList)
//			; //TODO
//		return null; //XXX
	}

	/**
	 * Generate proposals for per-object configuration keys
	 */
	protected IContentProposal[] getCandidatesForPerObjectConfig() {
		//XXX TODO...		
		//		if (key.endsWith(".apply-default") || key.endsWith(".ev-output")) {
		//			proposals.add("true\n");
		//			proposals.add("false\n");
		//		}
		//		if (key.endsWith(".record-interval")) { 
		//			proposals.add("$1..");
		//			proposals.add("$1..$2");
		//			proposals.add("..$2"); //XXX use templated proposals here!
		//		}
		return null;
	}
	
	protected static IContentProposal[] toProposals(String[] strings) {
		IContentProposal[] p = new IContentProposal[strings.length];
		for (int i=0; i<p.length; i++)
			p[i] = new ContentProposal(strings[i], strings[i], null);
		return p;
	}
	
}
