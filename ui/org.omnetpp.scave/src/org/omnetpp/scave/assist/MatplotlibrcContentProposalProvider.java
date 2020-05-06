package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProvider;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.pychart.PythonProcess;

/**
 * Proposal provider for Matplotlib rcParams, used in the Advanced tab of Matplotlib chart dialogs.
 *
 * @author andras
 */
public class MatplotlibrcContentProposalProvider extends ContentProposalProvider {

    public MatplotlibrcContentProposalProvider() {
        super(false, true);
    }

    @Override
    protected List<IContentProposal> getProposalCandidates(String text) {
        Map<String, String> rcParams = PythonProcess.getMatplotlibRcParams();
        if (rcParams == null)
            return new ArrayList<IContentProposal>();

        String prefix = getCompletionPrefix(text);

        List<IContentProposal> proposals = new ArrayList<>();
        if (!prefix.contains(":")) {
            // propose keys
            for (String key : rcParams.keySet())
                proposals.add(new ContentProposalEx(key + " : ", key + " : ", key + "\nDefault: " + rcParams.get(key)));
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
