package org.omnetpp.ned.editor.text;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWhitespaceDetector;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.ned.editor.text.assist.NedContextType;

/**
 * This class contains all the possible keywords for syntax highlighting and context assist functions.
 *
 * @author rhornig
 */
public final class NedHelper {

    /**
     * Convenience method, to return a system default color. Color constants come from SWT class e.g. SWT.COLOR_RED
     * @param color
     * @return
     */
    public static Color getColor(int color) {
        return Display.getDefault().getSystemColor(color);
    }

    /**
     * A generic white space detector
     */
    public static class NedWhitespaceDetector implements IWhitespaceDetector {

        public boolean isWhitespace(char character) {
            return Character.isWhitespace(character);
        }
    }

    /**
     * Detector for normal NED keywords (may start with letter, @ or _ and contain letter number or _)
     */
    public static class NedWordDetector implements IWordDetector {

        public boolean isWordStart(char character) {
            return Character.isLetter(character) || character == '_' || character == '@';
        }

        public boolean isWordPart(char character) {
            return Character.isLetterOrDigit(character) || character == '_';
        }
    }

    /**
     * Detector for extreme NED keywords (in: out: --> <-- .. ...) where the keyword may contain special chars
     */
    public static class NedSpecialWordDetector implements IWordDetector {

        public boolean isWordStart(char c) {
            return Character.isLetter(c) || c == '-' || c == '<' || c == '>' || c == '.';
        }

        public boolean isWordPart(char c) {
            return isWordStart(c) || c == ':';
        }
    }

    /**
     * Detects kewords that athere to the @whatewer syntax, staring with @ and continuing with letters only.
     */
    public static class NedAtWordDetector implements IWordDetector {

        public boolean isWordStart(char c) {
            return (c == '@');
        }

        public boolean isWordPart(char c) {
            return Character.isLetter(c);
        }
    }

    /**
     * Detects kewords that look like an XML tag.
     */
    public static class NedDocTagDetector implements IWordDetector {

        public boolean isWordStart(char c) {
            return (c == '<');
        }

        public boolean isWordPart(char c) {
            return Character.isLetter(c) || c == '/' || c == '>';
        }
    }

    // word lists for syntax highlighting
    // TODO these are both for NED and MSG files. Once a separate MSG editor is done keywords should be split
    public final static String[] highlightPrivateDocTodo = { "CHECKME", "FIXME", "TBD", "TODO" };
    public final static String[] highlightDocTags = { "a", "b", "body", "br", "center", "caption", "code", "dd", "dfn", "dl", "dt", "em", "form", "font", "hr", "h1", "h2", "h3", "i", "input", "img", "li", "meta", "multicol", "ol", "p", "small", "span", "strong", "sub", "sup", "table", "td", "th", "tr", "tt", "kbd", "ul", "var" };
    public final static String[] highlightDocKeywords = { "author", "bug", "date", "see", "since", "todo", "version", "warning" };
    public final static String[] highlightNedTypes = { "bool", "double", "volatile", "inout", "input", "int", "output", "string", "xml" };
    public final static String[] highlightNedSpecialKeywords = { "-->", "<--", "<-->", ".." };
    public final static String[] highlightNedKeywords = { "allowunconnected", "channel", "channelinterface", "connections", "extends", "for", "gates", "if", "import", "index", "interface", "like", "module", "network", "package", "parameters", "property", "simple", "submodules", "this", "typename", "types", "withcppclass" };
    public final static String[] highlightNedFunctions = { "acos", "asin", "atan", "atan2", "bernoulli", "beta", "binomial", "cauchy", "ceil", "chi_square", "const", "cos", "default", "erlang_k", "exp", "exponential", "fabs", "floor", "fmod", "gamma_d", "genk_exponential", "genk_intuniform", "genk_normal", "genk_truncnormal", "genk_uniform", "geometric", "hypergeometric", "hypot", "intuniform", "log", "log10", "lognormal", "max", "min", "negbinomial", "normal", "pareto_shifted", "poisson", "pow", "sin", "sizeof", "sqrt", "student_t", "tan", "triang", "truncnormal", "uniform", "weibull", "xmldoc" };
    public final static String[] highlightConstants = { "false", "true" };

