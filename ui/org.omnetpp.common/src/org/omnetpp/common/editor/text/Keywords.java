/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  'License' for details on this and other legal matters.
*--------------------------------------------------------------*/

package org.omnetpp.common.editor.text;

/**
 * NED/MSG keywords, for syntax highlighting and completion
 *
 * @author Andras
 */
public class Keywords {
    public static final String NED_IDENT_REGEX = "[a-zA-Z\u0080-\uffff_][a-zA-Z\u0080-\uffff_0-9]*";  // note rudimentary support for internationalized identifiers, bug #419

    public static final String MSG_IDENT_REGEX = "[a-zA-Z_][a-zA-Z_0-9]*";  // MSG files don't support international identifiers (because C++ doesn't either)

    public final static String[] DOC_TODO = { "CHECKME", "FIXME", "TBD", "TODO" };

    public final static String[] DOC_TAGS = { "a", "b", "body", "br", "center", "caption", "code",
            "dd", "dfn", "dl", "dt", "em", "form", "font", "hr", "h1", "h2", "h3", "i", "input",
            "img", "li", "meta", "multicol", "ol", "p", "pre", "small", "span", "strong", "sub", "sup",
            "table", "td", "th", "tr", "tt", "kbd", "u", "ul", "var" };

    public final static String[] DOC_KEYWORDS = { "author", "bug", "date", "see", "since", "todo",
            "version", "warning" , "page", "titlepage"};

    public final static String[] NED_GATE_TYPES = { "inout", "input", "output" };

    public final static String[] NED_PARAM_TYPES = { "bool", "double", "int", "string", "object", "xml" };

    public final static String[] NED_PARAM_TYPE_MODIFIERS = { "volatile" };

    public final static String[] NED_TYPE_KEYWORDS = concat(NED_GATE_TYPES, NED_PARAM_TYPES, NED_PARAM_TYPE_MODIFIERS);

    public final static String[] NED_SPECIAL_KEYWORDS = { "-->", "<--", "<-->", ".." };

    public final static String[] NED_COMPONENTTYPE_KEYWORDS = { "channel", "channelinterface", "simple", "module", "network", "moduleinterface" };

    public final static String[] NED_SECTION_KEYWORDS = { "parameters", "gates", "types", "submodules", "connections" };

    public final static String[] NED_OTHER_KEYWORDS = { "allowunconnected", "extends", "for", "if", "import", "like", "package", "property" };

    public final static String[] NED_NONEXPR_KEYWORDS = concat(NED_COMPONENTTYPE_KEYWORDS, NED_SECTION_KEYWORDS, NED_OTHER_KEYWORDS);

    public final static String[] NED_EXPR_KEYWORDS = { "sizeof", "const", "default", "ask", "this", "index", "typename" };

    public final static String[] BOOL_CONSTANTS = { "false", "true", "undefined", "nan", "inf", "null", "nullptr" };
    
    public final static String[] MSG_TYPE_KEYWORDS = { // types recognized by the message compiler
    		"abstract", "bool", "char", "short", "int", "long", "float", "double", "unsigned", "string", "simtime_t", 
    		"int8_t", "int16_t", "int32_t", "int64_t", "uint8_t", "uint16_t", "uint32_t", "uint64_t",
    		"int8", "int16", "int32", "int64", "uint8", "uint16", "uint32", "uint64" };  

    public final static String[] MSG_KEYWORDS = {"cplusplus", "namespace", "struct", "message", "packet", "class", "enum", "extends", "import" };

    public final static String[] MSG_SECTION_KEYWORDS = { "properties", "fields"};


    public final static String[] NED_FUNCTIONS = { "acos", "ancestorIndex", "asin", "atan", "atan2", "bernoulli",
            "beta", "binomial", "cauchy", "ceil", "chi_square", "choose", "contains", "convertUnit", "cos", "dropUnit",
            "endsWith", "erlang_k", "exp", "expand", "exponential", "fabs", "firstAvailable", "floor", "fmod", "fullName",
            "fullPath", "gamma_d", "genk_exponential", "genk_intuniform", "genk_normal", "genk_truncnormal",
            "genk_uniform", "geometric", "hypergeometric", "hypot", "indexOf", "intuniform", "intuniformexl", "length",
            "log", "log10", "lognormal", "max", "min", "negbinomial", "normal", "parentIndex", "pareto_shifted",
            "poisson", "pow", "replace", "replaceFirst", "replaceUnit", "select", "simTime", "sin", "sqrt",
            "startsWith", "student_t", "substring", "substringAfter", "substringAfterLast", "substringBefore",
            "substringBeforeLast", "tail", "tan", "toLower", "toUpper", "triang", "trim", "truncnormal", "uniform",
            "unitOf", "weibull", "xml", "xmldoc" };

    public final static String[] NED_RESERVED_WORDS = concat(NED_TYPE_KEYWORDS, NED_SPECIAL_KEYWORDS, NED_NONEXPR_KEYWORDS, NED_EXPR_KEYWORDS, BOOL_CONSTANTS);

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
