package org.omnetpp.ned.editor.text.assist;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationPresenter;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.eclipse.jface.text.templates.Template;
import org.omnetpp.ned.editor.text.NedHelper;
import org.omnetpp.ned2.model.NEDElement;
import org.omnetpp.ned2.model.pojo.SubmoduleNode;
import org.omnetpp.resources.INEDComponent;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;

// TODO a better structure is needed for storing the completion proposals
// TODO context help can be supported, to show the documentation of the proposed keyword
// TODO if we want F4 "Open Declaration": see
//    action org.eclipse.jdt.ui.edit.text.java.open.editor
//    org.eclipse.jdt.ui.actions.IJavaEditorActionDefinitionIds
//    org.eclipse.jdt.internal.ui.infoviews.SourceView
//    org.eclipse.jdt.ui.actions.OpenEditorActionGroup
// FIXME gets confused inside the "types:" section
/**
 * NED completion processor.
 */
public class NedCompletionProcessor extends IncrementalCompletionProcessor {

	/**
	 * Simple content assist tip closer. The tip is valid in a range
	 * of 5 characters around its popup location.
	 */
	protected static class Validator implements IContextInformationValidator, IContextInformationPresenter {

		protected int fInstallOffset;

		/*
		 * @see IContextInformationValidator#isContextInformationValid(int)
		 */
		public boolean isContextInformationValid(int offset) {
			return Math.abs(fInstallOffset - offset) < 5;
		}

		/*
		 * @see IContextInformationValidator#install(IContextInformation, ITextViewer, int)
		 */
		public void install(IContextInformation info, ITextViewer viewer, int offset) {
			fInstallOffset= offset;
		}

		/*
		 * @see org.eclipse.jface.text.contentassist.IContextInformationPresenter#updatePresentation(int, TextPresentation)
		 */
		public boolean updatePresentation(int documentPosition, TextPresentation presentation) {
			return false;
		}
	}

	// for CompletionInfo.sectionType
	protected static final int SECT_GLOBAL = 0;
	protected static final int SECT_PARAMETERS = 1;
	protected static final int SECT_GATES = 2;
	protected static final int SECT_TYPES = 3;
	protected static final int SECT_CONNECTIONS = 4;
	protected static final int SECT_SUBMODULES = 5;
	protected static final int SECT_SUBMODULE_PARAMETERS = 6;
	protected static final int SECT_SUBMODULE_GATES = 7;

	protected static class CompletionInfo {
		public String linePrefix; // relevant line (lines) just before the insertion point
		public String linePrefixTrimmed; // like linePrefix, but last identifier (which the user is currently typing) chopped
		public String componentName;
		public int sectionType; // SECT_xxx
		public String submoduleTypeName;
	}

	protected IContextInformationValidator fValidator= new Validator();

    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
		long startMillis = System.currentTimeMillis(); // measure time

		List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();

		// find out where we are: in which module, submodule, which section etc.
		CompletionInfo info = computeCompletionInfo(viewer, documentOffset);
		NEDResources res = NEDResourcesPlugin.getNEDResources();

		String line = info.linePrefixTrimmed;
		INEDComponent parentComponent = null;
		if (info.componentName!=null)
			parentComponent = res.getComponent(info.componentName); // hopefully autobuilder (reconciler) has already run on current source
		INEDComponent submoduleType = null;
		if (info.submoduleTypeName!=null)
			submoduleType = res.getComponent(info.submoduleTypeName);

