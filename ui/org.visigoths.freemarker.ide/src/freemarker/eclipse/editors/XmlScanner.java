/*
 * Copyright (c) 2003 The Visigoth Software Society. All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution, if
 *    any, must include the following acknowledgement:
 *       "This product includes software developed by the
 *        Visigoth Software Society (http://www.visigoths.org/)."
 *    Alternately, this acknowledgement may appear in the software
 *    itself, if and wherever such third-party acknowledgements
 *    normally appear.
 *
 * 4. Neither the name "FreeMarker", "Visigoth", nor any of the names
 *    of the project contributors may be used to endorse or promote
 *    products derived from this software without prior written
 *    permission. For written permission, please contact
 *    visigoths@visigoths.org.
 *
 * 5. Products derived from this software may not be called
 *    "FreeMarker" or "Visigoth" nor may "FreeMarker" or "Visigoth"
 *    appear in their names without prior written permission of the
 *    Visigoth Software Society.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE VISIGOTH SOFTWARE SOCIETY OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by many
 * individuals on behalf of the Visigoth Software Society. For more
 * information on the Visigoth Software Society, please see
 * http://www.visigoths.org/
 */

package freemarker.eclipse.editors;

import java.util.Vector;

import org.eclipse.jface.text.rules.ICharacterScanner;
import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.MultiLineRule;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WhitespaceRule;

/**
 * The XML tag scanner. This class handles the lexical scanning
 * of the XML tag partitions, separating out FreeMarker constructs
 * into their own tokens (with their own colors).
 *
 * @version $Id: version $
 * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
 * @author <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
 */
@SuppressWarnings({"rawtypes", "unchecked"})
public class XmlScanner extends RuleBasedScanner {

    /**
     * The stack of initiated character sequences. When a quote or a
     * FreeMarker directive is encountered, the starting character
     * sequence is added to the stack. The stack can then be used to
     * check for a matching end sequence. It is also used to ensure
     * proper nesting of attributes and directives.
     */
    private Vector stack = new Vector();

    /**
     * Creates a new XML tag scanner.
     *
     * @param manager        the color manager to use
     */
    public XmlScanner(ITokenManager manager) {
        Vector   rules = new Vector();
        IRule[]  result;
        IToken    comment;
        IToken    directive;
        IToken    interpolation;
        IToken    string;

        // Retrieve tokens
        string = manager.getStringToken();
        comment = manager.getCommentToken();
        directive = manager.getDirectiveToken();
        interpolation = manager.getInterpolationToken();

        // Add rules for FreeMarker constructs
        rules.add(new MultiLineRule("<#--", "-->", comment));
        rules.add(new XmlDirectiveRule(directive));
        rules.add(new InterpolationRule(interpolation));

        // Add rule for XML attributes
        rules.add(new XmlAttributeRule(string));

        // Add generic whitespace rule.
        rules.add(new WhitespaceRule(new WhitespaceDetector()));

        // Set scanner rules
        result = new IRule[rules.size()];
        rules.copyInto(result);
        setRules(result);
    }

    /**
     * Returns the top character sequence in the stack.
     *
     * @return the top character sequence in the stack, or
     *         null if the stack is empty
     */
    protected String topSequence() {
        if (stack.isEmpty()) {
            return null;
        } else {
            return (String) stack.lastElement();
        }
    }

    /**
     * Adds a new character sequence to the top of the stack.
     *
     * @param str            the character sequence to add
     */
    protected void pushSequence(String str) {
        stack.add(str);
    }

    /**
     * Removes the top character sequence from the stack.
     */
    protected void popSequence() {
        if (stack.size() > 0) {
            stack.remove(stack.size() - 1);
        }
    }

    /**
     * A simple XML attribute value rule. This rule is used to match
     * XML attribute values. It interrupts and resumes matching for
     * FreeMarker directives and interpolations.
     *
     * @version $Id: version $
     * @author <a href="mailto:stephan@chaquotay.net">Stephan Mueller</a>
     * @author <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
     */
    protected class XmlAttributeRule implements IRule {

        /**
         * The token to return for this rule.
         */
        private final IToken token;

        /**
         * Creates a new XML attribute rule.
         *
         * @param token          the token to use for this rule
         */
        public XmlAttributeRule(IToken token) {
            this.token = token;
        }

        /**
         * Checks if character is an XML quote character.
         *
         * @param c              the character to check
         *
         * @return true if the character is a quote character, or
         *         false otherwise
         */
        private boolean isQuoteChar(char c) {
            return c == '"' || c == '\'';
        }

        /*
         * @see org.eclipse.jface.text.rules.IRule#evaluate(org.eclipse.jface.text.rules.ICharacterScanner)
         */
        public IToken evaluate(ICharacterScanner scanner) {
            String  start;
            int     c;

            // Check for quote start (if not already read)
            start = topSequence();
            if (start == null || !isQuoteChar(start.charAt(0))) {
                c = scanner.read();
                if (c == EOF || !isQuoteChar((char) c)) {
                    scanner.unread();
                    return Token.UNDEFINED;
                }
                pushSequence(String.valueOf((char) c));
                start = topSequence();
            }

            // Read until quote end, interpolation or (user-)directive
            do {
                c = scanner.read();
                if (c == '$' || c == '#') {
                    c = scanner.read();
                    scanner.unread();
                    if (c == '{') {
                        scanner.unread();
                        return token;
                    }
                    c = '$';
                } else if (c == '<') {
                    c = scanner.read();
                    if (c == '/') {
                        c = scanner.read();
                        scanner.unread();
                    }
                    scanner.unread();
                    if (c == '#' || c == '@') {
                        scanner.unread();
                        return token;
                    }
                    c = '<';
                }
            } while (c != EOF && c != start.charAt(0));

            // Remove from stack and return result
            popSequence();
            return token;
        }
    }

    /**
     * A simple FreeMarker directive rule. This rule is used to match
     * FreeMarker directives withing XML partitions. It interrupts
     * and resumes matching for FreeMarker comments.
     *
     * @version $Id: version $
     * @author <a href="mailto:per&#64;percederberg.net">Per Cederberg</a>
     */
    protected class XmlDirectiveRule implements IRule {

        /**
         * The token to return for this rule.
         */
        private final IToken token;

        /**
         * Creates a new XML attribute rule.
         *
         * @param token          the token to use for this rule
         */
        public XmlDirectiveRule(IToken token) {
            this.token = token;
        }

        /*
         * @see org.eclipse.jface.text.rules.IRule#evaluate(org.eclipse.jface.text.rules.ICharacterScanner)
         */
        public IToken evaluate(ICharacterScanner scanner) {
            String  start;
            int     parens = 0;
            int     c;

            // Check for directive start (if not already read)
            start = topSequence();
            if (start == null || !start.equals("<#")) {
                if (!FreemarkerTools.readDirectiveStart(scanner)) {
                    return Token.UNDEFINED;
                }
                pushSequence("<#");
            }

            // Read until directive end or comment
            do {
                c = scanner.read();
                if (c == '<') {
                    scanner.unread();
                    if (FreemarkerTools.readCommentStart(scanner)) {
                        scanner.unread();
                        scanner.unread();
                        scanner.unread();
                        scanner.unread();
                        return token;
                    }
                    c = scanner.read();
                } else if (c == '"') {
                    FreemarkerTools.readQuoteEnd(scanner);
                } else if (c == '(') {
                    parens++;
                } else if (c == ')') {
                    parens--;
                }
            } while (c != EOF && (c != '>' || parens > 0));

            // Remove from stack and return result
            popSequence();
            return token;
        }
    }
}
