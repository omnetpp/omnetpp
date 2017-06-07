package org.omnetpp.scave.computed;

import static org.omnetpp.common.util.StringUtils.isEmpty;
import static org.omnetpp.scave.computed.ExpressionEvaluator.isAggregateFunction;
import static org.omnetpp.scave.computed.ExpressionEvaluator.isFieldAccess;

import java.text.ParseException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.commons.lang3.ObjectUtils;
import org.apache.commons.lang3.mutable.MutableBoolean;
import org.eclipse.core.runtime.Assert;
import org.eclipse.core.runtime.IStatus;
import org.eclipse.core.runtime.MultiStatus;
import org.omnetpp.common.util.Tuple4;
import org.omnetpp.scave.ScavePlugin;
import org.omnetpp.scave.computed.ExpressionEvaluator.Environment;
import org.omnetpp.scave.computed.ExpressionEvaluator.EvaluationException;
import org.omnetpp.scave.computed.ExpressionEvaluator.Value;
import org.omnetpp.scave.computed.ExpressionSyntax.Expr;
import org.omnetpp.scave.computed.ExpressionSyntax.FunctionCall;
import org.omnetpp.scave.computed.ExpressionSyntax.StatisticRef;
import org.omnetpp.scave.computed.ExpressionSyntax.StringTemplate;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableDefPattern;
import org.omnetpp.scave.computed.ExpressionSyntax.VariableRef;
import org.omnetpp.scave.computed.ExpressionValidator.Context;
import org.omnetpp.scave.computed.ExpressionVisitors.SimpleNodeIterator;
import org.omnetpp.scave.engine.IDList;
import org.omnetpp.scave.engine.ResultFileManager;
import org.omnetpp.scave.engine.ResultItem;
import org.omnetpp.scave.engine.Run;
import org.omnetpp.scave.engine.RunAttribute;
import org.omnetpp.scave.engine.ScalarResult;
import org.omnetpp.scave.engine.Statistics;
import org.omnetpp.scave.engine.StringMap;
import org.omnetpp.scave.engine.StringVector;
import org.omnetpp.scave.model2.StatUtils;
import org.omnetpp.scave.script.ComputeScalarCommand;

/**
 * Implements the computation of new scalars,
 * including grouping and averaging.
 *
 * The computation described by the ComputeScalar node is
 * performed in the following steps:
 * <ol>
 * <li>
 * The input statistics are sorted into groups according to their run.
 * The next two steps of the computation is performed for each run independently.
 * <li>
 * Within each run, evaluate the grouping expression for each input statistics.
 * The grouping expression can refer to the fields of the statistic (name, module,
 * run attributes). The value of the grouping expression is the group identifier
 * of the statistic. Statistics are further sorted into groups according to their
 * group identifier. This step is optional, by default each statistic belongs
 * to the same group.
 * <li>
 * Evaluate the value expression for each group of input statistics. The value
 * expression may refer to the group identifier as the 'group' variable. Variables
 * that are defined in the grouping expression and having the same value for all
 * members of the group can also be referenced. Evaluation of the value expression
 * can give multiple values, these will be the values of the computed scalars.
 * The modules and names of the scalars are obtained by evaluating the module and
 * name expressions respectively.
 * In the module and name expressions the variables defined in the grouping or
 * value expression can be accessed.
 * <li> Optionally compute the averages of the new scalars, that has the same module,
 * the same name and the same experiment/measurement in different runs; i.e. average
 * their values over replications. Add the average of as the value of the new scalar.
 * If requested also add min/max/stddev/confint as additional scalars. If there is more
 * than one replication, then a new run also created, and associated with the scalar.
 * The run represents a set of runs, and has the attributes that are common in the
 * individual runs.
 * </ol>
 *
 * @author tomi
 */
public class ComputedScalarEngine {
    private ResultFileManager resultFileManager;

    public ComputedScalarEngine(ResultFileManager manager) {
        this.resultFileManager = manager;
    }

    /**
     * Computed scalar, a result of the computation.
     */
    static class ComputedScalar {
        String runName;
        StringMap runAttributes;
        String module;
        String name;
        double value;

        ComputedScalar() {
        }

        ComputedScalar(String runName, StringMap runAttrs, String module, String name, double value) {
            this.runName = runName;
            this.runAttributes = runAttrs;
            this.module = module;
            this.name = name;
            this.value = value;
        }
    }

