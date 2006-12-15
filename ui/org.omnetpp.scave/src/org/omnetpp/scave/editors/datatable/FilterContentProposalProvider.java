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
	private static ContentProposal[] fieldProposals;
	private static ContentProposal[] operatorProposals;
	private static final IContentProposal[] EMPTY = null; // new IContentProposal[0];
	
	private static final boolean debug = true;

	private Map<String,ContentProposal[]> patternProposals = new HashMap<String,ContentProposal[]>();
	
	enum TokenType {
		OPERATOR,
		FIELD,
		PATTERN,
	}
	
	static {
		String[] filterFields = Filter.getFieldNames();
		fieldProposals = new ContentProposal[filterFields.length];
		for (int i = 0; i < filterFields.length; ++i) {
			fieldProposals[i] = new ContentProposal(filterFields[i], true);
		}
		operatorProposals = new ContentProposal[3];
		operatorProposals[0] = new ContentProposal("AND");
		operatorProposals[1] = new ContentProposal("AND");
		operatorProposals[2] = new ContentProposal("AND");
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
		IContentProposal[] proposals = EMPTY;
		Token token = getContainingToken(contents, position);
		if (token != null) {
			Node parent = token.getParent();
			if (parent != null) {
				int type = parent.getType();
				if (type == Node.FIELDPATTERN && token == parent.getField()) {
					proposals = getFieldProposals(token.getValue());
				}
				else if (type == Node.PATTERN && token == parent.getPattern() ||
						 type == Node.FIELDPATTERN && (token == parent.getPattern() || token == parent.getOpeningParen())) {
					String field = type == Node.PATTERN ? Filter.getDefaultField() : parent.getFieldName();
					boolean addClosingParen = type == Node.FIELDPATTERN && parent.getClosingParen().isEmpty();
					proposals = getPatternProposals(field, parent.getPatternString(), addClosingParen);
				}
			}
		}
		if (debug) {
			if (proposals != null)
				for (IContentProposal proposal : proposals) {
					System.out.println("Proposal: " + proposal.getContent());
				}
			else
				System.out.println("No proposals");
		}
		return proposals;
	}
	
	private Token getContainingToken(String contents, final int position) {
		class Visitor implements INodeVisitor
		{
			Token token;
			
			public boolean visit(Node node) {
				return token == null;
			}
			
			public void visit(Token token) {
				if (debug)
					System.out.println("Visiting: " + token);
				if (token.getStartPos() < position && position <= token.getEndPos())
					this.token = token;
			}
		}

		if (debug)
			System.out.println("Position: " + position);
		Node root = FilterSyntax.parseFilter(contents);
		if (debug)
			System.out.println("Parse tree:\n" + root);
		Visitor visitor = new Visitor();
		root.accept(visitor);
		if (debug)
			System.out.println("Found: " + visitor.token);
		return visitor.token;
	}
	
	
	private IContentProposal[] getFieldProposals(String prefix) {
		// TODO: sort proposals
		return getFilteredProposals(fieldProposals, prefix, false);
	}
	
	private IContentProposal[] getPatternProposals(String field, String prefix, boolean addClosingParen) {
		return getFilteredProposals(patternProposals.get(field), prefix, addClosingParen);
	}
	
	private IContentProposal[] getFilteredProposals(ContentProposal[] proposals, String prefix, boolean addClosingParen) {
		List<ContentProposal> result = new ArrayList<ContentProposal>(fieldProposals.length);
		int prefixLength = prefix.length();
		if (proposals != null) {
			for (ContentProposal proposal : proposals) {
				if (proposal.getContent().startsWith(prefix)) {
					proposal.setPrefixLength(prefixLength);
					proposal.setClosingParenNeeded(addClosingParen);
					result.add(proposal);
				}
			}
		}
		return result.toArray(new IContentProposal[result.size()]);
	}
	
	static class ContentProposal implements IContentProposal {
		
		private String content;
		private int cursorPosition;
		private String label;
		private String description;
		private int prefixLength;
		private boolean closingParenNeeded;
		
		public ContentProposal(String proposal) {
			this(proposal, false);
		}
		
		public ContentProposal(String proposal, boolean addParenthesis) {
			if (addParenthesis) {
				content = proposal + "()";
				cursorPosition = proposal.length()+1;
			}
			else {
				content = proposal;
				cursorPosition = proposal.length();
			}
			label = proposal;
		}

		public String getContent() {
			String result  = prefixLength == 0 ? content : content.substring(prefixLength);
			if (closingParenNeeded) result += ")";
			return result;
		}
		public int getCursorPosition() {
			int position = cursorPosition - prefixLength;
			if (closingParenNeeded)
				position++;
			return position;
		}
		public String getDescription() {
			return description;
		}
		public String getLabel() {
			return label;
		}
		
		public void setPrefixLength(int length) {
			this.prefixLength = length;
		}
		
		public void setClosingParenNeeded(boolean closingParenNeeded) {
			this.closingParenNeeded = closingParenNeeded;
		}
	}
}
