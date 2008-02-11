package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.scave.engine.ResultItemFields;
import org.omnetpp.scave.model2.FilterField;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterSyntax;
import org.omnetpp.scave.model2.FilterUtil;
import org.omnetpp.scave.model2.FilterField.Kind;
import org.omnetpp.scave.model2.FilterSyntax.INodeVisitor;
import org.omnetpp.scave.model2.FilterSyntax.Node;
import org.omnetpp.scave.model2.FilterSyntax.Token;

/**
 * Content proposal provider for the filter text fields. 
 */
class FilterContentProposalProvider implements IContentProposalProvider
{
	private static final boolean debug = true;

	private static ContentProposal[] binaryOperatorProposals;
	private static ContentProposal[] unaryOperatorProposals;
	private static ContentProposal[] fieldPrefixProposals;
	private static ContentProposal noProposal = new ContentProposal("", "No proposal");
	
	// Proposals for field names
	private ContentProposal[] fieldProposals;
	// Proposals for prefixed field names
	private ContentProposal[] prefixedFieldProposals;
	// Maps fields names to patterns
	private Map<String,ContentProposal[]> patternProposals = new HashMap<String,ContentProposal[]>();
	
	// decorators
	private static final int DEC_NONE		= 0x00;
	private static final int DEC_OP 		= 0x01;
	private static final int DEC_CP 		= 0x02;
	private static final int DEC_QUOTE		= 0x04;
	private static final int DEC_SP_BEFORE	= 0x08;
	private static final int DEC_SP_AFTER	= 0x10;
	
	
	static {
		binaryOperatorProposals = new ContentProposal[] {
				new ContentProposal("OR"),
				new ContentProposal("AND")
		};
		unaryOperatorProposals = new ContentProposal[] {
				new ContentProposal("NOT")
		};
		fieldPrefixProposals = new ContentProposal[] {
				new ContentProposal("attr:"),
				new ContentProposal("param:")
		};
	}
	
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
	 * Implementation of {@link IContentProposal.getProposals}.
	 * If the cursor is at the right end of a token then completions offered,
	 * otherwise if the cursor is in the middle or at the beginning of a token
	 * then a replacement offered.
	 * 
	 * To execute the replacement, the decorated field should have
	 * an IControlContentAdapter2.
	 * 
	 * The returned proposals are instances of FilterContentProposalProvider.ContentProposal.
	 */
	public IContentProposal[] getProposals(String contents, int position) {
		List<IContentProposal> result = new ArrayList<IContentProposal>();
		
		Token token = getContainingOrPrecedingToken(contents, position);
		if (token != null) {
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
					decorators = DEC_QUOTE |
								 (parent.getOpeningParen().isEmpty() ? DEC_OP : DEC_NONE) |
								 (parent.getPattern().isEmpty() && parent.getClosingParen().isEmpty() ? DEC_CP : DEC_NONE);
					collectFilteredProposals(result, fieldProposals, "", startIndex, endIndex, decorators);
					collectFilteredProposals(result, fieldPrefixProposals, "", startIndex, endIndex, DEC_NONE);
				}
				// after field name: complete field name
				else if (type == Node.FIELDPATTERN && token == parent.getField() && atEnd) {
					prefix = parent.getFieldName();
					startIndex = token.getStartPos();
					endIndex = token.getEndPos();
					collectFilteredProposals(result, fieldProposals, prefix, startIndex, endIndex,	DEC_QUOTE);
					collectFilteredProposals(result, prefixedFieldProposals, prefix, startIndex, endIndex, DEC_QUOTE);
				}
				// inside the pattern of a field pattern: replace pattern with filter hints of the field
				else if (type == Node.FIELDPATTERN && token == parent.getPattern() && !atEnd) {
					startIndex = parent.getPattern().getStartPos();
					endIndex = parent.getPattern().getEndPos();
					decorators = DEC_QUOTE | (parent.getClosingParen().isEmpty() ? DEC_CP : DEC_NONE);
					collectFilteredProposals(result, patternProposals.get(parent.getFieldName()), "", startIndex, endIndex,
							decorators);
				}
				// after the '(' of a field pattern: complete the pattern with hints of the field
				else if (type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getOpeningParen()) && atEnd) {
					prefix = parent.getPatternString();
					startIndex = parent.getPattern().getStartPos();
					endIndex = parent.getPattern().getEndPos();
					decorators = DEC_QUOTE | (parent.getClosingParen().isEmpty() ? DEC_CP : DEC_NONE);
					collectFilteredProposals(result, patternProposals.get(parent.getFieldName()), prefix, startIndex, endIndex,
							decorators);
				}
				// after a ')' of a field pattern or parenthesised expression: insert binary operator
				else if ((type == Node.FIELDPATTERN || type == Node.PARENTHESISED_EXPR)&& token == parent.getClosingParen()) {
					int spaceBefore = (token.getEndPos() == position ? DEC_SP_BEFORE : DEC_NONE);
					collectFilteredProposals(result, binaryOperatorProposals, "", position, position, spaceBefore);
				}
				// after pattern (may be a field name without '('): complete to field name, pattern, unary operator
				else if (type == Node.PATTERN && token == parent.getPattern()) {
					prefix = parent.getPatternString();
					startIndex = parent.getPattern().getStartPos();
					endIndex = parent.getPattern().getEndPos();
					String field = FilterUtil.getDefaultField();
					collectFilteredProposals(result, unaryOperatorProposals, prefix, startIndex, endIndex, DEC_NONE);
					collectFilteredProposals(result, fieldProposals, prefix, startIndex, endIndex, DEC_QUOTE | DEC_OP | DEC_CP);
					collectFilteredProposals(result, fieldPrefixProposals, prefix, startIndex, endIndex, DEC_NONE);
					if (endIndex > startIndex)
						collectFilteredProposals(result, prefixedFieldProposals, prefix, startIndex, endIndex, DEC_QUOTE | DEC_OP | DEC_CP);
					collectFilteredProposals(result, patternProposals.get(field), prefix, startIndex, endIndex, DEC_QUOTE);
				}
				// inside unary operator: replace unary operator
				else if (type == Node.UNARY_OPERATOR_EXPR && !atEnd) {
					collectFilteredProposals(result, unaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
							DEC_NONE);
				}
				// inside binary operator: replace the binary operator
				else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) {
					collectFilteredProposals(result, binaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
							DEC_NONE);
				}
				// incomplete binary operator
				else if ((type == Node.BINARY_OPERATOR_EXPR && token.isIncomplete())) {
					collectFilteredProposals(result, binaryOperatorProposals, token.getValue(), token.getStartPos(), token.getEndPos(), DEC_NONE);
				}
				// after unary or binary operator or empty input: insert unary operator, field name or default pattern
				else if (((type == Node.BINARY_OPERATOR_EXPR  || type == Node.UNARY_OPERATOR_EXPR) && atEnd) ||
						 (type == Node.ROOT && token.getType() == FilterSyntax.TokenType.END)) {
					int spaceBefore = (token.getEndPos() == position ? DEC_SP_BEFORE : DEC_NONE);
					collectFilteredProposals(result, unaryOperatorProposals, "", position, position, spaceBefore);
					collectFilteredProposals(result, fieldProposals, "", position, position, spaceBefore | DEC_QUOTE | DEC_OP | DEC_CP);
					collectFilteredProposals(result, fieldPrefixProposals, "", position, position, spaceBefore);
					collectFilteredProposals(result, patternProposals.get(FilterUtil.getDefaultField()), "", position, position, spaceBefore | DEC_QUOTE);
				}
			}
		} 
		
		if (result.isEmpty()) {
			result.add(noProposal);
		}

		if (debug) {
			for (IContentProposal proposal : result) {
				System.out.println("Proposal: " + proposal.getContent());
			}
		}

		return result.toArray(new IContentProposal[result.size()]);
	}
	
	/**
	 * Finds the leaf node (token) in the parse tree that contains the {@code position}
	 * either in the middle or at the right end.
	 * 
	 * @param contents the content of the filter field
	 * @param position a position within the {@code contents}
	 * @return the token containing the position or null if no such token 
	 */
	private Token getContainingOrPrecedingToken(String contents, final int position) {
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
					System.out.println("Visiting: " + token);
				if (!found) {
					if (token.getStartPos() >= position && this.token != null)
						found = true;
					else
						this.token = token;
				}
			}
		}

		if (debug) {
			System.out.println("Position: " + position);
			System.out.println("Parsing: " + contents);
		}
		Node root = FilterSyntax.parseFilter(contents);
		if (debug)
			System.out.println("Parse tree:\n" + root);
		Visitor visitor = new Visitor();
		root.accept(visitor);
		if (debug)
			System.out.println("Found: " + visitor.token);
		return visitor.token;
	}
	
	
	/**
	 * Collects the items from {@code proposals} starting with {@code prefix} into {@result}.
	 * The proposals are modified according to the other parameters.
	 * 
	 * @param result the list of the collected proposals
	 * @param proposals	the list of proposals to be filtered
	 * @param prefix the required prefix of the proposals
	 * @param startIndex the start index of the range to be replaced
	 * @param endIndex the end index of the range to be replaced
	 * @param addOpeningParen if true '(' appended
	 * @param addClosingParen if true ") " appended
	 * @param addQuotes if true '"' are added around
	 * @param addSpace if true ' ' appended 
	 */
	private void collectFilteredProposals(List<IContentProposal> result, ContentProposal[] proposals, String prefix,
			int startIndex, int endIndex, int decorators) {
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
	
	/**
	 * Content proposal for the filter field.
	 * 
	 * The proposal specify a replacement of a range of text in the field
	 * with the new content. The content can be decorated to add quotes,
	 * parenthesis.
	 * 
	 * The class implemented as mutable data.
	 */
	public static class ContentProposal implements IContentProposal {
		
		private int startIndex;				// start of the replaced range
		private int endIndex;				// end of the replaced range
		private String content;				// replacement
		private int cursorPosition;			// position of the cursor relative to content after the proposal accepted
		private String label;				// text displayed in the proposal popup
		private String description;			// desciption the proposal, TODO: fill this field
		private int decorators;				// additional characters added to the content
		
		public ContentProposal(String proposal) {
			this(proposal, proposal);
		}

		public ContentProposal(String content, String label) {
			this.content = content;
			this.label = label;
			this.cursorPosition = content.length();
		}
		
		public String getContent() {
			StringBuffer result = new StringBuffer(
					((decorators & DEC_QUOTE) != 0 ? FilterUtil.quoteStringIfNeeded(content) : content));
			if ((decorators & DEC_SP_BEFORE) != 0) result.insert(0, ' ');
			if ((decorators & DEC_OP) != 0) result.append('(');
			if ((decorators & DEC_CP) != 0) result.append(") ");
			if ((decorators & DEC_SP_AFTER) != 0) result.append(' ');
			return result.toString(); 
		}
		
		public int getStartIndex() {
			return startIndex;
		}
		
		public int getEndIndex() {
			return endIndex;
		}
		
		public int getCursorPosition() {
			int position = cursorPosition;
			if ((decorators & DEC_QUOTE) != 0) position+= FilterUtil.quoteStringIfNeeded(content).length() - content.length(); // put cursor after the ending '"'
			if ((decorators & DEC_SP_BEFORE) != 0) position++;
			if ((decorators & DEC_OP) != 0)	position++; // put cursor after the '('
			else if ((decorators & DEC_CP) != 0) position += 2; // put cursor after the ')' if no opening inserted
			if ((decorators & DEC_SP_AFTER) != 0) position++;
			return position;
		}
		
		public String getDescription() {
			return description;
		}
		public String getLabel() {
			return label;
		}
		
		public void setStartIndex(int index) {
			startIndex = index;
		}
		
		public void setEndIndex(int index) {
			endIndex = index;
		}
		
		public void setDecorators(int decorators) {
			this.decorators = decorators;
		}
		
		public boolean startsWith(String prefix) {
			return content.startsWith(prefix);
		}
	}
	
	public static void test() {
		FilterContentProposalProvider provider = new FilterContentProposalProvider();
		provider.getProposals("", 0);
	}
}
