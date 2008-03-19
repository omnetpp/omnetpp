#
# generate classes from NED DTD
#

#
# NOTE: some REDHAT Perl versions are broken, and cannot recognize the \s,
# so this code had to be patched up!!!
#
# If you have a good perl version and want readable REs in here, do these
# replacements throughout:
#
# [ \t\n]   --> \s
# [^ \t\n]  --> [^\s]
#
$verbose = 0;

$filename = $ARGV[0];
$filename =~ /./ || die "*** usage: prog dtdfile importpackage package outdir\n";

$javaimportedpackage = $ARGV[1];
$javaimportedpackage =~ /./ || die "*** usage: prog dtdfile importpackage package outdir\n";

$javapackage = $ARGV[2];
$javapackage =~ /./ || die "*** usage: prog dtdfile importpackage package outdir\n";

$outdir = $ARGV[3];
$outdir =~ /./ || die "*** usage: prog dtdfile importpackage package outdir\n";


$buf="";
open(IN,$filename) || die "*** cannot open input file $filename";
while (<IN>) {
    s/\r$//; # cygwin/mingw perl does not do CR/LF translation
    $buf .= $_;
}
$buf =~ s/<!--.*?-->//sg;

close(IN);

$elements = ();

#
# Parse ELEMENTs in DTD
#
while ($buf =~ s/(<!ELEMENT[ \t\n]+([^ \t\n]+)[ \t\n]+(.*?)>)//s) {
    $decl = $1;
    $element = $2;
    push(@elements,$element);

    $elementsdef = $3;
    $elementsdef =~ s/[ \t\n]//g;
    $elementdef{$element} = $elementsdef;
    $elementsdef =~ s/[*+?,|()]/ /g;  # simplify it a bit...
    $elementsdef =~ s/[ \t\n]+/ /g;  # simplify it a bit...
    @children = ();
    @childmult = ();
    foreach $child (split(' ',$elementsdef)) {
        if ($child eq 'EMPTY') {next;}
        if ($child eq 'ANY') {next;}
        $mult = 1;
        if ($child =~ s/([*+?])$//) {
           $mult = $1;
        }
        push(@children,$child);
        push(@childmult,$mult);
    }
    $elementdecl{$element} = $decl;
    $children{$element} = [ @children ];
    $childmult{$element} = [ @childmult ];

    print "$element: $elementdef{$element}\n" if ($verbose);

}

#
# Parse ATTLISTs in DTD
#
while ($buf =~ s/(<!ATTLIST[ \t\n]+([^ \t\n]+)[ \t\n]+(.*?)>)//s) {
    $decl = $1;
    $element = $2;
    $attlist = $3."\n";

    @attnames = ();
    @atttypes = ();
    @attvals = ();
    while ($attlist =~ s/^([^\n]*)\n//s) {
        $line = $1;
        if ($line =~ /^[ \t\n]+$/) {
            next;
        }
        if ($line =~ /^[ \t\n]*([^ \t\n]+)[ \t\n]+([^ \t\n]+)[ \t\n]+([^ \t\n]+)[ \t\n]*$/) {
            push(@attnames,$1);
            push(@atttypes,$2);
            push(@attvals,$3);
        } elsif ($line ne '') {
            print STDERR "unrecognized line in DTD: '$line'\n";
        }
    }

    $attlistdecl{$element} = $decl;
    $att_names{$element} = [ @attnames ];
    $att_types{$element} = [ @atttypes ];
    $att_vals{$element} =  [ @attvals ];
}


