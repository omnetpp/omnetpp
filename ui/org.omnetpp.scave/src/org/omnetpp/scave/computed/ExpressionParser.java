package org.omnetpp.scave.computed;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.Stack;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.collections.Trie;
import org.omnetpp.scave.computed.ExpressionSyntax.AnyCharPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ArgList;
import org.omnetpp.scave.computed.ExpressionSyntax.CharacterSetPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Expr;
import org.omnetpp.scave.computed.ExpressionSyntax.ExprPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.LikeOp;
import org.omnetpp.scave.computed.ExpressionSyntax.LiteralPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ManyCharsPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.NumLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.NumericRangePattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.PatternElement;
import org.omnetpp.scave.computed.ExpressionSyntax.SplicedExpr;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.StringLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.StringTemplate;
import org.omnetpp.scave.computed.ExpressionSyntax.TemplateElement;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;

/**
 * This class implements parsers for expressions, patterns, and string templates.
 * <p>
 * Expression syntax:
 * <pre>
 *   expr = NUMBER
 *        | STRING
 *        | NAME
 *        | STATISTIC
 *        | PATTERN DOT STATISTIC
 *        | ( expr )
 *        | expr + expr | expr - expr | expr * expr | expr / expr | expr ^ expr | expr % expr
 *        | - <expr>
 *        | expr '|' expr | expr & expr | expr # expr | expr << expr | expr >> expr | ~ expr
 *        | expr ++ expr
 *        | expr == expr | expr != expr | expr < expr | expr <= expr | expr > expr | expr >= expr
 *        | ! expr | expr '||' expr | expr && expr
 *        | expr ? expr : expr
 *        | expr =~ PATTERN
 *        | FUN '(' args ')'
 *
 *   args = expr
 *        | args , expr
 * </pre>
 *
 *   Each binary operator is left associative, except ^ which associates to the right.
 *   Precedencies (decreases from top to bottom):
 * <pre>
 *       - (unary) ! ~
 *       ^
 *       * / %
 *       + -
 *       << >>
 *       & | #
 *       ++
 *       == != < > <= >=
 *       && || ##
 *       ? :
 * </pre>
 * Pattern syntax:
 * <pre>
 * pattern = pelem
 *         | pattern pelem
 *
 * pelem = ? | * | **
 *       | { charlist } | {^ charlist }
 *       | { rangelist }
 *       | [ rangelist }
 *       | ${ expr }
 *       | ${ name = pattern }
 *       | literal
 *
 * name = letter
 *      | name alphanum
 *
 * letter = character in range {a-zA-Z_}
 * alphanum = character in range {a-zA-Z_0-9}
 *
 * rangelist = range
 *           | rangelist , range
 *
 * range = integer | integer .. integer
 *
 * charslist = charrange
 *           | } charrange
 *           | charlist charrange
 *
 * charrange = char | char - char
 * char = any char except '}'
 * </pre>
 *
 * String template syntax:
 * <pre>
 * template = telem
 *          | template telem
 *
 * telem = ${ expr }
 *       | literal
 *
 * literal = any chars until "${" or end of string, '\' quotes next character
 * </pre>
 *
 * @see ExpressionSyntax
 */
class ExpressionParser {

    /**
     * Type of lexical tokens.
     * <p>
     * There are two precedence levels for each type, see {@link #parseExpr}.
     */
    enum TokenType
    {
        NUMBER(1000,999),  // float constant
        STRING(1000,999),  // string constant
        NAME(1000,999),    // variable name, simple statistic name; (letter=a-zA-Z_)
        FUN(998,999),      // function name (same as NAME but followed by '(')
        STATISTIC(1000,999),  // name of a statistic, can be quoted
        PATTERN(999,900),  // module name pattern, for qualifying statistic names

        DOT(".", 999,999), // separates pattern from statistic

        LIKE("=~", 900,900),

        UMINUS(80,81),     // unary -

        POWER("^", 70,71),

        MULT("*", 60,59),
        DIV("/", 60,59),
        MOD("%", 60,59),

        PLUS("+", 50,49),
        MINUS("-", 50,49),

        SHIFTLEFT("<<", 40,39),
        SHIFTRIGHT(">>", 40,39),

        BINCOMPL("~", 80,81),
        BINOR("|", 30,29),
        BINAND("&", 30,29),
        BINXOR("#", 30,29),

        CONCAT("++", 25,24), // string concatenation

        EQ("==", 20,19),
        NEQ("!=", 20,19),
        LT("<", 20,19),
        GT(">", 20,19),
        LE("<=", 20,19),
        GE(">=", 20,19),

        NOT("!", 80,81),
        OR("||", 10,9),
        AND("&&", 10,9),
        XOR("##", 10,9),

        QUESTION("?", 5,6),
        COLON(":", 4,5),

        COMMA(",", 1,1),      // separates function args

        LPAREN("(", 0,999),
        RPAREN(")", 1000,0),

        EOS(-1,-1);        // end of string

        private String name;
        private int leftPrec;
        private int rightPrec;

        // Maps token names to values
        private static final Trie<TokenType> map = initMap();

        private TokenType(int leftPrec, int rightPrec)
        {
            this.leftPrec = leftPrec;
            this.rightPrec = rightPrec;
        }

        private TokenType(String name, int leftPrec, int rightPrec)
        {
            this.name = name;
            this.leftPrec = leftPrec;
            this.rightPrec = rightPrec;
        }

        public String getName() { return name; }
        public int getLeftPrec() { return leftPrec; }
        public int getRightPrec() { return rightPrec; }

