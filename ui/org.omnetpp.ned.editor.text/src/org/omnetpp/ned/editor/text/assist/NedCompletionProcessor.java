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

	protected IContextInformationValidator fValidator= new Validator();

    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
    	List result = new ArrayList();

    	String context = getCompletionContext(viewer, documentOffset, NedHelper.nedWordDetector);
    	System.out.println("NedCompletionProcessor.computeCompletionProposals(): context: >>"+context+"<<");
    	getComponentName(viewer, documentOffset); //XXXX
    	NEDResources res = NEDResourcesPlugin.getNEDResources();    	

    	// match various "extends" clauses
    	if (context.matches(".* simple .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleNames());
    	else if (context.matches(".* module .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleNames());
    	else if (context.matches(".* channel .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getChannelNames());
    	else if (context.matches(".* interface .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (context.matches(".* channelinterface .* extends +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getChannelInterfaceNames());

    	// match "like" clauses
    	// XXX ":" matches "gates:", "parameters:" etc as well!!!
    	if (context.matches(".* simple .* like +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (context.matches(".* module .* like +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (context.matches(".* channel .* like +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getChannelInterfaceNames());
    	else if (context.matches(".*:.* like +[^ ]*")) // match "submod : ModuleType" syntax
			addProposals(viewer, documentOffset, result, res.getModuleInterfaceNames());
    	else if (context.matches(".*: +[^ ]*"))
			addProposals(viewer, documentOffset, result, res.getModuleNames());

    	// expressions
    	if (context.matches(".*=.*")) {
    		//XXX parameter names, gate names, types,...
			addProposals(viewer, documentOffset, result, NedHelper.proposedNedFunctions);
			addProposals(viewer, documentOffset, result, NedHelper.proposedConstants);
    	}

    	// propose parameter and gate types
    	if (context.matches(" [a-z]*")) {
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
        
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    }

	private void addProposals(ITextViewer viewer, int documentOffset, List result, String[] proposals) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals, ""));
	}

	private void addProposals(ITextViewer viewer, int documentOffset, List result, Set<String> proposals) {
		result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector, "", proposals.toArray(new String[0]), ""));
	}

	private String getCompletionContext(ITextViewer viewer, int documentOffset, IWordDetector wordDetector) {
		IDocument docu = viewer.getDocument();
        int offset = documentOffset;
        try {
    		int lineno = docu.getLineOfOffset(offset);
    		int lineStartOffset = docu.getLineOffset(lineno);
        	String context = docu.get(lineStartOffset, offset-lineStartOffset);
        	while (lineno>0) {
        		// get previous line
        		lineno--;
        		lineStartOffset = docu.getLineOffset(lineno);
        		String line = docu.get(lineStartOffset, offset-lineStartOffset);
        		
        		// strip comment from it
        		int commentPos = line.indexOf("//");  // string constants may also contain "//", but let's ignore that...
        		if (commentPos!=-1)
        			line = line.substring(0, commentPos);

        		// if line contains ";" or curly brace, stop there
        		int pos = line.indexOf(';');
        		int p2 = line.indexOf('{');
        		if (p2>=0 && p2<pos)
        			pos = p2;
        		int p3 = line.indexOf('{');
        		if (p3>=0 && p3<pos)
        			pos = p3;
        		if (pos>=0)
        			line = line.substring(pos+1); // chop substring before ";" etc

        		// prepend to completion context
        		context = line.trim()+" "+context;
        		
        		if (pos>=0)
        			break;
        	}
        	context = context.replace('\t', ' ');
        	context = context.replace('\n', ' ');
        	context = context.replace('\r', ' ');
        	return context;

        } catch (BadLocationException e) {
        	return null;
        }
	}

	private String getComponentName(ITextViewer viewer, int documentOffset) {
		IDocument docu = viewer.getDocument();
        int offset = documentOffset;
        try {
    		String source = docu.get(0,offset);
    		// kill string literals
			source = source.replaceAll("\".*\"", "\"...\"");
    		// kill comments
    		source = source.replaceAll("(?m)//.*", "");
    		// kill {...} regions (including bodies of inner types, etc)
    		while (source.matches("(?s).*\\{[^\\{\\}]*\\}.*"))
    			source = source.replaceAll("(?s)\\{[^\\{\\}]*\\}", "@@");

			// detect what section we are in 
			String section;
			if (source.matches("(?s).*\\bconnections\\b.*"))
				section = "connections";
			else if (source.matches("(?s).*\\btypes\\b.*"))
				section = "types";
			else if (source.matches("(?s).*\\bsubmodules\\b.*\\bgates\\b.*"))
				section = "submodule-gates";
			else if (source.matches("(?s).*\\bsubmodules\\b.*\\{.*"))
				section = "submodule-parameters";
			else if (source.matches("(?s).*\\bsubmodules\\b.*"))
				section = "submodules";
			else if (source.matches("(?s).*\\bgates\\b.*"))
				section = "gates";
			else if (source.matches("(?s).*\\{.*"))
				section = "parameters";
			else
				section = "global";

			// detect module name
			String pat = "(?s).*(simple|module|network|channel|interface|channelinterface)\\s+(withcppclass\\s+)?([A-Za-z_][A-Za-z0-9_]+)";
			Matcher matcher = Pattern.compile(pat).matcher(source);
			String componentName = null;
			if (matcher.lookingAt())
				componentName = matcher.group(3);

			// detect submodule type
			String pat2 = "(?s).*[:\\s]([A-Za-z_][A-Za-z0-9_]+)\\s*\\{";
			Matcher matcher2 = Pattern.compile(pat2).matcher(source);
			String submoduleTypeName = null;
			if (matcher2.lookingAt())
				submoduleTypeName = matcher2.group(1);
			
			System.out.println(">>>"+source+"<<<");
			System.out.println("SECTION:"+section+"  COMPONENT:"+componentName+"  SUBMODTYPENAME:"+submoduleTypeName);
        	return "";

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
