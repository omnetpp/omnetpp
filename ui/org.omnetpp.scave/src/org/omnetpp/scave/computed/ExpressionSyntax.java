package org.omnetpp.scave.computed;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Syntax nodes for expressions, patterns, and string templates.
 * <p>
 * Expressions are used in the 'value' and 'grouping' fields of
 * the ComputeScalars node. They are composed of numeric constants,
 * statistic references, variable references, function applications,
 * and the pattern matching operator. They might contain embedded
 * patterns.
 * <p>
 * Patterns can be used as the module names of statistic references,
 * or as the right-hand-side of pattern matching operator. They are
 * composed of literal strings, wildcards (?, *, and **), enumerated
 * character sets, numeric ranges, variable bindings,
 * and expressions. Expressions are evaluated and their value matched
 * literally.
 * <p>
 * String templates are used in the 'target module' field of the
 * ComputeScalars node. They are composed of literal strings,
 * and expressions. The values of the expressions are spliced
 * into the result string.
 *
 * @author tomi
 */
class ExpressionSyntax {

    interface Node
    {
        abstract <T,C> T accept(Visitor<T,C> v, C context);
    }

    /*-----------------------------------------------------------------------
     *                           Expression
     *-----------------------------------------------------------------------*/

    static abstract class Expr implements Node
    {
    }

    static class NumLiteral extends Expr
    {
        double value;

        NumLiteral(double value) { this.value = value; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitNumLiteral(this, context); }
    }

    static class StringLiteral extends Expr implements TemplateElement {
        String value;

        StringLiteral(String value) { this.value = value; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitStringLiteral(this, context); }
    }

    static class StatisticRef extends Expr
    {
        String name;
        Pattern modulePattern;

        StatisticRef(String name, Pattern module) { this.name = name; this.modulePattern = module; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitStatisticRef(this, context); }
    }

    static class VariableRef extends Expr
    {
        String name;

        VariableRef(String name) { this.name = name; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitVariableRef(this, context); }
    }

    static class FunctionCall extends Expr
    {
        String name;
        List<Expr> args;

        FunctionCall(String name, List<Expr> args) { this.name = name; this.args = args; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitFunctionCall(this, context); }
    }

    static class LikeOp extends Expr {
        Expr left;
        Pattern right;

        LikeOp(Expr left, Pattern right) { this.left = left; this.right = right; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitLikeOp(this, context); }
    }

    /*-----------------------------------------------------------------------
     *                              Pattern
     *-----------------------------------------------------------------------*/

    static class Pattern implements Node
    {
        List<PatternElement> elements;

        Pattern(List<PatternElement> elements) { this.elements = elements; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitPattern(this, context); }
    }

    static abstract class PatternElement implements Node
    {
    }

    static class LiteralPattern extends PatternElement
    {
        String value;

        LiteralPattern(String value) { this.value = value; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitLiteralPattern(this, context); }
    }