        public static TokenType getNamedTokenByPrefix(String name) {
            return map.longestPrefixSearch(name);
        }

        private static Trie<TokenType> initMap() {
            Trie<TokenType> trie = new Trie<TokenType>();
            for (TokenType tokenType : TokenType.class.getEnumConstants())
                if (tokenType.name != null) {
                    TokenType old = trie.put(tokenType.name, tokenType);
                    Assert.isTrue(old == null, "Duplicate TokenType name: " + tokenType.name + " in " + old + " and " + tokenType);
                }
            return trie;
        }
    }

    static class Token
    {
        TokenType type;
        String value;
        int start, end; // location

        Token(TokenType type, String value, int start, int end) {
            this.type = type;
            this.value = value;
            this.start = start;
            this.end = end;
        }

        @Override
        public String toString() {
            return "Token ["+ value + "]";
        }
    }


    /*--------------------------------------------------------------------------------------------
     *                               Expression lexer
     *--------------------------------------------------------------------------------------------*/

    /**
     * Parses {@code str} as a list of tokens.
     */
    List<Token> tokenizeExpr(String str)
            throws ParseException
    {
        return tokenizeExpr(str, 0, str.length());
    }

    /**
     * Parses a segment of {@code str} as a list of tokens.
     * @param str the string to be parsed
     * @param start the start position of the segment
     * @param end the end position of segment
     */
    private List<Token> tokenizeExpr(String str, int start, int end)
        throws ParseException
    {
        List<Token> tokens = new ArrayList<Token>();

        int tokenStart = start, tokenEnd;
        while (tokenStart < end) {
            char ch = str.charAt(tokenStart);
            if (isDigit(ch)) {
                tokenEnd = readNumber(str, tokenStart, end);
                tokens.add(new Token(TokenType.NUMBER, str.substring(tokenStart, tokenEnd), tokenStart, tokenEnd));
                tokenStart = tokenEnd;
            }
            else if (isLetter(ch)) { // pattern, statistic, or variable
                int patternEnd = readPattern(str, tokenStart, end, tokens);
                if (patternEnd != -1) {
                    tokens.add(new Token(TokenType.PATTERN, str.substring(tokenStart, patternEnd), tokenStart, patternEnd));
                    tokenStart = patternEnd;
                }
                else {
                    tokenEnd = readName(str, tokenStart, end);
                    String name = str.substring(tokenStart, tokenEnd);
                    tokens.add(new Token(TokenType.NAME, name, tokenStart, tokenEnd));
                    tokenStart = tokenEnd;
                }
            }
            else if (ch == '\'' || ch == '"') { // quoted string or statistic name
                StringBuilder string = new StringBuilder();
                tokenEnd = readQuotedString(str, tokenStart, end, string);
                if (tokenEnd == -1)
                    throw new ParseException("missing closing quote", tokenStart);
                if (ch == '"')
                    tokens.add(new Token(TokenType.STRING, string.toString(), tokenStart, tokenEnd));
                else
                    tokens.add(new Token(TokenType.STATISTIC, string.toString(), tokenStart, tokenEnd));
                tokenStart = tokenEnd;
            }
            else {
                switch (ch) {
                case ' ': case '\t':
                    tokenStart++;
                    break;
                case '*': case '?': // operator or pattern?
                    int pos2 = readPattern(str, tokenStart, end, tokens);
                    if (pos2 != -1) {
                        tokens.add(new Token(TokenType.PATTERN, str.substring(tokenStart, pos2), tokenStart, pos2));
                        tokenStart = pos2;
                        break;
                    }
                    else
                        /* fall through to default case */;
                default:
                    TokenType tokenType = TokenType.getNamedTokenByPrefix(str.substring(tokenStart));
                    if (tokenType != null) {
                        String name = tokenType.getName();
                        int nameLength = name.length();
                        tokens.add(new Token(tokenType, name, tokenStart, tokenStart+nameLength));
                        tokenStart += nameLength;
                    }
                    else {
                        tokenEnd = readPattern(str, tokenStart, end, tokens);
                        if (tokenEnd == -1)
                            throw new ParseException("unexpected character: "+String.valueOf(ch), tokenStart);
                        tokens.add(new Token(TokenType.PATTERN, str.substring(tokenStart, tokenEnd), tokenStart, tokenEnd));
                        tokenStart = tokenEnd;
                    }
                    break;
                }
            }
        }

        // Make some changes to help easier parsing:
        //  1. NAME followed by '(' is changed to FUN
        //  2. NAME following a DOT is changed to STATISTIC
        //  2. '-' must follow ')', NUMBER, NAME, or STATISTIC, otherwise it is changed to UMINUS
        int numTokens = tokens.size();
        int lastIndex = numTokens - 1;
        Token token, prev, next;
        for (int i = 0; i < numTokens; ++i) {
            token = tokens.get(i);
            switch(token.type) {
            case NAME:
                next = i<lastIndex ? tokens.get(i+1) : null;
                prev = i > 0 ? tokens.get(i-1): null;
                if (next != null && next.type == TokenType.LPAREN)
                    token.type = TokenType.FUN;
                else if (prev != null && prev.type == TokenType.DOT)
                    token.type = TokenType.STATISTIC;
                break;
            case MINUS:
                prev = i > 0 ? tokens.get(i-1) : null;
                if (prev == null || (prev.type != TokenType.RPAREN &&
                        prev.type != TokenType.NUMBER && prev.type != TokenType.NAME && prev.type != TokenType.STATISTIC))
                    token.type = TokenType.UMINUS;
                break;
            }
        }
        return tokens;
    }

