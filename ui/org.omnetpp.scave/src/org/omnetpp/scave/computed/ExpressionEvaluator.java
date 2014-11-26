package org.omnetpp.scave.computed;

import static org.omnetpp.common.util.Pair.pair;
import static org.omnetpp.common.util.StringUtils.choose;
import static org.omnetpp.common.util.StringUtils.replace;
import static org.omnetpp.common.util.StringUtils.replaceFirst;
import static org.omnetpp.common.util.StringUtils.substring;
import static org.omnetpp.common.util.StringUtils.tail;
import static org.omnetpp.common.util.StringUtils.substringAfter;
import static org.omnetpp.common.util.StringUtils.substringAfterLast;
import static org.omnetpp.common.util.StringUtils.substringBefore;
import static org.omnetpp.common.util.StringUtils.substringBeforeLast;
import static org.omnetpp.common.util.StringUtils.replaceOnce;
import static org.omnetpp.scave.computed.ExpressionSyntax.manychars;
import static org.omnetpp.scave.computed.ExpressionSyntax.pattern;
import static org.omnetpp.scave.computed.ExpressionSyntax.pexpr;
import static org.omnetpp.scave.computed.ExpressionSyntax.vardef;
import static org.omnetpp.scave.computed.ExpressionSyntax.varref;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.eclipse.core.runtime.Assert;
import org.omnetpp.common.util.Pair;
import org.omnetpp.scave.computed.ExpressionSyntax.Expr;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.LikeOp;
import org.omnetpp.scave.computed.ExpressionSyntax.NumLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.Pattern;
import org.omnetpp.scave.computed.ExpressionSyntax.SplicedExpr;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.StringLiteral;
import org.omnetpp.scave.computed.ExpressionSyntax.StringTemplate;
import org.omnetpp.scave.computed.ExpressionSyntax.TemplateElement;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;
import org.omnetpp.scave.computed.ExpressionVisitors.SimpleVisitor;
import org.omnetpp.scave.computed.ExpressionVisitors.VisitorBase;
import org.omnetpp.scave.engine.HistogramResult;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.VectorResult;

/**
 * Class to evaluate expressions and string templates.
 *
 * @author tomi
 */
public class ExpressionEvaluator {

    /**
     * The expression thrown by the evaluator.
     */
    static class EvaluationException extends RuntimeException {
        private static final long serialVersionUID = 1L;

        public EvaluationException(String message) {
            super(message);
        }
    }

    /**
     * The environment maps variable names to their values.
     */
    static class Environment implements Cloneable {
        private HashMap<String,String> vars;

        public Environment() {
            vars = new HashMap<String, String>();
        }

        public boolean define(String name, String value) {
            String old = vars.put(name, value);
            return old != null;
        }

        public String lookup(String name) {
            return vars.get(name);
        }

        public Set<String> getNames() {
            return vars.keySet();
        }

        @SuppressWarnings("unchecked")
        public Environment clone() {
            try {
                Environment env = (Environment)super.clone();
                env.vars = (HashMap<String,String>)this.vars.clone(); // keys and values are not cloned
                return env;
            } catch (CloneNotSupportedException e) {
                Assert.isTrue(false);
                return null;
            }
        }
    }

    /**
     * A value is the result of the evaluation.
     * <p>
     * It is the union of string/integer/double/boolean/ID types.
     * <p>
     * There are implicit conversions between string, numeric, and boolean types.
     * <p>
     * The evaluation may change the environment as a side effect, so
     * there is an associated environment in each value. E.g. evaluating
     * "${m=**}.a" may yield the value 3 and the environment "m=module23".
     */
    static class Value {
        private String str;         // string value
        private double num;         // integer, double, or boolean value (false=0.0, true=anything else)
        private boolean isNumValid; // is the num field filled in?
        private long id = -1;       // ID of a vector/histogram

        Environment env;    // the final environment

        Value(String str, Environment env) { this.str = str; isNumValid = false; this.env = env; }
        Value(double num, Environment env) { this.num = num; isNumValid = true; this.env = env; }
        Value(boolean b, Environment env) { this.num = (b ? 1.0 : 0.0); isNumValid = true; this.env = env; }
        Value(long id, Environment env) { this.id = id; isNumValid = false; this.env = env; }