    // word lists for completion
    public final static String[] proposedPrivateDocTodo = highlightPrivateDocTodo;
    public final static String[] proposedDocTags = highlightDocTags;
    public final static String[] proposedDocKeywords = highlightDocKeywords;
    public final static String[] proposedNedBaseParamTypes = { "bool", "double", "int", "string", "xml" };
    public final static String[] proposedNedParamTypes = { "bool", "double", "int", "string", "xml", "volatile bool", "volatile double", "volatile int", "volatile string", "volatile xml" };
    public final static String[] proposedNedGateTypes = { "inout", "input", "output" };
    public final static String[] proposedNedTopLevelKeywords = { "import", "network", "package", "property"};
    public final static String[] proposedNedTypeDefinerKeywords = { "channel", "channel withcppclass", "channelinterface", "interface", "module", "simple"};
    public final static String[] proposedNedSectionNameKeywords = {"connections:", "connections allowunconnected:", "gates:", "parameters:", "submodules:", "types:"};
    public final static String[] proposedNedConnsKeywords = {"allowunconnected"};
    public final static String[] proposedNedOtherExpressionKeywords = {"index", "this"};
    public final static String[] proposedConstants = { "false", "true" };
    

    private static Template makeShortTemplate(String pattern, String description) {
        String name = pattern.replaceAll("\\$\\{(.*?)\\}", "$1");  // remove ${} from parameters
        return new Template(name, description, NedContextType.DEFAULT_CONTEXT_TYPE, pattern, false);
    }

    private static Template makeTemplate(String name, String description, String pattern) {
        return new Template(name, description, NedContextType.DEFAULT_CONTEXT_TYPE, pattern, false);
    }
            
