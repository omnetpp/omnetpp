package org.omnetpp.common.editor.text;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.util.DisplayUtils;

/**
 * Data for syntax highlighting
 * 
 * @author rhornig, andras
 */
public class SyntaxHighlightHelper {
    // word lists for syntax highlighting
	public final static String[] highlightPrivateDocTodo = Keywords.DOC_TODO;
	public final static String[] highlightDocTags = Keywords.DOC_TAGS;
	public final static String[] highlightDocKeywords = Keywords.DOC_KEYWORDS;
	public final static String[] highlightNedTypes = Keywords.NED_TYPE_KEYWORDS;
	public final static String[] highlightNedSpecialKeywords = Keywords.NED_SPECIAL_KEYWORDS;
    public final static String[] highlightNedKeywords = Keywords.concat(Keywords.NED_NONEXPR_KEYWORDS, Keywords.NED_EXPR_KEYWORDS);
	public final static String[] highlightNedFunctions = Keywords.NED_FUNCTIONS;
	public final static String[] highlightMsgTypes = Keywords.MSG_TYPE_KEYWORDS;
	public final static String[] highlightMsgKeywords = Keywords.concat(Keywords.MSG_KEYWORDS, Keywords.MSG_SECTION_KEYWORDS);
	public final static String[] highlightConstants = Keywords.BOOL_CONSTANTS;

    // tokens for syntax highlighting
	public final static IToken docDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
	public final static IToken docKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));
	public final static IToken docTagToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_RED)));
	public final static IToken docPrivateDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLUE), null, SWT.ITALIC));
	public final static IToken docPrivateTodoToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));
	public final static IToken codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
	public final static IToken codeKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_RED), null, SWT.BOLD));
	public final static IToken codeFunctionToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA), null, SWT.BOLD));
	public final static IToken codeTypeToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE), null, SWT.BOLD));
	public final static IToken codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
	public final static IToken codePropertyToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK), null, SWT.BOLD));
	public final static IToken codeStringToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
	public final static IToken codeNumberToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));

	/**
	 * Convenience method, to return a system default color. Color constants come from SWT class e.g. SWT.COLOR_RED
	 */
	private static Color getColor(final int color) {
	    final Color colors[] = new Color[1];

	    DisplayUtils.runNowOrSyncInUIThread(new Runnable() {
            public void run() {
                colors[0] = Display.getDefault().getSystemColor(color);
            }
	    });

	    return colors[0];
	}

	/**
	 * Detector for normal NED keywords (may start with letter, @ or _ and contain letter number or _)
	 */
	public static class NedWordDetector implements IWordDetector {
	    public boolean isWordStart(char character) {
	        return Character.isLetter(character) || character == '_' || character == '@';
	    }
	
	    public boolean isWordPart(char character) {
	        return Character.isLetterOrDigit(character) || character == '_';
	    }
	}

	/**
	 * Detector for a NED qualified type names (a sequence of identifiers, separated by dots) 
	 */
	public static class NedDottedWordDetector implements IWordDetector {
	    public boolean isWordStart(char character) {
	        return Character.isLetter(character) || character == '_' || character == '@';
	    }
	
	    public boolean isWordPart(char character) {
	        return Character.isLetterOrDigit(character) || character == '_' || character == '.';
	    }
	}

	/**
	 * Detector for extreme NED keywords (in: out: --> <-- .. ...) where the keyword may contain special chars
	 */
	public static class NedSpecialWordDetector implements IWordDetector {
	    public boolean isWordStart(char c) {
	        return Character.isLetter(c) || c == '-' || c == '<' || c == '>' || c == '.';
	    }
	
	    public boolean isWordPart(char c) {
	        return isWordStart(c) || c == ':';
	    }
	}
	
	/**
	 * Detects keywords that are starting with @ and continuing with letters only.
	 */
	public static class NedAtWordDetector implements IWordDetector {
	    public boolean isWordStart(char c) {
	        return (c == '@');
	    }
	
	    public boolean isWordPart(char c) {
	        return Character.isLetter(c);
	    }
	}
	
	/**
	 * Detects keywords that look like an XML tag.
	 */
	public static class NedDocTagDetector implements IWordDetector {
	    public boolean isWordStart(char c) {
	        return (c == '<');
	    }
	
	    public boolean isWordPart(char c) {
	        return Character.isLetter(c) || c == '/' || c == '>';
	    }
	}
}