		if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES)
		{
			System.out.println("testing proposals for GLOBAL and TYPES scope");

			// match various "extends" clauses
			if (line.matches(".*\\bsimple .* extends"))
				addProposals(viewer, documentOffset, result, res.getModuleNames(), "module type");
			else if (line.matches(".*\\bmodule .* extends"))
				addProposals(viewer, documentOffset, result, res.getModuleNames(), "module type");
			else if (line.matches(".*\\bchannel .* extends"))
				addProposals(viewer, documentOffset, result, res.getChannelNames(), "channel type");
			else if (line.matches(".*\\binterface .* extends"))
				addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames(), "module interface type");
			else if (line.matches(".*\\bchannelinterface .* extends"))
				addProposals(viewer, documentOffset, result, res.getChannelInterfaceNames(), "channel interface type");

			// match "like" clauses
			// XXX match "," as well (multiple interfaces)
			if (line.matches(".*\\bsimple .* like"))
				addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames(), "module interface type");
			else if (line.matches(".*\\bmodule .* like"))
				addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames(), "module interface type");
			else if (line.matches(".*\\bchannel .* like"))
				addProposals(viewer, documentOffset, result, res.getChannelInterfaceNames(), "channel interface type");

			if (!line.equals("") && !line.matches("\\b(extends|like)\\b"))
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedInheritanceKeywords, null);
		}

		// propose line start: param names, gate names, keywords
		if (line.equals("")) {
			// offer param and gate names
			if (info.sectionType == SECT_PARAMETERS && parentComponent!=null)
				addProposals(viewer, documentOffset, result, parentComponent.getParamNames(), "parameter");
			if (info.sectionType == SECT_GATES && parentComponent!=null)
				addProposals(viewer, documentOffset, result, parentComponent.getGateNames(), "gate");
			if (info.sectionType == SECT_SUBMODULE_PARAMETERS && submoduleType!=null)
				addProposals(viewer, documentOffset, result, submoduleType.getParamNames(), "parameter");
			if (info.sectionType == SECT_SUBMODULE_GATES && submoduleType!=null)
				addProposals(viewer, documentOffset, result, submoduleType.getGateNames(), "gate");

			// offer param and gate type name keywords
			if (info.sectionType == SECT_PARAMETERS)
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedParamTypes, null);
			else if (info.sectionType == SECT_GATES)
					addProposals(viewer, documentOffset, result, NedHelper.proposedNedGateTypes, null);

			// provide global start keywords and section names
	    	if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES) {
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedGlobalStartingKeywords, null);
	    	}
	    	else if (info.sectionType==SECT_PARAMETERS || info.sectionType==SECT_GATES ||
					info.sectionType==SECT_TYPES || info.sectionType==SECT_SUBMODULES) {
	    		addProposals(viewer, documentOffset, result, NedHelper.proposedNedSectionNameKeywords, null);
	    	}
	    	else if (info.sectionType==SECT_SUBMODULE_PARAMETERS) {
	    		addProposals(viewer, documentOffset, result, new String[]{"gates:"}, null);
	    	}

	    	// offer templates
	    	if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES) {
			    addProposals(viewer, documentOffset, result, NedHelper.proposedNedGlobalTempl);
	    	}
	    	else if (info.sectionType==SECT_SUBMODULES) {
	    		addProposals(viewer, documentOffset, result, NedHelper.proposedNedSubmoduleTempl);
	    	}
		}

		// offer existing and standard property names after "@"
		if (line.matches("@")) {
			addProposals(viewer, documentOffset, result, NedHelper.proposedNedComponentPropertyNames, "standard property");
			if (info.sectionType == SECT_PARAMETERS && parentComponent!=null)
				addProposals(viewer, documentOffset, result, parentComponent.getPropertyNames(), "property");
			if (info.sectionType == SECT_SUBMODULE_PARAMETERS && submoduleType!=null)
				addProposals(viewer, documentOffset, result, submoduleType.getPropertyNames(), "property");
		}
		else if (line.endsWith("@")) {
			if (info.sectionType == SECT_PARAMETERS || info.sectionType == SECT_SUBMODULE_PARAMETERS)
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedParamPropertyNames, "standard property");
			if (info.sectionType == SECT_GATES || info.sectionType == SECT_SUBMODULE_GATES)
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedGatePropertyNames, "standard property");
		}

		// expressions: after "=", opening "[", "if" or "for"
		if (line.contains("=") || line.matches(".*\\b(if|for)\\b.*") || containsOpenBracket(line)) {
			System.out.println("proposals for expressions");

			// offer parameter names, gate names, types,...
			if (line.matches(".*\\bthis *\\.")) {
				if (submoduleType!=null)
					addProposals(viewer, documentOffset, result, submoduleType.getParamNames(), "parameter");
			}
			else if (line.matches(".*\\bsizeof *\\(")) {
				if (parentComponent!=null) {
					addProposals(viewer, documentOffset, result, parentComponent.getGateNames(), "gate");
					addProposals(viewer, documentOffset, result, parentComponent.getSubmodNames(), "submodule");
				}
			}
	    	else if (line.endsWith(".")) {
	    		// after dot: offer params (and after sizeof(), gates too) of given submodule
	    		if (parentComponent!=null) {
					String submodTypeName = extractSubmoduleTypeName(line, parentComponent);
					System.out.println(" offering params of type "+submodTypeName);
					INEDComponent submodType = res.getComponent(submodTypeName);
					if (submodType!=null) {
						if (line.matches(".*\\bsizeof *\\(.*"))
							addProposals(viewer, documentOffset, result, submodType.getGateNames(), "gate");
						addProposals(viewer, documentOffset, result, submodType.getParamNames(), "param");
					}
	    		}
	    	}
			else {
				if (parentComponent!=null) {
					addProposals(viewer, documentOffset, result, parentComponent.getParamNames(), "parameter");
				}
			}
			addProposals(viewer, documentOffset, result, NedHelper.proposedConstants, null);

			//addProposals(viewer, documentOffset, result, NedHelper.proposedNedFunctions, "function");
		    addProposals(viewer, documentOffset, result, NedHelper.proposedNedOperatorsTempl);
		    addProposals(viewer, documentOffset, result, NedHelper.proposedNedDistributionsTempl);
		    addProposals(viewer, documentOffset, result, NedHelper.proposedNedFunctionsTempl);

		    addProposals(viewer, documentOffset, result, NedHelper.proposedNedOtherKeywords, null);
		}

		// complete submodule type name
		if (info.sectionType == SECT_SUBMODULES) {
			System.out.println("testing proposals for SUBMODULES scope");
			if (line.matches(".*:")) {
				// XXX offer "like" template too
				addProposals(viewer, documentOffset, result, res.getModuleNames(), "module type");
			}
			else if (line.matches(".*: *<")) {  // "like" syntax
				if (parentComponent!=null)
					addProposals(viewer, documentOffset, result, parentComponent.getParamNames(), "parameter");
			}
			else if (line.matches(".*: *<.*>")) {   // "like" syntax, cont'd
					addProposals(viewer, documentOffset, result, new String[]{" like "}, null);
			}
			else if (line.matches(".*\\blike")) {
				addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames(), "module interface type");
				addProposals(viewer, documentOffset, result, res.getModuleNames(), "module type");
			}
		}

		if (info.sectionType == SECT_CONNECTIONS) {
			System.out.println("testing proposals for CONNECTIONS scope");
			if (line.matches(".*\\bconnections")) {
				// user forgot "allowunconnected" keyword
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedConnsKeywords, null);
			}

			if (line.equals("") || line.endsWith("-->") || line.endsWith("<-->") || line.endsWith("<--")) {
	    		// right at line start or after arrow: offer submodule names and parent module's gates
	    		if (parentComponent!=null) {
	    			addProposals(viewer, documentOffset, result, parentComponent.getSubmodNames(), "submodule");
	    			addProposals(viewer, documentOffset, result, parentComponent.getGateNames(), "gate");
	    		}
	    	}
	    	else if (line.endsWith(".")) {
	    		// after dot: offer gates of given submodule
	    		if (parentComponent!=null) {
					String submodTypeName = extractSubmoduleTypeName(line, parentComponent);
					System.out.println(" offering gates of type "+submodTypeName);
					INEDComponent submodType = res.getComponent(submodTypeName);
					if (submodType!=null)
						addProposals(viewer, documentOffset, result, submodType.getGateNames(), "gate");
	    		}
	    	}

			// offer templates for connection, loop connection, connection with channel, etc
		    addProposals(viewer, documentOffset, result, NedHelper.proposedNedConnectionTempl);
		}

		// offer keywords as fallback -- removed because those proposals rarely made sense
		//if (result.isEmpty())
		//	addProposals(viewer, documentOffset, result, NedHelper.proposedNedOtherKeywords, null);

	    // get all the template proposals from the parent
		//XXX update templates (templates/ned.xml) to current NED syntax!!!
	    List<ICompletionProposal> tmp = Arrays.asList(super.computeCompletionProposals(viewer, documentOffset));
	    result.addAll(tmp);

		long millis = System.currentTimeMillis()-startMillis;
		System.out.println("Proposal creation: "+millis+"ms");

	    return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
	}

	private boolean containsOpenBracket(String line) {
		while (line.matches(".*\\[[^\\[\\]]*\\].*"))
			line = line.replaceAll("\\[[^\\[\\]]*\\]", "###");
        return line.contains("[");
	}

	private String extractSubmoduleTypeName(String line, INEDComponent parentComponent) {
		// first, get rid of everything before any arrow(s), because it causes a problem for the next regexp
		line = line.replaceFirst("^.*(-->|<--|<-->)", "");
		// identifier followed by ".", potentially a submodule index ("[something]") in between
		Matcher matcher = Pattern.compile("([A-Za-z_][A-Za-z0-9_]*) *(\\[[^\\[\\]]*\\])? *\\.$").matcher(line);
		if (matcher.find()) { // use find() because line may start with garbage
			String submoduleName = matcher.group(1);
			NEDElement submodNode = parentComponent.getMember(submoduleName);
			if (submodNode instanceof SubmoduleNode) {
				SubmoduleNode submod = (SubmoduleNode) submodNode;
				String submodTypeName = submod.getType();
				if (submodTypeName==null || submodTypeName.equals(""))
					submodTypeName = submod.getLikeType();
				return submodTypeName;
			}
		}
		return null; // bad luck
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, String[] proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals, "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals.toArray(new String[0]), "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List<ICompletionProposal> result, Template[] templates) {
	    result.addAll(Arrays.asList(createTemplateProposals(viewer, documentOffset, templates)));
	}

	private CompletionInfo computeCompletionInfo(ITextViewer viewer, int documentOffset) {
		IDocument docu = viewer.getDocument();
        int offset = documentOffset;
        try {
    		String source = docu.get(0,offset);
    		// kill string literals
			source = source.replaceAll("\".*\"", "\"###\"");
    		// kill comments
    		source = source.replaceAll("(?m)//.*", "");

    		// completion prefix (linePrefix): stuff after last semicolon,
    		// curly brace, "parameters:", "gates:", "connections:" etc.
    		String prefix = source;
    		prefix = prefix.replaceAll("(?s)\\s+", " "); // normalize whitespace
    		prefix = prefix.replaceFirst(".*[;\\{\\}]", "");
    		prefix = prefix.replaceFirst(".*\\b(parameters|gates|types|submodules|connections|connections +[a-z]+) *:", "");
    		String prefix2 = prefix.replaceFirst("[a-zA-Z_][a-zA-Z0-9_]*$", "").trim(); // chop off last word

    		// kill {...} regions (including bodies of inner types, etc)
    		while (source.matches("(?s).*\\{[^\\{\\}]*\\}.*"))
    			source = source.replaceAll("(?s)\\{[^\\{\\}]*\\}", "###");

			// detect what section we are in
			int sectionType;
			if (source.matches("(?s).*\\bconnections\\b.*"))
				sectionType = SECT_CONNECTIONS;
			else if (source.matches("(?s).*\\btypes\\b.*"))
				sectionType = SECT_TYPES;
			else if (source.matches("(?s).*\\bsubmodules\\b.*\\bgates\\b.*"))
				sectionType = SECT_SUBMODULE_GATES;
			else if (source.matches("(?s).*\\bsubmodules\\b.*\\{.*"))
				sectionType = SECT_SUBMODULE_PARAMETERS;
			else if (source.matches("(?s).*\\bsubmodules\\b.*"))
				sectionType = SECT_SUBMODULES;
			else if (source.matches("(?s).*\\bgates\\b.*"))
				sectionType = SECT_GATES;
			else if (source.matches("(?s).*\\{.*"))
				sectionType = SECT_PARAMETERS;
			else
				sectionType = SECT_GLOBAL;

			// detect module name: identifier after last "simple", "module", etc.
			String pat = "(?s).*(simple|module|network|channel|interface|channelinterface)\\s+(withcppclass\\s+)?([A-Za-z_][A-Za-z0-9_]+)";
			Matcher matcher = Pattern.compile(pat).matcher(source);
			String componentName = null;
			if (matcher.lookingAt())
				componentName = matcher.group(3);

			// detect submodule type: last occurrence of "identifier {"
			String submoduleTypeName = null;
			if (sectionType == SECT_SUBMODULE_GATES || sectionType == SECT_SUBMODULE_PARAMETERS) {
				String pat2 = "(?s).*[:\\s]([A-Za-z_][A-Za-z0-9_]+)\\s*\\{";
				Matcher matcher2 = Pattern.compile(pat2).matcher(source);
				if (matcher2.lookingAt())
					submoduleTypeName = matcher2.group(1);
			}

			//System.out.println(">>>"+source+"<<<");
			System.out.println("SECTIONTYPE:"+sectionType+"  COMPONENT:"+componentName+"  SUBMODTYPENAME:"+submoduleTypeName);
			System.out.println("PREFIX: >>"+prefix+"<<");
			System.out.println("PREFIX2: >>"+prefix2+"<<");

			CompletionInfo ret = new CompletionInfo();
			ret.linePrefix = prefix;
			ret.linePrefixTrimmed = prefix2;
			ret.componentName = componentName;
			ret.sectionType = sectionType;
			ret.submoduleTypeName = submoduleTypeName;
			return ret;

        } catch (BadLocationException e) {
        	return null;
        }
	}

	public IContextInformation[] computeContextInformation(ITextViewer viewer, int documentOffset) {
		//XXX what the heck is this?
		//		IContextInformation[] result= new IContextInformation[5];
		//		for (int i= 0; i < result.length; i++)
		//			result[i]= new ContextInformation(
		//				MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.display.pattern"), new Object[] { new Integer(i), new Integer(documentOffset) }),  //$NON-NLS-1$
		//				MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.value.pattern"), new Object[] { new Integer(i), new Integer(documentOffset - 5), new Integer(documentOffset + 5)})); //$NON-NLS-1$
		//		return result;
		return null;
	}

	public char[] getCompletionProposalAutoActivationCharacters() {
		return new char[] { '.' };
	}

	public char[] getContextInformationAutoActivationCharacters() {
		return new char[] { '(' };
	}

	public IContextInformationValidator getContextInformationValidator() {
		return fValidator;
	}

	public String getErrorMessage() {
		return null;
	}
}