        double asNumber() {
            if (id != -1)
                throw new EvaluationException("scalar expected");
            if (isNumValid)
                return num;
            try {
                num = Double.parseDouble(str);
                isNumValid = true;
                return num;
            } catch (NumberFormatException e) {
                return Double.NaN;
            }
        }

        String asString() {
            if (str == null) {
                if (isNumValid) {
                    if (Math.rint(num) == num)
                        str = String.valueOf(Math.round(num));
                    else
                        str = String.valueOf(num);
                }
                else if (id != -1)
                    throw new EvaluationException("string or number expected");
            }
            return str;
        }

        int asInt() {
            return (int)asNumber();
        }

        long asID() {
            if (id == -1)
                throw new EvaluationException("statistic expected");
            return id;
        }

        boolean asBool() {
            return asNumber() != 0.0;
        }
    }

    // methods for creating/accessing values
    static Value number(double val, Environment env) { return new Value(val,env); }
    static Value string(String val, Environment env) { return new Value(val, env); }
    static Value bool(boolean val, Environment env) { return new Value(val, env); }
    static Value id(long id, Environment env) { return new Value(id, env); }
    static List<Value> list(Value... vals) { return Arrays.asList(vals); }
    static Value single(List<Value> values) { Assert.isTrue(values.size()==1); return values.get(0); }

    private final ResultFileManager manager;
    private final IDList input;

    /**
     * Creates an evaluator for expressions that do not contain
     * statistic references.
     */
    ExpressionEvaluator() {
        this.manager = null;
        this.input = null;
    }

    /**
     * Creates an evaluator for expressions that might contain
     * statistic references. The references are resolved to items
     * in the specified {@code input} list.
     */
    ExpressionEvaluator(ResultFileManager manager, IDList input) {
        this.manager = manager;
        this.input = input;
    }

    /**
     * Evaluates the expression to a single value.
     * Use this method when the expression does not
     * references to statistics.
     */
    Value evaluateSimple(Expr expr, Environment env) {
        List<Value> values = evaluate(expr, env);
        Assert.isTrue(values.size() == 1);
        return values.get(0);
    }

    /**
     * Evaluates the expression in the given environment.
     * The result is multiple value in general, because
     * the statistic names might refer to multiple statistics
     * (e.g. with different module names).
     */
    List<Value> evaluate(Expr expr, Environment env) {
        ExprEvaluator visitor = new ExprEvaluator();
        return visitor.eval(expr, env);
    }

    /**
     * Evaluates a string template in the given environment.
     */
    String evaluate(StringTemplate template, Environment env) {
        StringTemplateEvaluator visitor = new StringTemplateEvaluator(env);
        return template.accept(visitor, null);
    }

    /**
     * Utility class to evaluate an expression.
     */
    private class ExprEvaluator extends VisitorBase<List<Value>,Environment> {

        /**
         * Evaluates {@code expr} in the specified environment.
         */
        List<Value> eval(Expr expr, Environment env) {
            return expr.accept(this, env);
        }

        /**
         * Matches {@code str} with {@code pattern} in the specified environment.
         * Returns {@code str} if the match successful, {@code false} otherwise.
         * The environment is not modified when the match fails.
         */
        Value match(String str, Pattern pattern, Environment env) {
            PatternMatcher matcher = new PatternMatcher(str);
            Environment newEnv = matcher.fullMatch(pattern, env);
            if (newEnv != null) {
                return string(str, newEnv);
            }
            else
                return bool(false, env);
        }

        /**
         * Evaluates a numeric constant.
         */
        public List<Value> visitNumLiteral(NumLiteral lit, Environment env) {
            return list(number(lit.value, env));
        }

        /**
         * Evaluates a string constant.
         */
        public List<Value> visitStringLiteral(StringLiteral lit, Environment env) {
            return list(string(lit.value, env));
        }

        /**
         * Evaluates a statistic (scalar/vector/histogram) reference.
         * <p>
         * The value of a scalar reference is the value of the scalar.
         * The value of a vector/histogram reference is the ID of the vector/histogram.
         * The module part of the reference may add new variables to the environment.
         */
        public List<Value> visitStatisticRef(StatisticRef ref, Environment env) {
            return evaluateStatisticRef(ref.modulePattern, ref.name, env);
        }

