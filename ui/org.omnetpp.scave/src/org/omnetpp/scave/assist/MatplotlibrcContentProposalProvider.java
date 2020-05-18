package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProviderBase;
import org.omnetpp.scave.pychart.PythonProcess;

/**
 * Proposal provider for Matplotlib rcParams, used in the Advanced tab of Matplotlib chart dialogs.
 *
 * @author andras
 */
public class MatplotlibrcContentProposalProvider extends ContentProposalProviderBase {

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        Map<String, String> rcParams = PythonProcess.getMatplotlibRcParams();
        if (rcParams == null)
            return new IContentProposal[0];

        String completionPrefix = getLinePrefix(contents, position).stripLeading();

        List<IContentProposal> proposals = new ArrayList<>();
        if (!completionPrefix.contains(":")) {
            // propose keys
            for (String key : rcParams.keySet())
                proposals.add(new ContentProposalEx(key + " : ", key + " : ", key + "\nDefault: " + rcParams.get(key)));
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
