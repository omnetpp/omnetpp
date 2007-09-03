package org.omnetpp.ned.editor.text;


import org.eclipse.core.runtime.NullProgressMonitor;
import org.eclipse.jface.text.DefaultIndentLineAutoEditStrategy;
import org.eclipse.jface.text.IAutoEditStrategy;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.ITextDoubleClickStrategy;
import org.eclipse.jface.text.ITextHover;
import org.eclipse.jface.text.contentassist.ContentAssistant;
import org.eclipse.jface.text.contentassist.IContentAssistant;
import org.eclipse.jface.text.hyperlink.IHyperlinkDetector;
import org.eclipse.jface.text.hyperlink.URLHyperlinkDetector;
import org.eclipse.jface.text.presentation.IPresentationReconciler;
import org.eclipse.jface.text.presentation.PresentationReconciler;
import org.eclipse.jface.text.reconciler.IReconciler;
import org.eclipse.jface.text.reconciler.MonoReconciler;
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
import org.omnetpp.ned.editor.text.util.NEDHyperlinkDetector;
import org.omnetpp.ned.editor.text.util.NedAnnotationHover;
import org.omnetpp.ned.editor.text.util.NedAutoIndentStrategy;
import org.omnetpp.ned.editor.text.util.NedDoubleClickSelector;
import org.omnetpp.ned.editor.text.util.NedTextHover;

/**
 * Configuration for an <code>SourceViewer</code> which shows NED code.
 *
 * @author rhornig
 */
public class NedSourceViewerConfiguration extends SourceViewerConfiguration {

	private TextualNedEditor editor = null; // because NEDReconcileStrategy will need IFile from editorInput

	public NedSourceViewerConfiguration(TextualNedEditor editor) {
		this.editor = editor;
	}
	
	@Override
	public IAnnotationHover getAnnotationHover(ISourceViewer sourceViewer) {
		return new NedAnnotationHover();
	}

    @Override
	public String[] getIndentPrefixes(ISourceViewer sourceViewer, String contentType) {
        return new String[] { "    ", "" }; 
    }
		
	@Override
	public IAutoEditStrategy[] getAutoEditStrategies(ISourceViewer sourceViewer, String contentType) {
		IAutoEditStrategy strategy= (IDocument.DEFAULT_CONTENT_TYPE.equals(contentType) ? new NedAutoIndentStrategy() : new DefaultIndentLineAutoEditStrategy());
		return new IAutoEditStrategy[] { strategy };
	}
	
    @Override
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
	
	@Override
	public String[] getDefaultPrefixes(ISourceViewer sourceViewer, String contentType) {
		return new String[] {NedContentAssistPartitionScanner.NED_DOC.equals(contentType) ? "// " : null};
	}
	
	@Override
	public ITextDoubleClickStrategy getDoubleClickStrategy(ISourceViewer sourceViewer, String contentType) {
		return new NedDoubleClickSelector();
	}
	
	@Override
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
	
	@Override
	public ITextHover getTextHover(ISourceViewer sourceViewer, String contentType) {
		return new NedTextHover(editor);
	}

	@Override
	public IReconciler getReconciler(ISourceViewer sourceViewer) {
		// Installs background NED parsing.
		// Based on: JavaSourceViewerConfiguration.getReconciler() in JDT which
		// creates and configures JavaReconciler; than in turn will eventually
		// result in calls to org.eclipse.jdt.internal.compiler.parser.Parser.
		MonoReconciler reconciler = new MonoReconciler(new NEDReconcileStrategy(editor), true);
		reconciler.setIsIncrementalReconciler(false);
		reconciler.setIsAllowedToModifyDocument(false);
		reconciler.setProgressMonitor(new NullProgressMonitor());
		reconciler.setDelay(500);
		return reconciler;
	}
    
    @Override
    public IHyperlinkDetector[] getHyperlinkDetectors(ISourceViewer sourceViewer) {
        if (sourceViewer == null)
            return null;

        return new IHyperlinkDetector[] { new URLHyperlinkDetector(), new NEDHyperlinkDetector(editor) };
    }
}
