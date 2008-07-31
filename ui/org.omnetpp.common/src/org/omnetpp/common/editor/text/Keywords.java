package org.omnetpp.common.editor.text;

/**
 * NED/MSG keywords, for syntax highlighting and completion
 * 
 * @author Andras
 */
public class Keywords {
	
	public final static String[] DOC_TODO = { "CHECKME", "FIXME", "TBD", "TODO" };

	public final static String[] DOC_TAGS = { "a", "b", "body", "br", "center", "caption", "code",
			"dd", "dfn", "dl", "dt", "em", "form", "font", "hr", "h1", "h2", "h3", "i", "input",
			"img", "li", "meta", "multicol", "ol", "p", "small", "span", "strong", "sub", "sup",
			"table", "td", "th", "tr", "tt", "kbd", "ul", "var" };

	public final static String[] DOC_KEYWORDS = { "author", "bug", "date", "see", "since", "todo",
			"version", "warning" };

	public final static String[] GATE_TYPES = { "inout", "input", "output" };

	public final static String[] PARAM_TYPES = { "bool", "double", "int", "string", "xml" };
	
	public final static String[] PARAM_TYPE_MODIFIERS = { "volatile" };

	public final static String[] TYPE_KEYWORDS = concat(GATE_TYPES, PARAM_TYPES, PARAM_TYPE_MODIFIERS);

	public final static String[] SPECIAL_KEYWORDS = { "-->", "<--", "<-->", ".." };

	public final static String[] NEDTYPE_KEYWORDS = { "channel", "channelinterface", "simple", "module", "network", "moduleinterface" };
	
	public final static String[] SECTION_KEYWORDS = { "parameters", "gates", "types", "submodules", "connections" };

	public final static String[] OTHER_KEYWORDS = { "allowunconnected", "extends", "for", "if", "import", "like", "package", "property" };

	public final static String[] NONEXPR_KEYWORDS = concat(NEDTYPE_KEYWORDS, SECTION_KEYWORDS, OTHER_KEYWORDS);

	public final static String[] EXPR_KEYWORDS = { "sizeof", "const", "default", "this", "index", "typename", "xmldoc" };

	public final static String[] CONSTANTS = { "false", "true" };
	
	public final static String[] MSG_TYPE_KEYWORDS = {"abstract", "readonly", "bool", "char", "short", "int", "long", "double", "unsigned", "string"};

	public final static String[] MSG_KEYWORDS = {"cplusplus", "struct", "message", "packet", "class", "noncobject", "enum", "extends"};

	public final static String[] MSG_SECTION_KEYWORDS = {  "properties", "fields"};

	
	public final static String[] FUNCTIONS = { "acos", "asin", "atan", "atan2", "bernoulli",
			"beta", "binomial", "cauchy", "ceil", "chi_square", "cos", "erlang_k", "exp",
			"exponential", "fabs", "floor", "fmod", "gamma_d", "genk_exponential",
			"genk_intuniform", "genk_normal", "genk_truncnormal", "genk_uniform", "geometric",
			"hypergeometric", "hypot", "intuniform", "log", "log10", "lognormal", "max", "min",
			"negbinomial", "normal", "pareto_shifted", "poisson", "pow", "sin", "sqrt",
			"student_t", "tan", "triang", "truncnormal", "uniform", "weibull" };

	public final static String[] RESERVED_WORDS = concat(TYPE_KEYWORDS, SPECIAL_KEYWORDS, NONEXPR_KEYWORDS, EXPR_KEYWORDS, CONSTANTS);
	/**
	 * Utility function: concatenates string arrays.
	 */
	public static final String[] concat(String[] ...arrays) {
		int total = 0;
		for (String[] array : arrays)
			total += array.length;
		String[] result = new String[total];
		int pos = 0;
		for (String[] array : arrays) {
			System.arraycopy(array, 0, result, pos, array.length);
			pos += array.length;
		}
		return result;
	}
}
