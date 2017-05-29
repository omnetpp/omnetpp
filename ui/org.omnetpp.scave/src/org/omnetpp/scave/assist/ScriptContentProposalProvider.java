/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.IContentProposalEx;
import org.omnetpp.scave.model2.FilterHints;

public class ScriptContentProposalProvider implements IContentProposalProvider {
    protected FilterContentProposalProvider filterContentProposalProvider = new FilterContentProposalProvider();
    protected static final boolean debug = true;

    ContentProposalEx p[] = {
            new ContentProposalEx("ADD"),
            new ContentProposalEx("scalars"),
            new ContentProposalEx("vectors"),
            new ContentProposalEx("histograms"),
            new ContentProposalEx("statistics"),
            new ContentProposalEx("WHERE")
    };

    public IContentProposal[] getProposals(String contents, int position) {
        List<IContentProposal> proposals = new ArrayList<IContentProposal>();

        int lineStartPos = contents.lastIndexOf('\n', position-1) + 1;
        
        String WHERE = " WHERE ";
        if (contents.substring(lineStartPos, position).contains(WHERE)) {
            int filterStartPos = contents.indexOf(WHERE, lineStartPos) + WHERE.length();
            Assert.isTrue(filterStartPos != -1);
            String filterPrefix = contents.substring(filterStartPos, position); 
            IContentProposalEx[] filterProposals = filterContentProposalProvider.getProposals(filterPrefix, filterPrefix.length());
            for (IContentProposal p : filterProposals) {
                ContentProposalEx pp = (ContentProposalEx)p;
                pp.setStartIndex(filterStartPos + pp.getStartIndex());
                pp.setEndIndex(filterStartPos + pp.getEndIndex());
                proposals.add(p);  //FIXME adds extra ")" after inserting values
            }
        }
        else {
            int spaceBefore = 0;
            collectFilteredProposals(proposals, p, "", position, position, spaceBefore);
        }

        return proposals.toArray(new IContentProposal[proposals.size()]);
    }

    public void setFilterHints(FilterHints filterHints) {
        filterContentProposalProvider.setFilterHints(filterHints);
    }

    protected void collectFilteredProposals(List<IContentProposal> result, ContentProposalEx[] proposals, String prefix, int startIndex, int endIndex, int decorators) {
        if (proposals != null) {
            for (ContentProposalEx proposal : proposals) {
                if (proposal.startsWith(prefix)) {
                    proposal.setStartIndex(startIndex);
                    proposal.setEndIndex(endIndex);
                    proposal.setDecorators(decorators);
                    result.add(proposal);
                }
            }
        }
    }

}
