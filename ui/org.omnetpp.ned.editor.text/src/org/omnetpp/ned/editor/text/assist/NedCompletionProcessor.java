package org.omnetpp.ned.editor.text.assist;


import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.*;
import org.omnetpp.ned.editor.text.NedEditorMessages;
import org.omnetpp.ned.editor.text.NedHelper;

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
        // TODO maybe the order should be checked 
        result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector,
                    "", NedHelper.proposedNedKeywords, ""));

        result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector,
                    "", NedHelper.proposedNedTypes, ""));

        result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector,
                "", NedHelper.proposedNedFunctions, "()"));

        result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector,
                "", NedHelper.proposedConstants, ""));

        // sort all the functions together
        Collections.sort(result, CompletionProposalComparator.getInstance());

        // get all the template proposals from the parent
        List templateList = Arrays.asList(super.computeCompletionProposals(viewer, documentOffset));
        Collections.sort(templateList, CompletionProposalComparator.getInstance());
        result.addAll(templateList);

        
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
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