#
# Create class names, enum names, variable names, etc.
#
foreach $element (@elements)
{
    @attnames = @{$att_names{$element}};
    @atttypes = @{$att_types{$element}};
    @attvals = @{$att_vals{$element}};
    @children = @{$children{$element}};
    @childmult = @{$childmult{$element}};

    $attcount = $#attnames+1;
    $childcount = $#children+1;

    # element class
    $elementclass = $element."Element";
    $elementclass =~ s/-(.)/-uc($1)/eg;
    $elementclass =~ s/[^a-zA-Z0-9_]//g;
    $elementclass =~ s/(.)(.*)/uc($1).$2/e;
    $elementclass{$element} = $elementclass;

    # enum name
    $enumname = 'NED_'.uc($element);
    $enumname =~ s/-/_/g;
    $enumname{$element} = $enumname;

    # children
    @childvars = ();
    for ($i=0; $i<$childcount; $i++)
    {
        $varname = @children[$i];
        $varname =~ s/-(.)/-uc($1)/eg;
        $varname =~ s/[^a-zA-Z0-9_]//g;
        $varname =~ s/(.)(.*)/uc($1).$2/e;
        push(@childvars,$varname);
    }
    $childvars{$element} = [ @childvars ];

    # attribute names, types
    @varnames = ();
    @ucvarnames = ();
    @attnameconsts = ();
    @argtypes = ();
    @enumnames = ();
    for ($i=0; $i<$attcount; $i++)
    {
        $varname = $attnames[$i];
        $varname =~ s/-(.)/-uc($1)/eg;
        $varname =~ s/[^a-zA-Z0-9_]//g;
        $varname =~ s/(.)(.*)/lc($1).$2/e;

        $ucvarname = $varname;
        $ucvarname =~ s/(.)(.*)/uc($1).$2/e;

        $attnameconst = "ATT_".uc($attnames[$i]);
        $attnameconst =~ s/-/_/eg;

        if ($varname eq "package") {$varname = "package_";}

        $enumname = "";
        if ($atttypes[$i] eq '(true|false)') {
           $argtype = "boolean";
        }
        elsif ($atttypes[$i] =~ /^\(.*\)$/) {
           $argtype = "int";
           if ($atttypes[$i] eq '(input|output|inout)') {
              $enumname = "gatetype";
           }
           elsif ($atttypes[$i] eq '(double|int|string|bool|xml)') {
              $enumname = "partype";
           }
           elsif ($atttypes[$i] eq '(double|int|string|bool|spec)') {
              $enumname = "littype";
           }
           elsif ($atttypes[$i] eq '(l2r|r2l|bidir)') {
              $enumname = "arrowdir";
           }
           elsif ($atttypes[$i] eq '(i|o)') {
              $enumname = "subgate";
           }

           else {
              die "Error: unrecognized enum $atttypes[$i]\n";
           }
        }
        else {
           $argtype = "String";
        }
        push(@varnames,$varname);
        push(@ucvarnames,$ucvarname);
        push(@attnameconsts,$attnameconst);
        push(@argtypes,$argtype);
        push(@enumnames,$enumname);
    }

    $att_varnames{$element} = [ @varnames ];
    $att_ucvarnames{$element} = [ @ucvarnames ];
    $att_attnameconsts{$element} = [ @attnameconsts ];
    $att_argtypes{$element} = [ @argtypes ];
    $att_enumnames{$element} = [ @enumnames ];
}

#------------------------------------------------------------------------
#
# Data classes
#

