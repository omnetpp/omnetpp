package freemarker.eclipse.editors;

import org.eclipse.jface.text.rules.IToken;

/**
 * Implementations of this interface provide a set of tokens which should be
 * used by ITokenScanner-s when they scan a piece of ftl code.
 *
 * This interface makes it possible to replace the default TokenManager with
 * a non-SWT-dependent (and therefore lightweight) mock at test time.
 *
 * @author <a href="mailto:stephan&#64;chaquotay.net">Stephan Mueller</a>
 * @version $Id: ITokenManager.java,v 1.1 2004/02/05 00:16:23 stephanmueller Exp $
 */
public interface ITokenManager {
    public IToken getInterpolationToken();
    public IToken getXmlToken();
    public IToken getXmlCommentToken();
    public IToken getTextToken();
    public IToken getDirectiveToken();
    public IToken getStringToken();
    public IToken getCommentToken();
    public IToken getWhitespaceToken();
    public void dispose();
}