        /**
         * Evaluates a variable reference. If the variable is not found,
         * it tries to evaluate it as a statistic reference, because the
         * parser is unable to distinguish variable names from simple
         * statistic references.
         */
        public List<Value> visitVariableRef(VariableRef ref, Environment env) {
            String val = env.lookup(ref.name);
            if (val != null)
                return list(string(val, env));
            if (manager != null && input != null)
                return evaluateStatisticRef(null, ref.name, env);
            throw new EvaluationException("undefined variable: "+ref.name);
        }

        /**
         * Evaluates a statistic (scalar/vector/histogram) reference.
         * <p>
         * The value of a scalar reference is the value of the scalar.
         * The value of a vector/histogram reference is the ID of the vector/histogram.
         * The module part of the reference may add new variables to the environment.
         */
        private List<Value> evaluateStatisticRef(Pattern modulePattern, String name, Environment env) {
            Assert.isNotNull(manager);
            Assert.isNotNull(input);

            if (modulePattern == null) {
                modulePattern = env.lookup("$m") == null ?
                                    pattern(vardef("$m", pattern(manychars(true)))) :
                                    pattern(pexpr(varref("$m")));
            }

            IDList candidates = manager.filterIDList(input, (String)null, (String)null, name);
            List<Value> result = new ArrayList<Value>();
            for (int i = 0; i < candidates.size(); i++) {
                long id = candidates.get(i);
                ResultItem item = manager.getItem(id);
                PatternMatcher matcher = new PatternMatcher(item.getModuleName());
                Environment newEnv = matcher.fullMatch(modulePattern, env);
                if (newEnv != null) {
                    if (ResultFileManager.getTypeOf(id) == ResultFileManager.SCALAR) {
                        ScalarResult scalar = manager.getScalar(id);
                        result.add(number(scalar.getValue(), newEnv));
                    }
                    else
                        result.add(id(id, newEnv));
                }
            }
            return result;
        }

        /**
         * Evaluates a pattern matching expression (&lt;str&gt; =~ &lt;pattern&gt;).
         */
        public List<Value> visitLikeOp(LikeOp op, Environment env) {
            Value str = single(eval(op.left, env));
            Pattern pattern = op.right;
            return list(match(str.asString(), pattern, str.env));
        }

        public List<Value> visitFunctionCall(FunctionCall call, Environment env) {
            if (isAggregateFunction(call.name) && call.args.size() == 1) {
                // this is either aggregate funtion call or field access.
                // disambiguate between them by evaluating the argument and
                // examining its type
                List<Value> arg = eval(call.args.get(0), env);
                if (areScalarValues(arg))
                    return callAggregateFunction(call.name, arg, env);
                else {
                    List<Value> result = new ArrayList<Value>(arg.size());
                    for (Value value : arg)
                        result.add(applyFunction(call.name, list(value)));
                    return result;
                }
            }
            // evaluation of conditional operator is special
            else if ("?".equals(call.name)) {
                Assert.isTrue(call.args.size() == 3);
                return evalConditional(call.args.get(0), call.args.get(1), call.args.get(2), env);
            }
            // normal operator and function calls
            else {
                List<Value> evaledArgs = new ArrayList<Value>(call.args.size());
                for (int i = 0; i < call.args.size(); i++)
                    evaledArgs.add(null);
                List<Value> result = new ArrayList<Value>();
                evaluateArgumentsAndApplyFunction(call.name, call.args, 0, env, evaledArgs, result);
                return result;
            }
        }

        /**
         * Returns true if none of the values are vectors/histograms, false if
         * all of them are vectors/histograms. Throws an exception in the mixed case.
         */
        private boolean areScalarValues(List<Value> vals) {
            boolean allScalar = true;
            boolean someScalar = false;
            for (Value val : vals) {
                if (val.id != -1)
                    allScalar = false;
                else
                    someScalar = true;
            }
            if (someScalar && !allScalar)
                throw new EvaluationException("cannot apply aggregate function to a mix of scalars and vectors/histograms");
            return allScalar;
        }

