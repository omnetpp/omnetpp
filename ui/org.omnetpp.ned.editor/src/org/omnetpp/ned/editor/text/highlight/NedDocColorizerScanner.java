/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.ned.editor.text.highlight;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.eclipse.jface.text.rules.ICharacterScanner;
import org.eclipse.jface.text.rules.IRule;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.RuleBasedScanner;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.rules.WordRule;
import org.omnetpp.common.editor.text.SyntaxHighlightHelper;
import org.omnetpp.common.util.StringUtils;

/**
 * A rule based NedDoc scanner.
 *
 * @author rhornig
 */
public class NedDocColorizerScanner extends RuleBasedScanner {

    /**
     * IRule to detect recognized HTML tags.
     *
     * Note: We need our own IRule, because apparently no built-in class can handle this task properly.
     * SingleLineRule cannot be used for the <foo>, </foo>, <foo/> cases, as it expects an
     * endSequence -- or if you pass in "" or null, the token will last till the end of the line.
     * WordRule is also out of question, because it is built upon IWordDetector that cannot detect
     * XML tags: it only has isWordStart() and isWordPart() methods (there's no isWordEnd()),
     * so it cannot isolate "<foo>" from a "<foo>bar" string.
     *
     * @author Andras
     */
    public static class XmlTagRule implements IRule {
        private Set<String> recognizedTags;
        private IToken token;
        private StringBuffer buffer = new StringBuffer();
        private final static char[] ENDTAG_CHARS = new char[] {' ', '\t', '>'};

        public XmlTagRule(String[] recognizedTags, IToken token) {
            this.recognizedTags = new HashSet<String>(Arrays.asList(recognizedTags));
            this.token = token;
        }

        public IToken evaluate(ICharacterScanner scanner) {
            int c = scanner.read();
            if (c != '<') {
                scanner.unread();
                return Token.UNDEFINED;
            }

            // read tag
            buffer.setLength(0);
            do {
                buffer.append((char) c);
                c = scanner.read();
            } while (c != ICharacterScanner.EOF && c != '>');
            buffer.append((char) c);

            String text = buffer.toString();
            if (text.length() >= 3) { // including '<' and '>'
                int start = text.charAt(1) == '/' ? 2 : 1;
                int end = StringUtils.indexOfAny(text, ENDTAG_CHARS);
                if (end > 0) {
                    String tag = text.substring(start, end);
                    if (recognizedTags.contains(tag))
                        return token;
                }
            }

            // unread unrecognized sequence
            for (int i=0; i<text.length(); i++)
                scanner.unread();
            return Token.UNDEFINED;
        }
    }

    /**
     * Create a new neddoc scanner for the given color provider.
     */
     public NedDocColorizerScanner() {
        super();
        // this is the default token for a comment
        setDefaultReturnToken(SyntaxHighlightHelper.docDefaultToken);

        List<IRule> list = new ArrayList<IRule>();

        // Add rules for supported HTML tags
        list.add(new XmlTagRule(SyntaxHighlightHelper.highlightDocTags, SyntaxHighlightHelper.docTagToken));

        // Add word rule for keywords
        WordRule keywordRule = new WordRule(new SyntaxHighlightHelper.NedAtWordDetector(), Token.UNDEFINED);
        for (String keyword : SyntaxHighlightHelper.highlightDocKeywords)
            keywordRule.addWord("@" + keyword, SyntaxHighlightHelper.docKeywordToken);
        list.add(keywordRule);

        IRule[] result = new IRule[list.size()];
        list.toArray(result);
        setRules(result);
    }
}
