package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.contentassist.ContentProposal;
import org.omnetpp.common.util.MatchExpressionContentProposalProvider;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.Node;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.FilterField.Kind;

/**
 * Content proposal provider for the filter text fields.
 */
class FilterContentProposalProvider extends MatchExpressionContentProposalProvider
{
	private static ContentProposal[] fieldPrefixProposals = new ContentProposal[] {
	    new ContentProposal("attr:"),
        new ContentProposal("param:")
    };

	// Proposals for field names
	private ContentProposal[] fieldProposals;
	// Proposals for prefixed field names
	private ContentProposal[] prefixedFieldProposals;
	// Maps fields names to patterns
	private Map<String,ContentProposal[]> patternProposals = new HashMap<String,ContentProposal[]>();

	public FilterContentProposalProvider() {
		String[] filterFields = ResultItemFields.getFieldNames().toArray();
		prefixedFieldProposals = new ContentProposal[0];
		fieldProposals = new ContentProposal[filterFields.length];
		for (int i = 0; i < filterFields.length; ++i) {
			fieldProposals[i] = new ContentProposal(filterFields[i], filterFields[i]+"()");
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
		List<ContentProposal> fieldProposals = new ArrayList<ContentProposal>();
		List<ContentProposal> prefixedFieldProposals = new ArrayList<ContentProposal>();
		for (FilterField field : fields) {
			String fieldName = field.getFullName();
			if (field.getKind() == Kind.ItemField)
				fieldProposals.add(new ContentProposal(fieldName, fieldName+"()"));
			else
				prefixedFieldProposals.add(new ContentProposal(fieldName, fieldName+"()"));
		}
		this.fieldProposals = fieldProposals.toArray(new ContentProposal[fieldProposals.size()]);
		this.prefixedFieldProposals = prefixedFieldProposals.toArray(new ContentProposal[fieldProposals.size()]);
		patternProposals.clear();
		for (FilterField field : fields) {
			String[] patterns = hints.getHints(field);
			if (patterns != null) {
				ContentProposal[] proposals = new ContentProposal[patterns.length];
				for (int i = 0; i < patterns.length; ++i)
					proposals[i] = new ContentProposal(patterns[i]);
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
	public void addProposalsForToken(String contents, int position, Token token, List<IContentProposal> proposals) {
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
				decorators = ContentProposal.DEC_QUOTE |
							 (parent.getOpeningParen().isEmpty() ? ContentProposal.DEC_OP : ContentProposal.DEC_NONE) |
							 (parent.getPattern().isEmpty() && parent.getClosingParen().isEmpty() ? ContentProposal.DEC_CP : ContentProposal.DEC_NONE);
				collectFilteredProposals(proposals, fieldProposals, "", startIndex, endIndex, decorators);
				collectFilteredProposals(proposals, fieldPrefixProposals, "", startIndex, endIndex, ContentProposal.DEC_NONE);
			}
			// after field name: complete field name
			else if (type == Node.FIELDPATTERN && token == parent.getField() && atEnd) {
				prefix = parent.getFieldName();
				startIndex = token.getStartPos();
				endIndex = token.getEndPos();
				collectFilteredProposals(proposals, fieldProposals, prefix, startIndex, endIndex,	ContentProposal.DEC_QUOTE);
				collectFilteredProposals(proposals, prefixedFieldProposals, prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE);
			}
			// inside the pattern of a field pattern: replace pattern with filter hints of the field
			else if (type == Node.FIELDPATTERN && token == parent.getPattern() && !atEnd) {
				startIndex = parent.getPattern().getStartPos();
				endIndex = parent.getPattern().getEndPos();
				decorators = ContentProposal.DEC_QUOTE | (parent.getClosingParen().isEmpty() ? ContentProposal.DEC_CP : ContentProposal.DEC_NONE);
				collectFilteredProposals(proposals, patternProposals.get(parent.getFieldName()), "", startIndex, endIndex,
						decorators);
			}
			// after the '(' of a field pattern: complete the pattern with hints of the field
			else if (type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getOpeningParen()) && atEnd) {
				prefix = parent.getPatternString();
				startIndex = parent.getPattern().getStartPos();
				endIndex = parent.getPattern().getEndPos();
				decorators = ContentProposal.DEC_QUOTE | (parent.getClosingParen().isEmpty() ? ContentProposal.DEC_CP : ContentProposal.DEC_NONE);
				collectFilteredProposals(proposals, patternProposals.get(parent.getFieldName()), prefix, startIndex, endIndex,
						decorators);
			}
			// after a ')' of a field pattern or parenthesized expression: insert binary operator
			else if ((type == Node.FIELDPATTERN || type == Node.PARENTHESISED_EXPR)&& token == parent.getClosingParen()) {
				int spaceBefore = (token.getEndPos() == position ? ContentProposal.DEC_SP_BEFORE : ContentProposal.DEC_NONE);
				collectFilteredProposals(proposals, binaryOperatorProposals, "", position, position, spaceBefore);
			}
			// after pattern (may be a field name without '('): complete to field name, pattern, unary operator
			else if (type == Node.PATTERN && token == parent.getPattern()) {
				prefix = parent.getPatternString();
				startIndex = parent.getPattern().getStartPos();
				endIndex = parent.getPattern().getEndPos();
				String field = FilterUtil.getDefaultField();
				collectFilteredProposals(proposals, unaryOperatorProposals, prefix, startIndex, endIndex, ContentProposal.DEC_NONE);
				collectFilteredProposals(proposals, fieldProposals, prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE | ContentProposal.DEC_OP | ContentProposal.DEC_CP);
				collectFilteredProposals(proposals, fieldPrefixProposals, prefix, startIndex, endIndex, ContentProposal.DEC_NONE);
				if (endIndex > startIndex)
					collectFilteredProposals(proposals, prefixedFieldProposals, prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE | ContentProposal.DEC_OP | ContentProposal.DEC_CP);
				collectFilteredProposals(proposals, patternProposals.get(field), prefix, startIndex, endIndex, ContentProposal.DEC_QUOTE);
			}
			// inside unary operator: replace unary operator
			else if (type == Node.UNARY_OPERATOR_EXPR && !atEnd) {
				collectFilteredProposals(proposals, unaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
						ContentProposal.DEC_NONE);
			}
			// inside binary operator: replace the binary operator
			else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) {
				collectFilteredProposals(proposals, binaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
						ContentProposal.DEC_NONE);
			}
			// incomplete binary operator
			else if ((type == Node.BINARY_OPERATOR_EXPR && token.isIncomplete())) {
				collectFilteredProposals(proposals, binaryOperatorProposals, token.getValue(), token.getStartPos(), token.getEndPos(), ContentProposal.DEC_NONE);
			}
			// after unary or binary operator or empty input: insert unary operator, field name or default pattern
			else if (((type == Node.BINARY_OPERATOR_EXPR  || type == Node.UNARY_OPERATOR_EXPR) && atEnd) ||
					 (type == Node.ROOT && token.getType() == MatchExpressionSyntax.TokenType.END)) {
				int spaceBefore = (token.getEndPos() == position ? ContentProposal.DEC_SP_BEFORE : ContentProposal.DEC_NONE);
				collectFilteredProposals(proposals, unaryOperatorProposals, "", position, position, spaceBefore);
				collectFilteredProposals(proposals, fieldProposals, "", position, position, spaceBefore | ContentProposal.DEC_QUOTE | ContentProposal.DEC_OP | ContentProposal.DEC_CP);
				collectFilteredProposals(proposals, fieldPrefixProposals, "", position, position, spaceBefore);
				collectFilteredProposals(proposals, patternProposals.get(FilterUtil.getDefaultField()), "", position, position, spaceBefore | ContentProposal.DEC_QUOTE);
			}
		}
	}
}
