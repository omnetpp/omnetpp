package org.omnetpp.scave.computed;

import static org.omnetpp.common.util.IteratorUtils.concatMap;
import static org.omnetpp.common.util.IteratorUtils.map;

import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import org.omnetpp.common.util.IteratorUtils.Mapping;
import org.omnetpp.scave.computed.ExpressionEvaluator.Environment;
import org.omnetpp.scave.computed.ExpressionSyntax.AnyCharPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.CharacterSetPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.NumericRangePattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ExprPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.LiteralPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ManyCharsPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.PatternElement;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionVisitors.VisitorBase;

/**
 * Utility class to match a string with a parsed pattern.
 *
 * @author tomi
 */
class PatternMatcher {

    private final String string;
    private final int end;

    PatternMatcher(String string) {
        this.string = string;
        this.end = string.length();
    }

    /**
     * Matches the string with {@code pattern} in the specified
     * environment. Variables in {@code env} can be refered in the
     * pattern. The received {@code env} is not modified.
     *
     * On success, it returns an extended environment which contains
     * the bound variables.
     * On failure it returns {@code null}.
     */
    Environment fullMatch(Pattern pattern, Environment env) {
        MatcherVisitor visitor = new MatcherVisitor();
        Iterator<Context> matches = visitor.match(pattern, env, 0);
        while (matches.hasNext()) {
            Context match = matches.next();
            if (match.pos == end)
                return match.env;
        }
        return null;
    }

    /**
     * Intermediary state of the matching process.
     */
    private static class Context {
        final int pos;         // position of the next character in the input string to be matched
        final Environment env; // current environment
        Context(int pos, Environment env) {
            this.pos = pos;
            this.env = env;
        }
    }

    /** Match failure */
    private static final Iterator<Context> FAIL = Collections.<Context>emptySet().iterator();
    /** Single match result */
    private static Iterator<Context> single(Context context) { return Collections.singleton(context).iterator(); }

    /**
     * Implementation of the matcher as a visitor.
     * Each node expects the current position and environment, and
     * returns zero or more (position,environment) pairs as an iterator.
     * <p>
     * Failure represented as an empty iterator. Most pattern elements
     * yields at most one match, except '*' and '**' wildcards.
     */
    private class MatcherVisitor extends VisitorBase<Iterator<Context>,Context> {

        /**
         * Match some part of the input string starting at {@code pos}
         * with {@code pattern} in the specified environment.
         *
         * @param pattern the pattern to be matched
         * @param env the environment containing variables defined outside the pattern
         * @param pos a position in the input string where the match starts
         * @return zero or more match
         */
        Iterator<Context> match(Pattern pattern, Environment env, int pos) {
            return match(pattern.elements, 0, single(new Context(pos, env)));
        }

        /**
         * Match some part of the input string with a list of pattern elements
         * starting at position {@code k} in the {@code elements} list.
         * The match tried for each input {@code context}, individual results
         * are concatenated.
         *
         * @return an iterator of successful matches containing the end positions and environments
         */
        Iterator<Context> match(List<PatternElement> elements, int k, Iterator<Context> contexts) {
            if (k == elements.size()) {
                return contexts;
            }
            final PatternElement element = elements.get(k);
            Iterator<Context> result = concatMap(contexts, new Mapping<Context,Iterator<Context>>() {
                public Iterator<Context> map(Context context) {
                    return element.accept(MatcherVisitor.this, context);
                }
            });
            return match(elements, k+1, result);
        }

        /**
         * Match the start of the input string with a literal pattern.
         */
        @Override
        public Iterator<Context> visitLiteralPattern(LiteralPattern lit, Context context) {
            return matchLiteral(lit.value, context);
        }

        /**
         * Match the next character of the input string with the any char pattern ('?').
         * The match fails if there is no more character in the input string.
         */
        @Override
        public Iterator<Context> visitAnyCharPattern(AnyCharPattern p, Context context) {
            int pos = context.pos;
            if (pos < end) {
                return single(new Context(pos+1, context.env));
            }
            return FAIL;
        }

