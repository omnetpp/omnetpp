/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.inifile.editor.text;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWhitespaceDetector;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.common.color.ColorFactory;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;
import org.omnetpp.common.util.DisplayUtils;

/**
 * This class contains all the possible keywords for syntax highlighting and context assist functions.
 *
 * @author rhornig
 */
public class InifileTextEditorHelper {
    public static final String DEFAULT_CONTEXT_TYPE = "org.omnetpp.inifile.editor.default";

    /**
     * A generic white space detector
     */
    public static class WhitespaceDetector implements IWhitespaceDetector {

        public boolean isWhitespace(char character) {
            return Character.isWhitespace(character);
        }
    }

    /**
     * Detects words separated by whitespace.
     */
    public static class SpaceSeparatedWordDetector implements IWordDetector {

        public boolean isWordStart(char c) {
            return isWordPart(c);
        }

        public boolean isWordPart(char c) {
            return c!=' ' && c!='\t' && c!='\f' && c!='\n' && c!='\r';
        }
    }

    /**
     * Word detector for ini files. Key names may contain hyphens
     * ("sim-time-limit"), so we recognize it as a word part as well.
     */
    public static class InifileWordDetector implements IWordDetector {

        public boolean isWordStart(char character) {
            return Character.isLetter(character) || character == '_';
        }

        public boolean isWordPart(char character) {
            return Character.isLetterOrDigit(character) || character == '_' || character == '-';
        }
    }

    private static Color getColor(int swtColor) {
        Color[] holder = new Color[1];
        DisplayUtils.runNowOrSyncInUIThread(() -> { holder[0] = Display.getDefault().getSystemColor(swtColor); });
        return holder[0];
    }

    // whitespace and word detectors for tokenization
    public final static WhitespaceDetector whitespaceDetector = new WhitespaceDetector();
    public final static IWordDetector spaceSeparatedWordDetector = new SpaceSeparatedWordDetector();
    public final static IWordDetector inifileWordDetector = new InifileWordDetector();

    // tokens for syntax highlighting in light theme
    // TODO these styles should be configurable
    public static IToken commentToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
    public static IToken codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
    public static IToken codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
    public static IToken codeConfigKeyToken = new Token(new TextAttribute(SyntaxHighlightHelper.ECLIPSE_PURPLE, null, SWT.BOLD));
    public static IToken codeKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA), null, SWT.BOLD));
    public static IToken codeFunctionToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA)));
    public static IToken codeStringToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
    public static IToken codeNumberToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
    public static IToken codeBoolToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
    public static IToken sectionHeadingToken = new Token(new TextAttribute(SyntaxHighlightHelper.ECLIPSE_DARKBLUE, null, SWT.BOLD));

    static {
        if (DisplayUtils.isDarkTheme()) {
            commentToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
            codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND)));
            codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_WIDGET_FOREGROUND)));
            codeConfigKeyToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_CYAN), null, SWT.BOLD));
            codeKeywordToken = new Token(new TextAttribute(ColorFactory.KHAKI3, null, SWT.BOLD));
            codeFunctionToken = new Token(new TextAttribute(ColorFactory.LIGHT_SLATE_BLUE));
            codeStringToken = new Token(new TextAttribute(ColorFactory.PALE_GREEN3));
            codeNumberToken = new Token(new TextAttribute(ColorFactory.PALE_GREEN3));
            codeBoolToken = new Token(new TextAttribute(ColorFactory.PALE_GREEN3));
            sectionHeadingToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_CYAN), null, SWT.BOLD));
        }
    }

}
