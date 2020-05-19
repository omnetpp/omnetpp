package org.omnetpp.common.util;

import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.AND;
import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.CP;
import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.MATCHES;
import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.NOT;
import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.OP;
import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.OR;
import static org.omnetpp.common.util.MatchExpressionSyntax.TokenType.STRING_LITERAL;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.fieldassist.IContentProposal;
import org.omnetpp.common.Debug;
import org.omnetpp.common.contentassist.ContentProposalEx;
import org.omnetpp.common.contentassist.ContentProposalProviderBase;
import org.omnetpp.common.util.MatchExpressionSyntax;
import org.omnetpp.common.util.MatchExpressionSyntax.Token;
import org.omnetpp.common.util.MatchExpressionSyntax.TokenType;

/**
 * Base class for content proposal providers for OMNeT++ match expressions,
 * see the MatchExpression C++ class (src/common/matchexpression.h).
 *
 * Subclasses only need to provide the list of possible field names and values via
 * method, and the rest is done within this class.
 *
 * @author andras
 */
public abstract class MatchExpressionContentProposalProvider extends ContentProposalProviderBase {
    private static final ContentProposalEx P_AND = new ContentProposalEx("AND ");
    private static final ContentProposalEx P_OR = new ContentProposalEx("OR ");
    private static final ContentProposalEx P_NOT = new ContentProposalEx("NOT ");
    private static final ContentProposalEx P_MATCHES = new ContentProposalEx("=~ ");

    // save the arg list of the current getProposals() call, in case subclasses need it
    private String contentsArg;
    private int positionArg;

    public IContentProposal[] getProposals(String contents, int position) {
        contentsArg = contents;
        positionArg = position;

        Token[] tokens = getLastFewTokens(contents.substring(0, position), 4);

        TokenType lastTokenType = tokens.length == 0 ? null : tokens[0].getType();
        boolean lastTokenPossiblyIncomplete = tokens.length > 0 && tokens[0].getEndPos() == position && (lastTokenType==STRING_LITERAL || lastTokenType==AND || lastTokenType==OR || lastTokenType==NOT);
        String prefix = "";
        if (lastTokenPossiblyIncomplete) {
            // remove incomplete last token at index 0
            prefix = contents.substring(tokens[0].getStartPos(), tokens[0].getEndPos()); // contains opening quote
            tokens = Arrays.copyOfRange(tokens, 1, tokens.length);
            lastTokenType = tokens.length == 0 ? null : tokens[0].getType();
        }

        Debug.print("Last tokens (reverse order):");
        for (Token token : tokens)
            Debug.print("  " + token.toString());
        Debug.println();

        List<IContentProposal> proposals = new ArrayList<>();

        // empty, ... AND|OR|( -> propose field name, default-field values
        if (lastTokenType == null || lastTokenType == AND || lastTokenType == OR || lastTokenType == OP) {
            Debug.println(" --> offer field names, default-field values, NOT");
            proposals.addAll(getFieldNameProposals(prefix));
            proposals.addAll(getDefaultFieldValueProposals(prefix));
            proposals.add(P_NOT);
        }

        if (lastTokenType == NOT) {
            Debug.println(" --> offer field names, default-field values");
            proposals.addAll(getFieldNameProposals(prefix));
            proposals.addAll(getDefaultFieldValueProposals(prefix));
        }

        // ... ) -> propose AND, OR
        if (lastTokenType == CP) {
            Debug.println(" --> offer AND, OR");
            proposals.add(P_AND);
            proposals.add(P_OR);
        }

        // field =~ -> propose field value
        if (lastTokenType == MATCHES && tokens.length >= 2 && tokens[1].getType()==STRING_LITERAL) {
            String fieldName = tokens[1].getValue();
            Debug.println(" --> offer values for " + fieldName);
            proposals.addAll(getFieldValueProposals(fieldName, prefix));
        }

        // field =~ value -> propose AND, OR
        if (lastTokenType == STRING_LITERAL)
        {
            if (tokens.length >= 3 && tokens[1].getType()==MATCHES && tokens[2].getType()==STRING_LITERAL) {
                Debug.println(" --> offer AND, OR");
                proposals.add(P_AND);
                proposals.add(P_OR);
            }
            else {
                Debug.println(" --> offer MATCHES, AND, OR");
                proposals.add(P_MATCHES);
                proposals.add(P_AND);
                proposals.add(P_OR);
            }
        }

        contentsArg = null; // allow gc

        return filterAndWrapProposals(proposals, prefix, false, position);
    }

    /**
     * Return field name proposals.
     */
    abstract protected List<IContentProposal> getFieldNameProposals(String prefix);

    /**
     * Return value proposals for the default field.
     */
    abstract protected List<IContentProposal> getDefaultFieldValueProposals(String prefix);

    /**
     * Return value proposals for the given field.
     */
    abstract protected List<IContentProposal> getFieldValueProposals(String fieldName, String prefix);

    protected String getFullContent() {
        Assert.isTrue(contentsArg != null, "not inside a getProposal call");
        return contentsArg;
    }

    protected String getFullContentPrefix() {
        Assert.isTrue(contentsArg != null, "not inside a getProposal call");
        return contentsArg.substring(0, positionArg);
    }

    /**
     * Helper: Return the last n tokens in reverse order, that is [0] is the last one.
     */
    private Token[] getLastFewTokens(String text, int n) {
        // The text can be very long, so we only tokenize the end of it, a part that's
        // long enough to likely to contain as many tokens as we need to return.
        // However, we must not start inside a quoted string constant, so we need to pre-scan
        // the string from the beginning to know what is is quotes and what isn't.
        int numCharsToScan = 1000;
        int tailStartPos = text.length() < numCharsToScan ? 0 : findGoodStartingPoint(text, text.length()-numCharsToScan);
        String tail = text.substring(tailStartPos);

        // Use lexer to enumerate the tokens
        MatchExpressionSyntax.Lexer lexer = new MatchExpressionSyntax.Lexer(tail, tailStartPos);
        List<Token> lastTokens = new LinkedList<>();
        while (true) {
            Token token = lexer.getNextToken();
            if (token.getType() == TokenType.END)
                break;
            //System.out.println(token.getValue() + "  should correspond to: " + text.substring(token.getStartPos(), token.getEndPos()));
            lastTokens.add(0, token);
            if (lastTokens.size() == n+1)
                lastTokens.remove(lastTokens.size()-1);
        }
        return lastTokens.toArray(new Token[0]);
    }

    /**
     * Helper: Find the position of the first space or newline after the given position
     * which is not part of a quoted string literal.
     */
    private int findGoodStartingPoint(String text, int nearPos) {
        boolean insideQuotes = false;
        int length = text.length();
        for (int pos = 0; pos < length; pos++) {
            char ch = text.charAt(pos);
            if (pos >= nearPos && (ch==' ' || ch=='\n') && !insideQuotes)
                return pos;
            if (ch == '"')
                insideQuotes = !insideQuotes;
            if (ch == '\\')
                pos++;
        }
        return 0; // start from the beginning
    }
}
