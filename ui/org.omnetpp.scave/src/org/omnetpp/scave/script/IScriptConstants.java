/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
 *--------------------------------------------------------------*/

package org.omnetpp.scave.script;

import java.util.HashMap;
import java.util.Map;

import org.apache.commons.lang3.ArrayUtils;

/**
 *
 * @author tomi, andras
 */
public interface IScriptConstants {
    public static final String ADD = "ADD";
    public static final String APPLY = "APPLY";
    public static final String COMPUTE = "COMPUTE";

    public static final String WHERE = "WHERE";
    public static final String NAMED = "NAMED";
    public static final String VALUE = "VALUE";
    public static final String MODULE = "MODULE";
    public static final String GROUPING = "GROUPING";
    public static final String OPTIONS = "OPTIONS";

    public static final String AVERAGEREPLICATIONS = "averageReplications";
    public static final String COMPUTEMINMAX = "computeMinMax";
    public static final String COMPUTESTDDEV = "computeStddev";
    public static final String COMPUTECONFIDENCEINTERVAL = "computeConfidenceInterval";

    public static final String[] COMMANDS = { ADD, APPLY, COMPUTE };
    public static final String[] COMPUTE_CLAUSE_KEYWORDS = { NAMED, VALUE, MODULE, GROUPING, OPTIONS, WHERE };
    public static final String[] COMPUTE_OPTION_KEYWORDS = { AVERAGEREPLICATIONS, COMPUTEMINMAX, COMPUTESTDDEV, COMPUTECONFIDENCEINTERVAL };

    public static final String[] COMMAND_ANDS_CLAUSE_COMMANDS = ArrayUtils.addAll(COMMANDS, COMPUTE_CLAUSE_KEYWORDS);

    public static final String ADD_HELP = 
            "Add items to the data set using a filter expression.";

    public static final String APPLY_HELP = 
            "Apply an operation to elements of the data set. The scope can be narrowed using a filter expression.";

    public static final String COMPUTE_HELP =
            "Compute new scalars from the data set.";

    public static final String WHERE_HELP =
            "Select result items using a filter expression.";

    public static final String COMPUTESCALAR_VALUE_HELP =
            "An arithmetic expression composed of constants, scalar values, vector or histogram " +
            "fields, operators, and functions.\n" +
            "\n" +
            "Constants\n" +
            "\n" +
            "Numeric constants: -1, 3.14159, 4.2e1. In boolean expressions, 0 means false, any other value means true.\n" +
            "\n" +
            "Scalars\n" +
            "\n" +
            "Simply use the scalar name (e.g. pkLoss), or enclose it with apostrophes if the " +
            "name of the scalar contains special characters (e.g. 'rcvdPk:count'.) If there " +
            "are several scalars with that name in the input dataset, usually recorded by " +
            "different modules, then the computation is iterated over the modules. The scalar " +
            "name cannot contain wildcards or dollar variables.\n" +
            "\n" +
            "Qualified names (i.e. <module>.<scalar>) can also be used to name scalars. The module " +
            "part can be a pattern that is matched against the full paths of modules. The same " +
            "pattern syntax is used as in ini files and in other parts of the Analysis Tool. " +
            "(Quick reminder: use * and ** for wildcards, {5..10} for numeric range, [5..10] for " +
            "module index range). If multiple scalars match the qualified name, they will be iterated, " +
            "i.e. the expression will be computed for each value. This means that if there are several " +
            "such pattern in the expression, then the computation is performed on their Cartesian product.\n" +
            "\n" +
            "If the expression mentions several unqualified scalars, they are expected to come from " +
            "the same module. For example, if your expression is foo+bar but the foo and bar scalars " +
            "have been recorded by different modules, the result will be empty.\n" +
            "\n" +
            "The iteration can be restricted by binding some part of the module name to variables, " +
            "and use those variables in other patterns. The ${x=<pattern>} syntax in a module name " +
            "pattern binds the part of the module name matched to pattern to a variable named x. " +
            "These variables can be referred as ${x} in other patterns. The ${...} syntax also allows " +
            "you to embed expressions (e.g. ${x+1}) into the pattern. For example, if there are " +
            "3 clients (cli0,cli1,cli2) and 3 servers (srv0,srv1,srv2), and each client sends packets " +
            "to the corresponding server, then the packet losses can be computed from the number of " +
            "sent and received packets as:\n" +
            "\n" +
            "cli${i={0..2}}.pkSent - srv${i}.pkRcvd.\n" +
            "\n" +
            "Vectors and histograms\n" +
            "\n" +
            "Use count(<name>), mean(<name>), min(<name>), max(<name>), stddev(<name>), variance(<name>) " +
            "to access the fields of vectors and histograms. Here <name> is the name of the vector or " +
            "histogram (quoted if necessary).\n" +
            "\n" +
            "The same rules apply to qualified names and iterations over modules as in the case of scalars.\n" +
            "\n" +
            "Functions\n" +
            "\n" +
            "The following functions can be applied to scalars or to an expression that yields a " +
            "scalar value: count(), mean(), min(), max(), stddev(), variance(). These aggregate " +
            "functions produce one value for a group of scalars instead of one for each scalar. " +
            "By default, each scalar belongs to the same group, but it is possible to group them " +
            "by module name (see Grouping). Aggregate functions cannot cross simulation run boundaries, " +
            "e.g they cannot be used to compute average over all replications of the same configuration; " +
            "use the 'Average replications' checkbox for that.\n" +
            "\n" +
            "Operators\n" +
            "\n" +
            "The following operators can be used:\n" +
            "\n" +
            "Arithmetic: + - * / ^ %\n" +
            "Bitwise: | & # ~ << >>\n" +
            "Comparison: == != < > <= >=\n" +
            "Boolean: ! || &&\n" +
            "Conditional: ?:";

