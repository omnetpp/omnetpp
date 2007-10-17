package org.omnetpp.msg.editor;


import org.eclipse.jface.text.DefaultIndentLineAutoEditStrategy;
import org.eclipse.jface.text.IAutoEditStrategy;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.presentation.IPresentationReconciler;
import org.eclipse.jface.text.presentation.PresentationReconciler;
import org.eclipse.jface.text.rules.DefaultDamagerRepairer;
import org.eclipse.jface.text.source.ISourceViewer;
import org.eclipse.jface.text.source.SourceViewerConfiguration;

import org.omnetpp.msg.editor.highlight.MsgCodeColorizerScanner;
import org.omnetpp.msg.editor.highlight.MsgDocColorizerScanner;
import org.omnetpp.msg.editor.highlight.MsgPrivateDocColorizerScanner;
import org.omnetpp.msg.editor.highlight.MsgSyntaxHighlightPartitionScanner;
import org.omnetpp.msg.editor.utils.MsgAutoIndentStrategy;

/**
 * Configuration for an <code>SourceViewer</code> which shows NED code.
 *
 * @author rhornig
 */
public class MsgSourceViewerConfiguration extends SourceViewerConfiguration {

    @Override
	public String[] getIndentPrefixes(ISourceViewer sourceViewer, String contentType) {
        return new String[] { "    ", "" }; 
    }
		
	@Override
	public IAutoEditStrategy[] getAutoEditStrategies(ISourceViewer sourceViewer, String contentType) {
		IAutoEditStrategy strategy= (IDocument.DEFAULT_CONTENT_TYPE.equals(contentType) ? new MsgAutoIndentStrategy() : new DefaultIndentLineAutoEditStrategy());
		return new IAutoEditStrategy[] { strategy };
	}
	
	
	@Override
	public String[] getDefaultPrefixes(ISourceViewer sourceViewer, String contentType) {
		return new String[] {MsgSyntaxHighlightPartitionScanner.MSG_DOC.equals(contentType) ? "// " : null};
	}
	
	@Override
	public IPresentationReconciler getPresentationReconciler(ISourceViewer sourceViewer) {
		PresentationReconciler reconciler= new PresentationReconciler();
        // syntax highlighting is using a separate partitioner
		reconciler.setDocumentPartitioning(MsgSyntaxHighlightPartitionScanner.PARTITIONING_ID);

        // colorizers for ned code
		DefaultDamagerRepairer dr = new DefaultDamagerRepairer(new MsgCodeColorizerScanner());
		reconciler.setDamager(dr, IDocument.DEFAULT_CONTENT_TYPE);
		reconciler.setRepairer(dr, IDocument.DEFAULT_CONTENT_TYPE);

        // colorizer for normal ned doc 
		dr = new DefaultDamagerRepairer(new MsgDocColorizerScanner());
		reconciler.setDamager(dr, MsgSyntaxHighlightPartitionScanner.MSG_DOC);
		reconciler.setRepairer(dr, MsgSyntaxHighlightPartitionScanner.MSG_DOC);
        
        // colorizer for private ned doc
        dr = new DefaultDamagerRepairer(new MsgPrivateDocColorizerScanner());
		reconciler.setDamager(dr, MsgSyntaxHighlightPartitionScanner.MSG_PRIVATE_DOC);
		reconciler.setRepairer(dr, MsgSyntaxHighlightPartitionScanner.MSG_PRIVATE_DOC);

		return reconciler;
	}
	
}
