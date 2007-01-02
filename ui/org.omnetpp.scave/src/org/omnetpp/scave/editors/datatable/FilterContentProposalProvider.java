package org.omnetpp.scave.editors.datatable;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.scave.model2.Filter;
import org.omnetpp.scave.model2.FilterHints;
import org.omnetpp.scave.model2.FilterSyntax;
import org.omnetpp.scave.model2.FilterSyntax.INodeVisitor;
import org.omnetpp.scave.model2.FilterSyntax.Node;
import org.omnetpp.scave.model2.FilterSyntax.Token;

class FilterContentProposalProvider implements IContentProposalProvider
{
	private static final boolean debug = true;

	private static ContentProposal[] fieldProposals;
	private static ContentProposal[] binaryOperatorProposals;
	private static ContentProposal[] unaryOperatorProposals;
	private static ContentProposal noProposal = new ContentProposal("", "No proposal");
	
	private Map<String,ContentProposal[]> patternProposals = new HashMap<String,ContentProposal[]>();
	
	static {
		String[] filterFields = Filter.getFieldNames();
		fieldProposals = new ContentProposal[filterFields.length];
		for (int i = 0; i < filterFields.length; ++i) {
			fieldProposals[i] = new ContentProposal(filterFields[i]);
		}
		binaryOperatorProposals = new ContentProposal[] {
				new ContentProposal("OR"),
				new ContentProposal("AND")
		};
		unaryOperatorProposals = new ContentProposal[] {
				new ContentProposal("NOT")
		};
	}
	
	public void setFilterHints(FilterHints hints) {
		// add pattern proposals for the filter hints
		patternProposals.clear();
		for (String field : Filter.getFieldNames()) {
			String[] patterns = hints.getHints(field);
			if (patterns != null) {
				ContentProposal[] proposals = new ContentProposal[patterns.length];
				for (int i = 0; i < patterns.length; ++i)
					proposals[i] = new ContentProposal(patterns[i]);
				patternProposals.put(field, proposals);
			}
		}
	}