        /**
         * Match zero or more characters of the input string with the '*' or '**' pattern.
         */
        @Override
        public Iterator<Context> visitManyCharsPattern(final ManyCharsPattern p, final Context context) {
            return new Iterator<Context>() {
                int start = context.pos;
                int pos = context.pos;
                public boolean hasNext() { return pos == start || (pos > start && pos <= end && (string.charAt(pos-1) != '.' || p.acceptDot));  }
                public Context next() { return new Context(pos++, context.env); }
                public void remove() { throw new UnsupportedOperationException(); }
            };
        }

        /**
         * Match the next character of the input string with an enumerated character set.
         * <p>
         * If there is no more character in the input string then the match fails.
         * Otherwise it succeeds if the next character is in the character set
         * specified by {@code p}.
         */
        @Override
        public Iterator<Context> visitCharacterSetPattern(CharacterSetPattern p, Context context) {
            int pos = context.pos;
            if (pos < end) {
                char ch = string.charAt(pos);
                boolean match = p.negate;
                for (int i = 0; i < p.chars.length(); i+=2) {
                    char from = p.chars.charAt(i);
                    char to = p.chars.charAt(i+1);
                    if (from <= ch && ch <= to) {
                        match = !match;
                        break;
                    }
                }
                if (match)
                    return single(new Context(pos+1, context.env));
            }
            return FAIL;
        }

        /**
         * Match the input string with one of the integers enumerated in the pattern element.
         * <p>
         * If there is no more character in the input, or if it does not start with a digit,
         * then the match fails. Otherwise an integer is read from the input, and the match
         * succeeds iff the number is contained by a range specified in {@code p}.
         */
        @Override
        public Iterator<Context> visitNumericRangePattern(NumericRangePattern p, Context context) {
            int pos = context.pos;
            if (pos < end) {
                int start = pos;
                int val = 0;
                for ( ; pos < end; pos++) {
                    char ch = string.charAt(pos);
                    if (ch >= '0' && ch <= '9')
                        val = 10 * val + (ch - '0');
                    else
                        break;
                }
                if (pos != start) {
                    boolean match = false;
                    for (int i = 0; i < p.values.size(); i+=2) {
                        int from = p.values.get(i);
                        int to = p.values.get(i+1);
                        if ((from == -1 || from <= val) && (to == -1 || val <= to)) {
                            match = true;
                            break;
                        }
                    }
                    if (match)
                        return single(new Context(pos, context.env));
                }
            }
            return FAIL;
        }

        /**
         * Match the input string with a nested pattern, and bind the
         * matched part to a variable.
         */
        @Override
        public Iterator<Context> visitVariableDefPattern(final VariableDefPattern p, Context context) {
            int pos = context.pos;
            final int start = pos;
            Iterator<Context> result = match(p.pattern, context.env, context.pos);
            return map(result, new Mapping<Context,Context>() {
                public Context map(Context context) {
                    String value = string.substring(start, context.pos);
                    Environment newEnv = context.env.clone();
                    newEnv.define(p.name, value);
                    return new Context(context.pos, newEnv);
                }
            });
        }

        /**
         * Match the input string with an expression.
         * First evaluates the expression to a string, and match the input string
         * with the result literally.
         */
        @Override
        public Iterator<Context> visitExprPattern(ExprPattern p, Context context) {
            ExpressionEvaluator evaluator = new ExpressionEvaluator();
            String value = evaluator.evaluateSimple(p.expr, context.env).asString();
            return matchLiteral(value, context);
        }

        /**
         * Match the input characters literally with {@code literal}.
         */
        private Iterator<Context> matchLiteral(String literal, Context context) {
            int pos = context.pos;
            int len = literal.length();
            if (pos + len <= end && string.substring(pos, pos+len).equals(literal)) {
                    return single(new Context(pos+len, context.env));
            }
            return FAIL;
        }
    }
}
