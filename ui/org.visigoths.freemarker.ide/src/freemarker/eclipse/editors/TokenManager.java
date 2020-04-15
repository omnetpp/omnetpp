package freemarker.eclipse.editors;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.swt.graphics.Color;

import freemarker.eclipse.preferences.IPreferenceConstants;


/**
 * The default implementation of ITokenManager. It provides all necessary
 * tokens, using preference-based color settings.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: TokenManager.java,v 1.1 2004/02/05 00:16:23 stephanmueller Exp $
 */
public class TokenManager implements IPreferenceConstants, ITokenManager {

    private ColorManager manager = null;

    public TokenManager() {
        manager = new ColorManager();
    }

    public IToken getInterpolationToken() {
        return getTokenFromPreferenceColor(COLOR_INTERPOLATION);
    }

    public IToken getXmlToken() {
        return getTokenFromPreferenceColor(COLOR_XML_TAG);
    }

    public IToken getXmlCommentToken() {
        return getTokenFromPreferenceColor(COLOR_XML_COMMENT);
    }

    public IToken getTextToken() {
        return getTokenFromPreferenceColor(COLOR_TEXT);
    }

    public IToken getDirectiveToken() {
        return getTokenFromPreferenceColor(COLOR_DIRECTIVE);
    }

    public IToken getStringToken() {
        return getTokenFromPreferenceColor(COLOR_STRING);
    }

    public IToken getCommentToken() {
        return getTokenFromPreferenceColor(COLOR_COMMENT);
    }

    private IToken getTokenFromPreferenceColor(String color) {
        Color c = manager.getColorFromPreference(color);
        return new Token(new TextAttribute(c));
    }

    public IToken getWhitespaceToken() {
        return Token.WHITESPACE;
    }

    public void dispose() {
        manager.dispose();
    }

}
