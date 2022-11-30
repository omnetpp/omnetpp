/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.editor.text;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.util.DisplayUtils;

/**
 * Data for syntax highlighting
 *
 * @author rhornig, andras
 */
public class SyntaxHighlightHelper {
    // colors from the default Eclipse source code editors (JDT,CDT)
    public final static Color ECLIPSE_DARKBLUE = new Color(0, 0, 192); // constants
    public final static Color ECLIPSE_PURPLE = new Color(127, 0, 85); // keywords
    public final static Color ECLIPSE_GREEN = new Color(63, 127, 95); // comments

    // word lists for syntax highlighting
    public final static String[] highlightPrivateDocTodo = Keywords.DOC_TODO;
    public final static String[] highlightDocTags = Keywords.DOC_TAGS;
    public final static String[] highlightDocKeywords = Keywords.DOC_KEYWORDS;
    public final static String[] highlightNedTypes = Keywords.NED_TYPE_KEYWORDS;
    public final static String[] highlightNedSpecialKeywords = Keywords.NED_SPECIAL_KEYWORDS;
    public final static String[] highlightNedKeywords = Keywords.concat(Keywords.NED_NONEXPR_KEYWORDS, Keywords.NED_EXPR_KEYWORDS);
    public final static String[] highlightNedExprKeywords = Keywords.NED_EXPR_KEYWORDS;
    public final static String[] highlightNedFunctions = Keywords.NED_FUNCTIONS;
    public final static String[] highlightMsgTypes = Keywords.MSG_TYPE_KEYWORDS;
    public final static String[] highlightMsgKeywords = Keywords.concat(Keywords.MSG_KEYWORDS, Keywords.MSG_SECTION_KEYWORDS);
    public final static String[] highlightConstants = Keywords.BOOL_CONSTANTS;

    // tokens for syntax highlighting (in light theme)
    public static IToken docDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC)); // gray50
    public static IToken docKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));
    public static IToken docTagToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_RED)));
    public static IToken docPrivateDefaultToken = new Token(new TextAttribute(ColorFactory.GREY80, null, SWT.ITALIC));
    public static IToken docPrivateTodoToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));
    public static IToken codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND)));
    public static IToken codeKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_RED), null, SWT.BOLD));
    public static IToken codeFunctionToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA)));
    public static IToken codeTypeToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE), null, SWT.BOLD));
    public static IToken codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND)));
    public static IToken codePropertyToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND), null, SWT.BOLD));
    public static IToken codeStringToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
    public static IToken codeNumberToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));

    public final static boolean isDarkTheme = DisplayUtils.isDarkTheme(); 
    
    static {
        if (isDarkTheme) {
            docDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
            docKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_YELLOW)));
            docTagToken = new Token(new TextAttribute(ColorFactory.GREY31));
            docPrivateDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLUE), null, SWT.ITALIC));
            docPrivateTodoToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));
            codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND)));
            codeKeywordToken = new Token(new TextAttribute(ColorFactory.LIGHT_GOLDENROD4, null, SWT.BOLD));
            codeFunctionToken = new Token(new TextAttribute(ColorFactory.LIGHT_SLATE_BLUE));
            codeTypeToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_CYAN), null, SWT.BOLD));
            codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND)));
            codePropertyToken = new Token(new TextAttribute(ColorFactory.KHAKI3, null, SWT.BOLD));
            codeStringToken = new Token(new TextAttribute(ColorFactory.PALE_GREEN3));
            codeNumberToken = new Token(new TextAttribute(ColorFactory.PALE_GREEN3));
        }
    }

    public static void init() {
        // Since this class does Display thread synchronization in getColor(),
        // which is called from the static {} block above, it might cause a
        // deadlock if it happens to run at the wrong time. This surfaced
        // when enabling the "Source on NED and MSG type pages" in the dialog,
        // but only when running from the IDE (and not if from opp_neddoc).
        // So this empty method is here just so user classes can make sure
        // the static {} block runs early enough, where/when it should.
    }

    /**
     * Convenience method, to return a system default color. Color constants come from SWT class e.g. SWT.COLOR_RED
     */
    private static Color getColor(final int swtColor) {
        Color[] holder = new Color[1];
        DisplayUtils.runNowOrSyncInUIThread(() -> { holder[0] = Display.getDefault().getSystemColor(swtColor); });
        return holder[0];
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

    /**
     * Detects tags in display strings.
     */
    public static class NedDisplayStringTagDetector implements IWordDetector {
        public boolean isWordStart(char c) {
            return Character.isLetter(c);
        }

        public boolean isWordPart(char c) {
            return c != ';' && c != '"';
        }
    }

    /**
     * Detects image names in display strings.
     */
    public static class NedDisplayStringImageNameDetector implements IWordDetector {
        public boolean isWordStart(char character) {
            return Character.isLetter(character);
        }

        public boolean isWordPart(char character) {
            return Character.isLetterOrDigit(character) || character == '/';
        }
    }

    /**
     * Detects tags in a property.
     */
    public static class NedPropertyTagDetector implements IWordDetector {
        public boolean isWordStart(char c) {
            return Character.isLetter(c);
        }

        public boolean isWordPart(char c) {
            return c != ';' && c != '(' && c != ')' && c != ' ';
        }
    }

    /**
     * Detects values in a property.
     */
    public static class NedPropertyTagValueDetector implements IWordDetector {
        public boolean isWordStart(char c) {
            return Character.isLetter(c);
        }

        public boolean isWordPart(char c) {
            return c != ';' && c != '(' && c != ')' && c != ' ' && c != '=' && c != ',';
        }
    }
}