        /**
         * Evaluates the arguments of the function call, iterates on their values,
         * applies the funtion, and collects the result into the {@code result} list.
         * <p>
         * Arguments are evaluated from left to right. Arguments may depend on
         * variables defined in arguments to their left.
         * <p>
         * If an argument evaluates to multiple values, then the function call is iterated
         * over those values, thus giving multiple results too.
         *
         * @param fun the function to be applied
         * @param argExprs argument expressions of the call
         * @param pos a position of the next argument to be evaluated
         * @param env the environment in which the next argument to be evaluated
         * @param args values of already evaluated arguments
         * @param result a list collecting the values of the whole function call expression
         */
        private void evaluateArgumentsAndApplyFunction(String fun, List<Expr> argExprs, int pos, Environment env,
                                                        List<Value> args, List<Value> result) {
            // if there are no more argument to evaluate,
            // then apply the function to the argument values
            if (pos == argExprs.size()) {
                Value value = applyFunction(fun, args);
                result.add(value);
            }
            // otherwise evaluate the next argument, iterate the values,
            // and evaluate the rest arguments in the environment of the value
            else {
                List<Value> values = eval(argExprs.get(pos), env);
                for (Value value : values) {
                    args.set(pos, value);
                    evaluateArgumentsAndApplyFunction(fun, argExprs, pos+1, value.env, args, result);
                }
            }
        }

