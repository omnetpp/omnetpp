package freemarker.eclipse.editors;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.jface.text.contentassist.IContentAssistProcessor;
import org.eclipse.jface.text.contentassist.IContextInformation;
import org.eclipse.jface.text.contentassist.IContextInformationValidator;

/**
 * Provides completion proposals inside FreeMarker directives. 
 *
 * @author <a href="mailto:andras@omnetpp.org">Andras Varga</a>
 */
public class FtlDirectiveCompletionProcessor implements IContentAssistProcessor {

    @Override
    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int offset) {
        System.out.println("FTL COMPL!");
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public IContextInformation[] computeContextInformation(ITextViewer viewer, int offset) {
        return null;
    }

    @Override
    public char[] getCompletionProposalAutoActivationCharacters() {
        return new char[] { '#' };
    }

    @Override
    public char[] getContextInformationAutoActivationCharacters() {
        return null;
    }

    @Override
    public IContextInformationValidator getContextInformationValidator() {
        return null;
    }

    @Override
    public String getErrorMessage() {
        return null;
    }

}
