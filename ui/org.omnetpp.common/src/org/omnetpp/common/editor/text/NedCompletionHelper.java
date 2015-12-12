/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.editor.text;

import java.util.ArrayList;

import org.eclipse.core.runtime.Assert;
import org.eclipse.jface.text.templates.Template;
import org.omnetpp.common.displaymodel.IDisplayString;
import org.omnetpp.common.engine.PStringVector;
import org.omnetpp.common.engine.UnitConversion;

/**
 * This class contains data for context assist functions for NED files.
 *
 * @author rhornig
 */
public final class NedCompletionHelper {
    /** This context's id */
    public static final String DEFAULT_NED_CONTEXT_TYPE= "org.omnetpp.ned.editor.text.default"; //$NON-NLS-1$

    // word lists for completion
    public final static String[] proposedPrivateDocTodo = Keywords.DOC_TODO;
    public final static String[] proposedDocTags = Keywords.DOC_TAGS;
    public final static String[] proposedDocKeywords = Keywords.DOC_KEYWORDS;
    public final static String[] proposedNedBaseParamTypes = Keywords.NED_PARAM_TYPES;
    public final static String[] proposedNedParamTypes = { "bool", "double", "int", "string", "xml", "volatile bool", "volatile double", "volatile int", "volatile string", "volatile xml" };
    public final static String[] proposedNedGateTypes = Keywords.NED_GATE_TYPES;
    public final static String[] proposedNedTopLevelKeywords = { "import", "network", "package", "property"};
    public final static String[] proposedNedTypeDefinerKeywords = { "channel", "channelinterface", "moduleinterface", "module", "simple"};
    public final static String[] proposedNedConnsKeywords = {"allowunconnected"};
    public final static String[] proposedNedOtherExpressionKeywords = Keywords.NED_EXPR_KEYWORDS;
    public final static String[] proposedConstants = Keywords.BOOL_CONSTANTS;


    public final static Template[] proposedNedFilePropertyTempl = {
        makeShortTemplate("@namespace(${namespace});", "property"),
    }; // XXX check what gets actually supported

    public final static Template[] proposedNedComponentPropertyTempl = {
        makeShortTemplate("@display(\"i=${icon}\");", "property"),
        makeShortTemplate("@class(${className});", "property"),
        makeShortTemplate("@contains(${label1});", "property"),
        makeShortTemplate("@labels(${label1});", "property"),
        makeShortTemplate("@signal[${name}];", "property"),
        makeShortTemplate("@signal[${name}](type=\"${type}\");", "property"),
        makeShortTemplate("@statistic[${name}];", "property"),
        makeShortTemplate("@statistic[${name}](title=\"${title}\";record=${expression1},${expression2});", "property"),
    }; // XXX check what gets actually supported! also: "recordstats", "kernel", ...

    public final static Template[] proposedNedComponentDisplayStringTempl;

    public final static Template[] proposedNedSubmoduleDisplayStringTempl;

    public final static Template[] proposedNedConnectionDisplayStringTempl;

