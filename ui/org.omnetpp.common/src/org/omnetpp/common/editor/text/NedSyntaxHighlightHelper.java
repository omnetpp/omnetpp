package org.omnetpp.common.editor.text;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;

public class NedSyntaxHighlightHelper {
	// word lists for syntax highlighting
	// TODO these are both for NED and MSG files. Once a separate MSG editor is done keywords should be split
	public final static String[] highlightPrivateDocTodo = NedKeywords.DOC_TODO;
	public final static String[] highlightDocTags = NedKeywords.DOC_TAGS;
	public final static String[] highlightDocKeywords = NedKeywords.DOC_KEYWORDS;
	public final static String[] highlightNedTypes = NedKeywords.TYPES;
	public final static String[] highlightNedSpecialKeywords = NedKeywords.SPECIAL;
	public final static String[] highlightNedKeywords = NedKeywords.concat(NedKeywords.KEYWORDS, NedKeywords.EXPRESSION_KEYWORDS);
	public final static String[] highlightNedFunctions = NedKeywords.FUNCTIONS;
	public final static String[] highlightConstants = NedKeywords.CONSTANTS;

	// tokens for syntax highlighting
	public final static IToken docDefaultToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
	public final static IToken docKeywordToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_BLUE)));
	public final static IToken docTagToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_RED)));
	public final static IToken docPrivateDefaultToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_BLUE), null, SWT.ITALIC));
	public final static IToken docPrivateTodoToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_BLUE)));
	public final static IToken codeDefaultToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_BLACK)));
	public final static IToken codeKeywordToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_RED), null, SWT.BOLD));
	public final static IToken codeFunctionToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_MAGENTA), null, SWT.BOLD));
	public final static IToken codeTypeToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_BLUE), null, SWT.BOLD));
	public final static IToken codeIdentifierToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_BLACK)));
	public final static IToken codePropertyToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_BLACK), null, SWT.BOLD));
	public final static IToken codeStringToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_GREEN)));
	public final static IToken codeNumberToken = new Token(new TextAttribute(NedSyntaxHighlightHelper.getColor(SWT.COLOR_DARK_GREEN)));

	/**
	 * Convenience method, to return a system default color. Color constants come from SWT class e.g. SWT.COLOR_RED
	 */
	public static Color getColor(int color) {
	    return Display.getDefault().getSystemColor(color);
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