        /**
         * Applies a function or operator to its arguments.
         */
        private Value applyFunction(String function, List<Value> args) {
            int numArgs = args.size();
            int len = function.length();
            Assert.isTrue(numArgs >= 1);
            Environment env = args.get(numArgs-1).env;

            try {
            if (len == 1) {
                char ch = function.charAt(0);
                if (numArgs == 1) {
                    Value arg = args.get(0);
                    switch (ch) {
                    case '-': return number(-arg.asNumber(), env);
                    case '~': return number(~arg.asInt(), env);
                    case '!': return bool(!arg.asBool(), env);
                    }
                }
                else if (numArgs == 2) {
                    Value arg0 = args.get(0), arg1 = args.get(1);
                    switch (ch) {
                    case '+': return number(arg0.asNumber() + arg1.asNumber(), env);
                    case '-': return number(arg0.asNumber() - arg1.asNumber(), env);
                    case '*': return number(arg0.asNumber() * arg1.asNumber(), env);
                    case '/': return number(arg0.asNumber() / arg1.asNumber(), env);
                    case '%': return number(arg0.asNumber() % arg1.asNumber(), env);
                    case '^': return number(Math.pow(arg0.asNumber(), arg1.asNumber()), env);

                    case '|': return number(arg0.asInt() | arg1.asInt(), env);
                    case '&': return number(arg0.asInt() & arg1.asInt(), env);
                    case '#': return number(arg0.asInt() ^ arg1.asInt(), env);

                    case '<': return bool(arg0.asNumber() < arg1.asNumber(), env);
                    case '>': return bool(arg0.asNumber() > arg1.asNumber(), env);
                    }
                }
            }
            else if (len == 2) {
                char ch0 = function.charAt(0), ch1 = function.charAt(1);
                if (numArgs == 2) {
                    Value arg0 = args.get(0), arg1 = args.get(1);
                    if (ch0 == '=' && ch1 == '=')
                        return bool(arg0.asNumber() == arg1.asNumber(), env);
                    if (ch0 == '!' && ch1 == '=')
                        return bool(arg0.asNumber() != arg1.asNumber(), env);
                    if (ch0 == '<' && ch1 == '=')
                        return bool(arg0.asNumber() <= arg1.asNumber(), env);
                    if (ch0 == '>' && ch1 == '=')
                        return bool(arg0.asNumber() >= arg1.asNumber(), env);
                    if (ch0 == '<' && ch1 == '<')
                        return number(arg0.asInt() << arg1.asInt(), env);
                    if (ch0 == '>' && ch1 == '>')
                        return number(arg0.asInt() >> arg1.asInt(), env);
                    if (ch0 == '|' && ch1 == '|')
                        return bool(arg0.asBool() || arg1.asBool(), env);
                    if (ch0 == '&' && ch1 == '&')
                        return bool(arg0.asBool() && arg1.asBool(), env);
                    if (ch0 == '+' && ch1 == '+')
                        return string(arg0.asString() + arg1.asString(), env);
                }
            }
            else {
                Function fn = Function.getFunction(function, numArgs);
                if (fn != null) {
                    if (fn.isFieldAccess()) {
                        long id = args.get(0).asID();
                        ResultItem item = manager.getItem(id);
                        Statistics stat;
                        if (item instanceof VectorResult)
                            stat = ((VectorResult)item).getStatistics();
                        else if (item instanceof HistogramResult)
                            stat = ((HistogramResult)item).getStatistics();
                        else
                            throw new EvaluationException(function+"() expects a vector or histogram argument");
                        double v = getField(stat, function);
                        return number(v, env);
                    }
                    else {
                        if (numArgs == 1) {
                            Value arg0 = args.get(0);
                            switch (fn) {
                            case sin: return number(Math.sin(arg0.asNumber()), env);
                            case cos: return number(Math.cos(arg0.asNumber()), env);
                            case tan: return number(Math.tan(arg0.asNumber()), env);
                            case asin: return number(Math.asin(arg0.asNumber()), env);
                            case acos: return number(Math.acos(arg0.asNumber()), env);
                            case atan: return number(Math.atan(arg0.asNumber()), env);
                            case rad: return number(Math.toRadians(arg0.asNumber()), env);
                            case deg: return number(Math.toDegrees(arg0.asNumber()), env);
                            case exp: return number(Math.exp(arg0.asNumber()), env);
                            case log: return number(Math.log(arg0.asNumber()), env);
                            case log10: return number(Math.log10(arg0.asNumber()), env);
                            case sinh: return number(Math.sinh(arg0.asNumber()), env);
                            case cosh: return number(Math.cosh(arg0.asNumber()), env);
                            case tanh: return number(Math.tanh(arg0.asNumber()), env);
                            case sqrt: return number(Math.sqrt(arg0.asNumber()), env);
                            case cbrt: return number(Math.cbrt(arg0.asNumber()), env);
                            case ceil: return number(Math.ceil(arg0.asNumber()), env);
                            case floor: return number(Math.floor(arg0.asNumber()), env);
                            case round: return number(Math.round(arg0.asNumber()), env);
                            case fabs: return number(Math.abs(arg0.asNumber()), env);
                            case signum: return number(Math.signum(arg0.asNumber()), env);
                            case length: return number(arg0.asString().length(), env);
                            case trim: return string(arg0.asString().trim(), env);
                            case toUpper: return string(arg0.asString().toUpperCase(), env);
                            case toLower: return string(arg0.asString().toLowerCase(), env);
                            }
                        }
                        else if (numArgs == 2) {
                            Value arg0 = args.get(0), arg1 = args.get(1);
                            switch(fn) {
                            case atan2: return number(Math.atan2(arg0.asNumber(), arg1.asNumber()), env);
                            case hypot: return number(Math.hypot(arg0.asNumber(), arg1.asNumber()), env);
                            case rem: return number(Math.IEEEremainder(arg0.asNumber(), arg1.asNumber()), env);
                            case min_2: return number(Math.min(arg0.asNumber(), arg1.asNumber()), env);
                            case max_2: return number(Math.max(arg0.asNumber(), arg1.asNumber()), env);
                            case contains: return bool(arg0.asString().contains(arg1.asString()), env);
                            case substring: return string(arg0.asString().substring(arg1.asInt()), env);
                            case startsWith: return bool(arg0.asString().startsWith(arg1.asString()), env);
                            case endsWith: return bool(arg0.asString().endsWith(arg1.asString()), env);
                            case indexOf: return number(arg0.asString().indexOf(arg1.asString()), env);
                            case tail: return string(tail(arg0.asString(), arg1.asInt()), env);
                            case substringBefore: return string(substringBefore(arg0.asString(), arg1.asString()), env);
                            case substringAfter: return string(substringAfter(arg0.asString(), arg1.asString()), env);
                            case substringBeforeLast: return string(substringBeforeLast(arg0.asString(), arg1.asString()), env);
                            case substringAfterLast: return string(substringAfterLast(arg0.asString(), arg1.asString()), env);
                            case choose: return string(choose(arg0.asInt(), arg1.asString()), env);
                            }
                        }
                        else if (numArgs == 3) {
                            Value arg0 = args.get(0), arg1 = args.get(1), arg2 = args.get(2);
                            switch (fn) {
                            case substring_3: return string(substring(arg0.asString(), arg1.asInt(), arg2.asInt()), env);
                            case replace: return string(arg0.asString().replace(arg1.asString(), arg2.asString()), env);
                            case replaceFirst: return string(replaceOnce(arg0.asString(), arg1.asString(), arg2.asString()), env);
                            }
                        }
                        else if (numArgs == 4) {
                            Value arg0 = args.get(0), arg1 = args.get(1), arg2 = args.get(2), arg3 = args.get(3);
                            switch (fn) {
                            case replace_4: return string(replace(arg0.asString(), arg1.asString(), arg2.asString(), arg3.asInt()), env);
                            case replaceFirst_4: return string(replaceFirst(arg0.asString(), arg1.asString(), arg2.asString(), arg3.asInt()), env);
                            }
                        }

                        // varargs
                        if (fn == Function.select && numArgs >= 2) {
                            int index = args.get(0).asInt();
                            if (index < 0)
                                throw new EvaluationException("select(): index is negative");
                            if (index > numArgs-2)
                                throw new EvaluationException("select(): index ("+index+") is too large");
                            return args.get(index+1); // XXX env?
                        }
                        else if (fn == Function.locate) {
                            double val = args.get(0).asNumber();
                            int index;
                            for (index = 0; index < numArgs - 1; ++index) {
                                if (val < args.get(index+1).asNumber())
                                    break;
                            }
                            return number(index, env);
                        }
                    }
                }
            }
            } catch (RuntimeException e) {
                String message = e.getMessage();
                String prefix = function + "():";
                throw new EvaluationException(message.startsWith(prefix) ? message : prefix + " " + message);
            }

            throw new EvaluationException("unrecognized function call: " + function + "()");
        }