    static {
        ArrayList<Template> componentTemplates = new ArrayList<Template>();
        ArrayList<Template> submoduleTemplates = new ArrayList<Template>();
        ArrayList<Template> connectionTemplates = new ArrayList<Template>();

        for (IDisplayString.Tag tag : IDisplayString.Tag.values()) {
            String fullPattern = tag.name() + "=";

            boolean first = true;
            for (IDisplayString.Prop prop : IDisplayString.Prop.values()) {
                if (tag.equals(prop.getTag())) {
                    if (!first)
                        fullPattern += ",";
                    else
                        first = false;

                    fullPattern += "${" + prop.getName().replaceAll("[^a-zA-Z0-9]", "") + "}";
                }
            }

            int shortParameterCount = -1;
            switch (tag) {
                case bgp: break;
                case bgb: shortParameterCount = 2; break;
                case bgi: break;
                case bgtt: break;
                case bgg: shortParameterCount = 1; break;
                case bgl: break;
                case bgu: break;
                case p: shortParameterCount = 2; break;
                case b: shortParameterCount = 2; break;
                case i: shortParameterCount = 1; break;
                case is: break;
                case i2: shortParameterCount = 1; break;
                case r: shortParameterCount = 1; break;
                case q: break;
                case t: shortParameterCount = 1; break;
                case tt: break;
                case ls: shortParameterCount = 1; break;
            }

            String description = tag.getDescription();
            String shortPattern = shortParameterCount == -1 ? null : fullPattern;
            int index = -1;
            for (int i = 0; i < shortParameterCount + 1; i++)
                index = shortPattern.indexOf('$', index + 1);
            if (index != -1)
                shortPattern = shortPattern.substring(0, index - 1);

            if ((tag.getType() & IDisplayString.COMPOUNDMODULE) != 0) {
                componentTemplates.add(makeShortTemplate(fullPattern, description));

                if (shortPattern != null)
                    componentTemplates.add(makeShortTemplate(shortPattern, description));
            }

            if ((tag.getType() & IDisplayString.SUBMODULE) != 0) {
                submoduleTemplates.add(makeShortTemplate(fullPattern, description));

                if (shortPattern != null)
                    submoduleTemplates.add(makeShortTemplate(shortPattern, description));
            }

            if ((tag.getType() & IDisplayString.CONNECTION) != 0) {
                connectionTemplates.add(makeShortTemplate(fullPattern, description));

                if (shortPattern != null)
                    connectionTemplates.add(makeShortTemplate(shortPattern, description));
            }
        }

        proposedNedComponentDisplayStringTempl = componentTemplates.toArray(new Template[0]);
        proposedNedSubmoduleDisplayStringTempl = submoduleTemplates.toArray(new Template[0]);
        proposedNedConnectionDisplayStringTempl = connectionTemplates.toArray(new Template[0]);
    }

    public final static Template[] proposedNedSubmodulePropertyTempl = {
        makeShortTemplate("@display(\"i=${icon}\");", "property"),
        makeShortTemplate("@dynamic()", "property"),
    }; //XXX check this list before release

    public final static Template[] proposedNedConnectionPropertyTempl = {
        makeShortTemplate("@display(\"i=${icon}\");", "property"),
    };

    public final static Template[] proposedNedParamPropertyTempl = {
        makeShortTemplate("@prompt(\"${message}\")", "property"),
        makeShortTemplate("@enum(${value1}, ${value2})", "property"),
        makeShortTemplate("@unit(${unitName})", "property"),
    }; //XXX check this list before release

    public final static Template[] proposedNedUnitTempl;

    static {
        PStringVector units = UnitConversion.getAllUnits();
        proposedNedUnitTempl = new Template[(int)units.size()];
        for (int i = 0; i < units.size(); i++) {
            String shortName = units.get(i);
            proposedNedUnitTempl[i] = makeTemplate(UnitConversion.getLongName(shortName), "unit", shortName);
        }
    }

    public final static Template[] proposedNedSignalPropertyParameterTempl = {
        makeShortTemplate("unit=${unit};", "parameter"),
        makeShortTemplate("enum=${value1},${value2};", "parameter"),
        makeShortTemplate("type=${type};", "parameter"),
    };

    public final static Template[] proposedNedStatisticPropertyParameterTempl = {
        makeShortTemplate("title=\"${title}\"", "parameter"),
        makeShortTemplate("unit=${unit};", "parameter"),
        makeShortTemplate("interpolationmode=${modes};", "parameter"),
        makeShortTemplate("source=${source};", "parameter"),
        makeShortTemplate("enum=${value1},${value2};", "parameter"),
        makeShortTemplate("record=${expression1, expression2};", "parameter"),
    };

    public final static Template[] proposedNedStatisticPropertyInterpolationModeParameterValueTempl = {
        makeShortTemplate("none", "interpolation mode"),
        makeShortTemplate("linear", "interpolation mode"),
        makeShortTemplate("sample-hold", "interpolation mode"),
        makeShortTemplate("backward-sample-hold", "interpolation mode"),
    };