    /**
     * Reads the characters of a number from a segment of the input string.
     * This method assumes that the segment starts with a digit.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment (exclusive)
     * @return the position after the last character of the number
     */
    private int readNumber(String str, int start, int end) {
        Assert.isTrue(start < end);
        int pos = start;
        char ch = str.charAt(pos);
        Assert.isTrue(isDigit(ch));

        for (pos++ ; pos < end && isDigit(str.charAt(pos)); pos++) {}
        if (pos < end && str.charAt(pos) == '.') {
            for (pos++; pos < end && isDigit(str.charAt(pos)); pos++) {}
        }
        if (pos < end && "eE".indexOf(str.charAt(pos)) != -1) {
            if ((++pos) < end && "+-".indexOf(str.charAt(pos)) != -1)
                pos++;
            for ( ; pos < end && isDigit(str.charAt(pos)); pos++) {}
        }
        return pos;
    }

    /**
     * Reads the characters of a quoted string from a segment of the input string.
     * <p>
     * This method assumes that the segment starts with a quote character (single or double).
     * The quoted string must end with the starting quote character. Characters within the string
     * can be quoted by a preceding '\'.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment (exclusive)
     * @return the position after the last character of the quoted string, or -1 when the closing
     *         quote not found
     */
    private int readQuotedString(String str, int start, int end, StringBuilder output) {
        Assert.isTrue(start < end);
        int pos = start;
        char startCh = str.charAt(pos);
        Assert.isTrue(startCh == '"' || startCh == '\'');

        for (pos++; pos < end; pos++) {
            char ch = str.charAt(pos);
            if (ch == startCh)
                return pos+1;
            if (ch == '\\' && ++pos < end)
                ch = str.charAt(pos);
            if (output != null)
                output.append(ch);
        }

        return -1; // end not found
    }

    /**
     * Reads the characters of a name from a segment of the input string.
     * <p>
     * This method assumes that the segment starts with a letter character (alphabetic or _).
     * The name contains alphanumeric characters and _.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment (exclusive)
     * @return the position after the last character of the name
     */
    private int readName(String str, int start, int end) {
        Assert.isTrue(start < end);
        int pos = start;
        char ch = str.charAt(pos);
        Assert.isTrue(isLetter(ch));
        for (pos++; pos < end; pos++) {
            ch = str.charAt(pos);
            if (!isLetter(ch) && !isDigit(ch))
                break;
        }
        return pos;
    }

    /**
     * Reads the characters of a pattern from a segment of the input string.
     * <p>
     * The pattern must start with a letter, '{', '?', '*', or '$' character.
     * After an '=~' operator the pattern must end with non-pattern-character or EOS;
     * otherwise it must end with a '.'.
     *
     * @param str the input string
     * @param start the start position if the segment
     * @param end the end position of the segment (exclusive)
     * @param tokens tokens parsed before the pattern
     * @return the position after the last character of the pattern, or -1 if no pattern found
     *
     * @see #isPatternChar(char)
     */
    private int readPattern(String str, int start, int end, List<Token> tokens) {
        int pos = start;
        char ch = str.charAt(pos);

        if (!isPatternStartChar(ch))
            return -1;

        if (!tokens.isEmpty() && tokens.get(tokens.size()-1).type == TokenType.LIKE) {
            int patternEnd = findPatternEnd(str, start, end);
            return patternEnd != -1 ? patternEnd : end;
        }
        else {
            int patternEnd = findPatternEnd(str, start, end); // eats .<name> too
            for (pos = patternEnd - 1; pos > start; pos--) {
                ch = str.charAt(pos);
                if (!isLetter(ch) && !isDigit(ch))
                    break;
            }
            if (pos == start || str.charAt(pos) != '.')
                return -1;
            return pos;
        }
    }

    /*---------------------------------------------------------------------------------------------
     *                                     Expression parsing
     *---------------------------------------------------------------------------------------------*/

    /**
     * Parses an expression from the given input string.
     * @param str the input string
     * @see #parseExpr(String,int,int,Set)
     */
    Expr parseExpr(String str)
            throws ParseException
    {
        return parseExpr(str, 0, str.length());
    }