    /**
     * Perform the computation described by {@code op} with the specified input.
     */
    List<ComputedScalar> computeScalars(ComputeScalarCommand op, IDList input) throws ParseException {
        ExpressionParser parser = new ExpressionParser();
        Expr valueExpr = parser.parseExpr(op.getValueExpr());
        Expr groupByExpr = isEmpty(op.getGroupByExpr()) ? null : parser.parseExpr(op.getGroupByExpr());
        if (isEmpty(op.getScalarName()))
            throw new EvaluationException("scalar name is unspecified");
        StringTemplate nameTmpl = parser.parseTemplate(op.getScalarName());
        StringTemplate moduleTmpl = isEmpty(op.getModuleExpr()) ? null : parser.parseTemplate(op.getModuleExpr());

        // group input by runs
        Map<String, IDList> runToInputMap = groupInputByRun(input);

        // compute the scalars in each run, average over replications if needed
        ComputedScalarSet scalars = new ComputedScalarSet(op);
        for (Map.Entry<String,IDList> entry : runToInputMap.entrySet()) {
            String runName = entry.getKey();
            StringMap runAttrs = resultFileManager.getRunByName(runName).getAttributes();
            IDList inputInRun = entry.getValue();
            List<ComputedScalar> newScalars = computeScalars(valueExpr, nameTmpl, moduleTmpl, groupByExpr, inputInRun);
            scalars.addAll(newScalars, runName, runAttrs);
        }
        return scalars.toList();
    }

    /**
     * Computes the scalars in one run.
     * <p>
     * This method computes the input groups, and iterates the computation in each group.
     */
    private List<ComputedScalar> computeScalars(Expr valueExpr, StringTemplate nameTmpl, StringTemplate moduleTmpl, Expr groupByExpr, IDList input)
            throws ParseException
    {
        if (groupByExpr != null) {
            List<ComputedScalar> result = new ArrayList<ComputedScalar>();
            List<StatisticGroup> groups = groupInputByGroupId(groupByExpr, input);
            for (StatisticGroup group : groups)
                result.addAll(computeScalars(valueExpr, nameTmpl, moduleTmpl, group.input, group.env));
            return result;
        }
        else {
            Environment empty = new Environment();
            return computeScalars(valueExpr, nameTmpl, moduleTmpl, input, empty);
        }
    }

    /**
     * Computes the scalars in one group of one run.
     * <p>
     * Here all inputs is from the same run, and belongs to the same group.
     * {@code env} contains the variables defined in the grouping expression.
     */
    private List<ComputedScalar> computeScalars(Expr valueExpr, StringTemplate nameTmpl, StringTemplate moduleTmpl, IDList input, Environment env) {
        ExpressionEvaluator evaluator = new ExpressionEvaluator(resultFileManager, input);
        List<ComputedScalar> result = new ArrayList<ComputedScalar>();
        for (Value val : evaluator.evaluate(valueExpr, env)) {
            ComputedScalar scalar = new ComputedScalar();
            scalar.value = val.asNumber();
            scalar.name = evaluator.evaluate(nameTmpl, val.env);
            scalar.module = moduleTmpl == null ?
                              val.env.lookup("$m") :
                              evaluator.evaluate(moduleTmpl, val.env);
            if (scalar.module == null)
                throw new EvaluationException("module name is not specified");
            result.add(scalar);
        }
        return result;
    }

    /**
     * Sort the input statistics according to their runs.
     * @param input the input statistics to be grouped
     * @return a map from runid to a list of statistics
     */
    private Map<String,IDList> groupInputByRun(IDList input) {
        Map<String, IDList> result = new LinkedHashMap<String, IDList>();
        for (int i = 0; i < input.size(); ++i) {
            long id = input.get(i);
            ResultItem item = resultFileManager.getItem(id);
            String run = item.getFileRun().getRun().getRunName();
            IDList idlist = result.get(run);
            if (idlist == null)
                result.put(run, idlist = new IDList());
            idlist.add(id);
        }
        return result;
    }

    /**
     * Class representing a group of input statistics.
     */
    static class StatisticGroup {
        Environment env = new Environment();   // variables defined in the grouping expression + the "group" variable
        IDList input = new IDList();           // the statistics in the group
        StatisticGroup(Environment env) { this.env = env; }
    }