    public final static Template[] proposedNedStatisticPropertyRecordParameterValueTempl = {
        makeShortTemplate("auto", "automatically select the best mode(s) for a statistic (currently selects histogram mode)"),
        makeShortTemplate("count", "record the number of values emitted; values are ignored"),
        makeShortTemplate("vector", "record all values"),
        makeShortTemplate("last", "record the last value"),
        makeShortTemplate("sum", "record the sum of the values"),
        makeShortTemplate("mean", "record the mean of the values"),
        makeShortTemplate("min", "record the minimum value"),
        makeShortTemplate("max", "record the maximum value"),
        makeShortTemplate("timeavg", "record the time average, interpreting the signal as a step function (sample-hold)"),
        makeShortTemplate("stats", "record the count, sum, mean, standard deviation, minimum and maximum of the values"),
        makeShortTemplate("histogram", "record everything stats records, plus a histogram of the values (bins are set up automatically)"),
    };

    public final static Template[] proposedNedGatePropertyTempl = {
        makeShortTemplate("@labels(${label1})", "property"),
        makeShortTemplate("@inlabels(${inLabel1})", "property"),
        makeShortTemplate("@outlabels(${outLabel1})", "property"),
    }; //XXX check this list before release

    // MSG specific completions - not used currently
    //  public final static String[] proposedMsgTypes = { "bool", "char", "double", "int", "long", "numeric", "short", "string", "unsigned", "xml" };
    //  public final static String[] proposedMsgKeywords = { "abstract", "ancestor", "channel", "class", "connections", "const", "cplusplus", "datarate", "delay", "display", "do", "endchannel", "endfor", "endmodule", "endnetwork", "endsimple", "enum", "error", "extends", "fields", "for", "gates", "gatesizes", "if", "import", "in:", "index", "like", "message", "module", "network", "nocheck", "noncobject", "on", "out:", "parameters", "properties", "ref", "simple", "sizeof", "struct", "submodules", "to" };

    public final static Template[] proposedNedOperatorsTempl = new Template[] {
        makeShortTemplate("const(${x})", "operator"),
        makeShortTemplate("default(${x})", "operator"),
        makeShortTemplate("sizeof(${gateOrSubmod})", "operator"),
    };

    public final static Template[] proposedNedFunctionsTempl = new Template[] {
        // math
        makeShortTemplate("acos(${x})", "function"),
        makeShortTemplate("asin(${x})", "function"),
        makeShortTemplate("atan(${x})", "function"),
        makeShortTemplate("atan2(${x},${y})", "function"),
        makeShortTemplate("sin(${x})", "function"),
        makeShortTemplate("cos(${x})", "function"),
        makeShortTemplate("tan(${x})", "function"),
        makeShortTemplate("ceil(${x})", "function"),
        makeShortTemplate("floor(${x})", "function"),
        makeShortTemplate("max(${a},${b})", "function"),
        makeShortTemplate("min(${a},${b})", "function"),
        makeShortTemplate("exp(${x})", "function"),
        makeShortTemplate("pow(${x},${y})", "function"),
        makeShortTemplate("sqrt(${x})", "function"),
        makeShortTemplate("fabs(${x})", "function"),
        makeShortTemplate("fmod(${x},${y})", "function"),
        makeShortTemplate("hypot(${x},${y})", "function"),
        makeShortTemplate("log(${x})", "function"),
        makeShortTemplate("log10(${x})", "function"),

        // unit
        makeShortTemplate("dropUnit(${quantity})", "function"),
        makeShortTemplate("replaceUnit(${quantity}, ${string})", "function"),
        makeShortTemplate("convertUnit(${quantity}, ${string})", "function"),
        makeShortTemplate("unitOf(${quantity})", "function"),

        // string
        makeShortTemplate("length(${string})", "function"),
        makeShortTemplate("contains(${string}, ${string})", "function"),
        makeShortTemplate("substring(${string}, ${int})", "function"),
        makeShortTemplate("substring(${string}, ${int}, ${int})", "function"),
        makeShortTemplate("substringBefore(${string}, ${string})", "function"),
        makeShortTemplate("substringAfter(${string}, ${string})", "function"),
        makeShortTemplate("substringBeforeLast(${string}, ${string})", "function"),
        makeShortTemplate("substringAfterLast(${string}, ${string})", "function"),
        makeShortTemplate("startsWith(${string}, ${string})", "function"),
        makeShortTemplate("endsWith(${string}, ${string})", "function"),
        makeShortTemplate("tail(${string}, ${int})", "function"),
        makeShortTemplate("replace(${string}, ${string}, ${string})", "function"),
        makeShortTemplate("replace(${string}, ${string}, ${string}, ${int})", "function"),
        makeShortTemplate("replaceFirst(${string}, ${string}, ${string})", "function"),
        makeShortTemplate("replaceFirst(${string}, ${string}, ${string}, ${int})", "function"),
        makeShortTemplate("trim(${string})", "function"),
        makeShortTemplate("indexOf(${string})", "function"),
        makeShortTemplate("choose(${int}, ${string})", "function"),
        makeShortTemplate("toUpper(${string})", "function"),
        makeShortTemplate("toLower(${string})", "function"),

        // conversion
        makeShortTemplate("int(${x})", "function"),
        makeShortTemplate("double(${x})", "function"),
        makeShortTemplate("string(${x})", "function"),

        // reflection
        makeShortTemplate("fullPath()", "function"),
        makeShortTemplate("fullName()", "function"),
        makeShortTemplate("parentIndex()", "function"),
        makeShortTemplate("ancestorIndex(${int})", "function"),

        // xml
        makeShortTemplate("xml(${string}, ${opt_xpath})", "function"),
        makeShortTemplate("xmldoc(${filename}, ${opt_xpath})", "function"),

        // misc
        makeShortTemplate("simTime()", "function"),
    };

