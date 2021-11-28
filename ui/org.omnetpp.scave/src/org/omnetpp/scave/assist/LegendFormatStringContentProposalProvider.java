package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProviderBase;

/**
 * Proposal provider for legend format strings in chart dialogs. Proposes column names
 * after $ and ${.
 *
 * @author andras
 */
public class LegendFormatStringContentProposalProvider extends ContentProposalProviderBase {
    private List<String> columnNames;

    public LegendFormatStringContentProposalProvider(List<String> columnNames) {
        this.columnNames = columnNames;
    }

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        if (columnNames == null || columnNames.isEmpty())
            return new IContentProposal[0];

        String completionPrefix = getLinePrefix(contents, position);
        completionPrefix = completionPrefix.replaceFirst(".*?(\\$\\{?\\w*)?$", "$1"); // keep only the trailing incomplete variable ref ('$foo', '${foo'), or nothing if the string doesn't end in one
        List<IContentProposal> proposals = new ArrayList<>();
        for (String name : columnNames) {
            proposals.add(new ContentProposalEx("$"+name));
            proposals.add(new ContentProposalEx("${"+name+"}"));
        }

        sort(proposals);

        return filterAndWrapProposals(proposals, completionPrefix, false, position);
    }
}
