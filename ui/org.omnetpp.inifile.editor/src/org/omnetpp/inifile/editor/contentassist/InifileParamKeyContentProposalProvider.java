package org.omnetpp.inifile.editor.contentassist;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
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
		super(true);
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
			ParamResolution[] resList = analyzer.getUnassignedParams(section);

			Set<String> proposals = new HashSet<String>(); 
			//XXX offer only one step at a time?
			proposals.add("**.apply-default" + eq);
			for (ParamResolution res : resList) {
				// offer three versions for each 
				//XXX alternative: offer some continuations after each dot?
				proposals.add("**." + res.paramValueNode.getName() + "" + eq);
				proposals.add(res.moduleFullPath.replaceFirst("^[^\\.]+", "**") + "." + res.paramValueNode.getName() + "" + eq);
				proposals.add(res.moduleFullPath + "." +res.paramValueNode.getName() + "" + eq);

				// propose .apply-default= lines; XXX only if at least some of these parameters have default value
				proposals.add("**." + res.paramValueNode.getName() + ".apply-default" + eq);
				proposals.add(res.moduleFullPath.replaceFirst("^[^\\.]+", "**") + "." + res.paramValueNode.getName() + ".apply-default" + eq);
				proposals.add(res.moduleFullPath + "." +res.paramValueNode.getName() + ".apply-default" + eq);
			}
			
			// sort, and convert to IContentProposal[]
			String[] a = proposals.toArray(new String[]{});
			Arrays.sort(a);
			ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();
			for (String x : a)
				result.add(new ContentProposal(x));
			return result.toArray(new IContentProposal[]{});
		}

		return null;
	}
}
