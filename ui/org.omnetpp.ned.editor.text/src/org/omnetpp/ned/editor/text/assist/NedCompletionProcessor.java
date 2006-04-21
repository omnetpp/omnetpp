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
import org.eclipse.jface.text.rules.IWordDetector;
import org.omnetpp.ned.editor.text.NedEditorMessages;
import org.omnetpp.ned.editor.text.NedHelper;
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

    	String line = info.linePrefix;
    	
    	// match various "extends" clauses
    	if (line.matches(".* simple .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleNames());
    	else if (line.matches(".* module .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleNames());
    	else if (line.matches(".* channel .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getChannelNames());
    	else if (line.matches(".* interface .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (line.matches(".* channelinterface .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getChannelInterfaceNames());

    	// match "like" clauses
    	if (line.matches(".* simple .* like +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (line.matches(".* module .* like +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (line.matches(".* channel .* like +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getChannelInterfaceNames());
    	else if (line.matches(".*:.* like +[^ ]*")) // match "submod : ModuleType" syntax
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (line.matches(".*: +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleNames());

    	// expressions
    	if (line.matches(".*=.*")) {
    		//XXX parameter names, gate names, types,...
			addProposals(viewer, documentOffset, result, NedHelper.proposedNedFunctions);
			addProposals(viewer, documentOffset, result, NedHelper.proposedConstants);
    	}

    	// propose parameter and gate types
    	if (line.matches(" [a-z]*")) {
    		// XXX only if we are in "parameters" or "gates"
    		addProposals(viewer, documentOffset, result, NedHelper.proposedNedTypes);
    	}

    	//XXX create proposals for connections section
    	
		// offer keywords as fallback
    	addProposals(viewer, documentOffset, result, NedHelper.proposedNedKeywords);

        // get all the template proposals from the parent
    	//XXX filter templates to compound module section!!!
        List templateList = Arrays.asList(super.computeCompletionProposals(viewer, documentOffset));
        Collections.sort(templateList, CompletionProposalComparator.getInstance());
        result.addAll(templateList);

    	long millis = System.currentTimeMillis()-startMillis;
    	System.out.println("Proposal creation: "+millis+"ms");
        
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    }

	private void addProposals(ITextViewer viewer, int documentOffset, List result, String[] proposals) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals, ""));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List result, Set<String> proposals) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals.toArray(new String[0]), ""));
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
    		prefix = prefix.replaceFirst("(?s).*[;\\{\\}]", "");
    		prefix = prefix.replaceFirst("(?s).*\\b(parameters|gates|types|connections|connections\\s+[a-z]+)\\s*:", "");
    		prefix = prefix.replaceAll("(?s)\\s+", " "); // normalize whitespace

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
			String pat2 = "(?s).*[:\\s]([A-Za-z_][A-Za-z0-9_]+)\\s*\\{";
			Matcher matcher2 = Pattern.compile(pat2).matcher(source);
			String submoduleTypeName = null;
			if (matcher2.lookingAt())
				submoduleTypeName = matcher2.group(1);

			//System.out.println(">>>"+source+"<<<");
			System.out.println("SECTIONTYPE:"+sectionType+"  COMPONENT:"+componentName+"  SUBMODTYPENAME:"+submoduleTypeName);
			System.out.println("PREFIX:"+prefix);

			CompletionInfo ret = new CompletionInfo();
			ret.linePrefix = prefix;
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