    static class AnyCharPattern extends PatternElement
    {
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitAnyCharPattern(this, context); }
    }

    static class ManyCharsPattern extends PatternElement
    {
        boolean acceptDot;

        ManyCharsPattern(boolean acceptDot) { this.acceptDot = acceptDot; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitManyCharsPattern(this, context); }
    }

    static class CharacterSetPattern extends PatternElement
    {
        String chars; // char ranges, starts are at even, ends at odd positions
        boolean negate;

        CharacterSetPattern(String chars, boolean negate ) { this.chars = chars; this.negate = negate; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitCharacterSetPattern(this, context); }
    }

    static class NumericRangePattern extends PatternElement
    {
        List<Integer> values; // numeric ranges, starts are at even, ends at odd positions (-1 = no limit)

        NumericRangePattern(List<Integer> values) { this.values = values; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitNumericRangePattern(this, context); }
    }

    static class VariableDefPattern extends PatternElement
    {
        String name;
        Pattern pattern;

        VariableDefPattern(String name, Pattern pattern) { this.name = name; this.pattern = pattern; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitVariableDefPattern(this, context); }
    }

    static class ExprPattern extends PatternElement
    {
        Expr expr;

        ExprPattern(Expr expr) { this.expr = expr; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitExprPattern(this, context); }
    }

    /*-----------------------------------------------------------------------
     *         String templates
     *-----------------------------------------------------------------------*/
    static class StringTemplate implements Node {
        List<TemplateElement> elements;

        StringTemplate(List<TemplateElement> elements) { this.elements = elements; }

        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitStringTemplate(this, context); }
    }

    interface TemplateElement extends Node{
    }

    static class SplicedExpr implements TemplateElement {
        Expr expr;

        SplicedExpr(Expr expr) { this.expr = expr; }
        public <T,C> T accept(Visitor<T,C> v, C context) { return v.visitSplicedExpr(this, context); }
    }

    static interface Visitor<T,C>
    {
        T visitNumLiteral(NumLiteral lit, C context);
        T visitStringLiteral(StringLiteral e, C context);
        T visitStatisticRef(StatisticRef ref, C context);
        T visitVariableRef(VariableRef e, C context);
        T visitFunctionCall(FunctionCall call, C context);
        T visitLikeOp(LikeOp op, C context);

        T visitPattern(Pattern pattern, C context);
        T visitLiteralPattern(LiteralPattern e, C context);
        T visitAnyCharPattern(AnyCharPattern e, C context);
        T visitManyCharsPattern(ManyCharsPattern e, C context);
        T visitCharacterSetPattern(CharacterSetPattern e, C context);
        T visitNumericRangePattern(NumericRangePattern e, C context);
        T visitVariableDefPattern(VariableDefPattern e, C context);
        T visitExprPattern(ExprPattern e, C context);

        T visitStringTemplate(StringTemplate t, C context);
        T visitSplicedExpr(SplicedExpr e, C context);
    }

    // list of Expr with handy constructors for operators
    static class ArgList extends ArrayList<Expr> {
        private static final long serialVersionUID = 1L;

        ArgList() {}
        ArgList(Expr expr) { super(1); add(expr); }
        ArgList(Expr left, Expr right) { super(2); add(left); add(right); }
        ArgList(Expr first, Expr second, Expr third) { super(3); add(first); add(second); add(third); }
    }

    // helper methods for creating syntax trees
    static Expr num(double value) { return new NumLiteral(value); }
    static Expr string(String value) { return new StringLiteral(value); }
    static Expr var(String name) { return new VariableRef(name); }
    static Expr scalar(String name) { return new StatisticRef(name, null); }
    static Expr scalar(String name, Pattern module) { return new StatisticRef(name, module); }
    static Expr op(String op, Expr opnd) { return new FunctionCall(op, new ArgList(opnd)); }
    static Expr op(String op, Expr left, Expr right) { return new FunctionCall(op, new ArgList(left, right)); }
    static Expr op(String op, Expr first, Expr second, Expr third) { return new FunctionCall(op, new ArgList(first, second, third)); }
    static Expr call(String fun, Expr... args) { return new FunctionCall(fun, Arrays.asList(args)); }
    static Pattern pattern(PatternElement... elements) { return new Pattern(Arrays.asList(elements)); }
    static ExprPattern pexpr(Expr expr) { return new ExprPattern(expr); }
    static VariableDefPattern vardef(String name, Pattern pattern) { return new VariableDefPattern(name, pattern); }
    static VariableRef varref(String name) { return new VariableRef(name); }
    static ManyCharsPattern manychars(boolean acceptDot) { return new ManyCharsPattern(acceptDot); }
    static CharacterSetPattern chars(String value) { return new CharacterSetPattern(value, false); }
    static StringTemplate template(TemplateElement... elements) { return new StringTemplate(Arrays.asList(elements)); }
    static SplicedExpr spliced(Expr expr) { return new SplicedExpr(expr); }
}
