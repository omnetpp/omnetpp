"""
    omnetpp.sphinx
    ~~~~~~~~~~~~~~

   Syntax highlighting code for NED, MSG and INI files

   Use defaults provided by highlight directive for code role.
   inline_highlight_respect_highlight = False

   Highlight also normal literals like :code:`literal`
   inline_highlight_literals = False

"""

from pygments.lexers.c_cpp import CLexer, CppLexer
from pygments.lexer import RegexLexer, include, bygroups, using, this, inherit, default, words
from pygments.token import Name, Keyword, Comment, Text, Operator, String, Number, Punctuation, Error

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = "default"

#####
class NedLexer(RegexLexer):
    name = 'ned'
    filenames = ['*.ned']

    #: optional Comment or Whitespace
    _ws = r'(?:\s|//.*?\n|/[*].*?[*]/)+'

    # The trailing ?, rather than *, avoids a geometric performance drop here.
    #: only one /* */ style comment
    _ws1 = r'\s*(?:/[*].*?[*]/\s*)?'

    tokens = {
        'whitespace': [
            (r'\n', Text),
            (r'\s+', Text),
            (r'\\\n', Text),  # line continuation
            (r'//(\n|[\w\W]*?[^\\]\n)', Comment.Single),
            (r'/(\\\n)?[*][\w\W]*?[*](\\\n)?/', Comment.Multiline),
            # Open until EOF, so no ending delimeter
            (r'/(\\\n)?[*][\w\W]*', Comment.Multiline),
        ],
        'statements': [
            (r'(L?)(")', bygroups(String.Affix, String), 'string'),
            (r"(L?)(')(\\.|\\[0-7]{1,3}|\\x[a-fA-F0-9]{1,2}|[^\\\'\n])(')",
             bygroups(String.Affix, String.Char, String.Char, String.Char)),
            (r'(true|false)\b', Name.Builtin),
            (r'(<-->|-->|<--|\.\.)', Keyword),
            (r'(bool|double|int|xml)\b', Keyword.Type),
            (r'(inout|input|output)\b', Keyword.Type),
            (r'(\d+\.\d*|\.\d+|\d+)[eE][+-]?\d+[LlUu]*', Number.Float),
            (r'(\d+\.\d*|\.\d+|\d+[fF])[fF]?', Number.Float),
            (r'0x[0-9a-fA-F]+[LlUu]*', Number.Hex),
            (r'#[0-9a-fA-F]+[LlUu]*', Number.Hex),
            (r'0[0-7]+[LlUu]*', Number.Oct),
            (r'\d+[LlUu]*', Number.Integer),
            (r'\*/', Error),
            (r'[~!%^&*+=|?:<>/-]', Operator),
            (r'[()\[\],.]', Punctuation),
            (words(("channel", "channelinterface", "simple", "module", "network", "moduleinterface"), suffix=r'\b'), Keyword),
            (words(("parameters", "gates", "types", "submodules", "connections"), suffix=r'\b'), Keyword),
            (words(("volatile", "allowunconnected", "extends", "for", "if", "import", "like", "package", "property"), suffix=r'\b'), Keyword),
            (words(("sizeof", "const", "default", "ask", "this", "index", "typename", "xmldoc"), suffix=r'\b'), Keyword),
            (words(("acos", "asin", "atan", "atan2", "bernoulli","beta", "binomial", "cauchy", "ceil", "chi_square", "cos", "erlang_k", "exp","exponential", "fabs", "floor", "fmod", "gamma_d", "genk_exponential","genk_intuniform", "genk_normal", "genk_truncnormal", "genk_uniform", "geometric","hypergeometric", "hypot", "intuniform", "log", "log10", "lognormal", "max", "min","negbinomial", "normal", "pareto_shifted", "poisson", "pow", "simTime", "sin", "sqrt","student_t", "tan", "triang", "truncnormal", "uniform", "weibull", "xml", "xmldoc"), suffix=r'\b'), Name.Builtin),
            ('@[a-zA-Z_]\w*', Name.Builtin),
            ('[a-zA-Z_]\w*', Name),
        ],
        'root': [
            include('whitespace'),
            # functions
            (r'((?:[\w*\s])+?(?:\s|[*]))'  # return arguments
             r'([a-zA-Z_]\w*)'             # method name
             r'(\s*\([^;]*?\))'            # signature
             r'([^;{]*)(\{)',
             bygroups(using(this), Name.Function, using(this), using(this),
                      Punctuation),
             'function'),
            # function declarations
            (r'((?:[\w*\s])+?(?:\s|[*]))'  # return arguments
             r'([a-zA-Z_]\w*)'             # method name
             r'(\s*\([^;]*?\))'            # signature
             r'([^;]*)(;)',
             bygroups(using(this), Name.Function, using(this), using(this),
                      Punctuation)),
            default('statement'),
        ],
        'statement': [
            include('whitespace'),
            include('statements'),
            ('[{}]', Punctuation),
            (';', Punctuation, '#pop'),
        ],
        'function': [
            include('whitespace'),
            include('statements'),
            (';', Punctuation),
            (r'\{', Punctuation, '#push'),
            (r'\}', Punctuation, '#pop'),
        ],
        'string': [
            (r'"', String, '#pop'),
            (r'\\([\\abfnrtv"\']|x[a-fA-F0-9]{2,4}|'
             r'u[a-fA-F0-9]{4}|U[a-fA-F0-9]{8}|[0-7]{1,3})', String.Escape),
            (r'[^\\"\n]+', String),  # all other characters
            (r'\\\n', String),  # line continuation
            (r'\\', String),  # stray backslash
        ]
    }

#####
class MsgLexer(CppLexer):
    name = 'msg'
    filenames = ['*.msg']
    mimetypes = ['text/x-msg']

    tokens = {
        'statements': [
            (words(("import","cplusplus", "namespace", "struct", "message",
                "packet", "class", "noncobject", "enum", "extends"), suffix=r'\b'), Keyword),
            (words(("properties", "fields"), suffix=r'\b'), Keyword),
            (r'(abstract|readonly|bool|char|short|int|long|double|unsigned|string)\b', Keyword.Type),
            (r'[~!%^&*+=|?:<>/@-]', Operator),
            inherit,
        ],
    }

#####
class IniLexer(RegexLexer):
    name = 'ini'
    filenames = ['*.ini']
    mimetypes = ['text/x-ini']

    tokens = {
        'root': [
            (r'[;#].*$', Comment.Single),
            (r'\s+', Text),
            (r'\[.*?\]', Keyword),
            (r'(.*?)([ \t]*)(=)([ \t]*)([^#\n]*(?:\n[ \t].+)*)',
             bygroups(Name.Attribute, Text, Operator, Text, String)),
        ],
    }

    def analyse_text(text):
        npos = text.find('\n')
        if npos < 3:
            return False
        return text[0] == '[' and text[npos-1] == ']'

