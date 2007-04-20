package org.omnetpp.inifile.editor.contentassist;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.engine.PatternMatcher;
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
//XXX for this to work well, parser SHOULD NOT STOP ON THE FIRST ERROR!!! should just go on.
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

			// collect unique full paths
			Set<String> fullPaths = new HashSet<String>(); 
			for (ParamResolution res : resList)
				fullPaths.add(res.moduleFullPath + "." +res.paramValueNode.getName());

			Set<String> moduleProposals = new HashSet<String>();
			Set<String> paramProposals = new HashSet<String>();
			Set<String> otherProposals = new HashSet<String>();
			
			// find last "**" or "." ==> (part1,rest)
			int lastDotPos = prefix.lastIndexOf('.');
			int lastXXPos = prefix.lastIndexOf("**");
			int restPos = Math.max(lastDotPos==-1 ? 0 : lastDotPos+1, lastXXPos==-1 ? 0 : lastXXPos+2);
			String prefixPart1 = prefix.substring(0, restPos);  // if no dot or **, prefixPart1 will be ""
			String prefixRest = prefix.substring(restPos);
			//System.out.println("prefix: "+prefixPart1+" + "+prefixRest);
			
			// check every fullPath:
			PatternMatcher prefixPart1Matcher = new PatternMatcher(prefixPart1, true, true, true);
			for (String fullPath : fullPaths) {
				// split it into two at each dot ==> (part1,rest).
				// like: "" + "net.host.param", "net." + "host.param", "net.host." + "param"
				for (int dotPos = -2; dotPos != -1; dotPos = fullPath.indexOf('.', dotPos+1)) {
					if (dotPos == -2) dotPos = -1; // get started
					String fullPathPart1 = fullPath.substring(0, dotPos+1);
					String fullPathRest = fullPath.substring(dotPos+1);
					//System.out.println("fullPath: "+fullPathPart1+" + "+fullPathRest);

					// prefix part1 should match fullPath part1, and fullPath rest must begin with prefix rest 
					if (prefixPart1Matcher.matches(fullPathPart1) && fullPathRest.startsWith(prefixRest)) {
						//System.out.println("        suggesting: "+prefixPart1+fullPathRest+" =");
						if (fullPathRest.contains(".")) {
							// offer only the next submodule (i.e. rest up to the next dot)
							int restDotPos = fullPathRest.indexOf('.');
							String nextSubmodule = fullPathRest.substring(0, restDotPos+1);
							moduleProposals.add(prefixPart1+nextSubmodule);
						}
						else {
							// if rest contains no dot, then it is the last segment of fullpath: the parameter name
							paramProposals.add(prefixPart1+fullPathRest);
						}
					}
				}
			}

			// add ** and * as proposals, if it looks appropriate
			if (!prefixPart1.endsWith("*") && !prefixPart1.endsWith("*.") && !moduleProposals.isEmpty()) {
				String prefixPart1dot = prefixPart1.endsWith(".") ? prefixPart1 : (prefixPart1+"."); 
				otherProposals.add(prefixPart1dot+"*.");
				otherProposals.add(prefixPart1dot+"**.");
			}
			
			// convert strings to actual proposals, and return them
			List<IContentProposal> proposals = new ArrayList<IContentProposal>();
			addProposals(proposals, moduleProposals, "", "");
			addProposals(proposals, paramProposals, " = ", " = ");
			addProposals(proposals, otherProposals, "", "");

			return proposals.toArray(new IContentProposal[]{});
		}

		return null;
	}

	private void addProposals(List<IContentProposal> proposals, Set<String> texts, String suffix, String labelSuffix) {
		for (String text : texts)
			proposals.add(new ContentProposal(text+suffix, text+labelSuffix, null));
	}
}