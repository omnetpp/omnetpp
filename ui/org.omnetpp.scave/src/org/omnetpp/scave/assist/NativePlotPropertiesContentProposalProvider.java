package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProviderBase;
import org.omnetpp.scave.charting.properties.PlotProperty;

/**
 * Proposal provider for native plot properties, used in the Advanced tab of native chart dialogs.
 *
 * @author andras
 */
public class NativePlotPropertiesContentProposalProvider extends ContentProposalProviderBase {

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        String completionPrefix = getLinePrefix(contents, position).stripLeading();

        List<IContentProposal> proposals = new ArrayList<>();
        if (!completionPrefix.contains(":")) {
            // propose keys
            for (PlotProperty p : PlotProperty.values())
                proposals.add(new ContentProposalEx(p.getName() + " : ", p.getName() + " : ", p.getName() + "\nDefault: " + p.getDefaultValueAsString()));
        }
        else {
            // propose value
            // String key = StringUtils.substringBefore(prefix, ":").trim();
            // ...
        }

        sort(proposals);
        return filterAndWrapProposals(proposals, completionPrefix, false, position);
    }
}
