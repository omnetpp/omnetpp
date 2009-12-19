package freemarker.eclipse.editors;

import org.eclipse.core.filebuffers.IDocumentSetupParticipant;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentExtension3;
import org.eclipse.jface.text.IDocumentPartitioner;
import org.eclipse.jface.text.rules.FastPartitioner;

/**
 * Adds content assist to the FreeMarker template editor. 
 *
 * @author <a href="mailto:andras@omnetpp.org">Andras Varga</a>
 */
public class SetupParticipant implements IDocumentSetupParticipant {

	public void setup(IDocument document) {
		if (document instanceof IDocumentExtension3) {
			IDocumentExtension3 extension3 = (IDocumentExtension3) document;
            
            // content assist partitioner setup
            IDocumentPartitioner contentPartitioner = new FastPartitioner(new PartitionScanner(), PartitionScanner.PARTITIONS);
			extension3.setDocumentPartitioner(PartitionScanner.PARTITIONING_ID, contentPartitioner);
			contentPartitioner.connect(document);

			document.setDocumentPartitioner(contentPartitioner);
		}
	}
}
