package org.omnetpp.scave.computed;

import java.util.List;

import org.omnetpp.scave.computed.ExpressionSyntax.AnyCharPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.CharacterSetPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.NumericRangePattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ExprPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.LikeOp;
import org.omnetpp.scave.computed.ExpressionSyntax.LiteralPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ManyCharsPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Node;
import org.omnetpp.scave.computed.ExpressionSyntax.NumLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.StringLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;

/**
 * Utility class to pretty-print an expression.
 * Used only for debugging.
 *
 * @author tomi
 */
class ExpressionPrinter extends ExpressionVisitors.SimpleVisitor<ExpressionPrinter> {
    private int level = 0;

    static void print(Node node, int indentLevel) {
        ExpressionPrinter printer = new ExpressionPrinter();
        printer.level = indentLevel;
        node.accept(printer, null);
    }

    ExpressionPrinter indent() {
        for (int i = 0; i < level; ++i)
            System.out.print(' ');
        return this;
    }

    ExpressionPrinter in() {
        level += 2;
        return this;
    }

    ExpressionPrinter out() {
        level -= 2;
        return this;
    }

    ExpressionPrinter print(Object o) {
        System.out.print(o);
        return this;
    }

    ExpressionPrinter list(List<?> l) {
        for (int i = 0; i < l.size(); ++i) {
            if (i > 0)
                print(",");
            print(l.get(i));
        }
        return this;
    }

    ExpressionPrinter format(String format, Object... args) {
        System.out.format(format, args);
        return this;
    }

    ExpressionPrinter node(Node node) {
        return node.accept(this, null);
    }

    ExpressionPrinter nodes(List<? extends Node> nodes) {
        for (int i = 0; i < nodes.size(); ++i) {
            node(nodes.get(i));
            if (i != nodes.size()-1)
                print(",\n");
        }
        return this;
    }

    protected ExpressionPrinter visitNumLiteral(NumLiteral lit) {
        return indent().format("NumLiteral(%f)", lit.value);
    }

    protected ExpressionPrinter visitStringLiteral(StringLiteral lit) {
        return indent().format("StringLiteral(%s)", lit.value);
    }

    protected ExpressionPrinter visitStatisticRef(StatisticRef ref) {
        indent();
        if (ref.modulePattern == null)
            format("StatisticRef(%s)", ref.name);
        else {
            format("StatisticRef(%s,\n", ref.name).in().node(ref.modulePattern).out().print(")");
        }
        return this;
    }

    protected ExpressionPrinter visitLikeOp(LikeOp op) {
        return indent().format("LikeOp(\n").in().
               node(op.left).print(",\n").node(op.right).out().print(")");
    }

    protected ExpressionPrinter visitFunctionCall(FunctionCall call) {
        return indent().format("FunctionCall(%s,\n", call.name).in().nodes(call.args).out().print(")");
    }

    protected ExpressionPrinter visitPattern(Pattern pattern) {
        return indent().print("Pattern(\n").in().nodes(pattern.elements).out().print(")");
    }

    protected ExpressionPrinter visitLiteralPattern(LiteralPattern e) {
        return indent().format("LiteralPattern(%s)", e.value);
    }

    protected ExpressionPrinter visitAnyCharPattern(AnyCharPattern e) {
        return indent().print("AnyCharPattern()");
    }

    protected ExpressionPrinter visitManyCharsPattern(ManyCharsPattern e) {
        return indent().format("ManyCharsPattern(%s)", e.acceptDot);
    }

    protected ExpressionPrinter visitCharacterSetPattern(CharacterSetPattern e) {
        return indent().format("EnumeratedCharsPattern(%s)", e.chars);
    }

    protected ExpressionPrinter visitNumericRangePattern(NumericRangePattern e) {
        return indent().print("EnumeratedNumsPattern(").list(e.values).print(")");
    }

    protected ExpressionPrinter visitVariableDefPattern(VariableDefPattern e) {
        return indent().format("VariableDef(%s,\n", e.name).in().
               node(e.pattern).out().print(")");
    }

    protected ExpressionPrinter visitExprPattern(ExprPattern e) {
        return indent().print("ExprPattern(\n").in().
               node(e.expr).out().print(")");
    }

    protected ExpressionPrinter visitVariableRef(VariableRef e) {
        return indent().format("VariableRef(%s)", e.name);
    }
}