foreach $element (@elements)
{
    $elementclass = $elementclass{$element};

    $elementclassshort = $elementclass;
    $elementclassshort =~ s/Element$//;

    $javafile = "$outdir/$elementclass.java";
    open(JAVA,">$javafile") || die "*** cannot open output file $javafile";

    @attnames = @{$att_names{$element}};
    @atttypes = @{$att_types{$element}};
    @attvals = @{$att_vals{$element}};
    @children = @{$children{$element}};
    @childmult = @{$childmult{$element}};

    @childvars = @{$childvars{$element}};
    @varnames = @{$att_varnames{$element}};
    @ucvarnames = @{$att_ucvarnames{$element}};
    @attnameconsts = @{$att_attnameconsts{$element}};
    @argtypes = @{$att_argtypes{$element}};
    @enumnames = @{$att_enumnames{$element}};

    $attcount = $#attnames+1;
    $childcount = $#children+1;

    $decl = $elementdecl{$element}."\n".$attlistdecl{$element}."\n";
    $decl =~ s/^/ * /mg;

    print JAVA "package $javapackage;\n\n";
    print JAVA "import $javaimportedpackage.*;\n\n";
    print JAVA "/**\n";
    print JAVA " * GENERATED CLASS. Represents the &lt;$element&gt; XML element. DTD declaration:\n";
    print JAVA " * \n";
    print JAVA " * <pre>\n";
    print JAVA $decl;
    print JAVA " * </pre>\n";
    print JAVA " */\n";
    print JAVA "public class $elementclass extends NEDElement implements NEDElementTags, NEDElementConstants\n";
    print JAVA "{\n";
    for ($i=0; $i<$attcount; $i++)
    {
        $vartype = $argtypes[$i];
        print JAVA "    private $vartype $varnames[$i];\n";
    }
    print JAVA "\n";
    for ($i=0; $i<$attcount; $i++)
    {
        print JAVA "    public static final String $attnameconsts[$i] = \"$attnames[$i]\";\n";
    }
    print JAVA "\n";
    print JAVA "    /** You should never create an instance directly, use factory class instead */\n";
    print JAVA "    protected $elementclass() {\n";
    print JAVA "        applyDefaults();\n";
    print JAVA "    }\n\n";
    print JAVA "    /** You should never create an instance directly, use factory class instead */\n";
    print JAVA "    protected $elementclass(INEDElement parent) {\n";
    print JAVA "        super(parent);\n";
    print JAVA "        applyDefaults();\n";
    print JAVA "    }\n\n";
    print JAVA "    public String getTagName() {\n";
    print JAVA "        return \"$element\";\n";
    print JAVA "    }\n\n";
    print JAVA "    public static String getStaticTagName() {\n";
    print JAVA "        return \"$element\";\n";
    print JAVA "    }\n\n";
    print JAVA "    public int getTagCode() {\n";
    print JAVA "        return $enumname{$element};\n";
    print JAVA "    }\n\n";
    print JAVA "    public int getNumAttributes() {\n";
    print JAVA "        return $attcount;\n";
    print JAVA "    }\n\n";
    print JAVA "    public String getAttributeName(int k) {\n";
    print JAVA "        switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        print JAVA "            case $i: return $attnameconsts[$i];\n";
    }
    print JAVA "            default: throw new IllegalArgumentException();\n";
    print JAVA "        }\n";
    print JAVA "    }\n\n";
    print JAVA "    public String getAttribute(int k) {\n";
    print JAVA "        switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        if ($argtypes[$i] eq "String") {
            print JAVA "            case $i: return get$ucvarnames[$i]();\n";
        }
        elsif ($argtypes[$i] eq "boolean") {
            print JAVA "            case $i: return boolToString(get$ucvarnames[$i]());\n";
        }
        elsif ($argtypes[$i] eq "int") {
            print JAVA "            case $i: return enumToString(get$ucvarnames[$i](), $enumnames[$i]_vals, $enumnames[$i]_nums, $enumnames[$i]_n);\n";
        }
        else {
            die "invalid argtype '$argtypes[$i]'";
        }
    }
    print JAVA "            default: throw new IllegalArgumentException();\n";
    print JAVA "        }\n";
    print JAVA "    }\n\n";
    print JAVA "    public void setAttribute(int k, String val) {\n";
    print JAVA "        switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        if ($argtypes[$i] eq "String") {
            print JAVA "            case $i: set$ucvarnames[$i](val); break;\n";
        }
        elsif ($argtypes[$i] eq "boolean") {
            print JAVA "            case $i: set$ucvarnames[$i](stringToBool(val)); break;\n";
        }
        elsif ($argtypes[$i] eq "int") {
            print JAVA "            case $i: set$ucvarnames[$i](stringToEnum(val, $enumnames[$i]_vals, $enumnames[$i]_nums, $enumnames[$i]_n)); break;\n";
        }
        else {
            die "invalid argtype '$argtypes[$i]'";
        }
    }
    print JAVA "            default: throw new IllegalArgumentException();\n";
    print JAVA "        }\n";
    print JAVA "    }\n\n";
    print JAVA "    public String getAttributeDefault(int k) {\n";
    print JAVA "        switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        $attval = $attvals[$i];
        $attval =~ s/\&#10;/\\n/g;
        if ($attval eq "#IMPLIED") {
            print JAVA "            case $i: return \"\";\n";
        } elsif ($attval eq "#REQUIRED") {
            print JAVA "            case $i: return null;\n";
        } else {
            print JAVA "            case $i: return $attval;\n";
        }
    }
    print JAVA "            default: throw new IllegalArgumentException();\n";
    print JAVA "        }\n";
    print JAVA "    }\n\n";

    for ($i=0; $i<$attcount; $i++)
    {
        print JAVA "    public $argtypes[$i] get$ucvarnames[$i]() {\n";
        print JAVA "        return $varnames[$i];\n";
        print JAVA "    }\n\n";

        if ($argtypes[$i] eq "int") {
            print JAVA "    public void set$ucvarnames[$i]($argtypes[$i] val) {\n";
            print JAVA "        validateEnum(val, $enumnames[$i]_vals, $enumnames[$i]_nums, $enumnames[$i]_n);\n";
            print JAVA "        $argtypes[$i] oldVal = $varnames[$i];\n";
            print JAVA "        $varnames[$i] = val;\n";
            print JAVA "        fireAttributeChanged($attnameconsts[$i], val, oldVal);\n";
            print JAVA "    }\n\n";
        } else {
            print JAVA "    public void set$ucvarnames[$i]($argtypes[$i] val) {\n";
            print JAVA "        $argtypes[$i] oldVal = $varnames[$i];\n";
            print JAVA "        $varnames[$i] = val;\n";
            print JAVA "        fireAttributeChanged($attnameconsts[$i], val, oldVal);\n";
            print JAVA "    }\n\n";
        }

    }
    print JAVA "    public $elementclass getNext${elementclassshort}Sibling() {\n";
    print JAVA "        return ($elementclass)getNextSiblingWithTag($enumname{$element});\n";
    print JAVA "    }\n\n";

    for ($i=0; $i<$childcount; $i++)
    {
        print JAVA "    public $elementclass{$children[$i]} getFirst$childvars[$i]Child() {\n";
        print JAVA "        return ($elementclass{$children[$i]})getFirstChildWithTag($enumname{$children[$i]});\n";
        print JAVA "    }\n\n";
    }
    print JAVA "}\n\n";
}


