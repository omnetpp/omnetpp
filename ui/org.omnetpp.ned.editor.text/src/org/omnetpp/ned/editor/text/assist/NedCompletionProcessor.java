package org.omnetpp.ned.editor.text.assist;


import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.eclipse.jface.text.BadLocationException;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IRegion;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.Region;
import org.eclipse.jface.text.TextPresentation;
import org.eclipse.jface.text.contentassist.*;
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

    	// XXX experimental: use hint word to make proposals...
    	String hintWord = getWordBefore(viewer, documentOffset, NedHelper.nedWordDetector);
    	System.out.println("NedCompletionProcessor.computeCompletionProposals(): hintword:"+hintWord);
    	if ("extends".equals(hintWord)) {
        	// XXX offer type list
    		String words[] = NEDResourcesPlugin.getNEDResources().getModuleNames().toArray(new String[0]);
            result.addAll(createProposals(viewer, documentOffset, NedHelper.nedWordDetector,
                    "", words, ""));
            System.out.println(words.length);
        }
    	
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
        //Collections.sort(result, CompletionProposalComparator.getInstance());

        // get all the template proposals from the parent
        List templateList = Arrays.asList(super.computeCompletionProposals(viewer, documentOffset));
        Collections.sort(templateList, CompletionProposalComparator.getInstance());
        result.addAll(templateList);

        
        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    } 
	
	private String getWordBefore(ITextViewer viewer, int documentOffset, IWordDetector wordDetector) {
		// Example: if cursor is just after "submod[i+3].to", we'd like to get back "submod"
		IDocument docu = viewer.getDocument();
        int offset = documentOffset;
        try {
            // go backwards, skipping the current word.
            while (offset > 0) {
            	char c = docu.getChar(offset - 1);
            	if (!wordDetector.isWordPart(c) && !wordDetector.isWordStart(c))
            		break;
                offset--;
            }

            // go backwards, skipping punctuation. 
            // simplification: we don't go over to previous line because it 
            // might end in a comment ("//") which we'd have to detect...
            //
            while (offset > 0) {
            	char c = docu.getChar(offset - 1);
            	if (c=='\n' || c==';' || c=='{' || c=='}')
            		return null;  // no word until end of previous "thing"
            	if (wordDetector.isWordPart(c))
            		break;
            	offset--;
            }

            // we need the word here
            int wordEndOffset = offset;

            // find the first char that may not be the trailing part of a word.
            while (offset > 0 && wordDetector.isWordPart(docu.getChar(offset - 1)))
                offset--;
            
            // check if the first char of the word is also ok.
            if (offset > 0 && wordDetector.isWordStart(docu.getChar(offset - 1)))
                offset--;

            // return word
            int wordBegOffset = offset;
            return docu.get(wordBegOffset, wordEndOffset-wordBegOffset);

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
