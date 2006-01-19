package org.omnetpp.ned.editor.text;


import org.eclipse.jface.text.*;
import org.eclipse.jface.text.contentassist.ContentAssistant;
import org.eclipse.jface.text.contentassist.IContentAssistant;
import org.eclipse.jface.text.presentation.IPresentationReconciler;
import org.eclipse.jface.text.presentation.PresentationReconciler;
import org.eclipse.jface.text.rules.DefaultDamagerRepairer;
import org.eclipse.jface.text.source.IAnnotationHover;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.SourceViewerConfiguration;
import org.omnetpp.ned.editor.text.assist.NedCompletionProcessor;
import org.omnetpp.ned.editor.text.assist.NedContentAssistPartitionScanner;
import org.omnetpp.ned.editor.text.assist.NedDocCompletionProcessor;
import org.omnetpp.ned.editor.text.assist.NedPrivateDocCompletionProcessor;
import org.omnetpp.ned.editor.text.highlight.NedCodeColorizerScanner;
import org.omnetpp.ned.editor.text.highlight.NedDocColorizerScanner;
import org.omnetpp.ned.editor.text.highlight.NedPrivateDocColorizerScanner;
import org.omnetpp.ned.editor.text.highlight.NedSyntaxHighlightPartitionScanner;
import org.omnetpp.ned.editor.text.util.NedAnnotationHover;
import org.omnetpp.ned.editor.text.util.NedAutoIndentStrategy;
import org.omnetpp.ned.editor.text.util.NedDoubleClickSelector;
import org.omnetpp.ned.editor.text.util.NedTextHover;

/**
 * Configuration for an <code>SourceViewer</code> which shows NED code.
 */
public class NedSourceViewerConfiguration extends SourceViewerConfiguration {

	public NedSourceViewerConfiguration() {
	}
	
	public IAnnotationHover getAnnotationHover(ISourceViewer sourceViewer) {
		return new NedAnnotationHover();
	}
		
	public IAutoEditStrategy[] getAutoEditStrategies(ISourceViewer sourceViewer, String contentType) {
		IAutoEditStrategy strategy= (IDocument.DEFAULT_CONTENT_TYPE.equals(contentType) ? new NedAutoIndentStrategy() : new DefaultIndentLineAutoEditStrategy());
		return new IAutoEditStrategy[] { strategy };
	}
	
    public IContentAssistant getContentAssistant(ISourceViewer sourceViewer) {

		ContentAssistant assistant= new ContentAssistant();
		assistant.setDocumentPartitioning(NedContentAssistPartitionScanner.PARTITIONING_ID);
		assistant.setContentAssistProcessor(new NedCompletionProcessor(), IDocument.DEFAULT_CONTENT_TYPE);
        assistant.setContentAssistProcessor(new NedDocCompletionProcessor(), NedContentAssistPartitionScanner.NED_DOC);
        assistant.setContentAssistProcessor(new NedPrivateDocCompletionProcessor(), NedContentAssistPartitionScanner.NED_PRIVATE_DOC);
        
		assistant.enableAutoActivation(true);
		assistant.setAutoActivationDelay(500);
		assistant.setProposalPopupOrientation(IContentAssistant.PROPOSAL_OVERLAY);
		assistant.setContextInformationPopupOrientation(IContentAssistant.CONTEXT_INFO_ABOVE);
        assistant.setInformationControlCreator(getInformationControlCreator(sourceViewer));

		return assistant;
	}
	
	public String getDefaultPrefix(ISourceViewer sourceViewer, String contentType) {
		return (NedContentAssistPartitionScanner.NED_DOC.equals(contentType) ? "// " : null); //$NON-NLS-1$
	}
	
	public ITextDoubleClickStrategy getDoubleClickStrategy(ISourceViewer sourceViewer, String contentType) {
		return new NedDoubleClickSelector();
	}
	
	public String[] getIndentPrefixes(ISourceViewer sourceViewer, String contentType) {
        // TODO indentation should be configurable
		return new String[] { "\t", "    " }; //$NON-NLS-1$ //$NON-NLS-2$
	}
	
	public IPresentationReconciler getPresentationReconciler(ISourceViewer sourceViewer) {
        
		PresentationReconciler reconciler= new PresentationReconciler();
        // syntax highlighting is using a separate partitioner
		reconciler.setDocumentPartitioning(NedSyntaxHighlightPartitionScanner.PARTITIONING_ID);

        // colorizers for ned code
		DefaultDamagerRepairer dr = new DefaultDamagerRepairer(new NedCodeColorizerScanner());
		reconciler.setDamager(dr, IDocument.DEFAULT_CONTENT_TYPE);
		reconciler.setRepairer(dr, IDocument.DEFAULT_CONTENT_TYPE);

        // colorizer for normal ned doc 
		dr = new DefaultDamagerRepairer(new NedDocColorizerScanner());
		reconciler.setDamager(dr, NedSyntaxHighlightPartitionScanner.NED_DOC);
		reconciler.setRepairer(dr, NedSyntaxHighlightPartitionScanner.NED_DOC);
        
        // colorizer for private ned doc
        dr = new DefaultDamagerRepairer(new NedPrivateDocColorizerScanner());
		reconciler.setDamager(dr, NedSyntaxHighlightPartitionScanner.NED_PRIVATE_DOC);
		reconciler.setRepairer(dr, NedSyntaxHighlightPartitionScanner.NED_PRIVATE_DOC);

		return reconciler;
	}
	
	public int getTabWidth(ISourceViewer sourceViewer) {
        // TODO should be configurable
		return 4;
	}
	
	public ITextHover getTextHover(ISourceViewer sourceViewer, String contentType) {
		return new NedTextHover();
	}
}
