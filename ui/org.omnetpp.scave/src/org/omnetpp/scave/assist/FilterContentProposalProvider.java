/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.scave.assist;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.IContentProposalEx;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterField.Kind;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;

/**
 * Content proposal provider for the filter text fields.
 */
public class FilterContentProposalProvider extends MatchExpressionContentProposalProvider
{
    private static ContentProposalEx[] fieldPrefixProposals = new ContentProposalEx[] {
        new ContentProposalEx("attr:"),
        new ContentProposalEx("itervar:"),
        new ContentProposalEx("param:")
    };

    // Proposals for field names
    private ContentProposalEx[] fieldProposals;
    // Proposals for prefixed field names
    private ContentProposalEx[] prefixedFieldProposals;
    // Maps fields names to patterns
    private Map<String,ContentProposalEx[]> patternProposals = new HashMap<String,ContentProposalEx[]>();

    public FilterContentProposalProvider() {
        String[] filterFields = ResultItemFields.getFieldNames().toArray();
        prefixedFieldProposals = new ContentProposalEx[0];
        fieldProposals = new ContentProposalEx[filterFields.length];
        for (int i = 0; i < filterFields.length; ++i) {
            fieldProposals[i] = new ContentProposalEx(filterFields[i], filterFields[i]+"()");
        }
    }