    /**
     * Sort input statistics according to their group identifier.
     * The group identifier is obtained by evaluating {@code groupByExpr}.
     */
    private List<StatisticGroup> groupInputByGroupId(Expr groupByExpr, IDList input)
        throws ParseException
    {
        ExpressionEvaluator evaluator = new ExpressionEvaluator(resultFileManager, input);

        Map<String, StatisticGroup> groups = new LinkedHashMap<String, StatisticGroup>();
        for (int i = 0; i < input.size(); ++i) {
            long id = input.get(i);
            ResultItem item = resultFileManager.getItem(id);
            Run run = item.getFileRun().getRun();
            StringVector runAttrs = run.getAttributes().keys();
            double val = item instanceof ScalarResult ? ((ScalarResult)item).getValue() : Double.NaN;

            Environment env = new Environment();
            env.define("name", item.getName());
            env.define("module", item.getModuleName());
            env.define("value", String.valueOf(val));
            env.define("run", run.getRunName());
            for (int j = 0; j < runAttrs.size(); ++j)
                env.define(runAttrs.get(j), run.getAttribute(runAttrs.get(j)));

            Value value = evaluator.evaluateSimple(groupByExpr, env);
            String groupID = value.asString();
            StatisticGroup group = groups.get(groupID);
            if (group == null) {
                value.env.define("group", groupID);
                groups.put(value.asString(), group = new StatisticGroup(value.env));
            }

            group.input.add(id);
        }

        return new ArrayList<StatisticGroup>(groups.values());
    }

    /**
     * Utility class to average computed scalars over replications.
     */
    private static class ComputedScalarSet {
        boolean averageReplications;
        boolean computeStddev;
        boolean computeMinMax;
        boolean computeConfInterval;
        double confidenceLevel;

        List<ComputedScalar> scalars = new ArrayList<ComputedScalar>();
        List<ComputedScalar> averagedScalars;

        ComputedScalarSet(ComputeScalarCommand op) {
            averageReplications = op.isAverageReplications();
            if (averageReplications) {
                computeStddev = op.isComputeStddev();
                computeMinMax = op.isComputeMinMax();
                computeConfInterval = op.isComputeConfidenceInterval();
                if (computeConfInterval) {
                    confidenceLevel = op.getConfidenceLevel();
                    Assert.isLegal(0.0 <= confidenceLevel && confidenceLevel <= 1.0);
                }
            }
        }

        /**
         * Adds new computed scalars to this set.
         * The scalars belongs to the same run.
         * Do not call this method after toList() has been called.
         */
        void addAll(List<ComputedScalar> scalars, String runName, StringMap runAttrs) {
            for (ComputedScalar scalar : scalars) {
                scalar.runName = runName;
                scalar.runAttributes = runAttrs;
                this.scalars.add(scalar);
            }
        }

        /**
         * Returns the scalars in this set.
         * These can be the individual scalars that has been added to the set,
         * or their averages/stddev/... over replications.
         */
        List<ComputedScalar> toList() {
            if (averageReplications) {
                if (averagedScalars == null) {
                    averagedScalars = new ArrayList<ComputedScalar>();
                    for (Map.Entry<ModuleNameExperimentMeasurement,AveragedScalar> entry : averageScalars(scalars).entrySet()) {
                        String module = entry.getKey().first;
                        String name = entry.getKey().second;
                        Set<String> runNames = entry.getValue().runNames;
                        String runName = runNames.size() == 1 ? runNames.iterator().next() : "runs-"+runNames.hashCode();
                        StringMap runAttrs = entry.getValue().runAttrs;
                        Statistics stat = entry.getValue().stat;
                        averagedScalars.add(new ComputedScalar(runName, runAttrs, module, name, stat.getMean()));
                        if (computeStddev)
                            averagedScalars.add(new ComputedScalar(runName, runAttrs, module, name+":stddev", stat.getStddev()));
                        if (computeMinMax) {
                            averagedScalars.add(new ComputedScalar(runName, runAttrs, module, name+":min", stat.getMin()));
                            averagedScalars.add(new ComputedScalar(runName, runAttrs, module, name+":max", stat.getMax()));
                        }
                        if (computeConfInterval) {
                            double value = StatUtils.confidenceInterval(stat, confidenceLevel);
                            averagedScalars.add(new ComputedScalar(runName, runAttrs, module, name+":confint", value));
                        }
                    }
                }
                return averagedScalars;
            }
            else
                return scalars;
        }