    public final static Template[] proposedNedContinuousDistributionsTempl = new Template[] {
        makeShortTemplate("beta(${alpha1}, ${alpha2})", "continuous distribution"),
        makeShortTemplate("cauchy(${a}, ${b})", "continuous distribution"),
        makeShortTemplate("chi_square(${k})", "continuous distribution"),
        makeShortTemplate("erlang_k(${k}, ${mean})", "continuous distribution"),
        makeShortTemplate("exponential(${mean})", "continuous distribution"),
        makeShortTemplate("gamma_d(${alpha}, ${beta})", "continuous distribution"),
        makeShortTemplate("lognormal(${m}, ${w})", "continuous distribution"),
        makeShortTemplate("normal(${mean}, ${stddev})", "continuous distribution"),
        makeShortTemplate("pareto_shifted(${a}, ${b}, ${c})", "continuous distribution"),
        makeShortTemplate("student_t(${i})", "continuous distribution"),
        makeShortTemplate("triang(${a}, ${b}, ${c})", "continuous distribution"),
        makeShortTemplate("truncnormal(${mean}, ${stddev})", "continuous distribution"),
        makeShortTemplate("uniform(${a}, ${b})", "continuous distribution"),
        makeShortTemplate("weibull(${a}, ${b})", "continuous distribution"),
    };

    public final static Template[] proposedNedContinuousDistributionsTemplExt = addRngNumArgument(proposedNedContinuousDistributionsTempl);

    public final static Template[] proposedNedDiscreteDistributionsTempl = new Template[] {
        makeShortTemplate("bernoulli(${p})", "discrete distribution"),
        makeShortTemplate("binomial(${n}, ${p})", "discrete distribution"),
        makeShortTemplate("geometric(${p})", "discrete distribution"),
        makeShortTemplate("intuniform(${a}, ${b})", "discrete distribution"),
        makeShortTemplate("negbinomial(${n}, ${p})", "discrete distribution"),
        makeShortTemplate("poisson(${lambda})", "discrete distribution"),
    };

    public final static Template[] proposedNedDiscreteDistributionsTemplExt = addRngNumArgument(proposedNedDiscreteDistributionsTempl);

