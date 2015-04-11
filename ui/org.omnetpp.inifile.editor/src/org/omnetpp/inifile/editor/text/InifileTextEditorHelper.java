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

/**
 * This class contains all the possible keywords for syntax highlighting and context assist functions.
 *
 * @author rhornig
 *
 */
//XXX revise...
public class InifileTextEditorHelper {
    public static final String DEFAULT_CONTEXT_TYPE = "org.omnetpp.inifile.editor.default";

    /**
     * Convenience method, to return a system default color. Color constants come from SWT class e.g. SWT.COLOR_RED
     */
    public static Color getColor(int color) {
        return Display.getDefault().getSystemColor(color);
    }

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

    // whitespace and word detectors for tokenization
    public final static WhitespaceDetector whitespaceDetector = new WhitespaceDetector();
    public final static IWordDetector spaceSeparatedWordDetector = new SpaceSeparatedWordDetector();
    public final static IWordDetector inifileWordDetector = new InifileWordDetector();

    // tokens for syntax highlighting
    // TODO these styles should be configurable
    public final static IToken commentToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
    public final static IToken codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
    public final static IToken codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
    public final static IToken codeConfigKeyToken = new Token(new TextAttribute(new Color(null, 0, 0, 192))); // Eclipse text editor blue
    public final static IToken codeKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA), null, SWT.BOLD));
    public final static IToken codeFunctionToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA)));
    public final static IToken codeStringToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
    public final static IToken codeNumberToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));

}
