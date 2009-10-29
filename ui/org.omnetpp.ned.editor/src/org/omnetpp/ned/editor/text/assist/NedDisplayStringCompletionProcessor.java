package org.omnetpp.ned.editor.text.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.text.ITextViewer;
import org.eclipse.jface.text.contentassist.ICompletionProposal;
import org.eclipse.swt.graphics.Image;
import org.eclipse.ui.texteditor.ITextEditor;
import org.omnetpp.common.editor.text.NedCompletionHelper;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper.NedDisplayStringTagDetector;
import org.omnetpp.common.image.ImageFactory;

public class NedDisplayStringCompletionProcessor extends AbstractNedCompletionProcessor {
    public NedDisplayStringCompletionProcessor(ITextEditor editor) {
        super(editor);
    }
    
    @Override
    public ICompletionProposal[] computeCompletionProposals(ITextViewer viewer, int documentOffset) {
        // long startMillis = System.currentTimeMillis(); // measure time

        List<ICompletionProposal> result = new ArrayList<ICompletionProposal>();

        // find out where we are: in which module, submodule, which section etc.
        CompletionInfo info = computeCompletionInfo(viewer, documentOffset);
        // if the position is invalid return no proposals
        if (info == null || info.linePrefix == null || info.linePrefixTrimmed == null)
            return new ICompletionProposal[0];
    
        String line = info.linePrefixTrimmed;

        // display string
        if (line.matches("@display\\(\".*")) {
            if (line.matches(".*(i|i2)=")) {
                List<String> names = ImageFactory.getImageNameList();
                String[] proposals = names.toArray(new String[0]);
                Image[] images = new Image[proposals.length];
                for (int i = 0; i < proposals.length; i++) {
                    String proposal = proposals[i];
                    images[i] = ImageFactory.getIconImage(proposal);
                }
                addProposals(viewer, documentOffset, result, proposals, proposals, images);
            }
            else if (info.sectionType == SECT_PARAMETERS)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedComponentDisplayStringTempl, new NedDisplayStringTagDetector());
            else if (info.sectionType == SECT_SUBMODULE_PARAMETERS)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedSubmoduleDisplayStringTempl, new NedDisplayStringTagDetector());
            else if (info.sectionType == SECT_CONNECTIONS)
                addProposals(viewer, documentOffset, result, NedCompletionHelper.proposedNedConnectionDisplayStringTempl, new NedDisplayStringTagDetector());
        }

        // long millis = System.currentTimeMillis()-startMillis;
        // Debug.println("Proposal creation: "+millis+"ms");

        return (ICompletionProposal[]) result.toArray(new ICompletionProposal[result.size()]);
    }
}
