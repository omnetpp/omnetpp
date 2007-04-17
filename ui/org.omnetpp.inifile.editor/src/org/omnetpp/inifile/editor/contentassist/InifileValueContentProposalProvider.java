package org.omnetpp.inifile.editor.contentassist;

import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_EXTENDS;
import static org.omnetpp.inifile.editor.model.ConfigurationRegistry.CFGID_NETWORK;

import java.util.ArrayList;
import java.util.Arrays;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
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
		String lastIncompleteWord = prefix.replaceFirst("^.*?([A-Za-z0-9_]*)$", "$1");

		String[] candidates = getCandidates(prefix);
		Arrays.sort(candidates);

		// collect those candidates that match the last incomplete word in the editor
		if (candidates!=null) {
			for (final String candidate : candidates) {
				if (candidate.startsWith(lastIncompleteWord) && candidate.length()!= lastIncompleteWord.length()) {
					final String content = candidate.substring(lastIncompleteWord.length(), candidate.length());
					result.add(new IContentProposal() {
						public String getContent() {
							return content;
						}
						public String getDescription() {
							return null;
						}
						public String getLabel() {
							return candidate + " (inserts \""+content+"\")"; //XXX
						}
						public int getCursorPosition() {
							return content.length();
						}
					});
				}
			}
		}

		if (result.isEmpty()) {
			// returning an empty array or null apparently causes NPE in the framework, 
			// so return a blank proposal instead
			result.add(new IContentProposal() {
				public String getContent() {
					return "";
				}
				public String getDescription() {
					return null;
				}
				public String getLabel() {
					return "(no proposal)";
				}
				public int getCursorPosition() {
					return 0;
				}
			});
		}

		return result.toArray(new IContentProposal[] {});
	}

	protected String[] getCandidates(String prefix) {
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

	protected String[] getCandidatesForConfig() {
		ConfigurationEntry entry = ConfigurationRegistry.getEntry(key);
		if (entry == null)
			return null;

		if (entry==CFGID_EXTENDS) {
			return doc.getSectionNames(); //XXX strip "Config "
		}
		if (entry==CFGID_NETWORK) {
			return NEDResourcesPlugin.getNEDResources().getModuleNames().toArray(new String[] {});  //XXX use getNetworkNames()
		}

//
//		// check value: if it is the right type
//		String value = doc.getValue(section, key);
//		String errorMessage = validateConfigValueByType(value, e.getType());
//		if (errorMessage != null) {
//			addError(section, key, errorMessage);
//			return;
//		}
//
//		if (e.getType()==ConfigurationEntry.Type.CFG_STRING && value.startsWith("\""))
//			value =	Common.parseQuotedString(value); // cannot throw exception: value got validated above
//
//		// check validity of some settings, like network=, preload-ned-files=, etc
//		if (e==CFGID_EXTENDS) {
//			if (!doc.containsSection(CONFIG_+value))
//				addError(section, key, "No such section: [Config "+value+"]");
//		}
//		else if (e==CFGID_NETWORK) {
//			INEDTypeInfo network = ned.getComponent(value);
//			if (network == null) {
//				addError(section, key, "No such NED network: "+value);
//				return;
//			}
//			NEDElement node = network.getNEDElement();
//			if (!(node instanceof CompoundModuleNode) || ((CompoundModuleNode)node).getIsNetwork()==false) {
//				addError(section, key, "Type '"+value+"' was not declared in NED with the keyword 'network'");
//				return;
//			}
//		}
//		// TODO Auto-generated method stub
		return null;
	}

}
