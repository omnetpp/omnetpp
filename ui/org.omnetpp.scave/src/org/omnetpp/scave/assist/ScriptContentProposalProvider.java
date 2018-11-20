/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang3.ArrayUtils;
import org.apache.commons.lang3.StringUtils;
import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.Debug;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.IContentProposalEx;
import org.omnetpp.scave.computed.VectorOperation;
import org.omnetpp.scave.computed.VectorOperations;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.script.IScriptConstants;

public class ScriptContentProposalProvider implements IContentProposalProvider, IScriptConstants {
    protected FilterContentProposalProvider filterContentProposalProvider = new FilterContentProposalProvider();
    protected static final boolean debug = true;
    private static final String SP = " ";

    public IContentProposal[] getProposals(String contents, int position) {
        List<IContentProposal> proposals = new ArrayList<IContentProposal>();

        int lineStartPos = contents.lastIndexOf('\n', position-1) + 1;

        String _WHERE_ = SP + WHERE + SP;
        if (contents.substring(lineStartPos, position).contains(_WHERE_)) {
            int filterStartPos = contents.indexOf(_WHERE_, lineStartPos) + _WHERE_.length();
            Assert.isTrue(filterStartPos != -1);
            int lineEndPos = contents.indexOf('\n', position);
            if (lineEndPos == -1)
                lineEndPos = contents.length();
            String filter = contents.substring(filterStartPos, lineEndPos);
            IContentProposalEx[] filterProposals = filterContentProposalProvider.getProposals(filter, position - filterStartPos);
            for (IContentProposal p : filterProposals) {
                ContentProposalEx pp = (ContentProposalEx)p;
                pp.setStartIndex(filterStartPos + pp.getStartIndex());
                pp.setEndIndex(filterStartPos + pp.getEndIndex());
                proposals.add(p);
            }
        }
        else {
            String linePrefix = contents.substring(lineStartPos, position);
            String lastWord = linePrefix.replaceFirst("^.* ", "");
            String trimmedLinePrefix = linePrefix.substring(0, linePrefix.length() - lastWord.length());

            ContentProposalEx[] candidates = null;
            if (StringUtils.isBlank(trimmedLinePrefix)) {
                candidates = new ContentProposalEx[] {
                        new ContentProposalEx(ADD+SP, ADD, ADD_HELP),
                        new ContentProposalEx(APPLY+SP, APPLY, APPLY_HELP),
                        new ContentProposalEx(COMPUTE+SP, COMPUTE, COMPUTE_HELP)
                };
            }

            if (trimmedLinePrefix.matches("(?i)ADD +")) {
                candidates = new ContentProposalEx[] {
                        new ContentProposalEx("scalars "),
                        new ContentProposalEx("vectors "),
                        new ContentProposalEx("histograms ")
                };
            }

            if (trimmedLinePrefix.matches("(?i)ADD +[a-z]+ ")) {
                candidates = new ContentProposalEx[] {
                        new ContentProposalEx(WHERE+SP, WHERE, WHERE_HELP)
                };
            }

            if (trimmedLinePrefix.matches("(?i)APPLY +")) {
                List<ContentProposalEx> tmp = new ArrayList<>();
                for (VectorOperation op : VectorOperations.ALL) {
                    tmp.add(new ContentProposalEx(op.getName(), op.getName(), op.getDescription()));
                }
                candidates = tmp.toArray(new ContentProposalEx[]{});
            }

            if (trimmedLinePrefix.matches("(?)COMPUTE +")) {
                candidates = new ContentProposalEx[] {
                        new ContentProposalEx("scalars "),
                        new ContentProposalEx("scalars NAMED foo AS "),
                        new ContentProposalEx("scalars NAMED <scalarName> AS <valueExpr> MODULE <moduleExpr> GROUPING <groupByExpr> OPTIONS averageReplications, computeStddev, computeConfidenceInterval, confidenceLevel, computeMinMax WHERE "),
                };
            }

            if (trimmedLinePrefix.matches("(?i)COMPUTE +[a-z]+ .*")) {
                List<ContentProposalEx> tmp = new ArrayList<>();
                String[] clauseKeywords = COMPUTE_CLAUSE_KEYWORDS;
                String[] tokens = trimmedLinePrefix.split(" ");
                String lastToken = tokens[tokens.length - 1].toUpperCase();
                if (ArrayUtils.contains(clauseKeywords, lastToken)) {
                    if (lastToken.equals(OPTIONS)) {
                        //FIXME only allows one option
                        for (String keyword : COMPUTE_OPTION_KEYWORDS)
                            if (!ArrayUtils.contains(tokens, keyword))
                                tmp.add(new ContentProposalEx(keyword + " ", keyword, HELP_TEXTS.get(keyword)));
                    }
                    else {
                        //TODO add proposals for that keyword
                    }
                }
                else {
                    for (String keyword : clauseKeywords)
                        if (!ArrayUtils.contains(tokens, keyword))
                            tmp.add(new ContentProposalEx(keyword + " ", keyword, HELP_TEXTS.get(keyword)));
                }
                candidates = tmp.toArray(new ContentProposalEx[]{});
            }

            if (candidates != null)
                collectFilteredProposals(proposals, candidates, lastWord, position-lastWord.length(), position, ContentProposalEx.DEC_NONE);
        }

        return proposals.toArray(new IContentProposal[proposals.size()]);
    }

    public void setFilterHints(FilterHints filterHints) {
        filterContentProposalProvider.setFilterHints(filterHints);
    }

    protected void collectFilteredProposals(List<IContentProposal> result, ContentProposalEx[] proposals, String prefix, int startIndex, int endIndex, int decorators) {
        if (proposals != null) {
            for (ContentProposalEx proposal : proposals) {
                Debug.println("prefix=" + prefix + "  proposal=" + proposal.getContent());
                if (StringUtils.startsWithIgnoreCase(proposal.getContent(), prefix)) {
                    proposal.setStartIndex(startIndex);
                    proposal.setEndIndex(endIndex);
                    proposal.setDecorators(decorators);
                    result.add(proposal);
                }
            }
        }
    }

}
