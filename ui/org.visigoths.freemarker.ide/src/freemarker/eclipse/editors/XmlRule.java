package freemarker.eclipse.editors;

import org.eclipse.jface.text.rules.ICharacterScanner;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.MultiLineRule;
import org.eclipse.jface.text.rules.Token;

import freemarker.eclipse.FreemarkerPlugin;
import freemarker.eclipse.preferences.IPreferenceConstants;

public class XmlRule extends MultiLineRule implements IPreferenceConstants {

    private boolean xmlHighlighting = true;

    /**
     * Creates a xml rule
     *
     * @param token the token which will be returned on success
     */
    public XmlRule(IToken token) {
        super("<", ">", token, (char) 0, false);
        // eventually we will need a mechanism to pass preference changes to the scanners/rules
        // ATM the user has to close and open a file to apply the changes to this rule
        xmlHighlighting = true;
        try {
            xmlHighlighting = FreemarkerPlugin.getInstance().getPreferenceStore().getBoolean(
                XML_HIGHLIGHTING);
        } catch (NullPointerException e) {}
    }

    /* (non-Javadoc)
     * @see org.eclipse.jface.text.rules.PatternRule#doEvaluate(org.eclipse.jface.text.rules.ICharacterScanner)
     */
    protected IToken doEvaluate(ICharacterScanner scanner) {
        if (!xmlHighlighting)
            return Token.UNDEFINED;
        return super.doEvaluate(scanner);
    }

    /* (non-Javadoc)
     * @see org.eclipse.jface.text.rules.PatternRule#doEvaluate(org.eclipse.jface.text.rules.ICharacterScanner, boolean)
     */
    protected IToken doEvaluate(ICharacterScanner scanner, boolean resume) {
        if (!xmlHighlighting)
            return Token.UNDEFINED;
        return super.doEvaluate(scanner, resume);
    }

    /**
     * Returns whether the end sequence was detected. This method
     * reads all characters until the end of the sequence or EOF is
     * detected. It attempts to avoid ending the XML tag inside a
     * FreeMarker construct.
     *
     * @param scanner        the character scanner
     *
     * @return true if the end sequence has been detected
     */
    protected boolean endSequenceDetected(ICharacterScanner scanner) {
        FreemarkerTools.readXmlTagEnd(scanner);
        return true;
    }

}
