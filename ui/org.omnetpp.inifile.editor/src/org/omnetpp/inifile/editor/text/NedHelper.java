package org.omnetpp.inifile.editor.text;

import org.eclipse.jface.text.TextAttribute;
import org.eclipse.jface.text.rules.IToken;
import org.eclipse.jface.text.rules.IWhitespaceDetector;
import org.eclipse.jface.text.rules.IWordDetector;
import org.eclipse.jface.text.rules.Token;
import org.eclipse.jface.text.templates.Template;
import org.eclipse.swt.SWT;
import org.eclipse.swt.graphics.Color;
import org.eclipse.swt.widgets.Display;
import org.omnetpp.inifile.editor.text.assist.NedContextType;

/**
 * This class contains all the possible keywords for syntax highlighting and context assist functions.
 *
 * @author rhornig
 *
 */
//XXX TODO rename, revise, possibly remove...
public class NedHelper {

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
     * Detector for normal NED keywords (may start with letter or _ and contain letter number or _)
     */
    public static class NedWordDetector implements IWordDetector {

        public boolean isWordStart(char character) {
            return Character.isLetter(character) || character == '_';
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
    static class NedAtWordDetector implements IWordDetector {

        public boolean isWordStart(char c) {
            return (c == '@');
        }

        public boolean isWordPart(char c) {
            return Character.isLetter(c);
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
    public final static String[] proposedNedGlobalStartingKeywords = { "channel", "channel withcppclass", "channelinterface", "import", "interface", "module", "network", "package", "property", "simple"};
    public final static String[] proposedNedSectionNameKeywords = {"connections:", "connections allowunconnected:", "gates:", "parameters:", "submodules:", "types:"};
    public final static String[] proposedNedConnsKeywords = {"allowunconnected"};
    public final static String[] proposedNedInheritanceKeywords = {"extends", "like"};
    public final static String[] proposedNedOtherKeywords = {"if", "index", "this", "typename", "for"};
    public final static String[] proposedNedFunctions = { "acos", "asin", "atan", "atan2", "bernoulli", "beta", "binomial", "cauchy", "ceil", "chi_square", "const", "cos", "default", "erlang_k", "exp", "exponential", "fabs", "floor", "fmod", "gamma_d", "genk_exponential", "genk_intuniform", "genk_normal", "genk_truncnormal", "genk_uniform", "geometric", "hypergeometric", "hypot", "intuniform", "log", "log10", "lognormal", "max", "min", "negbinomial", "normal", "pareto_shifted", "poisson", "pow", "sin", "sizeof", "sqrt", "student_t", "tan", "triang", "truncnormal", "uniform", "weibull", "xmldoc" };
    public final static String[] proposedConstants = { "false", "true" };
    public final static String[] proposedNedComponentPropertyNames = {"display(...)"}; //XXX offer as template! check what gets actually supported! also: "recordstats", "kernel", ... 
    public final static String[] proposedNedParamPropertyNames = {"prompt(...)", "choice(...)", "classname(...)"}; //XXX check this list before release  
    public final static String[] proposedNedGatePropertyNames = {"labels(...)", "inlabels(...)", "outlabels(...)"}; //XXX check this list before release

    
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

    private static Template makeShortTemplate(String pattern, String description) {
    	String name = pattern.replaceAll("\\$\\{(.*?)\\}", "$1");  // remove ${} from parameters
    	return new Template(name, description, NedContextType.DEFAULT_CONTEXT_TYPE, pattern, false);
    }

    // whitespace and word detectors for tokenization
    public final static NedWhitespaceDetector nedWhitespaceDetector = new NedWhitespaceDetector();
    public final static NedWordDetector nedWordDetector = new NedWordDetector();
    public final static NedSpecialWordDetector nedSpecialWordDetector = new NedSpecialWordDetector();
    public final static NedAtWordDetector nedAtWordDetector = new NedAtWordDetector();

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