    /**
     * Parses an expression from a segment of the input string.
     * <p>
     * The parser is implemented as a bottom-up operator precedence parser
     * (see Aho, Sethi, Ullman: Compilers: Principles, Techniques, and Tools, section 4.6).
     * <p>
     * As other bottom-up parsers, it reads the input from left-to-right,
     * and it either pushes the next token to a stack (shift step), or
     * replaces some objects from the top of the stack with a new object (reduce step).
     * The process stops when there is no more input tokens, or a parse error occurs.
     * <p>
     * Operator precedence parsers use three relations between tokens:
     * <ul>
     * <li>a &#x22d6; b: a "gives precedence to" b
     * <li>a &#x22d7; b: a "takes precedence over" b
     * <li>a &#x2250; b: a "has equal precedence to" b
     * </ul>
     * At most one of these relations holds between any two token,
     * but &#x2250; is not an equivalence, and &#x22d6; and &#x22d7; are
     * not partial orders in general. They can be approximated by
     * {@code leftPrec(a) < rightPrec(b)}, {@code leftPrec(a) > rightPrec(b)},
     * {@code leftPrec(a) == rightPrec(b)} resp.
     * <p>
     * The decision between shift and reduce is made by comparing precedences of the
     * topmost token of the stack, and of the next input token.
     * If <em>top</em> &#x22d7; <em>next</em>, then the stack already contains a handle on the top
     * that must be reduced, otherwise <em>next</em> is shifted onto the stack.
     *
     * @param str the input string
     * @param start the start of the segment
     * @param end the end of the segment
     */
    private Expr parseExpr(String str, int start, int end)
        throws ParseException
    {
        List<Token> tokens = tokenizeExpr(str, start, end);

        // add EOS to the end of tokens, and to the top of the stack
        Token eos = new Token(TokenType.EOS, "", end, end);
        tokens.add(eos);
        Stack<Object> stack = new Stack<Object>();
        stack.push(eos);

        int pos = 0;
        Token top = eos; // top token on the stack
        while(true) {
            Token next = tokens.get(pos);

            if (top == eos && next == eos) {
                if (stack.peek() instanceof Expr)
                    return (Expr)stack.peek();

                if (stack.peek() == eos)
                    throw new ParseException("missing expression", eos.start);
                else // it must be an ArgList, i.e. comma separated expressions
                    throw new ParseException("only one expression expected", eos.start);
            }

            if (top.type.getLeftPrec() <= next.type.getRightPrec()) {
                // shift
                stack.push(next);
                top = next;
                pos++;
            }
            else {
                // reduce
                new Handle(stack, str).reduce();

                // find new top
                for (int i = stack.size()-1; i >= 0; i--) {
                    if (stack.get(i) instanceof Token) {
                        top = (Token)stack.get(i);
                        break;
                    }
                }
            }
        }
    }

    /**
     * Class representing the handle on the top of the stack.
     * <p>
     * The handle consists of tokens and non-terminal values
     * that can be reduced to a new value by using a production rule.
     * <p>
     * The handle starts at the topmost token <em>b</em> that
     * <em>a &#x22d6; b</em> holds for the token <em>a</em> left to it.
     * (Non-terminal values between <em>a</em> and <em>b</em> are ignored.)
     *
     */
    private class Handle {
        String input;        // the input string
        Stack<Object> stack; // the stack that stores the handle
        int start;           // position if the first element of the handle
        int len;             // number of elements in the handle

        Handle(Stack<Object> stack, String input) {
            this.input = input;
            this.stack = stack;

            // find the start of the handle
            len = 0;
            Token right = null;
            for (start = stack.size() - 1; start > 0; start--) {
                Object o = stack.elementAt(start);
                len++;

                if (o instanceof Token)
                    right = (Token)o;

                Object left = stack.elementAt(start-1);
                if (left instanceof Token && right != null && ((Token)left).type.getLeftPrec() < right.type.getRightPrec())
                    break;
            }
        }

        // small utilities to make reduce() more readable. these methods will be inlined by the JVM.
        private boolean isToken(int i)   { return object(i) instanceof Token; }
        private boolean isToken(int i, TokenType type) { return (object(i) instanceof Token) && token(i).type == type; }
        private boolean isExpr(int i)    { return object(i) instanceof Expr; }
        private boolean isArgList(int i) { return object(i) instanceof ArgList; }
        private Object object(int i)     { return stack.elementAt(start+i); }
        private Token token(int i)       { return (Token)object(i); }
        private Expr expr(int i)         {return (Expr)object(i); }
        private ArgList args(int i)      { return (ArgList)object(i); }

