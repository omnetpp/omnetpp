package org.omnetpp.scave.computed;

import static org.omnetpp.scave.computed.ExpressionSyntax.call;
import static org.omnetpp.scave.computed.ExpressionSyntax.chars;
import static org.omnetpp.scave.computed.ExpressionSyntax.manychars;
import static org.omnetpp.scave.computed.ExpressionSyntax.num;
import static org.omnetpp.scave.computed.ExpressionSyntax.op;
import static org.omnetpp.scave.computed.ExpressionSyntax.pattern;
import static org.omnetpp.scave.computed.ExpressionSyntax.scalar;
import static org.omnetpp.scave.computed.ExpressionSyntax.string;
import static org.omnetpp.scave.computed.ExpressionSyntax.var;
import static org.omnetpp.scave.computed.ExpressionSyntax.vardef;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Set;

import org.apache.commons.collections.CollectionUtils;
import org.apache.commons.lang3.ObjectUtils;
import org.omnetpp.scave.computed.ExpressionEvaluator.Environment;
import org.omnetpp.scave.computed.ExpressionEvaluator.Value;
import org.omnetpp.scave.computed.ExpressionParser.Token;
import org.omnetpp.scave.computed.ExpressionSyntax.AnyCharPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.CharacterSetPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Expr;
import org.omnetpp.scave.computed.ExpressionSyntax.ExprPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.LikeOp;
import org.omnetpp.scave.computed.ExpressionSyntax.LiteralPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ManyCharsPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Node;
import org.omnetpp.scave.computed.ExpressionSyntax.NumLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.NumericRangePattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.SplicedExpr;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.StringLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.StringTemplate;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;
import org.omnetpp.scave.computed.ExpressionSyntax.Visitor;

public class ExpressionTests {

    public static void testAll() {
        testTokenizer();
        testPatternParser();
        testExprParser();
        testPatternMatcher();
        testExprEval();
        testTemplateEval();
        benchmarkExpressionParser();
    }

    static void testTokenizer() {
        System.out.println("------------- tokenized expressions ----------------");

        testTokenizer("8*byteCount/duration");
        testTokenizer("sum(pkLoss)");
        testTokenizer("sum(delay*pkCount) / sum(pkCount)");
        testTokenizer("pkLoss / pkSent");
        testTokenizer("${host=**}.networklayer.udp.pkCount + ${host}.networklayer.tcp.pkCount");
        testTokenizer("Network.A.scalar1 * Network.B.scalar2");
        testTokenizer("Network.A${i=*}.scalar1 * Network.B${i}.scalar2");
        testTokenizer("(${m=?}{$i=[0,2]}.sentPk - ${m}[${i+1}].recvPk) / ${m}[${i}].sentPk");
        testTokenizer("mean((${m=?}${i=[0,2]}.sentPk - ${m}[${i+1}].recvPk) / ${m}[${i}].sentPk)");
        testTokenizer("mean('busy:vector')");
        testTokenizer("${m={}}}.duration");
        testTokenizer("${m={^}}}.duration");
        testTokenizer("a ? \"hi\" : \"bye\"");
        testTokenizer("a ++ \"x\"");
    }