    public final static Template[] proposedNedGlobalTempl = new Template[] {
        makeTemplate("simple1", "create simple module",
                "//\n// TODO documentation\n//\n"+
                "simple ${SomeModule}\n{\n"+
                "    parameters:${cursor}\n"+
                "    gates:\n"+
                "}"),
        makeTemplate("simple2", "specialize simple module",
                "//\n// TODO documentation\n//\n"+
                "simple ${SomeModule} extends ${AnotherModule}\n{\n"+
                "    parameters:${cursor}\n"+
                "}"),
        makeTemplate("simple3", "simple module that complies to an interface",
                "//\n// TODO documentation\n//\n"+
                "simple ${SomeModule} like ${SomeInterface}\n{\n"+
                "    parameters:${cursor}\n"+
                "    gates:\n"+
                "}"),
        makeTemplate("module1", "create compound module",
                "//\n// TODO documentation\n//\n"+
                "module ${SomeModule}\n{\n"+
                "    parameters:${cursor}\n"+
                "    gates:\n"+
                "    submodules:\n"+
                "    connections:\n"+
                "}"),
        makeTemplate("module2", "specialize compound module",
                "//\n// TODO documentation\n//\n"+
                "module ${SomeModule} extends ${AnotherModule}\n{\n"+
                "    parameters:${cursor}\n"+
                "}"),
        makeTemplate("module3", "create compound module that complies to an interface",
                "//\n// TODO documentation\n//\n"+
                "module ${SomeModule} like ${SomeInterface}\n{\n"+
                "    parameters:${cursor}\n"+
                "    gates:\n"+
                "    submodules:\n"+
                "    connections:\n"+
                "}"),
        makeTemplate("moduleinterface", "create module interface",
                "//\n// TODO documentation\n//\n"+
                "moduleinterface ${SomeInterface}\n{\n"+
                "    parameters:${cursor}\n"+
                "    gates:\n"+
                "}"),
        makeTemplate("network1", "create network",
                "//\n// TODO documentation\n//\n"+
                "network ${SomeNetwork}\n{\n"+
                "    parameters:${cursor}\n"+
                "    submodules:\n"+
                "    connections:\n"+
                "}"),
        makeTemplate("channel1", "create channel",
                "//\n// TODO documentation\n//\n"+
                "channel ${SomeChannel}\n{\n"+
                "    ${cursor}\n"+
                "}"),
        makeTemplate("channel2", "channel with underlying C++ class", //XXX revise name
                "//\n// TODO documentation\n//\n"+
                "channel ${SomeChannel}\n{\n"+
                "    @class(${classname});${cursor}\n"+
                "}"),
        makeTemplate("channelinterface", "create channel interface",
                "//\n// TODO documentation\n//\n"+
                "channelinterface ${SomeChannelInterface}\n{\n"+
                "    ${cursor}\n"+
                "}"),
        // special compound modules with submodule topologies
        makeTemplate("moduletree", "module with binary tree topology",
                "//\n// Binary tree node\n//\n"+
                "simple ${BinaryTreeNode}\n{\n"+
                "    gates:\n"+
                "        inout ${parent};\n"+
                "        inout ${left};\n"+
                "        inout ${right};\n"+
                "}\n\n"+
                "//\n// TODO documentation\n//\n"+
                "module ${BinaryTree}\n{\n"+
                "    parameters:\n"+
                "        int height = default(5);\n"+
                "    submodules:\n"+
                "        ${node}[2^height-1]: ${BinaryTreeNode};\n"+
                "    connections allowunconnected:\n"+
                "        for i = 0..2^(height-1)-2 {\n"+
                "            ${node}[i].${left} <--> ${node}[2*i+1].${parent};\n"+
                "            ${node}[i].${right} <--> ${node}[2*i+2].${parent};\n"+
                "        }\n"+
                "}"),
        makeTemplate("modulemesh", "module with mesh topology",
                "//\n// Mesh node\n//\n"+
                "simple ${MeshNode}\n{\n"+
                "    gates:\n"+
                "        inout ${up};\n"+
                "        inout ${left};\n"+
                "        inout ${down};\n"+
                "        inout ${right};\n"+
                "}\n\n"+
                "//\n// TODO documentation\n//\n"+
                "module ${Mesh}\n{\n"+
                "    parameters:\n"+
                "        int height = default(4);\n"+
                "        int width = default(6);\n"+
                "    submodules:\n"+
                "        ${node}[height*width]: ${MeshNode};\n"+
                "    connections allowunconnected:\n"+
                "        for i=0..height-1, for j=0..width-1 {\n"+
                "            ${node}[i*width+j].${down} <--> ${node}[(i+1)*width+j].${up} if i!=height-1;\n"+
                "            ${node}[i*width+j].${right} <--> ${node}[i*width+j+1].${left} if j!=width-1;\n"+
                "        }\n"+
                "}"),
        makeTemplate("moduletrimesh", "module with triangle mesh topology",
                "//\n// Triangle mesh node\n//\n" +
                "simple ${TriMeshNode}\n" +
                "{\n" +
                "    gates:\n" +
                "        inout ${w};\n" +
                "        inout ${nw};\n" +
                "        inout ${sw};\n" +
                "        inout ${e};\n" +
                "        inout ${se};\n" +
                "        inout ${ne};\n" +
                "}\n\n" +
                "//\n// TODO documentation\n//\n" +
                "module ${TriMesh}\n{\n" +
                "    parameters:\n" +
                "        int rows = default(3);\n" +
                "        int cols = default(7);\n" +
                "    submodules:\n" +
                "        ${node}[rows*cols]: ${TriMeshNode};\n" +
                "    connections allowunconnected:\n" +
                "        for x=0..cols-1, for y=0..rows-1 {\n" +
                "            ${node}[y*cols+x].${e} <--> ${node}[y*cols+x+1].${w} if x<cols-1;\n" +
                "            ${node}[y*cols+x].${se} <--> ${node}[(y+1)*cols+x].${nw} if y<rows-1;\n" +
                "            ${node}[y*cols+x].${sw} <--> ${node}[(y+1)*cols+x-1].${ne} if x>0 && y<rows-1;\n" +
                "        }\n" +
                "}"),
        makeTemplate("modulehexmesh", "module with hexagonal mesh topology",
                "//\n// Hexagonal mesh node\n//\n"+
                "simple ${HexMeshNode}\n{\n"+
                "    gates:\n"+
                "        inout ${port}[];\n"+
                "}\n\n"+
                "//\n// TODO documentation\n//\n"+
                "module ${HexMesh}\n{\n"+
                "    parameters:\n"+
                "        int rows = default(3);\n"+
                "        int cols = default(3);\n" +
                "        int num = 2*(rows*cols+rows+cols);"+
                "    submodules:\n"+
                "        ${node}[num]: ${HexMeshNode};\n"+
                "    connections:\n"+
                "        for i = 0..num-1 {\n"+
                "            ${node}[i].${port}++ <--> ${node}[i+1].${port}++ if i<num-1 && i%(2*cols+2)!=2*cols;\n"+
                "            ${node}[i].${port}++ <--> ${node}[i+2*cols+1].${port}++ if i<num-2*cols-1 && i%2==0;\n"+
                "        }\n" +
                "}"),
    };