        /**
         * Reduces the handle, i.e. replaces the top elements of the stack that
         * corresponds to the right-hand-side of a production with the value
         * of its left-hand-side.
         *
         * @throws ParseException if the top of the stack can not be reduced
         */
        void reduce() throws ParseException {

            // the left-hand-side of the production
            Object lhs = null;

            if (len == 1) {
                if (isToken(0)) {
                    // expr = NAME
                    if (isToken(0, TokenType.NAME))
                        lhs = new VariableRef(token(0).value);
                    // expr = STATISTIC
                    else if (isToken(0, TokenType.STATISTIC))
                        lhs = new StatisticRef(token(0).value, null);
                    // expr = NUMBER
                    else if (isToken(0, TokenType.NUMBER))
                        lhs = new NumLiteral(Double.valueOf(token(0).value));
                    // expr = STRING
                    else if (isToken(0, TokenType.STRING))
                        lhs = new StringLiteral(token(0).value);
                }
            }
            else if (len == 2) {
                // expr = - expr
                //      | ~ expr
                //      | ! expr
                if ((isToken(0, TokenType.UMINUS) || isToken(0, TokenType.BINCOMPL) || isToken(0, TokenType.NOT)) && isExpr(1))
                    lhs = new FunctionCall(token(0).value, new ArgList(expr(1)));
                // expr = FUN args
                //      | FUN expr
                else if (isToken(0, TokenType.FUN)) {
                    if (isArgList(1))
                        lhs = new FunctionCall(token(0).value, args(1));
                    else if (isExpr(1))
                        lhs = new FunctionCall(token(0).value, new ArgList(expr(1)));
                }
            }
            else if (len == 3) {
                // expr = expr + expr | expr - expr | expr * expr | expr / expr | expr % expr | expr ^ expr
                //      | expr == expr | expr != expr | expr > expr | expr >= expr | expr < expr | expr <= expr
                //      | expr && expr | expr || expr
                //      | expr & expr | expr '|' expr | expr # expr | expr << expr | expr >> expr
                //      | expr ++ expr
                if (isExpr(0) && isToken(1) && isExpr(2)) {
                    Token token = token(1);
                    switch (token.type) {
                    case PLUS: case MINUS: case MULT: case DIV: case MOD: case POWER:
                    case EQ: case NEQ: case GT: case GE: case LT: case LE: case AND:
                    case OR: case BINAND: case BINOR: case BINXOR: case SHIFTLEFT:
                    case SHIFTRIGHT: case CONCAT:
                        lhs = new FunctionCall(token.value, new ArgList(expr(0), expr(2)));
                    }
                }
                // expr = ( expr )
                // args = ( args )
                else if (isToken(0) && (isExpr(1) || isArgList(1)) && isToken(2)) {
                    if (isToken(0, TokenType.LPAREN) && isToken(2, TokenType.RPAREN))
                        lhs = object(1);
                }
                // expr = expr LIKE PATTERN
                else if (isExpr(0) && isToken(1) && isToken(2)) {
                    if (isToken(1, TokenType.LIKE) && isToken(2, TokenType.PATTERN)) {
                        Pattern pattern = parsePattern(input, token(2).start, token(2).end);
                        lhs = new LikeOp(expr(0), pattern);
                    }
                }
                // expr = PATTERN . STATISTIC
                else if (isToken(0, TokenType.PATTERN) && isToken(1, TokenType.DOT) && isToken(2, TokenType.STATISTIC)) {
                    lhs = new StatisticRef(token(2).value, parsePattern(input, token(0).start, token(0).end));
                }
            }
            // expr = expr ? expr : expr
            else if (len == 5) {
                if (isExpr(0) && isToken(1, TokenType.QUESTION) && isExpr(2) && isToken(3, TokenType.COLON) && isExpr(4)) {
                    lhs = new FunctionCall(token(1).value, new ArgList(expr(0), expr(2), expr(4)));
                }
            }

            // args = expr , expr ...
            if (len >= 3 && (len % 2) == 1 && isToken(1, TokenType.COMMA)) {
                ArgList args = new ArgList();
                for (int i=0; i < len; i++) {
                    if ((i % 2) == 0) {
                        if (!isExpr(i)) {
                            if (isToken(i, TokenType.COMMA))
                                throw new ParseException("missing expression before comma", token(i).start);
                            else
                                throw new ParseException("missing expression", token(1).start);
                        }
                        args.add(expr(i));
                    }
                    else {
                        if (!isToken(i, TokenType.COMMA))
                            throw new ParseException("missing comma", token(1).start);
                    }
                }
                lhs = args;
            }

            if (lhs != null) {
                for (int i = 0; i < len; ++i)
                    stack.pop();
                stack.push(lhs);
            }
            else // no reduction found, generate a meaningful error message
                reportError();
        }

        /**
         * This method called when there is no handle found on the top of the stack.
         * Its purpose is to throw a parse exception with a meaningful error message.
         */
        private void reportError() throws ParseException {
            if (len >= 1 && isToken(0)) {
                Token token = token(0);
                switch(token.type) {
                case NUMBER: case STRING: case NAME: case STATISTIC: break; // should not happen
                case PATTERN: break; // should not happen
                case DOT: throw new ParseException("missing pattern before .", token.start);
                case LIKE: throw new ParseException("missing expression before =~", token.start);
                case LPAREN: case RPAREN: break;
                case UMINUS: case NOT: case BINCOMPL: break;
                case QUESTION: throw new ParseException("condition of ? operator is missing", token.start);
                case COLON: throw new ParseException("missing ? operator before :", token.start);
                default: // binary operator
                    throw new ParseException("left operand of " + token.value + " is missing", token.start); // TODO
                }
            }
            if (len >= 1 && isToken(len-1)) {
                Token token = token(len-1);
                switch(token.type) {
                case NUMBER: case STRING: case NAME: case STATISTIC:
                    throw new ParseException("missing operator before \""+token.value+"\"", token.start);
                case PATTERN:
                    if (len >= 2 && isToken(len-2, TokenType.LIKE))
                        throw new ParseException("left operand of =~ operator is missing", token.start);
                    else // should not happen, because the lexer does not recognize the PATTERN without the .<statistic> part
                        throw new ParseException("missing . and statistic name after pattern", token.end);
                case DOT: throw new ParseException("missing statistic name after .", token.end);
                case LIKE: throw new ParseException("missing pattern after =~ operator", token.end);
                case COMMA: throw new ParseException("missing expression after the comma", token.end);
                case LPAREN: throw new ParseException("missing expression after (", token.end);
                case RPAREN:
                    if (isToken(0, TokenType.LPAREN))
                        throw new ParseException("missing expression before )", token.start);
                    else if (len >= 2 && isToken(0, TokenType.FUN) && isToken(1, TokenType.LPAREN))
                        throw new ParseException("missing function arguments before )", token.start);
                    else
                        throw new ParseException("unmatched )", token.start);
                case UMINUS: case NOT: case BINCOMPL:
                    throw new ParseException("operand of " + token.value + " operator is missing", token.end);
                case QUESTION: throw new ParseException("missing expression", token.end);
                case COLON: throw new ParseException("missing expression", token.end);
                default: // binary operator
                    throw new ParseException("right operand of "+token.value+" is missing", token.end);
                }
            }
            throw new ParseException("parse error", 0); // general error message; should not see this often
        }
    }

    /*---------------------------------------------------------------------------------------------
     *                                      Pattern parsing
     *---------------------------------------------------------------------------------------------*/

    /**
     * Parse {@code str} as a pattern.
     *
     * @see #parsePattern(String,int,int)
     */
    Pattern parsePattern(String str)
        throws ParseException
    {
        return parsePattern(str, 0, str.length());
    }

