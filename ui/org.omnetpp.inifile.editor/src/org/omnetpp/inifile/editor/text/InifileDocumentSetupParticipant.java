/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text;

import org.eclipse.core.filebuffers.IDocumentSetupParticipant;
import org.eclipse.jface.text.IDocument;
import org.eclipse.jface.text.IDocumentExtension3;
import org.eclipse.jface.text.IDocumentPartitioner;
import org.eclipse.jface.text.rules.FastPartitioner;
import org.omnetpp.inifile.editor.text.highlight.InifileSyntaxHighlightPartitionScanner;

/**
 * Configures the inifile text editor by adding various features into it
 */
//FIXME: trying to use ctrl-H "Search" with NED files open result in "SWTException: Invalid thread access"!
public class InifileDocumentSetupParticipant implements IDocumentSetupParticipant {

    /**
     */
    public InifileDocumentSetupParticipant() {
    }

    /*
     * @see org.eclipse.core.filebuffers.IDocumentSetupParticipant#setup(org.eclipse.jface.text.IDocument)
     */
    public void setup(IDocument document) {
        if (document instanceof IDocumentExtension3) {
            IDocumentExtension3 extension3= (IDocumentExtension3) document;

//XXX unused code
//            // content assist partitioner setup
//            IDocumentPartitioner contentPartitioner =
//                new FastPartitioner(new NedContentAssistPartitionScanner(), NedContentAssistPartitionScanner.SUPPORTED_PARTITION_TYPES);
//          extension3.setDocumentPartitioner(NedContentAssistPartitionScanner.PARTITIONING_ID, contentPartitioner);
//          contentPartitioner.connect(document);

            // syntax highlighter partitioner setup
            IDocumentPartitioner highlightPartitioner =
                new FastPartitioner(new InifileSyntaxHighlightPartitionScanner(), InifileSyntaxHighlightPartitionScanner.SUPPORTED_PARTITION_TYPES);
            extension3.setDocumentPartitioner(InifileSyntaxHighlightPartitionScanner.PARTITIONING_ID, highlightPartitioner);
            highlightPartitioner.connect(document);

            // outline partitioner setup
            // XXX unused code
            //IDocumentPartitioner outlinePartitioner =
            //    new FastPartitioner(new NedOutlinePartitionScanner(), NedOutlinePartitionScanner.SUPPORTED_PARTITION_TYPES);
            //extension3.setDocumentPartitioner(NedOutlinePartitionScanner.PARTITIONING_ID, outlinePartitioner);
            //outlinePartitioner.connect(document);
        }
    }
}
