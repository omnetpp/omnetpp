package org.omnetpp.inifile.editor.contentassist;

import java.util.ArrayList;
import java.util.Arrays;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.eclipse.jface.fieldassist.IContentProposalProvider;
import org.omnetpp.common.util.StringUtils;

/**
 * A basic content proposal provider. Provides a basic IContentProposal implementation,
 * and sorts and filters by prefix the set of proposal candidates provided by the
 * abstract getProposalCandidates() method.
 * 
 * Note: although IContentProposalProvider is for field editors, we use this class
 * in the text editor content assist too; we just re-wrap IContentProposals to 
 * ICompletionProposal. 
 * 
 * @author Andras
 */
public abstract class ContentProposalProvider implements IContentProposalProvider {
	/**
	 * Value object that implements IContentProposal.
	 */
	static class ContentProposal implements IContentProposal, Comparable {
		private String content;
		private String label;
		private String description;
		private int cursorPosition;
		
		public ContentProposal(String content) {
			this(content, content, content, content.length());
		}

		public ContentProposal(String content, String label, String description) {
			this(content, label, description, content.length());
		}

		public ContentProposal(String content, String label, String description, int cursorPosition) {
			this.content = content;
			this.label = label;
			this.description = description;
			this.cursorPosition = cursorPosition;
		}

		public String getContent() {
			return content;
		}

		public int getCursorPosition() {
			return cursorPosition;
		}

		public String getDescription() {
			return description;
		}

		public String getLabel() {
			return label;
		}

		public int compareTo(Object o) {
			return label.compareTo(((IContentProposal)o).getLabel());
		}
	}
	
	/* (non-Javadoc)
	 * @see org.eclipse.jface.fieldassist.IContentProposalProvider#getProposals(java.lang.String, int)
	 */
	public IContentProposal[] getProposals(String contents, int position) {
		ArrayList<IContentProposal> result = new ArrayList<IContentProposal>();

		String prefix = contents.substring(0, position);
		
		// calculate the last word that the user started to type. This is the basis of
		// proposal filtering: they have to start with this prefix.
		String prefixToMatch = getPrefixToMatch(prefix);

		IContentProposal[] candidates = getProposalCandidates(prefix);

		if (candidates!=null) {
			Arrays.sort(candidates);

			// check if any of the proposals has description. If they do, we set "(no description)" 
			// on the others as well. Reason: if left at null, previous tooltip will be shown, 
			// which is very confusing.
			boolean descriptionSeen = false;
			for (IContentProposal p : candidates)
				if (!StringUtils.isEmpty(p.getDescription()))
					descriptionSeen = true;

			// collect those candidates that match the last incomplete word in the editor
			for (IContentProposal candidate : candidates) {
				String content = candidate.getContent();
				if (content.startsWith(prefixToMatch) && content.length()!= prefixToMatch.length()) {
					// from the content, drop the prefix that has already been typed by the user
					String modifiedContent = content.substring(prefixToMatch.length(), content.length());
					int modifiedCursorPosition = candidate.getCursorPosition() + modifiedContent.length() - content.length();
					String description = (StringUtils.isEmpty(candidate.getDescription()) && descriptionSeen) ? "(no description)" : candidate.getDescription();
					result.add(new ContentProposal(modifiedContent, candidate.getLabel(), description, modifiedCursorPosition));
				}
			}
		}

		if (result.isEmpty()) {
			// returning an empty array or null apparently causes NPE in the framework, so return a blank proposal instead
			result.add(new ContentProposal("", "(no proposal)", null, 0));
		}
		return result.toArray(new IContentProposal[] {});
	}

	protected String getPrefixToMatch(String prefix) {
		//return prefix; -- use this when the line should only contain one word (e.g.true/false)
		return prefix.replaceFirst("^.*?([A-Za-z0-9_]*)$", "$1");
	}

	/**
	 * Generate a list of proposal candidates. They will be sorted and filtered by prefix
	 * before presenting them to the user.
	 */
	abstract protected IContentProposal[] getProposalCandidates(String prefix);

	/**
	 * Turn strings into proposals.
	 */
	protected static IContentProposal[] toProposals(String[] strings) {
		IContentProposal[] p = new IContentProposal[strings.length];
		for (int i=0; i<p.length; i++)
			p[i] = new ContentProposal(strings[i], strings[i], null);
		return p;
	}
	
}
