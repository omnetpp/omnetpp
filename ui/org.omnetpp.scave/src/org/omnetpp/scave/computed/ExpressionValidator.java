package org.omnetpp.scave.computed;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.scave.computed.ExpressionEvaluator.Function;
import org.omnetpp.scave.computed.ExpressionParser.TokenType;
import org.omnetpp.scave.computed.ExpressionSyntax.Expr;
import org.omnetpp.scave.computed.ExpressionSyntax.ExprPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.LikeOp;
import org.omnetpp.scave.computed.ExpressionSyntax.Node;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.PatternElement;
import org.omnetpp.scave.computed.ExpressionSyntax.SplicedExpr;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.StringTemplate;
import org.omnetpp.scave.computed.ExpressionSyntax.TemplateElement;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;
import org.omnetpp.scave.computed.ExpressionVisitors.VisitorBase;

/**
 * This class validates an expression.
 *
 * The purpose of the validation is to decrease the chance
 * of errors during evaluation. The validation errors can
 * be presented to the user when he enters the expression.
 * The validation should mimic the semantics of the evaluation,
 * but it should be performed without knowing the values of input dataset,
 * and must be much faster.
 *
 * @see ExpressionEvaluator
 * @author tomi
 */
class ExpressionValidator {

    /**
     * Validates an expression.
     * This method checks variable references, and function calls.
     * Statistic references are also checked, but unknown statistic
     * names only generates a warning.
     * The method also collects the variables that are defined in the expression.
     *
     * @param expr the expression to be validated
     * @param vars predefined variables available when evaluating the expression
     * @param statistics recognized statistic names
     * @return an object containing the defined variables, and optionally an error message
     */
    Context validate(Expr expr, Set<String> vars, Set<String> statistics) {
        Assert.isNotNull(vars);
        Assert.isNotNull(statistics);
        Validator validator = new Validator();
        return validator.validate(expr, new Context(vars, statistics));
    }

    /**
     * Validates each spliced expression of the given {@code template}.
     * It also collects the variables that are defined after evaluating the template.
     *
     * @param template the template to be validated
     * @param vars predefined variables available when evaluating the expression
     * @return an object containing the defined variables, and optionally an error message
     */
    Context validate(StringTemplate template, Set<String> vars) {
        Assert.isNotNull(vars);
        Validator validator = new Validator();
        return validator.validate(template, new Context(vars));
    }

    static class Context implements Cloneable {
        Set<String> vars; // defined variables
        Set<String> statistics; // available statistics
        String error;   // first error; validation stops at first error
        String warning; // first warning
        Context(Set<String> vars) { this.vars = vars; this.statistics = Collections.emptySet(); }
        Context(Set<String> vars, Set<String> statistics) { this.vars = vars; this.statistics = statistics; }
        public Context clone() { try { return (Context)super.clone(); } catch (CloneNotSupportedException e) { return null; }}
        Context define(String var) { Context c = this.clone(); c.vars = new HashSet<String>(vars); c.vars.add(var); return c; }
        Context error(String error) { this.error = error; return this; }
        Context warn(String warning) { if (this.warning == null) this.warning = warning; return this; }
    }

    static class Validator extends VisitorBase<Context, Context> {

        Context validate(Node node, Context context) {
            if (context.error != null)
                return context;
            Context ctx = node.accept(this, context);
            return ctx == null ? context : ctx;
        }

        @Override
        public Context visitFunctionCall(FunctionCall call, Context context) {
            if (!isKnownOperator(call.name)) {
                if (!isKnownFunction(call.name))
                    return context.error("unknown function: " + call.name);
                Function function = Function.getFunction(call.name, call.args.size());
                if (function == null)
                    return context.error("wrong number of arguments in call of " + call.name);
            }
            for (Expr arg : call.args)
                context = validate(arg, context);
            return context;
        }

        @Override
        public Context visitLikeOp(LikeOp op, Context context) {
            context = validate(op.left, context);
            context = validate(op.right, context);
            return context;
        }

        @Override
        public Context visitStatisticRef(StatisticRef ref, Context context) {
            if (!context.statistics.contains(ref.name))
                context.warn("missing statistic: " + ref.name);
            if (ref.modulePattern != null)
                return validate(ref.modulePattern, context);
            else
                return context;
        }

        @Override
        public Context visitVariableRef(VariableRef e, Context context) {
            if (!context.vars.contains(e.name) && !context.statistics.contains(e.name))
                return context.warn("undefined variable: " + e.name);
            return context;
        }

        @Override
        public Context visitPattern(Pattern pattern, Context context) {
            for (PatternElement e : pattern.elements)
                context = validate(e, context);
            return context;
        }

        @Override
        public Context visitVariableDefPattern(VariableDefPattern e, Context context) {
            Context ctx = validate(e.pattern, context);
            return (ctx.error != null ? ctx : ctx.define(e.name));
        }

        @Override
        public Context visitExprPattern(ExprPattern e, Context context) {
            return validate(e.expr, context);
        }

        @Override
        public Context visitStringTemplate(StringTemplate t, Context context) {
            for (TemplateElement e : t.elements)
                context = validate(e, context);
            return context;
        }

        @Override
        public Context visitSplicedExpr(SplicedExpr e, Context context) {
            return validate(e.expr, context);
        }

        // set of known function names
        private static final Set<String> functions;
        static {
            functions = new HashSet<String>();
            for (Function fun : Function.class.getEnumConstants())
                functions.add(fun.getName());
        }

        private static boolean isKnownOperator(String op) {
            TokenType tokenType = TokenType.getNamedTokenByPrefix(op);
            return tokenType != null && op.equals(tokenType.getName());
        }

        private static boolean isKnownFunction(String fun) {
            return functions.contains(fun);
        }
    }
}