    public static final String COMPUTESCALAR_MODULE_HELP =
            "Module to hold the newly computed scalars. You have the following options:\n" +
            "\n" +
            "1. Implicit names\n" +
            "\n" +
            "If you leave this field empty, the new scalars will be assigned to the same module " +
            "as their input scalars. This option is only open if the value expression contains " +
            "a scalar which is not qualified with module path pattern; then the module of " +
            "that scalar will be used. (That is, if you have both **.mac.dropCount and thruput " +
            "in the value expression, then the module of thruput takes precedence.) However, " +
            "if you use aggregate functions that derive a value from scalars possibly from " +
            "multiple modules (e.g. mean(dropCount) which computes the mean over dropCount " +
            "scalars of several modules), then you have to specify the module explicitly " +
            "because it would be ambiguous.\n" +
            "\n" +
            "Example\n" +
            "\n" +
            "The dataset contains scalars a and b recorded for two modules m1 and m2. The value " +
            "expression a+b computes two scalars: one from m1.a+m1.b and one from m2.a+m2.b. " +
            "The implicit module name is m1 for the first, and m2 for the second scalar.+\n" +
            "\n" +
            "2. Literal names\n" +
            "\n" +
            "You can enter a qualified module name here, and this name will be used as the module " +
            "of the new scalars. You can use any qualified name as a module name, it is no need " +
            "to use the names of existing modules only.\n" +
            "\n" +
            "3. Variable substitution\n" +
            "\n" +
            "Expressions can be used as part of the module name by the ${...} syntax. These " +
            "expressions are evaluated to strings and their values are substituted into " +
            "the module name. Variables from the value expression and grouping expression " +
            "can be referenced here. For example, if the value expression contains a " +
            "Net.node[${index=*}].sink.pkcount scalar (and thus defines an index variable), " +
            "you can write e.g. Net.node[${2*index+1}] in the module field.\n" +
            "\n" +
            "Example\n" +
            "If the grouping expression defines the subnet variable, and it has A, B, C values " +
            "for the + three groups, you can specify the module name of the computed scalars " +
            "as SkyNet.${subnet} to yield names SkyNet.A, SkyNet.B, and SkyNet.C.";

    public static final String COMPUTESCALAR_NAMED_HELP =
            "Name of the newly computed scalars.\n" +
            "\n" +
            "You can enter any literal string here, and this name will be used as the name " +
            "of the new scalars. Expressions can be embedded into the name by the ${...} syntax. " +
            "These expressions are evaluated to strings and their values are substituted into the module name. " +
            "Variables from the value expression and grouping expression can be referenced here.";

