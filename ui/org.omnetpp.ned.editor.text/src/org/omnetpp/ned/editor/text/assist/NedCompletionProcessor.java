package org.omnetpp.ned.editor.text.assist;


import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.ContextInformation;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationPresenter;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.omnetpp.ned.editor.text.NedEditorMessages;
import org.omnetpp.ned.editor.text.NedHelper;
import org.omnetpp.resources.INEDComponent;
import org.omnetpp.resources.NEDResources;
import org.omnetpp.resources.NEDResourcesPlugin;

// TODO a better structure is needed for storing the completion proposals
// TODO proposalas should be context sensitive
// TODO even name and type proposalas can be done, once the NED files can be parsed in the background
// TODO context help can be supported, to show the documentation of the proposed keyword
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
	
		List result = new ArrayList();
	
		// find out where we are: in which module, submodule, which section etc.
		CompletionInfo info = computeCompletionInfo(viewer, documentOffset);
		NEDResources res = NEDResourcesPlugin.getNEDResources();    	
	
		String line = info.linePrefixTrimmed;
		INEDComponent parentComponent = null;
		if (info.componentName!=null)
			parentComponent = res.getComponent(info.componentName); 
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
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedInheritanceKeywords, "keyword");
		}
	
		// propose line start: param names, gate names, keywords
		// XXX propose templates too?
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
	    	if (info.sectionType==SECT_GLOBAL || info.sectionType==SECT_TYPES)
				addProposals(viewer, documentOffset, result, NedHelper.proposedNedGlobalStartingKeywords, null);
	    	else if (info.sectionType==SECT_PARAMETERS || info.sectionType==SECT_GATES || 
					info.sectionType==SECT_TYPES || info.sectionType==SECT_SUBMODULES) { 
	    		addProposals(viewer, documentOffset, result, NedHelper.proposedNedSectionNameKeywords, null);
	    	}
	    	else if (info.sectionType==SECT_SUBMODULE_PARAMETERS)
	    		addProposals(viewer, documentOffset, result, new String[]{"gates:"}, "keyword"); //XXX string constant
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
		
		// expressions: after "=", opening "[", "if" or "where"
		if (line.contains("=") || line.matches(".*\\b(if|where)\\b.*") || line.matches(".*\\[[^\\]]*")) {
			System.out.println("proposals for expressions");
	
			// offer parameter names, gate names, types,...
			if (line.matches(".*\\bthis *\\.")) {
				if (submoduleType!=null)
					addProposals(viewer, documentOffset, result, submoduleType.getParamNames(), "parameter");
			}
			else if (line.matches(".*\\bsizeof *\\(")) {
				if (parentComponent!=null) {
					addProposals(viewer, documentOffset, result, parentComponent.getGateNames(), "gate");
					addProposals(viewer, documentOffset, result, parentComponent.getSubmodNames(), "gate");
				}
			}
			else {
				if (parentComponent!=null) {
					addProposals(viewer, documentOffset, result, parentComponent.getParamNames(), "parameter");
				}
			}
			addProposals(viewer, documentOffset, result, NedHelper.proposedConstants, null);
			addProposals(viewer, documentOffset, result, NedHelper.proposedNedFunctions, "function"); //XXX arg help?
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
			//XXX offer templates for connection, loop connection, connection with channel, etc
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
	    			//XXX TODO: match out submod name, look it up and display its gates
	    		}
	    	}  		
		}
		
		// offer keywords as fallback -- removed because those proposals rarely made sense
		//if (result.isEmpty())
		//	addProposals(viewer, documentOffset, result, NedHelper.proposedNedOtherKeywords, null);
	
	    // get all the template proposals from the parent
		//XXX update templates (templates/ned.xml) to current NED syntax!!!
		//XXX filter templates according to compound module section!!!
	    List templateList = Arrays.asList(super.computeCompletionProposals(viewer, documentOffset));
	    Collections.sort(templateList, CompletionProposalComparator.getInstance());
	    result.addAll(templateList);
	
		long millis = System.currentTimeMillis()-startMillis;
		System.out.println("Proposal creation: "+millis+"ms");
	    
	    return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List result, String[] proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals, "", description));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List result, Set<String> proposals, String description) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals.toArray(new String[0]), "", description));
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
    		prefix = prefix.replaceFirst(".*\\b(parameters|gates|types|connections|connections +[a-z]+) *:", "");
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
		IContextInformation[] result= new IContextInformation[5];
		for (int i= 0; i < result.length; i++)
			result[i]= new ContextInformation(
				MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.display.pattern"), new Object[] { new Integer(i), new Integer(documentOffset) }),  //$NON-NLS-1$
				MessageFormat.format(NedEditorMessages.getString("CompletionProcessor.ContextInfo.value.pattern"), new Object[] { new Integer(i), new Integer(documentOffset - 5), new Integer(documentOffset + 5)})); //$NON-NLS-1$
		return result;
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
