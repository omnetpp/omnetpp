package org.omnetpp.common.contentassist;

import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.engine.Common;
import org.omnetpp.common.util.StringUtils;

/**
 * A "standard" implementation of IContentProposal as an immutable value class.
 *
 * The proposal specify a replacement of a range of text in the field
 * with the new content. The content can be decorated to add quotes,
 * parenthesis.
 * 
 * @author Andras
 */
@SuppressWarnings("unchecked")
public class ContentProposal implements IContentProposal, Comparable {
    // decorators
    public static final int DEC_NONE       = 0x00;
    public static final int DEC_OP         = 0x01;
    public static final int DEC_CP         = 0x02;
    public static final int DEC_QUOTE      = 0x04;
    public static final int DEC_SP_BEFORE  = 0x08;
    public static final int DEC_SP_AFTER   = 0x10;

	private String content;

	private String label;
	
	private String description;
    
	private int cursorPosition; // position of the cursor relative to content after the proposal accepted
    
	private int decorators; // additional characters added to the content
    
	private int startIndex; // start of the replaced range
    
	private int endIndex; // end of the replaced range
	
	public ContentProposal(String content) {
		this(content, content, content, content.length());
	}

    public ContentProposal(String content, String label) {
        this(content, label, null);
        this.cursorPosition = content.length();
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
        StringBuffer result = new StringBuffer(((decorators & DEC_QUOTE) != 0 ? quoteStringIfNeeded(content) : content));
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
        if ((decorators & DEC_QUOTE) != 0) position += quoteStringIfNeeded(content).length() - content.length(); // put cursor after the ending '"'
        if ((decorators & DEC_SP_BEFORE) != 0) position++;
        if ((decorators & DEC_OP) != 0) position++; // put cursor after the '('
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

	public int compareTo(Object o) {
		return label.compareTo(((IContentProposal)o).getLabel());
	}

	private static boolean needsQuotes(String pattern) {
        return Common.needsQuotes(pattern) || StringUtils.indexOfAny(pattern, " \t\n()") >= 0;
    }

	private static String quoteStringIfNeeded(String str) {
        return needsQuotes(str) ? Common.quoteString(str) : str;
    }
}