package org.omnetpp.scave.computed;

import org.omnetpp.scave.computed.ExpressionSyntax.AnyCharPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.CharacterSetPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.NumericRangePattern;
import org.omnetpp.scave.computed.ExpressionSyntax.Expr;
import org.omnetpp.scave.computed.ExpressionSyntax.ExprPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.LikeOp;
import org.omnetpp.scave.computed.ExpressionSyntax.LiteralPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.ManyCharsPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.NumLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.PatternElement;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.SplicedExpr;
import org.omnetpp.scave.computed.ExpressionSyntax.StringLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.StringTemplate;
import org.omnetpp.scave.computed.ExpressionSyntax.TemplateElement;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;
import org.omnetpp.scave.computed.ExpressionSyntax.Visitor;

class ExpressionVisitors {

    static class VisitorBase<T,C> implements Visitor<T,C> {
        public T visitNumLiteral(NumLiteral lit, C context) { return null; }
        public T visitStringLiteral(StringLiteral e, C context) { return null; }
        public T visitStatisticRef(StatisticRef ref, C context) { return null; }
        public T visitLikeOp(LikeOp op, C context) { return null; }
        public T visitFunctionCall(FunctionCall call, C context) { return null; }
        public T visitPattern(Pattern pattern, C context) { return null; }
        public T visitLiteralPattern(LiteralPattern e, C context) { return null; }
        public T visitAnyCharPattern(AnyCharPattern e, C context) { return null; }
        public T visitManyCharsPattern(ManyCharsPattern e, C context) { return null; }
        public T visitCharacterSetPattern(CharacterSetPattern e, C context) { return null; }
        public T visitNumericRangePattern(NumericRangePattern e, C context) { return null; }
        public T visitVariableDefPattern(VariableDefPattern e, C context) { return null; }
        public T visitExprPattern(ExprPattern e, C context) { return null; }
        public T visitVariableRef(VariableRef e, C context) { return null; }
        public T visitStringTemplate(StringTemplate t, C context) { return null; }
        public T visitSplicedExpr(SplicedExpr e, C context) { return null; }
    }

    static class SimpleVisitor<T> implements Visitor<T,Object>
    {
        protected T visitNumLiteral(NumLiteral lit) { return null; }
        protected T visitStringLiteral(StringLiteral e) { return null; }
        protected T visitStatisticRef(StatisticRef ref) { return null; }
        protected T visitLikeOp(LikeOp op) { return null; }
        protected T visitFunctionCall(FunctionCall call) { return null; }
        protected T visitPattern(Pattern pattern) { return null; }
        protected T visitLiteralPattern(LiteralPattern e) { return null; }
        protected T visitAnyCharPattern(AnyCharPattern e) { return null; }
        protected T visitManyCharsPattern(ManyCharsPattern e) { return null; }
        protected T visitCharacterSetPattern(CharacterSetPattern e) { return null; }
        protected T visitNumericRangePattern(NumericRangePattern e) { return null; }
        protected T visitVariableDefPattern(VariableDefPattern e) { return null; }
        protected T visitExprPattern(ExprPattern e) { return null; }
        protected T visitVariableRef(VariableRef e) { return null; }
        protected T visitStringTemplate(StringTemplate t) { return null; }
        protected T visitSplicedExpr(SplicedExpr e) { return null; }

        public T visitNumLiteral(NumLiteral lit, Object context) { return visitNumLiteral(lit); }
        public T visitStringLiteral(StringLiteral e, Object context) { return visitStringLiteral(e); }
        public T visitStatisticRef(StatisticRef ref, Object context) { return visitStatisticRef(ref); }
        public T visitLikeOp(LikeOp op, Object context) { return visitLikeOp(op); }
        public T visitFunctionCall(FunctionCall call, Object context) { return visitFunctionCall(call); }
        public T visitPattern(Pattern pattern, Object context) { return visitPattern(pattern); }
        public T visitLiteralPattern(LiteralPattern e, Object context) { return visitLiteralPattern(e); }
        public T visitAnyCharPattern(AnyCharPattern e, Object context) { return visitAnyCharPattern(e); }
        public T visitManyCharsPattern(ManyCharsPattern e, Object context) { return visitManyCharsPattern(e); }
        public T visitCharacterSetPattern(CharacterSetPattern e, Object context) { return visitCharacterSetPattern(e); }
        public T visitNumericRangePattern(NumericRangePattern e, Object context) { return visitNumericRangePattern(e); }
        public T visitVariableDefPattern(VariableDefPattern e, Object context) { return visitVariableDefPattern(e); }
        public T visitExprPattern(ExprPattern e, Object context) { return visitExprPattern(e); }
        public T visitVariableRef(VariableRef e, Object context) { return visitVariableRef(e); }
        public T visitStringTemplate(StringTemplate t, Object context) { return visitStringTemplate(t); }
        public T visitSplicedExpr(SplicedExpr e, Object context) { return visitSplicedExpr(e); }
    }

    static class SimpleNodeIterator<T> extends SimpleVisitor<T>
    {
        protected T visitStatisticRef(StatisticRef ref) {
            if (ref.modulePattern != null)
                ref.modulePattern.accept(this, null);
            return null;
        }

        protected T visitLikeOp(LikeOp op) {
            op.left.accept(this, null);
            op.right.accept(this, null);
            return null;
        }

        protected T visitFunctionCall(FunctionCall call) {
            for (Expr arg : call.args)
                arg.accept(this, null);
            return null;
        }

        protected T visitPattern(Pattern pattern) {
            for (PatternElement element : pattern.elements)
                element.accept(this, null);
            return null;
        }

        protected T visitVariableDefPattern(VariableDefPattern e) {
            e.pattern.accept(this, null);
            return null;
        }

        protected T visitExprPattern(ExprPattern e) {
            e.expr.accept(this, null);
            return null;
        }

        protected T visitStringTemplate(StringTemplate template) {
            for (TemplateElement element : template.elements)
                element.accept(this, null);
            return null;
        }
    }
}
