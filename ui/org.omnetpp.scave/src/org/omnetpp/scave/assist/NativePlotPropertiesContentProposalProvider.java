package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.StringUtils;
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
        String completionPrefix = getLinePrefix(contents, position);
        // XXX: could just use .stripLeading() instead, in Java 11
        completionPrefix = StringUtils.stripStart(completionPrefix, " \t\n\r");

        List<IContentProposal> proposals = new ArrayList<>();
        if (!completionPrefix.contains(":")) {
            // propose keys
            for (PlotProperty p : PlotProperty.values()) {
                boolean po = p.isPerObject();
                proposals.add(new ContentProposalEx(
                        p.getName() + (po ? "/1": "") + " : ",
                        p.getName() + (po ? "/<key>": "") + " : ",
                        p.getDefaultValue() == null ? "" : "Default: " +  p.getDefaultValueAsString()));
            }
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