    /**
     * Parse a segment of {@code str} as a pattern.
     * <p>
     * A pattern is a sequence of pattern elements.
     * A pattern element is one of:
     * <pre>
     *   ?
     *   *
     *   **
     *   {&lt;chars&gt}
     *   {^&lt;chars&gt}
     *   {&lt;numeric range&gt}
     *   [&lt;numeric range&gt]
     *   ${&lt;var&gt=&lt;pattern&gt}
     *   ${&lt;expr&gt}
     *   &lt;literal&gt;
     * </pre>
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     */
    private Pattern parsePattern(String str, int start, int end)
        throws ParseException
    {
        List<PatternElement> elements = new ArrayList<PatternElement>();
        int pos = start, elementEnd;

        while (pos < end)
        {
            char ch = str.charAt(pos);
            switch (ch) {
            case '?':
                elements.add(new AnyCharPattern());
                pos++;
                break;
            case '*':
                if (pos+1<end && str.charAt(pos+1) == '*') {
                    elements.add(new ManyCharsPattern(true));
                    pos += 2;
                }
                else {
                    elements.add(new ManyCharsPattern(false));
                    pos++;
                }
                break;
            case '{':
                elementEnd = parseNumericRangePattern(str, pos, end, elements);
                if (elementEnd == -1)
                    elementEnd = parseCharacterSetPattern(str, pos, end, elements);
                pos = elementEnd;
                break;
            case '[':
                elementEnd = -1;
                if (!elements.isEmpty() && (elements.get(elements.size()-1) instanceof LiteralPattern))
                    elementEnd = parseNumericRangePattern(str, pos, end, elements);
                if (elementEnd != -1)
                    elementEnd--; // do not skip closing ']'
                else
                    elementEnd = parseLiteralPattern(str, pos, end, elements);
                pos = elementEnd;
                break;
            case '$':
                if (pos+1 < end && str.charAt(pos+1) == '{') {
                    elementEnd = parseVariableDefPattern(str, pos, end, elements);
                    if (elementEnd == -1)
                        elementEnd = parseExpressionPattern(str, pos, end, elements);
                    if (elementEnd == -1)
                        elementEnd = parseLiteralPattern(str, pos, end, elements);
                    pos = elementEnd;
                }
                else
                    pos = parseLiteralPattern(str, pos, end, elements);
                break;
            default:
                pos = parseLiteralPattern(str, pos, end, elements);
                break;
            }
        }

        return new Pattern(elements);
    }

    /**
     * Parses a numeric range pattern from a segment of the input string,
     * and adds the parsed element to {@code output}.
     * <p>
     * A numeric range is either in the "{&lt;range&gt;}" or in the "[&lt;range&gt;]" form,
     * where, &lt;range&gt; is one of:
     * <pre>
     *   &lt;int&gt;..&lt;int&gt;
     *   ..&lt;int&gt;
     *   &lt;int&gt;..
     * </pre>
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @param output a list which the result element collected into
     * @return the position after the closing brace/bracket, or -1 if not found, or the range is malformed
     */
    // TODO allow comma separated ranges
    private int parseNumericRangePattern(String str, int start, int end, List<PatternElement> output) {
        Assert.isTrue(start < end && str.charAt(start)=='{' || str.charAt(start)=='[');

        char closingChar = str.charAt(start)=='{' ? '}' : ']';
        int lo = -1, up = -1;
        int pos = start+1; // skip '[' or '{'
        // read lower bound
        int numEnd = readInteger(str, pos, end);
        if (numEnd != -1) {
            lo = Integer.parseInt(str.substring(pos, numEnd));
            pos = numEnd;
        }
        // read ".."
        if (pos+1 >= end || str.charAt(pos)!='.' || str.charAt(pos+1)!='.')
            return -1;
        pos += 2;
        // read upper bound
        numEnd = readInteger(str, pos, end);
        if (numEnd != -1) {
            up = Integer.parseInt(str.substring(pos, numEnd));
            pos = numEnd;
        }

        // check and skip closing ']' or '}'
        if (str.charAt(pos++) != closingChar)
            return -1;

        if (output != null) {
            List<Integer> nums = new ArrayList<Integer>();
            nums.add(lo);
            nums.add(up);
            output.add(new NumericRangePattern(nums));
        }

        return pos;
    }

    /**
     * Returns {@code true} if the given segment of the input string
     * starts with a numeric range pattern.
     */
    private boolean isNumericRangePattern(String str, int start, int end) {
        return parseNumericRangePattern(str, start, end, null) != -1;
    }

    /**
     * Parses a character set pattern from a segment of the input string,
     * and adds the parsed element to the {@code output} list.
     * <p>
     * The character set pattern is either in the "{&lt;chars&gt;}" or in the "{^&lt;chars&gt;}" form,
     * where &lt;chars&gt; is a sequence of characters/character ranges. If the '}' is part of &lt;chars&gt;,
     * then it must be the first character.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @param output the list which collects the parsed elements
     * @return the position after the closing '}' character
     */
    private int parseCharacterSetPattern(String str, int start, int end, List<PatternElement> output)
        throws ParseException
    {
        int patternEnd = readCharacterSetPattern(str, start, end);
        if (patternEnd == -1)
            throw new ParseException("no matching '}' in pattern", start);

        boolean negate = false;
        int patternStart = start + 1; // skip '{'
        if (str.charAt(start+1) == '^') {
            negate = true;
            patternStart++;
        }

        StringBuilder sb = new StringBuilder();
        int pos = patternStart;
        while (pos < patternEnd-1) {
            char ch = str.charAt(pos);
            if (pos+2 < patternEnd-1 && str.charAt(pos+1) == '-') {
                sb.append(ch);
                sb.append(str.charAt(pos+2));
                pos += 3;
            }
            else {
                sb.append(ch).append(ch);
                pos++;
            }
        }

        output.add(new CharacterSetPattern(sb.toString(), negate));
        return patternEnd;
    }

