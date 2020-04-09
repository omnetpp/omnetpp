/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.assist;

import java.util.List;

import org.omnetpp.common.Debug;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.IContentProposalEx;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.common.util.StringUtils;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterHintsCache;
import org.omnetpp.scave.model2.FilterUtil;

/**
 * Content proposal provider for the filter text fields.
 */
public class FilterExpressionProposalProvider extends MatchExpressionContentProposalProvider {
    private static ContentProposalEx[] fieldPrefixProposals = new ContentProposalEx[] {
        new ContentProposalEx("runattr:"),
        new ContentProposalEx("attr:"),
        new ContentProposalEx("itervar:"),
        new ContentProposalEx("param:"),
        new ContentProposalEx("config:")
    };

    private static ContentProposalEx[] fieldNameProposals;

    // Provides field value hints
    private ResultFileManager manager;
    private IDList idlist;
    private FilterHintsCache filterHintsCache;

    public FilterExpressionProposalProvider() {
        String[] resultFieldNames = ResultItemFields.getFieldNames().toArray();
        fieldNameProposals = toProposals(resultFieldNames);
    }

    public void setFilterHintsCache(FilterHintsCache filterHintsCache) {
        this.filterHintsCache = filterHintsCache;
    }

    public void setIDList(ResultFileManager manager, IDList idlist) {
        this.manager = manager;
        this.idlist = idlist;
    }

