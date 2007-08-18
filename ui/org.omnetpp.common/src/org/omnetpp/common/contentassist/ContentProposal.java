package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.IContentProposal;

/**
 * A "standard" implementation of IContentProposal as an immutable value class.
 * 
 * @author Andras
 */
@SuppressWarnings("unchecked")
public class ContentProposal implements IContentProposal, Comparable {
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