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

/**
 * This class contains all the possible keywords for syntax highlighting and context assist functions.
 *
 * @author rhornig
 *
 */
//XXX TODO rename, revise, possibly remove...
public class InifileTextEditorHelper {
	public static final String DEFAULT_CONTEXT_TYPE= "org.omnetpp.ned.editor.text.default"; //$NON-NLS-1$

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
    public static class WhitespaceDetector implements IWhitespaceDetector {

        public boolean isWhitespace(char character) {
            return Character.isWhitespace(character);
        }
    }

    /**
     * Detects words separated by whitespace.
     */
    public static class SpaceSeparatedWordDetector implements IWordDetector {

        public boolean isWordStart(char c) {
            return isWordPart(c);
        }

        public boolean isWordPart(char c) {
            return c!=' ' && c!='\t' && c!='\f' && c!='\n' && c!='\r';
        }
    }

    /**
     * Word detector for ini files. Key names may contain hyphens
     * ("sim-time-limit"), so we recognize it as a word part as well.
     */
    public static class InifileWordDetector implements IWordDetector {

        public boolean isWordStart(char character) {
            return Character.isLetter(character) || character == '_';
        }

        public boolean isWordPart(char character) {
            return Character.isLetterOrDigit(character) || character == '_' || character == '-';
        }
    }

    // word lists for syntax highlighting
    // TODO these are both for NED and MSG files. Once a separate MSG editor is done keywords should be split
    public final static String[] highlightPrivateDocTodo = { "CHECKME", "FIXME", "TBD", "TODO" };
    public final static String[] highlightDocKeywords = { "author", "bug", "date", "see", "since", "todo", "version", "warning" };
    public final static String[] highlightNedFunctions = { "acos", "asin", "atan", "atan2", "bernoulli", "beta", "binomial", "cauchy", "ceil", "chi_square", "const", "cos", "default", "erlang_k", "exp", "exponential", "fabs", "floor", "fmod", "gamma_d", "genk_exponential", "genk_intuniform", "genk_normal", "genk_truncnormal", "genk_uniform", "geometric", "hypergeometric", "hypot", "intuniform", "log", "log10", "lognormal", "max", "min", "negbinomial", "normal", "pareto_shifted", "poisson", "pow", "sin", "sizeof", "sqrt", "student_t", "tan", "triang", "truncnormal", "uniform", "weibull", "xmldoc" };
    public final static String[] highlightConstants = { "false", "true" };

    // word lists for completion
    public final static String[] proposedPrivateDocTodo = highlightPrivateDocTodo;
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
    	return new Template(name, description, InifileTextEditorHelper.DEFAULT_CONTEXT_TYPE, pattern, false);
    }

    // whitespace and word detectors for tokenization
    public final static WhitespaceDetector whitespaceDetector = new WhitespaceDetector();
    public final static IWordDetector spaceSeparatedWordDetector = new SpaceSeparatedWordDetector();
    public final static IWordDetector inifileWordDetector = new InifileWordDetector();

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