#------------------------------------------------------------------------
#
# Java util class
#

$javafile = "$outdir/NEDElementTags.java";
open(JAVA,">$javafile") || die "*** cannot open output file $javafile";

print JAVA "package $javapackage;\n\n";
print JAVA "/**\n";
print JAVA " * GENERATED CLASS.\n";
print JAVA " */\n";
print JAVA "public interface NEDElementTags\n";
print JAVA "{\n";

$i = 1;
foreach $element (@elements)
{
    $elementclass = $elementclass{$element};
    print JAVA "    public static final int $enumname{$element} = $i;\n";
    $i++;
}

print JAVA "}\n\n";


#------------------------------------------------------------------------
#
# Factory class
#

$javafile = "$outdir/NEDElementFactory.java";
open(JAVA,">$javafile") || die "*** cannot open output file $javafile";

print JAVA "package $javapackage;\n\n";
print JAVA "import $javaimportedpackage.*;\n\n";
print JAVA "/**\n";
print JAVA " * GENERATED CLASS.\n";
print JAVA " */\n";
print JAVA "public class NEDElementFactory implements NEDElementTags\n";
print JAVA "{\n";
print JAVA "    private static NEDElementFactory instance;\n";
print JAVA "\n";
print JAVA "    public static NEDElementFactory getInstance() {\n";
print JAVA "        return instance;\n";
print JAVA "    }\n";
print JAVA "\n";
print JAVA "    public static void setInstance(NEDElementFactory inst) {\n";
print JAVA "        instance = inst;\n";
print JAVA "    }\n";
print JAVA "\n";
print JAVA "    public INEDElement createElement(String tagname) {\n";
print JAVA "        return createElement(tagname, null);\n";
print JAVA "    }\n\n";
print JAVA "    public INEDElement createElement(String tagname, INEDElement parent) {\n";
foreach $element (@elements)
{
    print JAVA "        if (tagname.equals($elementclass{$element}.getStaticTagName()))\n";
    print JAVA "            return new $elementclass{$element}(parent);\n";
}
print JAVA "        else\n";
print JAVA "            throw new RuntimeException(\"invalid tagname \"+tagname);\n";
print JAVA "    }\n\n";

print JAVA "    public INEDElement createElement(int tagcode) {\n";
print JAVA "        return createElement(tagcode, null);\n";
print JAVA "    }\n\n";

print JAVA "    public INEDElement createElement(int tagcode, INEDElement parent) {\n";
foreach $element (@elements)
{
    print JAVA "        if (tagcode==$enumname{$element})\n";
    print JAVA "            return new $elementclass{$element}(parent);\n";
}
print JAVA "        else\n";
print JAVA "            throw new RuntimeException(\"invalid tagcode \"+tagcode);\n";
print JAVA "    }\n\n";
print JAVA "}\n\n";

#------------------------------------------------------------------------
#
# Validator class
#

$javafile = "$outdir/AbstractNEDValidator.java";
open(JAVA,">$javafile") || die "*** cannot open output file $javafile";

print JAVA "package $javapackage;\n\n";
print JAVA "import $javaimportedpackage.*;\n\n";
print JAVA "/**\n";
print JAVA " * GENERATED CLASS. Base class for NED validators.\n";
print JAVA " */\n";
print JAVA "public abstract class AbstractNEDValidator implements NEDElementTags\n";
print JAVA "{\n";
print JAVA "    abstract public void validate(INEDElement node);\n\n";
print JAVA "    protected void validateElement(INEDElement node) {\n";
print JAVA "        switch (node.getTagCode()) {\n";
foreach $element (@elements)
{
    print JAVA "            case $enumname{$element}: validateElement(($elementclass{$element}) node); break;\n";
}
print JAVA "        }\n";
print JAVA "    }\n\n";
foreach $element (@elements)
{
    print JAVA "    abstract protected void validateElement($elementclass{$element} node);\n";
}
print JAVA "}\n\n";

