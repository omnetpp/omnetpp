package org.omnetpp.common.contentassist;

import java.util.ArrayList;

import org.eclipse.jface.contentassist.IContentAssistSubjectControl;
import org.eclipse.jface.contentassist.ISubjectControlContentAssistProcessor;
import org.eclipse.jface.contentassist.ISubjectControlContentAssistant;
import org.eclipse.jface.contentassist.SubjectControlContentAssistant;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.eclipse.jface.text.DefaultInformationControl;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IInformationControl;
import org.eclipse.jface.text.IInformationControlCreator;
import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.CompletionProposal;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContentAssistant;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;
import org.eclipse.swt.widgets.Shell;
import org.eclipse.ui.contentassist.ContentAssistHandler;
import org.omnetpp.common.ui.TableTextCellEditor;


/**
 * Utility functions related to text and field content assist.
 * @author Andras
 */
public class ContentAssistUtils {
	/** 
	 * Configures a table cell editor for content assist.
	 */
	public static ISubjectControlContentAssistant addContentAssist(final TableTextCellEditor cellEditor, final IContentProposalProvider provider) {
		// Create a control assist processor.
		//
		// Note: this needs to be an ISubjectControlContentAssistProcessor, simply 
		// being IContentAssistProcessor is not enough! (see instanceof in 
		// ContentAssistant.computeCompletionProposals())
		ISubjectControlContentAssistProcessor processor = new ISubjectControlContentAssistProcessor() {
			public ICompletionProposal[] computeCompletionProposals(IContentAssistSubjectControl contentAssistSubjectControl, int documentOffset) {
				IContentProposal[] proposals = provider.getProposals(contentAssistSubjectControl.getDocument().get(), documentOffset);
				
				// convert IContentProposals to ICompletionProposal
				ArrayList<ICompletionProposal> result = new ArrayList<ICompletionProposal>();
				for (IContentProposal p : proposals)
					result.add(new CompletionProposal(p.getContent(), documentOffset, 0, p.getCursorPosition(), null, p.getLabel(), null, null));
				return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
			}
			public IContextInformation[] computeContextInformation(IContentAssistSubjectControl contentAssistSubjectControl, int documentOffset) {
				return null;
			}
			public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int offset) {
				return null; // never called
			}
			public IContextInformation[] computeContextInformation(ITextViewer viewer, int offset) {
				return null;
			}
			public char[] getCompletionProposalAutoActivationCharacters() {
				return null;
			}
			public char[] getContextInformationAutoActivationCharacters() {
				return null;
			}
			public IContextInformationValidator getContextInformationValidator() {
				return null;
			}
			public String getErrorMessage() {
				return null;
			}
		};

		// Add content assist the text editor. Code taken from JDT, ChangeParametersControl.java line #573.
		// ChangeParametersControl.installParameterTypeContentAssist():
		//  1. SubjectControlContentAssistant contentAssistant= ControlContentAssistHelper.createJavaContentAssistant(processor);
		//  2. ContentAssistHandler.createHandlerForText(text, contentAssistant);

		final SubjectControlContentAssistant assistant = new SubjectControlContentAssistant();
		assistant.setContentAssistProcessor(processor, IDocument.DEFAULT_CONTENT_TYPE);
		assistant.setContextInformationPopupOrientation(IContentAssistant.CONTEXT_INFO_ABOVE);
		assistant.setInformationControlCreator(new IInformationControlCreator() {
			public IInformationControl createInformationControl(Shell parent) {
				//return new DefaultInformationControl(parent, SWT.NONE, new HTMLTextPresenter(true));
				return new DefaultInformationControl(parent);
			}
		});
		ContentAssistHandler.createHandlerForText(cellEditor.getText(), assistant);
		cellEditor.setContentAssistant(assistant);
		return assistant;
	}

}