        /**
         * Evaluates a conditional expression.
         * <p>
         * First the condition is evaluated, then either the true or false case
         * is evaluated depending the value of the condition.
         * <p>
         * If the condition has multiple values, then the computation iterates on them.
         */
        private List<Value> evalConditional(Expr cond, Expr trueCase, Expr falseCase, Environment env) {
            List<Value> values = eval(cond, env);
            List<Value> result = new ArrayList<Value>(values.size());
            for (Value value : values) {
                if (value.asBool())
                    result.addAll(eval(trueCase, value.env));
                else
                    result.addAll(eval(falseCase, value.env));
            }
            return result;
        }

        /**
         * Returns the value of the named field of the specified Statistics object.
         */
        private double getField(Statistics stat, String field) {
            return "count".equals(field) ? stat.getCount() :
                   "sum".equals(field) ? stat.getSum() :
                   "mean".equals(field) ? stat.getMean() :
                   "min".equals(field) ? stat.getMin() :
                   "max".equals(field) ? stat.getMax() :
                   "stddev".equals(field) ? stat.getStddev() :
                   "variance".equals(field) ? stat.getVariance() :
                   Double.NaN;
        }

        /**
         * Computes the value of an aggregate function on the specified values.
         */
        private List<Value> callAggregateFunction(String fun, List<Value> arg, Environment env) {
            if ("count".equals(fun))
                return list(number(arg.size(), env));

            if (arg.isEmpty())
                return list();

            Statistics stat = new Statistics();
            for (Value value : arg)
                stat.collect(value.asNumber());

            double v = getField(stat, fun);
            return list(number(v, env));
        }
    }

    // flag values for the Function.flags field
    private static final int
        FLAG_IS_FIELD_ACCESS = 1,
        FLAG_IS_AGGREGATING_FUNCTION = 2,
        FLAG_HAS_VARIABLE_ARGS = 4;

    /**
     * Enumerations of functions with their arities and properties.
     */
    public enum Function {
        count(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),
        sum(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),
        mean(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),
        min(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),
        max(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),
        stddev(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),
        variance(1, FLAG_IS_FIELD_ACCESS | FLAG_IS_AGGREGATING_FUNCTION),

        sin(1), cos(1), tan(1),
        asin(1), acos(1), atan(1), atan2(2),
        rad(1), deg(1),