    public final static Template[] proposedNedComponentPropertyTempl = {
        makeShortTemplate("@display(\"i=${icon}\");", "property") 
    }; // XXX check what gets actually supported! also: "recordstats", "kernel", ...  
    public final static Template[] proposedNedParamPropertyTempl = {
        makeShortTemplate("@prompt(\"${message}\")", "property"), 
        makeShortTemplate("@choice(${value1}, ${value2})", "property"), 
        makeShortTemplate("@classname(${className})", "property"), 
        makeShortTemplate("@unit(${unitName})", "property"), 
    }; //XXX check this list before release  
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
    	makeShortTemplate("xmldoc(${filename}, ${opt_xpath})", "operator"),
    };
    public final static Template[] proposedNedFunctionsTempl = new Template[] {
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
    };
    public final static Template[] proposedNedDistributionsTempl = new Template[] {
    	// continuous
    	makeShortTemplate("beta(${alpha1}, ${alpha2}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("cauchy(${a}, ${b}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("chi_square(${k}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("erlang_k(${k}, ${mean}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("exponential(${mean}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("gamma_d(${alpha}, ${beta}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("lognormal(${m}, ${w}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("normal(${mean}, ${stddev}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("pareto_shifted(${a}, ${b}, ${c}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("student_t(${i}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("triang(${a}, ${b}, ${c}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("truncnormal(${mean}, ${stddev}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("uniform(${a}, ${b}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("weibull(${a}, ${b}, ${opt_rngnum})", "distribution"),

    	// discrete
    	makeShortTemplate("bernoulli(${p}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("binomial(${n}, ${p}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("geometric(${p}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("intuniform(${a}, ${b}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("negbinomial(${n}, ${p}, ${opt_rngnum})", "distribution"),
    	makeShortTemplate("poisson(${lambda}, ${opt_rngnum})", "distribution"),
    };

    public final static Template[] proposedNedGlobalTempl = new Template[] {
    	makeTemplate("import", "import NED file", 
    			"import \"${FileName}\";\n"),
        makeTemplate("simple1", "create simple module",
                "//\n// ${description_of_the_simple_module}\n//\n"+
                "// @author ${user}\n//\n"+
        		"simple ${SomeModule} {\n"+
        		"    parameters:\n"+
        		"    gates:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("simple2", "specialize simple module",
        		"//\n// ${description_of_the_simple_module}\n//\n"+
                "// @author ${user}\n//\n"+
        		"simple ${SomeModule} extends ${AnotherModule} {\n"+
        		"    parameters:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("simple3", "simple module complying an interface",
        		"//\n// ${description_of_the_simple_module}\n//\n"+
                "// @author ${user}\n//\n"+
        		"simple ${SomeModule} like ${SomeInterface} {\n"+
        		"    parameters:\n"+
        		"    gates:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("module1", "create compound module",
        		"//\n// ${description_of_the_module}\n//\n"+
                "// @author ${user}\n//\n"+
        		"module ${SomeModule} {\n"+
        		"    parameters:\n"+
        		"    gates:\n"+
        		"    submodules:\n"+
        		"    connections:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("module2", "specialize compound module",
        		"//\n// ${description_of_the_module}\n//\n"+
                "// @author ${user}\n//\n"+
        		"module ${SomeModule} extends ${AnotherModule} {\n"+
        		"    parameters:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("module3", "compound module complying an interface",
        		"//\n// ${description_of_the_module}\n//\n"+
                "// @author ${user}\n//\n"+
        		"module ${SomeModule} like ${SomeInterface} {\n"+
        		"    parameters:\n"+
        		"    gates:\n"+
        		"    submodules:\n"+
        		"    connections:\n"+
        		"}\n${cursor}\n"),
		makeTemplate("interface", "create module interface",
				"//\n// ${description_of_the_interface}\n//\n"+
                "// @author ${user}\n//\n"+
				"interface ${SomeInterface} {\n"+
				"    parameters:\n"+
				"    gates:\n"+
				"}\n${cursor}\n"),
        makeTemplate("network1", "create network",
        		"//\n// ${description_of_the_network}\n//\n"+
                "// @author ${user}\n//\n"+
        		"network ${SomeNetwork} {\n"+
        		"    parameters:\n"+
        		"    submodules:\n"+
        		"    connections:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("network2", "create network instantiating a module",
        		"//\n// ${description_of_the_network}\n//\n"+
                "// @author ${user}\n//\n"+
        		"network ${SomeNetwork} extends ${SomeModule} {\n"+
        		"    parameters:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("channel1", "create channel",
        		"//\n// ${description_of_the_channel}\n//\n"+
                "// @author ${user}\n//\n"+
        		"channel ${SomeChannel} {\n"+
        		"    parameters:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("channel2", "channel with underlying C++ class", //XXX revise name
        		"//\n// ${description_of_the_channel}\n//\n"+
                "// @author ${user}\n//\n"+
        		"channel withcppclass ${SomeChannel} {\n"+
        		"    parameters:\n"+
        		"}\n${cursor}\n"),
        makeTemplate("channelinterface", "create channel interface",
        		"//\n// ${description_of_the_channel_interface}\n//\n"+
                "// @author ${user}\n//\n"+
        		"channelinterface ${SomeChannelInterface} {\n"+
        		"    parameters:\n"+
        		"    gates:\n"+
        		"}\n${cursor}\n"),
    };
    public final static Template[] proposedNedSubmoduleTempl = new Template[] {
        makeTemplate("submodule1", "submodule",
        		"        ${someSubmodule} : ${SomeModule};\n"),
        makeTemplate("submodule2", "submodule vector",
		        "        ${someSubmodule}[${size}] : ${SomeModule};\n"),
		makeTemplate("submodule3", "submodule with variable type",
		        "        ${someSubmodule} : <${stringParameter}> like ${SomeInterface};\n"),
        makeTemplate("submodule4", "submodule with parameter settings",
        		"        ${someSubmodule} : ${SomeModule} {\n"+
        		"            //...\n"+
        		"        }\n${cursor}\n"),
        makeTemplate("submodule5", "submodule with gate size settings",
        		"        ${someSubmodule} : ${SomeModule} {\n"+
        		"            gates:\n"+
        		"        }\n${cursor}\n"),
    };
    public final static Template[] proposedNedConnectionTempl = new Template[] {
        makeTemplate("connection1", "two one-way connections",
        		"        ${mod1}.${outgate1} --> ${mod2}.${ingate2};\n"+
        		"        ${mod1}.${ingate1} <-- ${mod2}.${outgate2};\n"),
        makeTemplate("connection2", "a single two-way connection (inout gates)",
				"        ${mod1}.${inoutgate1} <--> ${mod2}.${inoutgate2};\n"),
        makeTemplate("connection3", "connecting an inout gate with an input and an output",
        		"        ${mod1}.${outgate} --> ${mod2}.${inoutgate}$$i;\n"+
        		"        ${mod1}.${ingate} <-- ${mod2}.${inoutgate}$$o;\n"),
        makeTemplate("connection4", "connections to parent (2x one-way)",
        		"        ${mod}.${outgate} --> ${parentout};\n"+
        		"        ${mod}.${ingate} <-- ${parentin};\n"),
        makeTemplate("connection5", "connection with predefined channel",
        		"        ${mod1}.${inout1} <--> ${SomeChannel} <--> ${mod2}.${inout2};\n"),
        makeTemplate("connection6", "connection with channel parameters",
        		"        ${mod1}.${inout1} <--> {delay=${delay}; datarate=${txrate}; error=${ber}} <--> ${mod2}.${inout2};\n"),
        makeTemplate("connection7", "connection with predefined channel parameterized",
        		"        ${mod1}.${inout1} <--> ${SomeChannel} {${customParam}=${value};} <--> ${mod2}.${inout2};\n"),
        //XXX with [], with ++, with "where", connection templates...

        // templates based on for loops
        makeTemplate("forloop", "an empty for loop",
                "       for ${i} = ${start}..${end} {\n"+
                "           ${selection}\n"+
                "       };${cursor}"),
        makeTemplate("forbus", "connect modules with a bus",
                "       for ${i} = 0..${n}-1 {\n"+
                "           ${node1}.${out}[${i}] --> ${node2}.${in}[${i}];\n"+
                "       };${cursor}"),
        makeTemplate("forstar", "connect modules in a star topology",
                "       for ${i} = 0..${n}-1 {\n"+
                "           ${central}.${out}[${i}] --> ${satellite}[${i}].${in};\n"+
                "       };${cursor}"),
        makeTemplate("forbintree", "connect modules in a binary tree topology",
                "       for ${i} = 0..2^(${height}-1)-2 {\n"+
                "           ${node}[${i}].${downleft} --> ${node}[2*${i}+1].${fromupper};\n"+
                "           ${node}[${i}].${downright} --> ${node}[2*${i}+2].${fromupper};\n"+
                "       };${cursor}"),
        makeTemplate("forchain", "connect modules in a chain topology",
                "       for ${i} = 0..${n}-2 {\n"+
                "           ${node}[${i}].${out} --> ${node}[${i}+1].${in};\n"+
                "       };${cursor}"),
        makeTemplate("forfullgraph", "connect modules in a full graph topology",
                "       for ${i} = 0..${n}-1, for ${j}=0..${n}-1 {\n"+
                "           ${node}[${i}].${out}[${j}] --> ${node}[${j}].${in}[${i}] if ${i}!=${j};\n"+
                "       };${cursor}"),
        makeTemplate("forrandomgraph", "connect modules in a random graph topology",
                "       for ${i} = 0..${n}-1, for ${j}=0..${n}-1 {\n"+
                "           ${node}[${i}].${out}[${j}] --> ${node}[${j}].${in}[${i}] if ${i}!=${j} && uniform(0,1) < ${connectedness};\n"+
                "       };${cursor}"),
        makeTemplate("formesh", "connect modules in a mesh topology",
                "       for ${i}=0..${height}-1, for ${j}=0..${width}-1 {\n"+
                "           ${node}[${i}*${width}+${j}].${down} --> ${node}[(${i}+1)*${width}+${j}].${up} if ${i}!=${height}-1;\n"+
                "           ${node}[${i}*${width}+${j}].${right} --> ${node}[${i}*${width}+${j}+1].${left} if ${j}!=${width}-1;\n"+
                "       };${cursor}"),
        makeTemplate("fortrigrid", "connect modules in a triangle grid topology",
                "       for i = 0 .. ${rows}*${cols}-1 {\n"+
                "           ${node}[i].${n} <-- ${node}[i-2*${cols}].${s} if i+1>2*${cols};\n"+
                "           ${node}[i].${nw} <-- ${node}[${cols}-(i/${cols}+1)%2].${se} if i%(2*${cols})!=0 && i+1>${cols};\n"+
                "           ${node}[i].${sw} <-- ${node}[i+${cols}-1+(i/${cols})%2].${ne} if i%(2*${cols})!=0 && i<${rows}*${cols}-${cols};\n"+
                "       };${cursor}"),
        makeTemplate("forring", "connect modules in a ring topology",
                "       for ${i} = 0..${n}-1 {\n"+
                "           ${node}[${i}].${out} --> ${node}[(${i}+1) % ${n}].${in};\n"+
                "       };${cursor}"),
    };

    // whitespace and word detectors for tokenization
    public final static NedWhitespaceDetector nedWhitespaceDetector = new NedWhitespaceDetector();
    public final static NedWordDetector nedWordDetector = new NedWordDetector();
    public final static NedSpecialWordDetector nedSpecialWordDetector = new NedSpecialWordDetector();
    public final static NedAtWordDetector nedAtWordDetector = new NedAtWordDetector();
    public final static NedDocTagDetector nedDocTagDetector = new NedDocTagDetector();

    // tokens for syntax highlighting
    // TODO these styles should be configurable
    public final static IToken docDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GRAY), null, SWT.ITALIC));
    public final static IToken docKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));
    public final static IToken docTagToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_RED)));

    public final static IToken docPrivateDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLUE), null, SWT.ITALIC));
    public final static IToken docPrivateTodoToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE)));

    public final static IToken codeDefaultToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
    public final static IToken codeKeywordToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_RED), null, SWT.BOLD));
    public final static IToken codeFunctionToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_MAGENTA), null, SWT.BOLD));
    public final static IToken codeTypeToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_BLUE), null, SWT.BOLD));
    public final static IToken codeIdentifierToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK)));
    public final static IToken codePropertyToken = new Token(new TextAttribute(getColor(SWT.COLOR_BLACK), null, SWT.BOLD));
    public final static IToken codeStringToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));
    public final static IToken codeNumberToken = new Token(new TextAttribute(getColor(SWT.COLOR_DARK_GREEN)));

}