	public IContentProposal[] getProposals(String contents, int position) {
		List<IContentProposal> result = new ArrayList<IContentProposal>();
		
		Token token = getContainingOrPrecedingToken(contents, position);
		if (token != null) {
			Node parent = token.getParent();
			if (parent != null) {
				int type = parent.getType();
				String prefix;
				int startIndex, endIndex;
				boolean addOpeningParen, addClosingParen;
				boolean atEnd = token.getEndPos() <= position;

				if (type == Node.FIELDPATTERN && token == parent.getField() && !atEnd) { // replace field name
					prefix = "";
					startIndex = token.getStartPos();
					endIndex = token.getEndPos();
					addOpeningParen = parent.getOpeningParen().isEmpty();
					addClosingParen = parent.getPattern().isEmpty() && parent.getClosingParen().isEmpty();
					collectFilteredProposals(result, fieldProposals, prefix, startIndex, endIndex,
							addOpeningParen, addClosingParen, false);
				}
				else if (type == Node.FIELDPATTERN && token == parent.getField() && atEnd) { // extend field name
					prefix = parent.getFieldName();
					startIndex = parent.getField().getStartPos();
					endIndex = parent.getField().getEndPos();
					collectFilteredProposals(result, fieldProposals, prefix, startIndex, endIndex,
							false, false, true);
				}
				else if (type == Node.FIELDPATTERN && token == parent.getPattern() && !atEnd) { // replace pattern
					prefix = "";
					startIndex = parent.getPattern().getStartPos();
					endIndex = parent.getPattern().getEndPos();
					addClosingParen = parent.getClosingParen().isEmpty();
					collectFilteredProposals(result, patternProposals.get(parent.getFieldName()), prefix, startIndex, endIndex,
							false, addClosingParen, true);
				}
				else if (type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getOpeningParen()) && atEnd) { // extend pattern
					prefix = parent.getPatternString();
					startIndex = parent.getPattern().getStartPos();
					endIndex = parent.getPattern().getEndPos();
					addClosingParen = parent.getClosingParen().isEmpty();
					collectFilteredProposals(result, patternProposals.get(parent.getFieldName()), prefix, startIndex, endIndex,
							false, addClosingParen, true);
				}
				else if (type == Node.FIELDPATTERN && token == parent.getClosingParen()) {
					collectFilteredProposals(result, binaryOperatorProposals, "", position, position, false, false, false);
				}
				else if (type == Node.PATTERN && token == parent.getPattern()) { // extend pattern or field name
					prefix = parent.getPatternString();
					startIndex = parent.getPattern().getStartPos();
					endIndex = parent.getPattern().getEndPos();
					String field = Filter.getDefaultField();
					collectFilteredProposals(result, unaryOperatorProposals, prefix, startIndex, endIndex, false, false, false);
					collectFilteredProposals(result, fieldProposals, prefix, startIndex, endIndex, true, true, false);
					collectFilteredProposals(result, patternProposals.get(field), prefix, startIndex, endIndex, false, false, true);
				}
				else if (type == Node.UNARY_OPERATOR_EXPR && !atEnd) { // replace unary operator
					collectFilteredProposals(result, unaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
							false, false, false);
				}
				else if (type == Node.BINARY_OPERATOR_EXPR && !atEnd) { // replace binary operator
					collectFilteredProposals(result, binaryOperatorProposals, "", token.getStartPos(), token.getEndPos(),
							false, false, false);
				}
				else if ((type == Node.BINARY_OPERATOR_EXPR  || type == Node.UNARY_OPERATOR_EXPR || type == Node.PARENTHESISED_EXPR) && atEnd) { // insert after unary or binary operator
					collectFilteredProposals(result, unaryOperatorProposals, "", position, position, false, false, false);
					collectFilteredProposals(result, fieldProposals, "", position, position, true, true, false);
					collectFilteredProposals(result, patternProposals.get(Filter.getDefaultField()), "", position, position, false, false, true);
				}
				else if (type == Node.ROOT && token.getType() == FilterSyntax.TokenType.END) {
					prefix = "";
					startIndex = position;
					endIndex = position;
					//collectFilteredProposals(result, operatorProposals, prefix, startIndex, endIndex, false, false, false);
					collectFilteredProposals(result, fieldProposals, prefix, startIndex, endIndex, true, true, false);
					collectFilteredProposals(result, patternProposals.get(Filter.getDefaultField()), prefix, startIndex, endIndex, false, false, true);
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
	
	private Token getContainingOrPrecedingToken(String contents, final int position) {
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
	
	
	private void collectFilteredProposals(List<IContentProposal> result, ContentProposal[] proposals, String prefix,
			int startIndex, int endIndex, boolean addOpeningParen, boolean addClosingParen, boolean addQuotes) {
		if (proposals != null) {
			for (ContentProposal proposal : proposals) {
				if (proposal.startsWith(prefix)) {
					proposal.setStartIndex(startIndex);
					proposal.setEndIndex(endIndex);
					proposal.setOpeningParenNeeded(addOpeningParen);
					proposal.setClosingParenNeeded(addClosingParen);
					proposal.setQuotesNeeded(addQuotes);
					result.add(proposal);
				}
			}
		}
	}
	
	public static class ContentProposal implements IContentProposal {
		
		private int startIndex;
		private int endIndex;
		private String content;
		private int cursorPosition;
		private String label;
		private String description;
		private boolean quotesNeeded;       // if true surrounds the content with '"' characters
		private boolean openingParenNeeded; // if true puts an '(' after the content
		private boolean closingParenNeeded; // if true puts a ')' after the content and opening parenthesis
		private boolean containsWhitespace; // true if the content should be quoted
		
		
		public ContentProposal(String proposal) {
			this(proposal, proposal);
		}

		public ContentProposal(String content, String label) {
			this.content = content;
			this.label = label;
			this.cursorPosition = content.length();
			this.containsWhitespace = content.indexOf(' ') >= 0;
		}
		
		public String getContent() {
			String result  = content;
			if (quotesNeeded) result = "\"" + result + "\"";
			if (openingParenNeeded) result += "(";
			if (closingParenNeeded) result += ") ";
			return result;
		}
		
		public int getStartIndex() {
			return startIndex;
		}
		
		public int getEndIndex() {
			return endIndex;
		}
		
		public int getCursorPosition() {
			int position = cursorPosition;
			if (quotesNeeded) position++; // shift due to beginning '"'
			if (openingParenNeeded)	position++;
			else if (closingParenNeeded) position += 2; // put cursor after the closing parenthesis if no opening inserted
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
		
		public void setOpeningParenNeeded(boolean openingParenNeeded) {
			this.openingParenNeeded = openingParenNeeded;
		}
		
		public void setClosingParenNeeded(boolean closingParenNeeded) {
			this.closingParenNeeded = closingParenNeeded;
		}
		
		public void setQuotesNeeded(boolean quotesNeeded) {
			this.quotesNeeded = quotesNeeded && containsWhitespace;
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
