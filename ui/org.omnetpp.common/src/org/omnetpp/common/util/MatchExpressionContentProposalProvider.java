package org.omnetpp.common.util;

import java.util.ArrayList;
import java.util.List;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.Debug;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.util.MatchExpressionSyntax.INodeVisitor;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;

public abstract class MatchExpressionContentProposalProvider implements IContentProposalProvider {
    protected static final boolean debug = false;

    protected static ContentProposal noProposal = new ContentProposal("", "No proposal", null);

    protected static ContentProposal[] binaryOperatorProposals = new ContentProposal[] {
        new ContentProposal("OR"),
        new ContentProposal("AND")
    };
    
    protected static ContentProposal[] unaryOperatorProposals = new ContentProposal[] {
        new ContentProposal("NOT")
    };

    public IContentProposal[] getProposals(String contents, int position) {
        List<IContentProposal> proposals = new ArrayList<IContentProposal>();
        Token token = getContainingOrPrecedingToken(contents, position);

        if (token != null)
            addProposalsForToken(contents, position, token, proposals);

        if (proposals.isEmpty())
            proposals.add(noProposal);

        if (debug)
            for (IContentProposal proposal : proposals)
                Debug.println("Proposal: " + proposal.getContent());

        return proposals.toArray(new IContentProposal[proposals.size()]);
    }

    protected abstract void addProposalsForToken(String contents, int position, Token token, List<IContentProposal> proposals);

    /**
     * Finds the leaf node (token) in the parse tree that contains the {@code position}
     * either in the middle or at the right end.
     *
     * @param contents the content of the filter field
     * @param position a position within the {@code contents}
     * @return the token containing the position or null if no such token
     */
    protected Token getContainingOrPrecedingToken(String contents, final int position) {
        // Visitor for the parse tree which remembers the last visited node
        // before position
        class Visitor implements INodeVisitor
        {
            boolean found;
            Token token;

            public boolean visit(Node node) {
                return !found;
            }

            public void visit(Token token) {
                if (debug)
                    Debug.println("Visiting: " + token);
                if (!found) {
                    if (token.getStartPos() >= position && this.token != null)
                        found = true;
                    else
                        this.token = token;
                }
            }
        }

        if (debug) {
            Debug.println("Position: " + position);
            Debug.println("Parsing: " + contents);
        }

        Node root = MatchExpressionSyntax.parseFilter(contents);
        
        if (debug)
            Debug.println("Parse tree:\n" + root);
        
        Visitor visitor = new Visitor();
        root.accept(visitor);

        if (debug)
            Debug.println("Found: " + visitor.token);
        
        return visitor.token;
    }

    /**
     * Collects the items from {@code proposals} starting with {@code prefix} into {@result}.
     * The proposals are modified according to the other parameters.
     *
     * @param result the list of the collected proposals
     * @param proposals the list of proposals to be filtered
     * @param prefix the required prefix of the proposals
     * @param startIndex the start index of the range to be replaced
     * @param endIndex the end index of the range to be replaced
     * @param decorators various decoration options
     */
    protected void collectFilteredProposals(List<IContentProposal> result, ContentProposal[] proposals, String prefix, int startIndex, int endIndex, int decorators) {
        if (proposals != null) {
            for (ContentProposal proposal : proposals) {
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