    /**
     * The proposals for pattern fields depends on the content of the analysis file,
     * and this method is used to pass the offered proposals for each field as a
     * FilterHints object.
     * @param hints the hints for field patterns
     */
    public void setFilterHints(FilterHints hints) {
        // add pattern proposals for the filter hints
        FilterField[] fields = hints.getFields();
        List<ContentProposalEx> fieldProposals = new ArrayList<ContentProposalEx>();
        List<ContentProposalEx> prefixedFieldProposals = new ArrayList<ContentProposalEx>();
        for (FilterField field : fields) {
            String fieldName = field.getFullName();
            if (field.getKind() == Kind.ItemField)
                fieldProposals.add(new ContentProposalEx(fieldName, fieldName+"()"));
            else
                prefixedFieldProposals.add(new ContentProposalEx(fieldName, fieldName+"()"));
        }
        this.fieldProposals = fieldProposals.toArray(new ContentProposalEx[fieldProposals.size()]);
        this.prefixedFieldProposals = prefixedFieldProposals.toArray(new ContentProposalEx[fieldProposals.size()]);
        patternProposals.clear();
        for (FilterField field : fields) {
            String[] patterns = hints.getHints(field);
            if (patterns != null) {
                ContentProposalEx[] proposals = new ContentProposalEx[patterns.length];
                for (int i = 0; i < patterns.length; ++i)
                    proposals[i] = new ContentProposalEx(patterns[i]);
                patternProposals.put(field.getFullName(), proposals);
            }
        }
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

            // inside field name: replace field name
            if (type == Node.FIELDPATTERN && token == parent.getField() && !atEnd) {
                startIndex = token.getStartPos();
                endIndex = token.getEndPos();
                decorators = ContentProposalEx.DEC_QUOTE |
                             (parent.getOpeningParen().isEmpty() ? ContentProposalEx.DEC_OP : ContentProposalEx.DEC_NONE) |
                             (parent.getPattern().isEmpty() && parent.getClosingParen().isEmpty() ? ContentProposalEx.DEC_CP : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, fieldProposals, "", startIndex, endIndex, decorators);
                collectFilteredProposals(proposals, fieldPrefixProposals, "", startIndex, endIndex, ContentProposalEx.DEC_NONE);
            }
            // after field name: complete field name
            else if (type == Node.FIELDPATTERN && token == parent.getField() && atEnd) {
                prefix = parent.getFieldName();
                startIndex = token.getStartPos();
                endIndex = token.getEndPos();
                collectFilteredProposals(proposals, fieldProposals, prefix, startIndex, endIndex,   ContentProposalEx.DEC_QUOTE);
                collectFilteredProposals(proposals, prefixedFieldProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_QUOTE);
            }
            // inside the pattern of a field pattern: replace pattern with filter hints of the field
            else if (type == Node.FIELDPATTERN && token == parent.getPattern() && !atEnd) {
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                decorators = ContentProposalEx.DEC_QUOTE | (parent.getClosingParen().isEmpty() ? ContentProposalEx.DEC_CP : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, patternProposals.get(parent.getFieldName()), "", startIndex, endIndex,
                        decorators);
            }
            // after the '(' of a field pattern: complete the pattern with hints of the field
            else if (type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getOpeningParen()) && atEnd) {
                prefix = parent.getPatternString();
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                decorators = ContentProposalEx.DEC_QUOTE | (parent.getClosingParen().isEmpty() ? ContentProposalEx.DEC_CP : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, patternProposals.get(parent.getFieldName()), prefix, startIndex, endIndex,
                        decorators);
            }
            // after a ')' of a field pattern or parenthesized expression: insert binary operator
            else if ((type == Node.FIELDPATTERN || type == Node.PARENTHESISED_EXPR)&& token == parent.getClosingParen()) {
                int spaceBefore = (token.getEndPos() == position ? ContentProposalEx.DEC_SP_BEFORE : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, binaryOperatorProposals, "", position, position, spaceBefore);
            }
            // after pattern (may be a field name without '('): complete to field name, pattern, unary operator
            else if (type == Node.PATTERN && token == parent.getPattern()) {
                prefix = parent.getPatternString();
                startIndex = parent.getPattern().getStartPos();
                endIndex = parent.getPattern().getEndPos();
                String field = FilterUtil.getDefaultField();
                collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, fieldProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_QUOTE | ContentProposalEx.DEC_OP | ContentProposalEx.DEC_CP);
                collectFilteredProposals(proposals, fieldPrefixProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_NONE);
                if (endIndex > startIndex)
                    collectFilteredProposals(proposals, prefixedFieldProposals, prefix, startIndex, endIndex, ContentProposalEx.DEC_QUOTE | ContentProposalEx.DEC_OP | ContentProposalEx.DEC_CP);
                collectFilteredProposals(proposals, patternProposals.get(field), prefix, startIndex, endIndex, ContentProposalEx.DEC_QUOTE);
            }
            // inside unary operator: replace unary operator
            else if (type == Node.UNARY_OPERATOR_EXPR && !atEnd) {
                collectFilteredProposals(proposals, unaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
                        ContentProposalEx.DEC_NONE);
            }
            // inside binary operator: replace the binary operator
            else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) {
                collectFilteredProposals(proposals, binaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
                        ContentProposalEx.DEC_NONE);
            }
            // incomplete binary operator
            else if ((type == Node.BINARY_OPERATOR_EXPR && token.isIncomplete())) {
                collectFilteredProposals(proposals, binaryOperatorProposals, token.getValue(), token.getStartPos(), token.getEndPos(), ContentProposalEx.DEC_NONE);
            }
            // after unary or binary operator or empty input: insert unary operator, field name or default pattern
            else if (((type == Node.BINARY_OPERATOR_EXPR  || type == Node.UNARY_OPERATOR_EXPR) && atEnd) ||
                     (type == Node.ROOT && token.getType() == MatchExpressionSyntax.TokenType.END)) {
                int spaceBefore = (token.getEndPos() == position ? ContentProposalEx.DEC_SP_BEFORE : ContentProposalEx.DEC_NONE);
                collectFilteredProposals(proposals, unaryOperatorProposals, "", position, position, spaceBefore);
                collectFilteredProposals(proposals, fieldProposals, "", position, position, spaceBefore | ContentProposalEx.DEC_QUOTE | ContentProposalEx.DEC_OP | ContentProposalEx.DEC_CP);
                collectFilteredProposals(proposals, fieldPrefixProposals, "", position, position, spaceBefore);
                collectFilteredProposals(proposals, patternProposals.get(FilterUtil.getDefaultField()), "", position, position, spaceBefore | ContentProposalEx.DEC_QUOTE);
            }
        }
        System.out.println(proposals);
    }
}