    public final static Template[] proposedNedSubmoduleTempl = new Template[] {
        makeTemplate("submodule1", "submodule",
                "${someSubmodule} : ${SomeModule};"),
        makeTemplate("submodule2", "submodule vector",
                "${someSubmodule}[${size}] : ${SomeModule};"),
        makeTemplate("submodule3", "submodule with variable type",
                "${someSubmodule} : <${stringParameter}> like ${SomeInterface};"),
        makeTemplate("submodule4", "submodule with parameter settings",
                "${someSubmodule} : ${SomeModule} {\n"+
                "    ${cursor}\n"+
                "}"),
        makeTemplate("submodule5", "submodule with gate size settings",
                "${someSubmodule} : ${SomeModule} {\n"+
                "    gates:${cursor}\n"+
                "}"),
    };

    public final static Template[] proposedNedConnectionTempl = new Template[] {
        makeTemplate("connection1", "two one-way connections",
                "${mod1}.${outgate1} --> ${mod2}.${ingate2};\n"+
                "${mod1}.${ingate1} <-- ${mod2}.${outgate2};"),
        makeTemplate("connection2", "a single two-way connection (inout gates)",
                "${mod1}.${inoutgate1} <--> ${mod2}.${inoutgate2};"),
        makeTemplate("connection3", "connecting an inout gate with an input and an output",
                "${mod1}.${outgate} --> ${mod2}.${inoutgate}$$i;\n"+
                "${mod1}.${ingate} <-- ${mod2}.${inoutgate}$$o;"),
        makeTemplate("connection4", "connections to parent (2x one-way)",
                "${mod}.${outgate} --> ${parentout};\n"+
                "${mod}.${ingate} <-- ${parentin};"),
        makeTemplate("connection5", "connection with predefined channel",
                "${mod1}.${inout1} <--> ${SomeChannel} <--> ${mod2}.${inout2};"),
        makeTemplate("connection6", "connection with channel parameters",
                "${mod1}.${inout1} <--> {delay=${delay}; datarate=${txrate}; error=${ber};} <--> ${mod2}.${inout2};"),
        makeTemplate("connection7", "connection with predefined channel parameterized",
                "${mod1}.${inout1} <--> ${SomeChannel} {${customParam}=${value};} <--> ${mod2}.${inout2};"),
        //XXX with [], with ++, with "where", connection templates...

        // templates based on for loops
        makeTemplate("forloop", "an empty for loop",
                "for ${i} = ${start}..${end} {\n"+
                "    ${selection}${cursor}\n"+
                "}"),
        makeTemplate("forbus", "connect modules with a bus",
                "for ${i} = 0..${n}-1 {\n"+
                "    ${node1}.${out}[${i}] --> ${node2}.${in}[${i}];\n"+
                "}${cursor}"),
        makeTemplate("forstar", "connect modules in a star topology",
                "for ${i} = 0..${n}-1 {\n"+
                "    ${central}.${out}[${i}] --> ${satellite}[${i}].${in};\n"+
                "}${cursor}"),
        makeTemplate("forchain", "connect modules in a chain topology",
                "for ${i} = 0..${n}-2 {\n"+
                "    ${node}[${i}].${out} --> ${node}[${i}+1].${in};\n"+
                "}${cursor}"),
        makeTemplate("forfullgraph", "connect modules in a full graph topology",
                "for ${i} = 0..${n}-1, for ${j}=0..${n}-1 {\n"+
                "    ${node}[${i}].${out}[${j}] --> ${node}[${j}].${in}[${i}] if ${i}!=${j};\n"+
                "}${cursor}"),
        makeTemplate("forrandomgraph", "connect modules in a random graph topology",
                "for ${i} = 0..${n}-1, for ${j}=0..${n}-1 {\n"+
                "    ${node}[${i}].${out}[${j}] --> ${node}[${j}].${in}[${i}] if ${i}!=${j} && uniform(0,1) < ${connectedness};\n"+
                "}${cursor}"),
        makeTemplate("forring", "connect modules in a ring topology",
                "for ${i} = 0..${n}-1 {\n"+
                "    ${node}[${i}].${out} --> ${node}[(${i}+1) % ${n}].${in};\n"+
                "}${cursor}"),
    };

    /**
     * Utility function for creating a one-line template
     */
    public static Template makeShortTemplate(String pattern, String description) {
        String name = pattern.replaceAll("\\$\\{(.*?)\\}", "$1");  // remove ${} from parameters
        pattern = pattern.replace("\n", "\n${indent}");
        return new Template(name, description, DEFAULT_NED_CONTEXT_TYPE, pattern, false);
    }

    private static Template[] addRngNumArgument(Template[] templates) {
        Template[] result = new Template[templates.length];
        for (int i=0; i<templates.length; i++) {
            String pattern = templates[i].getPattern();
            Assert.isTrue(pattern.endsWith(")"));
            pattern = pattern.substring(0, pattern.length()-1) + ", ${rngNum})";
            result[i] = makeShortTemplate(pattern, templates[i].getDescription());
        }
        return result;
    }

    /**
     * Utility function for creating a multi-line template
     */
    public static Template makeTemplate(String name, String description, String pattern) {
        pattern = pattern.replace("\n", "\n${indent}");
        return new Template(name, description, DEFAULT_NED_CONTEXT_TYPE, pattern, false);
    }

}