    static void testTokenizer(String expr) {
        System.out.println(expr);
        try {
            ExpressionParser parser = new ExpressionParser();
            List<Token> tokens = parser.tokenizeExpr(expr);
            System.out.print("  ");
            printTokens(tokens);
            System.out.println("");
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage());
        }
    }

    static void printTokens(List<Token> tokens) {
        for (int i = 0; i < tokens.size(); ++i)
        {
            if (i > 0)
                System.out.print("; ");
            Token token = tokens.get(i);
            switch (token.type) {
            case NUMBER: System.out.print("NUMBER:"+token.value); break;
            case STRING: System.out.print("\""+token.value+"\""); break;
            case NAME: System.out.print("NAME:"+token.value); break;
            case FUN: System.out.print("FUN:"+token.value); break;
            case STATISTIC: System.out.print("STAT:"+token.value); break;
            case PATTERN: System.out.print("PATTERN:"+token.value); break;
            default: System.out.print(token.value); break;
            }
        }
    }

    static void testExprParser() {
        System.out.println("------------- parsed expressions ----------------");

        long start = System.currentTimeMillis();

        testExprParser("1+a*b", op("+", num(1), op("*", var("a"), var("b"))));
        testExprParser("(1+a)*b", op("*", op("+", num(1), var("a")), var("b")));
        testExprParser("-a", op("-", var("a")));
        testExprParser("!a==3", op("==", op("!", var("a")), num(3)));
        testExprParser("~a^2",  op("^", op("~", var("a")), num(2)));
        testExprParser("5^3^2", op("^", num(5), op("^", num(3), num(2))));
        testExprParser("f(a)", call("f", var("a")));
        testExprParser("f(1,2,3)", call("f", num(1), num(2), num(3)));
        testExprParser("f(2*(3+4), 5^(7))", call("f", op("*", num(2), op("+", num(3), num(4))), op("^", num(5), num(7))));
        testExprParser("A.bytecount");
        testExprParser("A.bytecount/B.duration");
        testExprParser("a ? b : c", op("?", var("a"), var("b"), var("c")));
        testExprParser("1+f(a==b?1+2:3+4,!a?1:2,x)");
        testExprParser("a?b:c?d:e", op("?", var("a"), var("b"), op("?", var("c"), var("d"), var("e"))));
        testExprParser("x=~A.B.*{1..10}");
        testExprParser("${m={}}}.duration", scalar("duration", pattern(vardef("m", pattern(chars("}}"))))));
        testExprParser("a ? \"hi\" : \"bye\"", op("?", var("a"), string("hi"), string("bye")));
        testExprParser("a ++ 1", op("++", var("a"), num(1)));

        // examples from spec
        testExprParser("8*bytecount/duration", op("/", op("*", num(8), var("bytecount")), var("duration")));
        testExprParser("sum(pkLoss)");
        testExprParser("module =~ Network.${subnet=*}.**");
        testExprParser("sum(delay*pkCount)/sum(pkCount)");
        testExprParser("pkLoss/pkSent");
        testExprParser("${host=**}.networklayer.udp.pkCount + ${host}.networklayer.tcp.pkCount");
        testExprParser("Network.A.scalar1 * Network.B.scalar2");
        testExprParser("Network.A${i=*}.scalar1 * Network.B${i}.scalar2");
        testExprParser("mean((${m=?}${i=[0,2]}.sentPk - ${m}[${i+1}].recvPk) / ${m}[${i}].sentPk)");
        testExprParser("module =~ ${subnet=?}**");

        testExprParser("*.a+b", op("+", scalar("a", pattern(manychars(false))), var("b")));
        testExprParser("(name =~ a) ? (value>10 ? 1 : 2) : 3");

        // negative test cases
        testExprParser("", "missing expression");
        testExprParser("*x", "left operand of * is missing");
        testExprParser("(a+b))", "unmatched )");
        testExprParser("module.", "missing statistic name after .");
        testExprParser("module =~ ", "missing pattern after =~ operator");
        testExprParser("fun(1,2,)", "missing expression after the comma");
        testExprParser("(", "missing expression after (");
        testExprParser("f()", "missing expression before )");
        testExprParser("!?1:2", "operand of ! operator is missing");
        testExprParser("~|x", "operand of ~ operator is missing");
        testExprParser("5*+3", "right operand of * is missing");
        testExprParser("choose(2 \"zero one two three\")", "missing operator before \"zero one two three\"");

        // old bugs
        testExprParser("module =~ *.r[${index=*}] ? ", "missing expression");

        System.out.format("\nTime of expression parsing test: %sms\n\n", System.currentTimeMillis() - start);
    }

    static void testExprParser(String expr) {
        testExprParser(expr, null);
    }

    static void testExprParser(String expr, Object expected) {
        System.out.println(expr);
        try {
            ExpressionParser parser = new ExpressionParser();
            Expr e = parser.parseExpr(expr);
            ExpressionPrinter.print(e, 2);
            System.out.println("\n");
            if (expected instanceof Expr && !equals(e, (Expr)expected))
                throw new RuntimeException("parse mismatch");
            if (expected instanceof String)
                throw new RuntimeException("missing parse error");
        } catch (ParseException e) {
            if (!(expected instanceof String) || !expected.equals(e.getMessage()))
                System.out.println("  ERROR: " + e.getMessage()+"\n");
            else
                System.out.println("  "+e.getMessage()+"\n");
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage()+"\n");
        }
    }

    static void benchmarkExpressionParser() {
        benchmarkExpressionParser("mean((${m=?}${i=[0,2]}.sentPk - ${m}[${i+1}].recvPk) / ${m}[${i}].sentPk)", null);
    }

    static void benchmarkExpressionParser(String expr, Expr expected) {
        System.out.println(expr);

        try {
            ExpressionParser parser = new ExpressionParser();
            Expr e;
            if (expected != null && !equals(e=parser.parseExpr(expr), expected)) {
                ExpressionPrinter.print(e, 4);
                System.out.println("  ERROR: wrong parse");
                return;
            }

            long start = System.currentTimeMillis();
            for (int i = 0; i < 1000; i++)
                e = parser.parseExpr(expr);
            System.out.format("  Time: %.2fms\n", (System.currentTimeMillis() - start)/1000.0);
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage());
        }
    }

    static void testPatternParser() {
        System.out.println("------------- parsed patterns ----------------");
        testPatternParser("${subnet=*}");
        testPatternParser("${host=**}.networklayer.udp");
        testPatternParser("Network.A");
        testPatternParser("Network.A${i=*}");
        testPatternParser("${m}${i=[0,2]}");
        testPatternParser("${m}[${i+1}]");
        testPatternParser("*.r[*]");
    }

    static void testPatternParser(String pattern) {
        System.out.println(pattern);
        try {
            ExpressionParser parser = new ExpressionParser();
            Pattern p = parser.parsePattern(pattern);
            ExpressionPrinter.print(p, 2);
            System.out.println("");
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage());
        }
    }

    static void testPatternMatcher() {
        System.out.println("------------- pattern matching ----------------");

        // test cases adapted from test/common/patternmatcher_1.test
        testPatternMatcher("a", "a", true);
        testPatternMatcher("alma", "alma", true);
        testPatternMatcher("a", "b", false);
        testPatternMatcher("a", "aa", false);
        testPatternMatcher("aa", "a", false);
        testPatternMatcher("alma", "what?", false);

        testPatternMatcher("a", "A", false);
        testPatternMatcher("A", "a", false);
        testPatternMatcher("A", "A", true);
        testPatternMatcher("a", "", false);

        testPatternMatcher("\\a\\a\\a", "aaa", true);
        testPatternMatcher("a\\%aa", "a%aa", true);
        testPatternMatcher("a\\?\\[a-z\\]\\*x\\*\\*", "a?[a-z]*x**", true);

        testPatternMatcher("a?ma", "alma", true);
        testPatternMatcher("a?ma", "a!ma", true);
        testPatternMatcher("a?ma", "ama", false);
        testPatternMatcher("a?ma", "allma", false);
        testPatternMatcher("?ma", "lma", true);
        testPatternMatcher("?ma", "!ma", true);
        testPatternMatcher("?ma", "ma", false);
        testPatternMatcher("?ma", "llma", false);
        testPatternMatcher("a?", "al", true);
        testPatternMatcher("a?", "a!", true);
        testPatternMatcher("a?", "a", false);
        testPatternMatcher("a?", "all", false);
        testPatternMatcher("?", "x", true);
        testPatternMatcher("?", "\\", true);
        testPatternMatcher("?", "7", true);
        testPatternMatcher("?", "?", true);
        testPatternMatcher("?", "[", true);
        testPatternMatcher("?", "]", true);
        testPatternMatcher("?", ".", true); // differs from common/PatternMatcher
        testPatternMatcher("??aa?b?", "!!aa!b!", true);
        testPatternMatcher("??aa?b?", "!!aa!b", false);

        testPatternMatcher("{c-e}", "c", true);
        testPatternMatcher("{c-e}", "d", true);
        testPatternMatcher("{c-e}", "e", true);
        testPatternMatcher("{c-e}", "-", false);
        testPatternMatcher("{c-e}", "b", false);
        testPatternMatcher("{c-e}", "f", false);
        testPatternMatcher("{c-e5-7}", "c", true);
        testPatternMatcher("{c-e5-7}", "5", true);
        testPatternMatcher("{c-e5-7}", "a", false);
        testPatternMatcher("{c-e5-7}", "4", false);
        testPatternMatcher("{c-e5-7}", "8", false);
        testPatternMatcher("{c-e5-7}", "-", false);
        testPatternMatcher("{ace}", "a", true);
        testPatternMatcher("{ace}", "c", true);
        testPatternMatcher("{ace}", "e", true);
        testPatternMatcher("{ace}", "b", false);
        testPatternMatcher("{-ace}", "-", true);
        testPatternMatcher("{-ace}", "a", true);
        testPatternMatcher("{-ace}", "c", true);
        testPatternMatcher("{-ace}", "b", false);
        testPatternMatcher("{-ace0-59}", "0", true);
        testPatternMatcher("{-ace0-59}", "3", true);
        testPatternMatcher("{-ace0-59}", "9", true);
        testPatternMatcher("{-ace0-59}", "-", true);
        testPatternMatcher("{-ace0-59}", "e", true);
        testPatternMatcher("{-ace0-59}", "b", false);
        testPatternMatcher("{-ace0-59}", "8", false);
        testPatternMatcher("{}ace}", "}", true);
        testPatternMatcher("{}ace}", "a", true);
        testPatternMatcher("{}ace}", "b", false);
        testPatternMatcher("{-*\\?[]{}", "-", true);
        testPatternMatcher("{-*\\?[]{}", "*", true);
        testPatternMatcher("{-*\\?[]{}", "\\", true);
        testPatternMatcher("{-*\\?[]{}", "?", true);
        testPatternMatcher("{-*\\?[]{}", "[", true);
        testPatternMatcher("{-*\\?[]{}", "]", true);
        testPatternMatcher("{-*\\?[]{}", "{", true);
        testPatternMatcher("{-*\\?[]{}", "a", false);
        testPatternMatcher("{-*\\?[]{}", "!", false);

        testPatternMatcher("{^c-e}", "c", false);
        testPatternMatcher("{^c-e}", "d", false);
        testPatternMatcher("{^c-e}", "e", false);
        testPatternMatcher("{^c-e}", "-", true);
        testPatternMatcher("{^c-e}", "b", true);
        testPatternMatcher("{^c-e}", "f", true);
        testPatternMatcher("{^c-e5-7}", "c", false);
        testPatternMatcher("{^c-e5-7}", "5", false);
        testPatternMatcher("{^c-e5-7}", "a", true);
        testPatternMatcher("{^c-e5-7}", "4", true);
        testPatternMatcher("{^c-e5-7}", "8", true);
        testPatternMatcher("{^c-e5-7}", "-", true);
        testPatternMatcher("{^ace}", "a", false);
        testPatternMatcher("{^ace}", "c", false);
        testPatternMatcher("{^ace}", "e", false);
        testPatternMatcher("{^ace}", "b", true);
        testPatternMatcher("{^-ace}", "-", false);
        testPatternMatcher("{^-ace}", "a", false);
        testPatternMatcher("{^-ace}", "c", false);
        testPatternMatcher("{^-ace}", "b", true);
        testPatternMatcher("{^-ace0-59}", "0", false);
        testPatternMatcher("{^-ace0-59}", "3", false);
        testPatternMatcher("{^-ace0-59}", "9", false);
        testPatternMatcher("{^-ace0-59}", "-", false);
        testPatternMatcher("{^-ace0-59}", "e", false);
        testPatternMatcher("{^-ace0-59}", "b", true);
        testPatternMatcher("{^-ace0-59}", "8", true);
        testPatternMatcher("{^}ace}", "}", false);
        testPatternMatcher("{^}ace}", "a", false);
        testPatternMatcher("{^}ace}", "b", true);
        testPatternMatcher("{^-*\\?[]{}", "-", false);
        testPatternMatcher("{^-*\\?[]{}", "*", false);
        testPatternMatcher("{^-*\\?[]{}", "\\", false);
        testPatternMatcher("{^-*\\?[]{}", "?", false);
        testPatternMatcher("{^-*\\?[]{}", "[", false);
        testPatternMatcher("{^-*\\?[]{}", "]", false);
        testPatternMatcher("{^-*\\?[]{}", "{", false);
        testPatternMatcher("{^-*\\?[]{}", "a", true);
        testPatternMatcher("{^-*\\?[]{}", "!", true);

        testPatternMatcher("*", "", true);
        testPatternMatcher("*", "$#%$%#$", true);
        testPatternMatcher("*", "%^${[12]!\\", true);
        testPatternMatcher("*", ".", false);
        testPatternMatcher("al*ma", "alma", true);
        testPatternMatcher("al*ma", "al!!!ma", true);
        testPatternMatcher("al*ma", "ala", false);
        testPatternMatcher("al*ma", "ama", false);
        testPatternMatcher("*foo", "foo", true);
        testPatternMatcher("*foo", "!!!foo", true);
        testPatternMatcher("*foo", "oo", false);
        testPatternMatcher("foo*", "foo", true);
        testPatternMatcher("foo*", "foo!!!", true);
        testPatternMatcher("foo*", "fo", false);
        testPatternMatcher("al*m*a", "allllllma", true);
        testPatternMatcher("al*m*a", "alllllmla", true);
        testPatternMatcher("al*m*a", "allmllmla", true);
        testPatternMatcher("al*m*a", "almla", true);
        testPatternMatcher("al*m*a", "alma", true);
        testPatternMatcher("al**m*a", "alma", true);

        testPatternMatcher("node[53..157]", "node[53]", true);
        testPatternMatcher("node[53..157]", "node[157]", true);
        testPatternMatcher("node[53..157]", "node[99]", true);
        testPatternMatcher("node[53..157]", "node[6]", false);
        testPatternMatcher("node[53..157]", "node[40]", false);
        testPatternMatcher("node[53..157]", "node[52]", false);
        testPatternMatcher("node[53..157]", "node[158]", false);
        testPatternMatcher("node[53..157]", "node[]", false);
        testPatternMatcher("node[53..157]", "node[ ]", false);
        testPatternMatcher("node[53..]", "node[53]", true);
        testPatternMatcher("node[53..]", "node[99]", true);
        testPatternMatcher("node[53..]", "node[52]", false);
        testPatternMatcher("node[53..]", "node[9]", false);
        testPatternMatcher("node[53..]", "node[]", false);
        testPatternMatcher("node[53..]", "node[ ]", false);
        testPatternMatcher("node[..157]", "node[157]", true);
        testPatternMatcher("node[..157]", "node[15]", true);
        testPatternMatcher("node[..157]", "node[0]", true);
        testPatternMatcher("node[..157]", "node[158]", false);
        testPatternMatcher("node[..157]", "node[11111]", false);
        testPatternMatcher("node[..157]", "node[]", false);
        testPatternMatcher("node[..157]", "node[ ]", false);
        testPatternMatcher("node-{53..157}", "node-53", true);
        testPatternMatcher("node-{53..157}", "node-157", true);
        testPatternMatcher("node-{53..157}", "node-99", true);
        testPatternMatcher("node-{53..157}", "node-6", false);
        testPatternMatcher("node-{53..157}", "node-40", false);
        testPatternMatcher("node-{53..157}", "node-52", false);
        testPatternMatcher("node-{53..157}", "node-158", false);
        testPatternMatcher("node-{53..157}", "node- ", false);
        testPatternMatcher("node-{53..}", "node-53", true);
        testPatternMatcher("node-{53..}", "node-99", true);
        testPatternMatcher("node-{53..}", "node-52", false);
        testPatternMatcher("node-{53..}", "node-9", false);
        testPatternMatcher("node-{53..}", "node-", false);
        testPatternMatcher("node-{53..}", "node- ", false);
        testPatternMatcher("node-{..157}", "node-157", true);
        testPatternMatcher("node-{..157}", "node-15", true);
        testPatternMatcher("node-{..157}", "node-0", true);
        testPatternMatcher("node-{..157}", "node-158", false);
        testPatternMatcher("node-{..157}", "node-11111", false);
        testPatternMatcher("node-{..157}", "node-", false);
        testPatternMatcher("node-{..157}", "node- ", false);
        testPatternMatcher("node{2..13}[53..157][0..1]{2-3}x", "node2[53][0]3x", true);
        testPatternMatcher("node{2..13}[53..157][0..1]{2-3}x", "node13[157][1]2x", true);
        testPatternMatcher("node{2..13}[53..157][0..1]{2-3}x", "node13[157][1]x", false);
        testPatternMatcher("node{2..13}[53..157][0..1]{2-3}x", "node13[157][3]2x", false);

        // bugs
        testPatternMatcher("*.r[*]", "A.r[0]", true);

        // bindings
        Environment env = new Environment();
        testPatternMatcher("**", "a.b", true, "", env);
        testPatternMatcher("${i=**}", "a.b", true, "i=a.b;", env);
        testPatternMatcher("node${i={2}}", "node2", true, "i=2;", env);
        testPatternMatcher("node${i={2..13}}${j=[53..157]}${k=[0..1]}${l={2-3}}x", "node2[53][0]3x", true, "i=2;j=[53];k=[0];l=3;", env);
    }

    static void testPatternMatcher(String pattern, String string, boolean expected) {
        testPatternMatcher(pattern, string, expected, null, new Environment());
    }

    static void testPatternMatcher(String pattern, String string, boolean expected, String expectedEnv, Environment env)
    {
        try {
            System.out.println(string + " =~ " + pattern);
            ExpressionParser parser = new ExpressionParser();
            Pattern p = parser.parsePattern(pattern);
            PatternMatcher matcher = new PatternMatcher(string);
            Environment newEnv = matcher.fullMatch(p, env);
            boolean result = newEnv != null;
            if (result != expected)
                throw new RuntimeException("match failed");
            if (expectedEnv != null) {
                String newVars = getNewVarsAsString(newEnv, env);
                if (!expectedEnv.equals(newVars))
                    throw new RuntimeException("env failed: "+newVars);
            }
            System.out.println("  OK");
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage());
        }
    }

    static String getNewVarsAsString(Environment env, Environment base) {
        List<String> vars = new ArrayList<String>(env.getNames());
        Set<String> baseVars = base.getNames();
        Collections.sort(vars);

        if(!CollectionUtils.isSubCollection(baseVars, vars))
            throw new RuntimeException("variables removed by match");

        StringBuilder sb = new StringBuilder();
        for (String var : vars) {
            if (!baseVars.contains(var)) {
                sb.append(var).append('=').append(env.lookup(var)).append(";");
            }
        }
        return sb.toString();
    }

    static void testExprEval() {
        System.out.println("------------- expr evaluation ----------------");

        testExprEval("1", 1);
        testExprEval("1+2", 3);
        testExprEval("2*3", 6);
        testExprEval("6/3", 2);
        testExprEval("8%3", 2);
        testExprEval("2^4", 16);
        testExprEval("2|4", 6);
        testExprEval("2&3", 2);
        testExprEval("2#3", 1);
        testExprEval("2<<2",8);
        testExprEval("8>>2",2);
        testExprEval("1==1",true);
        testExprEval("1==0",false);
        testExprEval("1!=1",false);
        testExprEval("1!=0",true);
        testExprEval("1<2",true);
        testExprEval("2<1",false);
        testExprEval("\"ab\" ++ \"cd\"", "abcd");
        testExprEval("\"abcd\" =~ *b*", "abcd");
        testExprEval("\"abcd\" =~ x*", false);

        testExprEval("sin(1)", Math.sin(1));
        testExprEval("cos(1)", Math.cos(1));
        testExprEval("tan(1)", Math.tan(1));
        testExprEval("asin(0.5)", Math.asin(0.5));
        testExprEval("acos(0.5)", Math.acos(0.5));
        testExprEval("atan(1)", Math.atan(1));
        testExprEval("atan2(1,2)", Math.atan2(1,2));
        testExprEval("rad(1)", Math.toRadians(1));
        testExprEval("deg(1)", Math.toDegrees(1));
        testExprEval("exp(1)", Math.exp(1));
        testExprEval("log(2)", Math.log(2));
        testExprEval("log10(10)", Math.log10(10));
        testExprEval("sqrt(2)", Math.sqrt(2));
        testExprEval("cbrt(2)", Math.cbrt(2));
        testExprEval("hypot(1,2)", Math.hypot(1,2));
        testExprEval("sinh(1)", Math.sinh(1));
        testExprEval("cosh(1)", Math.cosh(1));
        testExprEval("tanh(1)", Math.tanh(1));
        testExprEval("rem(5,3)", Math.IEEEremainder(5,3));
        testExprEval("ceil(0.7)", Math.ceil(0.7));
        testExprEval("floor(0.7)", Math.floor(0.7));
        testExprEval("round(3.14)", (int)Math.round(3.14));
        testExprEval("fabs(-2.5)", Math.abs(-2.5));
        testExprEval("signum(-2)", Math.signum(-2));
        testExprEval("min(3,4)", 3);
        testExprEval("max(3,4)", 4);
        testExprEval("select(1, 0,10,20,30)", 10);
        testExprEval("locate(15, 0,10,20,30)", 2);
        testExprEval("locate(0, 10,20,30)", 0);
        testExprEval("locate(40, 10,20,30)", 3);
        testExprEval("length(\"abc\")", 3);
        testExprEval("contains(\"abcde\", \"bc\")", true);
        testExprEval("substring(\"abcde\", 2)", "cde");
        testExprEval("substring(\"abcde\", 2, 2)", "cd");
        testExprEval("substringBefore(\"a,b,c\", \",\")", "a");
        testExprEval("substringAfter(\"a,b,c\", \",\")", "b,c");
        testExprEval("substringBeforeLast(\"a,b,c\", \",\")", "a,b");
        testExprEval("substringAfterLast(\"a,b,c\", \",\")", "c");
        testExprEval("startsWith(\"abc\", \"ab\")", true);
        testExprEval("endsWith(\"abc\", \"bc\")", true);
        testExprEval("tail(\"abc\", 2)", "bc");
        testExprEval("replace(\"abcd\", \"bc\", \"BC\")", "aBCd");
        testExprEval("replace(\"abcdabcd\", \"bc\", \"BC\", 2)", "abcdaBCd");
        testExprEval("replaceFirst(\"abcdabcd\", \"bc\", \"BC\")", "aBCdabcd");
        testExprEval("replaceFirst(\"abcdabcdabcd\", \"bc\", \"BC\", 2)", "abcdaBCdabcd");
        testExprEval("trim(\"  abcd \t \")", "abcd");
        testExprEval("indexOf(\"abcdef\", \"cd\")", 2);
        testExprEval("choose(2, \"zero one two three\")", "two");
        testExprEval("toUpper(\"abcdef0123\")", "ABCDEF0123");
        testExprEval("toLower(\"ABCDEF0123\")", "abcdef0123");

        Environment env = new Environment();
        env.define("a", "42");
        env.define("b", "3");
        env.define("c", "hello");
        testExprEval("a", "42", "", env);
        testExprEval("b", "3", "", env);
        testExprEval("c", "hello", "", env);
        testExprEval("a+b", 45, "", env);
        testExprEval("c =~ hello", true, "", env);
        testExprEval("c =~ ${v=**}", true, "v=hello;", env);

        // old bugs
        env = new Environment();
        env.define("module", "N.r[0]");
        env.define("name", "drops");
        env.define("value", "42");
        testExprEval("name =~ drops && floor( value / 10)", true, "", env);
        testExprEval("(module=~** ) ? 9999: 0", 9999, "", env);
    }

    static void testExprEval(String expr, Object expected) {
        testExprEval(expr, expected, null, new Environment());
    }

    static void testExprEval(String expr, Object expected, String expectedEnv, Environment env) {
        try {
            System.out.println(expr);
            ExpressionParser parser = new ExpressionParser();
            Expr e = parser.parseExpr(expr);
            ExpressionEvaluator evaluator = new ExpressionEvaluator();
            List<Value> values = evaluator.evaluate(e, env);
            if (values == null || values.isEmpty())
                throw new RuntimeException("no value returned");
            if (values.size() > 1)
                throw new RuntimeException("multiple values returned");
            if (!equals(values.get(0), expected))
                throw new RuntimeException("wrong result");
            if (expectedEnv != null) {
                String newVars = getNewVarsAsString(values.get(0).env, env);
                if (!expectedEnv.equals(newVars))
                    throw new RuntimeException("env failed: "+newVars);
            }
            System.out.println("  OK");
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage());
        }
    }

    static void testTemplateEval() {
        System.out.println("------------- template evaluation ----------------");

        testTemplateEval("abd!@#$$%^&*?{}}.[]", "abd!@#$$%^&*?{}}.[]");

        Environment env = new Environment();
        env.define("a", "42");
        env.define("b", "3");
        env.define("c", "hello");
        testTemplateEval("${a}", "42", env);
        testTemplateEval("${b}", "3", env);
        testTemplateEval("${c}", "hello", env);
        testTemplateEval("${a}${b}${c}", "423hello", env);
        testTemplateEval("${a}/${b}/${c}", "42/3/hello", env);
        testTemplateEval("x${a+b}", "x45", env);
    }

    static void testTemplateEval(String template, String expected) {
        testTemplateEval(template, expected, new Environment());
    }

    static void testTemplateEval(String template, String expected, Environment env) {
        try {
            System.out.println(template);
            ExpressionParser parser = new ExpressionParser();
            StringTemplate t = parser.parseTemplate(template);
            ExpressionEvaluator evaluator = new ExpressionEvaluator();
            String value = evaluator.evaluate(t, env);
            if (value == null || !value.equals(expected))
                throw new RuntimeException("wrong result");
            System.out.println("  OK");
        } catch (Exception e) {
            System.out.println("  ERROR: "+e.getMessage());
        }
    }

    static boolean equals(Value value, Object other) {
        if (value == null && other == null)
            return true;
        if (value == null)
            return false;
        if (other instanceof Integer)
            return value.asInt() == (Integer)other;
        if (other instanceof Float)
            return value.asNumber() == (Float)other;
        if (other instanceof Double)
            return value.asNumber() == (Double)other;
        if (other instanceof Boolean)
            return value.asBool() == (Boolean)other;
        if (other instanceof String)
            return other.equals(value.asString());
        return false;
    }

    static boolean equals(Node first, Node second) {
        NodeComparator comparator = new NodeComparator();
        return comparator.compare(first, second);
    }

    // TODO reduce boilerplate
    static class NodeComparator implements Visitor<Boolean, Node> {

        public boolean compare(Node first, Node second) {
            if ((first == null) != (second == null))
                return false;
            return first == null || first.accept(this, second);
        }

        private static boolean equals(Object lhs, Object rhs) {
            return ObjectUtils.equals(lhs, rhs);
        }

        private Boolean equals(Node lhs, Node rhs) {
            if ((lhs == null) != (rhs == null))
                return false;
            return lhs == null || lhs.accept(this, rhs);
        }

        private Boolean equalNodes(List<? extends Node> lhs, List<? extends Node> rhs) {
            if (lhs.size() != rhs.size())
                return false;
            for (int i = 0; i < lhs.size(); ++i)
                if (!equals(lhs.get(i), rhs.get(i)))
                    return false;
            return true;
        }

        private Boolean equalNums(List<? extends Number> lhs, List<? extends Number> rhs) {
            if (lhs.size() != rhs.size())
                return false;
            for (int i = 0; i < lhs.size(); ++i)
                if (!equals(lhs.get(i), rhs.get(i)))
                    return false;
            return true;
        }

        public Boolean visitNumLiteral(NumLiteral lit, Node rhs) {
            return rhs instanceof NumLiteral && equals(lit.value, ((NumLiteral)rhs).value);
        }

        public Boolean visitStatisticRef(StatisticRef ref, Node rhs) {
            return rhs instanceof StatisticRef && equals(ref.name, ((StatisticRef)rhs).name) &&
                    equals(ref.modulePattern, ((StatisticRef)rhs).modulePattern);
        }

        public Boolean visitLikeOp(LikeOp op, Node rhs) {
            return rhs instanceof LikeOp && equals(op.left, ((LikeOp)rhs).left) &&
                    equals(op.right, ((LikeOp)rhs).right);
        }

        public Boolean visitFunctionCall(FunctionCall call, Node rhs) {
            return rhs instanceof FunctionCall && equals(call.name, ((FunctionCall)rhs).name) &&
                    equalNodes(call.args, ((FunctionCall)rhs).args);
        }

        public Boolean visitPattern(Pattern pattern, Node rhs) {
            return rhs instanceof Pattern && equalNodes(pattern.elements, ((Pattern)rhs).elements);
        }

        public Boolean visitLiteralPattern(LiteralPattern e, Node rhs) {
            return rhs instanceof LiteralPattern && equals(e.value, ((LiteralPattern)rhs).value);
        }

        public Boolean visitAnyCharPattern(AnyCharPattern e, Node rhs) {
            return rhs instanceof AnyCharPattern;
        }

        public Boolean visitManyCharsPattern(ManyCharsPattern e, Node rhs) {
            return rhs instanceof ManyCharsPattern && e.acceptDot == ((ManyCharsPattern)rhs).acceptDot;
        }

        public Boolean visitCharacterSetPattern(CharacterSetPattern e, Node rhs) {
            return rhs instanceof CharacterSetPattern && equals(e.chars, ((CharacterSetPattern)rhs).chars) &&
                    e.negate == ((CharacterSetPattern)rhs).negate;
        }

        public Boolean visitNumericRangePattern(NumericRangePattern e, Node rhs) {
            return rhs instanceof NumericRangePattern && equalNums(e.values, ((NumericRangePattern)rhs).values);
        }

        public Boolean visitVariableDefPattern(VariableDefPattern e, Node rhs) {
            return rhs instanceof VariableDefPattern && equals(e.name, ((VariableDefPattern)rhs).name) &&
                    equals(e.pattern, ((VariableDefPattern)rhs).pattern);
        }

        public Boolean visitExprPattern(ExprPattern e, Node rhs) {
            return rhs instanceof ExprPattern && equals(e.expr, ((ExprPattern)rhs).expr);
        }

        public Boolean visitVariableRef(VariableRef e, Node rhs) {
            return rhs instanceof VariableRef && equals(e.name, ((VariableRef)rhs).name);
        }

        public Boolean visitStringTemplate(StringTemplate t, Node rhs) {
            return rhs instanceof StringTemplate && equalNodes(t.elements, ((StringTemplate)rhs).elements);
        }

        public Boolean visitStringLiteral(StringLiteral e, Node rhs) {
            return rhs instanceof StringLiteral && equals(e.value, ((StringLiteral)rhs).value);
        }

        public Boolean visitSplicedExpr(SplicedExpr e, Node rhs) {
            return rhs instanceof SplicedExpr && equals(e.expr, ((SplicedExpr)rhs).expr);
        }
    }
}