    public static final String COMPUTESCALAR_GROUPBY_HELP =
            "Scalars can be grouped by module before applying aggregate functions, and here " +
            "you can enter the expression that denotes the group of each scalar.\n" +
            "\n" +
            "The aggregate functions (mean,min,max,...) compute one scalar for each run by default. " +
            "It is possible to group the scalars, and compute an aggregate value for each group, " +
            "by specifying a grouping expression. The grouping expression is evaluated for each " +
            "statistic in the input dataset, and the resulting value denotes the statistic's group. " +
            "Subsequent computations are performed on each group independently.\n" +
            "\n" +
            "The following variables can be referenced in the grouping expression:\n" +
            "\n" +
            "    module: the name of the module that recorded the statistic\n" +
            "    name: the name of the statistic\n" +
            "    value: the value of the statistic, if it is a scalar, NaN otherwise\n" +
            "    run: the run identifier\n" +
            "    run attributes (configname, experiment, measurement, replication, etc.). However, " +
            "note that you cannot join data from several simulation runs into one group this way. \n" +
            "\n" +
            "Usually you want to extract some part of the module name as the group identifier. " +
            "The expression <str> =~ <pat> matches the string str with the pattern pat. " +
            "If there is no match, then the value of the expression is false, otherwise that part " +
            "of str that bound to a variable in pat.\n" +
            "\n" +
            "Examples\n" +
            "\n" +
            "If there are three subnets, (Network.A, Network.B,Network.C), each containing several modules, " +
            "you can compute the average of a scalar over subnets by specifying the following " +
            "grouping expression:\n" +
            "\n" +
            "(module =~ Network.${subnet=*}.**) ? subnet : 0\n" +
            "\n" +
            "The value of the subnet variable can be referred to in the value and module name expressions.";

    public static final String COMPUTESCALAR_OPTIONS_HELP = "TODO"; //TODO

    public static final String AVERAGEREPLICATIONS_HELP = 
            "The computation is performed in each run independently by default. If some run is a " +
            "replication of the same measurement with different seeds, you may want to average the results. " +
            "If the Average replications checkbox is selected, then only one scalar is added to the " +
            "dataset for each measurement.\n" +
            "\n" +
            "A new run generated for the scalar which represents the set of replications which it is " +
            "computed from. The attributes of this run are those that have the same value " +
            "in all replications.\n" +
            "\n" +
            "In addition to mean, you can also add other statistical properties of the computed scalar " +
            "to the dataset by selecting the corresponding checkboxes in the dialog. The names of " +
            "these new scalars will be formed by adding the :stddev, :confint, :min, or :max suffix " +
            "to the base name of the scalar.";
    
    public static final String COMPUTEMINMAX_HELP = 
            "Compute minimum and maximum, and add them to the data set with the :min and :max suffix. See also " + AVERAGEREPLICATIONS;
    
    public static final String COMPUTESTDDEV_HELP =
            "Compute standard deviations, and add them to the data set with the :stddev suffix. See also " + AVERAGEREPLICATIONS;
   
    public static final String COMPUTECONFIDENCEINTERVAL_HELP =
            "Compute confidence intervals, and add them to the data set with the :confint suffix. See also " + AVERAGEREPLICATIONS; //TODO confidence level

    @SuppressWarnings("serial")
    public static final Map<String,String> HELP_TEXTS = new HashMap<String,String>() {{
        // Commands
        put(ADD, ADD_HELP);
        put(APPLY, APPLY_HELP);
        put(COMPUTE, COMPUTE_HELP);

        // COMPUTE clauses
        put(NAMED, COMPUTESCALAR_NAMED_HELP);
        put(VALUE, COMPUTESCALAR_VALUE_HELP);
        put(MODULE, COMPUTESCALAR_MODULE_HELP);
        put(GROUPING, COMPUTESCALAR_GROUPBY_HELP);
        put(OPTIONS, COMPUTESCALAR_OPTIONS_HELP);

        // COMPUTE options
        put(AVERAGEREPLICATIONS, AVERAGEREPLICATIONS_HELP);
        put(COMPUTEMINMAX, COMPUTEMINMAX_HELP);
        put(COMPUTESTDDEV, COMPUTESTDDEV_HELP);
        put(COMPUTECONFIDENCEINTERVAL, COMPUTECONFIDENCEINTERVAL_HELP);
    }};
}