        exp(1), log(1), log10(1),
        sqrt(1), cbrt(1), hypot(2),

        sinh(1), cosh(1), tanh(1),

        rem(2), ceil(1), floor(1), round(1),
        fabs(1), signum(1),
        min_2("min", 2), max_2("max", 2),

        length(1), contains(2),
        substring(2), substring_3("substring", 3),
        substringBefore(2), substringAfter(2), substringBeforeLast(2), substringAfterLast(2),
        startsWith(2), endsWith(2), tail(2), replace(3), replace_4("replace", 4),
        replaceFirst(3), replaceFirst_4("replaceFirst", 4), trim(1), indexOf(2),
        choose(2), toUpper(1), toLower(1),

        select(2, FLAG_HAS_VARIABLE_ARGS),
        locate(2, FLAG_HAS_VARIABLE_ARGS);

        private String name; // name of the function
        private int arity;   // number of expected arguments, or minimum number of arguments for varargs
        private int flags;

        // Maps name/arities to function objects
        private static final Map<Pair<String,Integer>, Function> map = initMap();

        private Function(String name, int arity, int flags) {
            this.name = name != null ? name : name();
            this.arity = arity;
            this.flags = flags;
        }

        private Function(String name, int arity) {
            this(name, arity, 0);
        }

        private Function(int arity) {
            this(null, arity, 0);
        }

        private Function(int arity, int flags) {
            this(null, arity, flags);
        }

        public String getName() { return name; }
        public int getArity() { return arity; }
        public boolean isFieldAccess() { return (flags & FLAG_IS_FIELD_ACCESS) != 0; }
        public boolean isAggregateFunction() { return (flags & FLAG_IS_AGGREGATING_FUNCTION) != 0; }
        public boolean hasVariableArgs() { return (flags & FLAG_HAS_VARIABLE_ARGS) != 0; }

        private static Map<Pair<String,Integer>, Function> initMap() {
            Map<Pair<String,Integer>, Function> map = new HashMap<Pair<String,Integer>, Function>();
            for (Function function : Function.class.getEnumConstants()) {
                int arity = function.hasVariableArgs() ? -1 : function.arity;
                Pair<String,Integer> key = pair(function.name, arity);
                Function old = map.put(key, function);
                Assert.isTrue(old == null, "Duplicate function: " + function.name + " / " + function.arity);
            }
            return map;
        }

        /**
         * Finds the function with the given name, and given number of arguments.
         * @param name the name of the function
         * @param numArgs the number of arguments of the function call
         * @return the function object, or {@code null} if not found
         */
        public static Function getFunction(String name, int numArgs) {
            // try exact arity
            Function function = map.get(pair(name, numArgs));
            if (function != null)
                return function;
            // try vararg
            function = map.get(pair(name, -1));
            return function != null && function.arity <= numArgs ? function : null;
        }
    }

    /**
     * Returns true if the function name can be a field accessor of
     * a vector/histogram.
     */
    static boolean isFieldAccess(String fun) {
        Function function = Function.getFunction(fun, 1);
        return function != null && function.isFieldAccess();
    }

    /**
     * Returns true if the function is an aggregate function,
     * i.e. it creates a single value from a set of values.
     */
    static boolean isAggregateFunction(String fun) {
        Function function = Function.getFunction(fun, 1);
        return function != null && function.isAggregateFunction();
    }

    /**
     * Utility class to evaluate a string template.
     * The value of the string template is a string computed by appending
     * the literals and the values of spliced expressions.
     * <p>
     * Note: each expression is evaluated in the same environment,
     * they are not supposed to define new variables.
     */
    private class StringTemplateEvaluator extends SimpleVisitor<String> {

        Environment env;

        StringTemplateEvaluator(Environment env) {
            this.env = env;
        }

        @Override
        protected String visitStringTemplate(StringTemplate t) {
            StringBuilder sb = new StringBuilder();
            for (TemplateElement element : t.elements)
                sb.append(element.accept(this, null));
            return sb.toString();
        }

        @Override
        protected String visitStringLiteral(StringLiteral e) {
            return e.value;
        }

        @Override
        protected String visitSplicedExpr(SplicedExpr e) {
            Value value = evaluateSimple(e.expr, env);
            return value.asString();
        }
    }
}