    /**
     * If the cursor is at the right end of a token then completions offered,
     * otherwise if the cursor is in the middle or at the beginning of a token
     * then a replacement offered.
     *
     * To execute the replacement, the decorated field should have
     * an IControlContentAdapter2.
     *
     * The returned proposals are instances of FilterContentProposalProvider.ContentProposal.
     */
    @Override
    public void addProposalsForToken(String contents, int position, Token token, List<IContentProposalEx> proposals) {
        Node parent = token.getParent();
        if (parent != null) {
            int type = parent.getType();
            String prefix;
            int startIndex, endIndex, decorators;
            boolean atEnd = token.getEndPos() <= position;

            Debug.println("PARENT: " + parent + " TYPE=" + type + " atEnd=" + atEnd);

            // inside field name: replace field name
            if (type == Node.FIELDPATTERN && token == parent.getField() && !atEnd) {
                Debug.println("addProposalsForToken(): case 1");
                startIndex = token.getStartPos();
                endIndex = token.getEndPos();
                collectFilteredProposals(proposals, fieldNameProposals, "", startIndex, endIndex, ContentProposalEx.DEC_MATCHES);
                collectFilteredProposals(proposals, fieldPrefixProposals, "", startIndex, endIndex, ContentProposalEx.DEC_NONE);
            }
            // after field name: complete field name
            else if (type == Node.FIELDPATTERN && token == parent.getField() && atEnd) {
                Debug.println("addProposalsForToken(): case 2");
                prefix = parent.getFieldName();
                startIndex = token.getStartPos();
                endIndex = token.getEndPos();
                collectFilteredProposals(proposals, fieldNameProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_MATCHES);
                collectFilteredProposals(proposals, getNameProposals(prefix), prefix, startIndex, endIndex, ContentProposalEx.DEC_MATCHES);
            }
            // inside the pattern of a field pattern: replace pattern with filter hints of the field
            else if (type == Node.FIELDPATTERN && token == parent.getPattern() && !atEnd) {
                Debug.println("addProposalsForToken(): case 3");
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                collectFilteredProposals(proposals, getValueProposals(parent.getFieldName()), "", startIndex, endIndex, ContentProposalEx.DEC_QUOTE);
            }
            // after the '(' of a field pattern: complete the pattern with hints of the field
            else if (type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getMatchesOp()) && atEnd) {
                Debug.println("addProposalsForToken(): case 4");
                prefix = parent.getPatternString();
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                collectFilteredProposals(proposals, getValueProposals(parent.getFieldName()), prefix, startIndex, endIndex, ContentProposalEx.DEC_QUOTE);
            }
            // after a ')' of a field pattern or parenthesized expression: insert binary operator
            else if ((type == Node.FIELDPATTERN || type == Node.PARENTHESISED_EXPR)&& token == parent.getClosingParen()) {
                Debug.println("addProposalsForToken(): case 5");
                int spaceBefore = (token.getEndPos() == position ? ContentProposalEx.DEC_SP_BEFORE : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, binaryOperatorProposals, "", position, position, spaceBefore);
            }
            // after pattern (may be a field name without '('): complete to field name, pattern, unary operator
            else if (type == Node.PATTERN && token == parent.getPattern()) {
                Debug.println("addProposalsForToken(): case 6");
                prefix = parent.getPatternString();
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                String field = FilterUtil.getDefaultField();
                collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, fieldNameProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_MATCHES);
                collectFilteredProposals(proposals, fieldPrefixProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, getNameProposals(prefix), prefix, startIndex, endIndex, ContentProposalEx.DEC_MATCHES);
                collectFilteredProposals(proposals, getValueProposals(field), prefix, startIndex, endIndex, ContentProposalEx.DEC_QUOTE);
            }
            // inside unary operator: replace unary operator
            else if (type == Node.UNARY_OPERATOR_EXPR && !atEnd) {
                Debug.println("addProposalsForToken(): case 7");
                collectFilteredProposals(proposals, unaryOperatorProposals, "", token.getStartPos(), token.getEndPos(), ContentProposalEx.DEC_NONE);
            }
            // inside binary operator: replace the binary operator
            else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) {
                Debug.println("addProposalsForToken(): case 8");
                collectFilteredProposals(proposals, binaryOperatorProposals, "", token.getStartPos(), token.getEndPos(), ContentProposalEx.DEC_NONE);
            }
            // incomplete binary operator
            else if ((type == Node.BINARY_OPERATOR_EXPR && token.isIncomplete())) {
                Debug.println("addProposalsForToken(): case 9");
                collectFilteredProposals(proposals, binaryOperatorProposals, token.getValue(), token.getStartPos(), token.getEndPos(), ContentProposalEx.DEC_NONE);
            }
            // after unary or binary operator or empty input: insert unary operator, field name or default pattern
            else if (((type == Node.BINARY_OPERATOR_EXPR  || type == Node.UNARY_OPERATOR_EXPR) && atEnd) ||
                     (type == Node.ROOT && token.getType() == MatchExpressionSyntax.TokenType.END)) {
                Debug.println("addProposalsForToken(): case 10");
                int spaceBefore = (token.getEndPos() == position ? ContentProposalEx.DEC_SP_BEFORE : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, unaryOperatorProposals, "", position, position, spaceBefore);
                collectFilteredProposals(proposals, fieldNameProposals, "", position, position, spaceBefore | ContentProposalEx.DEC_MATCHES);
                collectFilteredProposals(proposals, fieldPrefixProposals, "", position, position, spaceBefore);
                collectFilteredProposals(proposals, getValueProposals(FilterUtil.getDefaultField()), "", position, position, spaceBefore | ContentProposalEx.DEC_QUOTE);
            }
        }
    }

    protected ContentProposalEx[] getNameProposals(String input) {
        try {
            String prefix = StringUtils.substringBefore(input, ":") + ":";
            FilterField.Kind kind = FilterField.getKind(prefix);
            String[] hints = filterHintsCache.getNameHints(manager, idlist, kind);
            return toProposals(prefix, hints, "");
        }
        catch (IllegalArgumentException e) { // invalid prefix
            return new ContentProposalEx[0];
        }
    }

    protected ContentProposalEx[] getValueProposals(String fieldName) {
        FilterField field = new FilterField(fieldName);
        String[] hints = filterHintsCache.getValueHints(manager, idlist, field);
        return toProposals(hints);
    }

    protected ContentProposalEx[] toProposals(String[] hints) {
        ContentProposalEx[] proposals = new ContentProposalEx[hints.length];
        for (int i=0; i<hints.length; i++)
            proposals[i] = new ContentProposalEx(hints[i]);
        return proposals;
    }

    protected ContentProposalEx[] toProposals(String prefix, String[] hints, String suffix) {
        ContentProposalEx[] proposals = new ContentProposalEx[hints.length];
        for (int i=0; i<hints.length; i++)
            proposals[i] = new ContentProposalEx(prefix + hints[i] + suffix);
        return proposals;
    }

}
