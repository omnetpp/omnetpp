package freemarker.eclipse.editors;

import org.eclipse.jface.text.rules.IWhitespaceDetector;

/**
 * @version $Id: WhitespaceDetector.java,v 1.1 2003/02/12 22:28:53 szegedia Exp $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 */
public class WhitespaceDetector implements IWhitespaceDetector {

    public boolean isWhitespace(char c) {
        return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
    }
}
