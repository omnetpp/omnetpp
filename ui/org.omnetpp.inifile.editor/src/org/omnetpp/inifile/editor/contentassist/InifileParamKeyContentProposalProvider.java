package org.omnetpp.inifile.editor.contentassist;

import java.util.ArrayList;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.inifile.editor.model.IInifileDocument;
import org.omnetpp.inifile.editor.model.InifileAnalyzer;
import org.omnetpp.inifile.editor.model.ParamResolution;

/**
 * Generate proposals for inifile parameter and per-object config keys.
 * 
 * Used for the text editor and field editors.
 * 
 * @author Andras
 */
public class InifileParamKeyContentProposalProvider extends ContentProposalProvider {
	private String section;
	private boolean addEqualSign = false;
	private IInifileDocument doc;
	private InifileAnalyzer analyzer;

	public InifileParamKeyContentProposalProvider(String section, boolean addEqualSign, IInifileDocument doc, InifileAnalyzer analyzer) {
		this.section = section;
		this.addEqualSign = addEqualSign;
		this.doc = doc;
		this.analyzer = analyzer;
	}

	public void configure(String section, boolean addEqualSign) {
		this.section = section;
		this.addEqualSign = addEqualSign;
	}
	
	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before presenting them to the user.
	 */
	protected IContentProposal[] getProposalCandidates(String prefix) {
		if (section != null) {
			String eq = addEqualSign ? " = " : "";
			ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
			ParamResolution[] resList = analyzer.getUnassignedParams(section);
			
			//XXX offer only one step at a time?
			result.add(new ContentProposal("**.apply-default" + eq));
			for (ParamResolution res : resList) {
				// offer three versions for each 
				//XXX alternative: offer some continuations after each dot?
				result.add(new ContentProposal("**." + res.paramNode.getName() + "" + eq));
				result.add(new ContentProposal(res.moduleFullPath.replaceFirst("^[^\\.]+", "**") + "." + res.paramNode.getName() + "" + eq));
				result.add(new ContentProposal(res.moduleFullPath + "." +res.paramNode.getName() + "" + eq));

				// propose .apply-default= lines; XXX only if at least some of these parameters have default value
				result.add(new ContentProposal("**." + res.paramNode.getName() + ".apply-default" + eq));
				result.add(new ContentProposal(res.moduleFullPath.replaceFirst("^[^\\.]+", "**") + "." + res.paramNode.getName() + ".apply-default" + eq));
				result.add(new ContentProposal(res.moduleFullPath + "." +res.paramNode.getName() + ".apply-default" + eq));
			}
		}

		return null;
	}
}
