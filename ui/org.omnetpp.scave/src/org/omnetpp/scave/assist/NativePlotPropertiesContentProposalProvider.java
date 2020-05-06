package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.charting.properties.PlotProperty;

/**
 * Proposal provider for native plot properties, used in the Advanced tab of native chart dialogs.
 *
 * @author andras
 */
public class NativePlotPropertiesContentProposalProvider extends ContentProposalProvider {

    public NativePlotPropertiesContentProposalProvider() {
        super(false, true);
    }

    @Override
    protected List<IContentProposal> getProposalCandidates(String text) {
        String prefix = getCompletionPrefix(text);

        List<IContentProposal> proposals = new ArrayList<>();
        if (!prefix.contains(":")) {
            // propose keys
            for (PlotProperty p : PlotProperty.values())
                proposals.add(new ContentProposalEx(p.getName() + " : ", p.getName() + " : ", p.getName() + "\nDefault: " + p.getDefaultValueAsString()));
        }
        else {
            // propose value
            // String key = StringUtils.substringBefore(prefix, ":").trim();
            // ...
        }

        return sort(proposals);
    }

    @Override
    protected String getCompletionPrefix(String text) {
      text = text.replace("\\\n", ""); // join continued lines
      if (text.contains("\n"))
          text =  StringUtils.substringAfterLast(text, "\n"); // take last line
      return text.stripLeading();
    }
}
