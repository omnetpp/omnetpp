package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.StringUtils;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProviderBase;
import org.omnetpp.scave.editors.VectorOperations.VectorOp;

/**
 * Proposal provider for the Vector Operations field, used in the Input tab of of vector plot dialogs.
 *
 * @author andras
 */
public class VectorOperationsContentProposalProvider extends ContentProposalProviderBase {
    protected List<VectorOp> operations;

    public VectorOperationsContentProposalProvider(List<VectorOp> operations) {
        this.operations = operations;
    }

    @Override
    public IContentProposal[] getProposals(String contents, int position) {
        String completionPrefix = getLinePrefix(contents, position);
        completionPrefix = completionPrefix.stripLeading();

        List<IContentProposal> proposals = new ArrayList<>();
        if (!completionPrefix.contains(":")) {
            proposals.add(new ContentProposalEx("apply:"));
            proposals.add(new ContentProposalEx("compute:"));
        }

        //TODO refine: if user edits an arg value, content assist for further args is lost (prefix won't match)
        if (completionPrefix.matches("\\w+:.*"))
            completionPrefix = StringUtils.substringAfter(completionPrefix, ":").strip();

        for (VectorOp op : operations)
            proposals.add(new ContentProposalEx(op.name + (op.hasArgs ? op.signature : ""), op.label, op.docstring));

        sort(proposals);

        return filterAndWrapProposals(proposals, completionPrefix, false, position);
    }
}