    /**
     * Parses an expression pattern from a segment of the input string,
     * and adds the parsed element to the {@code output} list.
     * <p>
     * The expression pattern is in the "${&lt;expr&gt;}" form.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @param output the list which collects the parsed elements
     * @return the position after the closing '}', or -1 if not found
     */
    private int parseExpressionPattern(String str, int start, int end, List<PatternElement> output)
        throws ParseException
    {
        int exprEnd = readExpressionPattern(str, start, end);
        if (exprEnd != -1) {
            output.add(new ExprPattern(parseExpr(str, start+2, exprEnd-1)));
            return exprEnd;
        }

        return -1;
    }

    /**
     * Parses a variable definition pattern from a segment if the input string,
     * and adds the parsed element to the {@code output} list.
     * <p>
     * The variable definition pattern is in the "${&lt;var&gt;=&lt;pattern&gt;}" form.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @param output the list which collects the parsed elements
     * @return the position after the closing '}', or -1 if not found
     */
    private int parseVariableDefPattern(String str, int start, int end, List<PatternElement> output)
        throws ParseException
    {
        int vardefEnd = readVariableDefPattern(str, start, end);
        if (vardefEnd != -1) {
            int varNameEnd = str.indexOf('=', start);
            String varName = str.substring(start+2, varNameEnd);
            output.add(new VariableDefPattern(varName, parsePattern(str, varNameEnd+1, vardefEnd-1)));
            return vardefEnd;
        }
        return -1;
    }

    /**
     * Parses a literal pattern from a segment of the input string,
     * and adds the parsed element to the {@code output} list.
     * <p>
     * The literal character end at the next '?', '{', '*', or '$'.
     * The '[' character starting a numeric range pattern is added
     * to the literal, but otherwise not handled as part of it.
     * Special characters can be quoted by preceding them with '\'.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @param output the list which collects the parsed elements
     * @return the position after the literal pattern
     */
    private int parseLiteralPattern(String str, int start, int end, List<PatternElement> output) {
        StringBuilder sb = new StringBuilder();
        int pos = start;
        for (; pos < end; pos++) {
            char ch = str.charAt(pos);
            if (pos > start && (ch == '?' || ch == '{' || ch == '*' || ch == '$'))
                break;
            if (ch == '\\' && pos+1 < end)
                sb.append(str.charAt(++pos));
            else
                sb.append(ch);
            if (ch == '[' && isNumericRangePattern(str, pos, end))
                break; // note: '[' already added to the literal
        }
        output.add(new LiteralPattern(sb.toString()));
        return pos;
    }

    /**
     * Reads characters of an integer from a segment of the input string.
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @return the position after the last digit of the number, or -1 if no digits were found
     */
    private int readInteger(String str, int start, int end) {
        int pos;
        for (pos = start; pos < end && Character.isDigit(str.charAt(pos)); pos++) ;
        return pos != start ? pos : -1;
    }

    /**
     * Read characters of a character set pattern (e.g. "{a-zA-Z_}") from a segment
     * of the input string.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @return the position of the character after the ending '}', or -1 if not found
     */
    private int readCharacterSetPattern(String str, int start, int end) {
        int pos = start+1; // skip {
        if (pos < end && str.charAt(pos) == '^')
            pos++;
        int begin = pos;
        while (pos < end) {
            char ch = str.charAt(pos);
            // Note: to make "}" part of the set, it must be first within the braces
            if (ch == '}' && pos != begin)
                break;
            else
                pos++;
        }
        if (pos == end)
            return -1;
        pos++; // skip }
        return pos;
    }

    /**
     * Reads characters of a variable definition pattern ("${<var>=<pattern>}")
     * from a segment of the input string.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @return the position after the ending '}' character, or -1 if the variable name or '}' not found
     */
    private int readVariableDefPattern(String str, int start, int end) {
        Assert.isTrue(start+1 < end && str.charAt(start) == '$' && str.charAt(start+1) == '{');
        int pos = start + 2; // skip "${"

        // looks like ${var=...} ?
        while (pos < end && isLetter(str.charAt(pos)))
            pos++;
        if (pos == start + 2)
            return -1;
        if (pos+1 >= end || str.charAt(pos) != '=' || str.charAt(pos+1) == '=' || str.charAt(pos+1) == '~')
            return -1;

        // find the closing '}'. assumption: pattern may contain '}' in num or char ranges, but not nested expressions
        int depth = 1;
        while (pos < end) {
            char ch = str.charAt(pos);
            switch(ch) {
            case '}':
                if (--depth == 0)
                    return pos+1;
                pos++;
                break;
            case '\\':
                pos += 2;
                break;
            case '{':
                depth++;
                if (pos+1 < end && str.charAt(pos+1) == '}')
                    pos +=2;
                else if (pos+2 < end && str.charAt(pos+1) == '^' && str.charAt(pos+2) == '}')
                    pos += 3;
                else
                    pos++;
                break;
            default: pos++; break;
            }
        }
        return -1;
    }