        /**
         * Group scalars according to their module name/experiment/measurement, and compute their
         * count/mean/stddev/min/max and common run attributes in each group.
         */
        private static Map<ModuleNameExperimentMeasurement, AveragedScalar> averageScalars(List<ComputedScalar> scalars) {
            Map<ModuleNameExperimentMeasurement, AveragedScalar> map = new LinkedHashMap<ModuleNameExperimentMeasurement, AveragedScalar>();
            for (ComputedScalar scalar : scalars) {
                ModuleNameExperimentMeasurement key = new ModuleNameExperimentMeasurement(scalar);
                AveragedScalar value = map.get(key);
                if (value == null)
                    map.put(key, value = new AveragedScalar(scalar));
                else {
                    value.collect(scalar);
                }
            }
            return map;
        }

        /**
         * Module name, experiment, measurement triple.
         */
        private static class ModuleNameExperimentMeasurement extends Tuple4<String,String,String,String> {
            ModuleNameExperimentMeasurement(ComputedScalar scalar) {
                super(scalar.module,
                      scalar.name,
                      scalar.runAttributes.get(RunAttribute.EXPERIMENT),
                      scalar.runAttributes.get(RunAttribute.MEASUREMENT));
            }
        }

        /**
         * This class collects the mean/stddev/..., and common run attributes
         * of averaged scalars.
         */
        private static class AveragedScalar {
            Set<String> runNames;
            StringMap runAttrs;
            Statistics stat;

            AveragedScalar(ComputedScalar scalar) {
                this.runNames = new HashSet<String>();
                this.runNames.add(scalar.runName);
                this.runAttrs = new StringMap(scalar.runAttributes);
                this.stat = StatUtils.singleValueStatistics(scalar.value);
            }

            void collect(ComputedScalar scalar) {
                runNames.add(scalar.runName);
                intersect(runAttrs, scalar.runAttributes);
                stat.collect(scalar.value);
            }

            void intersect(StringMap commonAttrs, StringMap attrs) {
                StringVector attrNames = commonAttrs.keys();
                for (int i = 0; i < attrNames.size(); ++i) {
                    String attrName = attrNames.get(i);
                    if (!ObjectUtils.equals(commonAttrs.get(attrName), attrs.get(attrName)))
                        commonAttrs.del(attrName);
                }
            }
        }
    }

    /*----------------------------------------------------------------------
     *       Static validation of ComputeScalar nodes (used by the UI)
     *----------------------------------------------------------------------*/

    // Identifiers of fields of the ComputeScalar node
    public static final int
        FIELD_VALUE = 0,
        FIELD_GROUPBY = 1,
        FIELD_NAME = 2,
        FIELD_MODULE = 3;

    public static IStatus error(String message) { return ScavePlugin.getErrorStatus(0, message, null); }
    public static IStatus warning(String message) { return ScavePlugin.getWarningStatus(message); }

    // variables that can be referenced in the grouping expression
    public static final Set<String> GROUPBY_PREDEFINED_VARIABLES;

    static {
        Set<String> vars = GROUPBY_PREDEFINED_VARIABLES = new HashSet<String>();
        vars.add("name");
        vars.add("module");
        vars.add("run");
        vars.add("value");
        StringVector attrNames = RunAttribute.getAttributeNames();
        for (int i = 0; i < attrNames.size(); ++i)
            vars.add(attrNames.get(i));
    }

