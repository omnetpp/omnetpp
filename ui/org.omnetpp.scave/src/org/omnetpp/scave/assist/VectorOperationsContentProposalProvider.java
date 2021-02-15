package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProviderBase;
import org.omnetpp.scave.editors.ChartScriptEditor;

/**
 * Proposal provider for the Vector Operations field, used in the Input tab of of vector plot dialogs.
 *
 * @author andras
 */
public class VectorOperationsContentProposalProvider extends ContentProposalProviderBase {

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        String completionPrefix = getLinePrefix(contents, position);
        completionPrefix = StringUtils.stripStart(completionPrefix, " \t\n\r");  // TODO use .stripLeading() instead, in Java 11

        List<IContentProposal> proposals = new ArrayList<>();
        if (!completionPrefix.contains(":")) {
            proposals.add(new ContentProposalEx("apply:"));
            proposals.add(new ContentProposalEx("compute:"));
        }

        if (completionPrefix.matches("\\w+:.*"))
            completionPrefix = StringUtils.substringAfter(completionPrefix, ":").strip();

        //TODO refine: if user edits an arg value, content assist for further args is lost (prefix won't match)
        var vectorOpData = ChartScriptEditor.getVectorOpData();
        for (var op : vectorOpData)
            proposals.add(new ContentProposalEx(op.code, op.code, op.comment));

        sort(proposals);

        return filterAndWrapProposals(proposals, completionPrefix, false, position);
    }
}
