package org.omnetpp.msg.editor;

import org.eclipse.core.filebuffers.IDocumentSetupParticipant;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentExtension3;
import org.eclipse.jface.text.IDocumentPartitioner;
import org.eclipse.jface.text.rules.FastPartitioner;

import org.omnetpp.msg.editor.highlight.MsgSyntaxHighlightPartitionScanner;

/**
 * Configures the NED editor by adding all sorts of features into it 
 *
 * @author rhornig
 */
public class MsgDocumentSetupParticipant implements IDocumentSetupParticipant {
	
	public void setup(IDocument document) {
		if (document instanceof IDocumentExtension3) {
			IDocumentExtension3 extension3 = (IDocumentExtension3) document;
            
            // syntax highlighter partitioner setup
            IDocumentPartitioner highlightPartitioner = 
                new FastPartitioner(new MsgSyntaxHighlightPartitionScanner(), MsgSyntaxHighlightPartitionScanner.SUPPORTED_PARTITION_TYPES);
            extension3.setDocumentPartitioner(MsgSyntaxHighlightPartitionScanner.PARTITIONING_ID, highlightPartitioner);
            highlightPartitioner.connect(document);
		}
	}
}