    /**
     * Performs static validation of a ComputeScalar node, and returns the errors and warnings.
     * <p>
     * This method checks that:
     * <ul>
     * <li>the grouping, value, name, and module expressions can be parsed
     * <li>there are no references to undefined variables or functions
     * <li>the module expression is filled in if the value expression does not define an
     *     implicit module name
     * </ul>
     *
     * @returns an array of MultiStatus objects, indexed by the field identifier
     */
    public IStatus[] validate(String valueText, String nameText, String moduleText, String groupByText, Set<String> statistics) {
        ExpressionParser parser = new ExpressionParser();
        ExpressionValidator validator = new ExpressionValidator();
        Set<String> emptySet = Collections.emptySet();

        Expr valueExpr = null;
        Set<String> vars = null;

        MultiStatus[] result = new MultiStatus[4];

        if (groupByText != null) {
            result[FIELD_GROUPBY] = new MultiStatus(ScavePlugin.PLUGIN_ID, 0, "Grouping", null);
            vars = null;
            if (!isEmpty(groupByText)) {
                try {
                    Expr expr = parser.parseExpr(groupByText);
                    Context context = validator.validate(expr, GROUPBY_PREDEFINED_VARIABLES, emptySet);
                    if (context.error != null)
                        result[FIELD_GROUPBY].add(error(context.error));
                    if (context.warning != null)
                        result[FIELD_GROUPBY].add(warning(context.warning));
                    vars = context.vars;
                    vars.add("group");
                } catch (ParseException e) {
                    result[FIELD_GROUPBY].add(error(e.getMessage()));
                }
            }
        }
        if (valueText != null) {
            result[FIELD_VALUE] = new MultiStatus(ScavePlugin.PLUGIN_ID, 0, "Value", null);
            if (isEmpty(valueText)) {
                result[FIELD_VALUE].add(error("Value expression is mandatory."));
            }
            else {
                try {
                    valueExpr = parser.parseExpr(valueText);
                    Context context = validator.validate(valueExpr, vars != null ? vars: emptySet, statistics);
                    if (context.error != null)
                        result[FIELD_VALUE].add(error(context.error));
                    if (context.warning != null)
                        result[FIELD_VALUE].add(warning(context.warning));
                    vars = context.vars;
                } catch (ParseException e) {
                    result[FIELD_VALUE].add(error(e.getMessage()));
                }
            }
        }
        if (nameText != null) {
            result[FIELD_NAME] = new MultiStatus(ScavePlugin.PLUGIN_ID, 0, "Name", null);
            if (isEmpty(nameText)) {
                result[FIELD_NAME].add(error("Scalar name is missing"));
            }
            else {
                try {
                    StringTemplate template = parser.parseTemplate(nameText);
                    Context context = validator.validate(template, vars != null ? vars : emptySet);
                    if (context.error != null)
                        result[FIELD_NAME].add(error(context.error));
                    if (context.warning != null)
                        result[FIELD_NAME].add(warning(context.warning));
                } catch (ParseException e) {
                    result[FIELD_NAME].add(error(e.getMessage()));
                }
            }
        }
        if (moduleText != null) {
            result[FIELD_MODULE] = new MultiStatus(ScavePlugin.PLUGIN_ID, 0, "Module", null);
            if (isEmpty(moduleText)) {
                if (valueExpr != null && !definesImplicitModuleName(valueExpr, vars != null ? vars : emptySet))
                    result[FIELD_MODULE].add(error("Module name is missing"));
            }
            else {
                try {
                    StringTemplate template = parser.parseTemplate(moduleText);
                    Context context = validator.validate(template, vars != null ? vars : emptySet);
                    if (context.error != null)
                        result[FIELD_MODULE].add(error(context.error));
                    if (context.warning != null)
                        result[FIELD_MODULE].add(warning(context.warning));
                    vars = context.vars;
                } catch (ParseException e) {
                    result[FIELD_MODULE].add(error(e.getMessage()));
                }
            }
        }

        return result;
    }

    /**
     * Returns true if the given value expression defines an
     * implicit module name ($m variable), i.e. it contains statistic
     * references in which the module part is empty.
     * Expressions in aggregate function calls are ignored,
     * because variables defined in the argument are not visible outside.
     */
    private boolean definesImplicitModuleName(Expr expr, Set<String> vars) {
        final MutableBoolean result = new MutableBoolean(false);
        final Set<String> variables = new HashSet<String>(vars);
        expr.accept(new SimpleNodeIterator<Object>() {
                        protected Object visitStatisticRef(StatisticRef ref) {
                            if (ref.modulePattern == null)
                                result.setValue(true);
                            return null;
                        }

                        protected Object visitVariableRef(VariableRef ref) {
                            if (!variables.contains(ref.name))
                                result.setValue(true); // assume it is a statistic ref
                            return null;
                        }

                        @Override
                        protected Object visitVariableDefPattern(VariableDefPattern e) {
                            variables.add(e.name);
                            return null;
                        }

                        protected Object visitFunctionCall(FunctionCall call) {
                            // skip expressions under count(...), mean(...), etc. calls
                            // except if it is a field access of a histogram/vector
                            if (isAggregateFunction(call.name) && call.args.size() == 1) {
                                if (isFieldAccess(call.name)) {
                                    Expr arg = call.args.get(0);
                                    if (arg instanceof StatisticRef || arg instanceof VariableRef)
                                        arg.accept(this, null);
                                }
                            }
                            else {
                                for (Expr arg : call.args)
                                    arg.accept(this, null);
                            }
                            return null;
                        }
                    }, null);
        return result.booleanValue();
    }
}