    /**
     * Reads characters of an expression nested in a pattern.
     * <p>
     * The method finds the '}' character that ends a "${<expr>}" or "${<var>=<pattern>}" form.
     * It is assumed that {@code start} is the position of the "${" prefix.
     * Because the input string might contain balanced '{' and '}' pairs in nested patterns,
     * and standalone '}' characters in character set patterns, the method counts their occurences.
     *
     * @param str the input string
     * @param start the start position of input segment
     * @param end the end position of the input segment
     * @return the position of the character after the closing '}', or -1 if not found
     */
    private int readExpressionPattern(String str, int start, int end) {
        Assert.isTrue(start+1 < end && str.charAt(start) == '$' && str.charAt(start+1) == '{');
        int depth = 1;
        int pos = start + 2;
        while (pos < end) {
            char ch = str.charAt(pos);
            switch(ch) {
            case '}':
                if (--depth == 0) return pos+1;
                break;
            case '\\': pos += 2; break;
            case '{':
                depth++;
                if (pos+1 < end && str.charAt(pos+1) == '}')
                    pos +=2;
                else if (pos+2 < end && str.charAt(pos+1) == '^' && str.charAt(pos+2) == '}')
                    pos += 3;
                else
                    pos++;
                break;
            default: pos++; break;
            }
        }
        return -1;
    }

    /**
     * Find the end of a pattern. The pattern starts at the {@code start}
     * position and spans at most the {@code end} position in the string.
     * @return the end position of the pattern (exclusive)
     */
    private int findPatternEnd(String str, int start, int end) {
        char ch;
        int pos = start;
        while (pos < end) {
            ch = str.charAt(pos);
            switch (ch) {
            case '$': // skip ${...}
                if (pos+1 < end && str.charAt(pos+1)=='{') {
                    int exprEnd = readExpressionPattern(str, pos, end);
                    if (exprEnd != -1)
                        pos = exprEnd;
                    else
                        return pos;
                }
                else
                    return pos; // $ only allowed in the form ${...}
                break;
            case '{': // skip {...}
                int charSetEnd = readCharacterSetPattern(str, pos, end); // works for numeric ranges too
                if (charSetEnd != -1)
                    pos = charSetEnd;
                else
                    return pos;
                break;
            case '\\':
                if (pos+1 < end)
                    pos += 2;
                else
                    return pos;
                break;
            default:
                if (!isPatternChar(ch))
                    return pos;
                else
                    pos++;
                break;
            }
        }
        return pos;
    }

    /*---------------------------------------------------------------------------------------------
     *                                     Template parsing
     *---------------------------------------------------------------------------------------------*/

    /**
     * Parses {@code str} as a string template.
     */
    StringTemplate parseTemplate(String str)
            throws ParseException
    {
        return parseTemplate(str, 0, str.length());
    }

    /**
     * Parses a string template from a segment of the input string.
     */
    private StringTemplate parseTemplate(String str, int start, int end)
            throws ParseException
    {
        List<TemplateElement> elements = new ArrayList<TemplateElement>();
        int pos = start;

        while (pos < end)
        {
            char ch = str.charAt(pos);
            switch (ch) {
            case '$':
                if (pos+1 < end && str.charAt(pos+1) == '{') {
                    int pos2 = parseSplicedExpr(str, pos, end, elements);
                    pos = pos2 != -1 ? pos2 : parseStringLiteral(str, pos, end, elements);
                }
                else
                    pos = parseStringLiteral(str, pos, end, elements);
                break;
            default:
                pos = parseStringLiteral(str, pos, end, elements);
                break;
            }
        }

        return new StringTemplate(elements);
    }

    /**
     * Parses a spliced expression from a segment of the input string,
     * and adds the parsed element to the {@code output} list.
     *
     * @param str the input string
     * @param start the start position of the segment
     * @param end the end position of the segment
     * @param output the list which collects the template elements
     * @return the position after the expression, or -1 if the end of the expression not found
     */
    private int parseSplicedExpr(String str, int start, int end, List<TemplateElement> output)
        throws ParseException
    {
        int exprEnd = readExpressionPattern(str, start, end);
        if (exprEnd != -1) {
            output.add(new SplicedExpr(parseExpr(str, start+2, exprEnd-1)));
            return exprEnd;
        }
        return -1;
    }

    /**
     * Parses a literal string from a segment of the input string,
     * and adds the parsed element to the {@code output} list.
     *
     * @param str the input string
     * @param start the start position of the input segment
     * @param end the end position of the input segment
     * @param output the list in which output is collected
     * @return the position after the literal
     */
    private int parseStringLiteral(String str, int start, int end, List<TemplateElement> output) {
        StringBuilder sb = new StringBuilder();
        int pos = start;
        for (; pos < end; pos++) {
            char ch = str.charAt(pos);
            if (pos > start && ch == '$')
                break;
            if (ch == '\\' && pos+1 < end)
                sb.append(str.charAt(++pos));
            else
                sb.append(ch);
        }
        output.add(new StringLiteral(sb.toString()));
        return pos;
    }

    private static boolean isLetter(char ch) {
        return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
    }

    private static boolean isDigit(char ch) {
        return ch >= '0' && ch <= '9';
    }

    private static boolean isPatternStartChar(char ch) {
        return ch == '$' || ch == '{' || ch == '?' || ch == '*' || isLetter(ch);
    }

    private static boolean isPatternChar(char ch) {
        // note that inside {} all characters are allowed, and within ${} expression characters are allowed
        return ch == '.' || ch == '$' || ch == '{' || ch == '}' || ch == '[' || ch == ']' || ch == ',' || ch == '?' || ch == '*' || isLetter(ch) || isDigit(ch);
    }
}
