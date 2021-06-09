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
$filename || die "*** missing <filename> argument\n";

$lang = $ARGV[1];
($lang eq 'msg' || $lang eq 'ned') || die "*** missing or invalid <language> argument, must be 'ned' or 'msg'\n";

$buf="";
open(IN,$filename) || die "*** cannot open input file $filename";
while (<IN>) {
    s/\r$//; # cygwin/mingw perl does not do CR/LF translation
    $buf .= $_;
}
$buf =~ s/<!--.*?-->//sg;

close(IN);

$ned = $lang;
$Ned = ucfirst($lang);
$NED = uc($lang);

$shortfilename = $filename;
$shortfilename =~ s!.*/!!;

$hfile = "${ned}elements.h";
$ccfile = "${ned}elements.cc";
$validatorhfile = "${ned}validator.h";
$validatorccfile = "${ned}validator.cc";
$dtdvalidatorbasehfile = "${ned}dtdvalidatorbase.h";
$dtdvalidatorhfile = "${ned}dtdvalidator.h";
$dtdvalidatorccfile = "${ned}dtdvalidator.cc";

$copyright = "//==========================================================================
// Part of the OMNeT++/OMNEST Discrete Event Simulation System
//
// Generated from $shortfilename by dtdclassgen.pl
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license\' for details on this and other legal matters.
*--------------------------------------------------------------*/

//
// THIS IS A GENERATED FILE, DO NOT EDIT!
//

";

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
    $elementclass = $element;
    $elementclass =~ s/-(.)/-uc($1)/eg;
    $elementclass =~ s/[^a-zA-Z0-9_]//g;
    $elementclass =~ s/(.)(.*)/uc($1).$2/e;
    $elementclass{$element} = $elementclass."Element";
    $shortelementclass{$element} = $elementclass;

    # enum name
    $enumname = $NED . '_'.uc($element);
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
    @attrids = ();
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

        $attrid = uc($attnames[$i]);
        $attrid =~ s/-/_/g;
        $attrid =~ s/[^A-Z0-9_]//g;
        $attrid = "ATT_" . $attrid;

        $enumname = "";
        if ($atttypes[$i] eq '(true|false)') {
           $argtype = "bool";
        }
        elsif ($atttypes[$i] =~ /^\(.*\)$/) {
           $argtype = "int";
           if ($atttypes[$i] eq '(input|output|inout)') {
              $enumname = "gatetype";
           }
           elsif ($atttypes[$i] eq '(double|int|string|bool|object|xml)') {
              $enumname = "partype";
           }
           elsif ($atttypes[$i] eq '(double|quantity|int|bool|string|spec)') {
              $enumname = "littype";
           }
           elsif ($atttypes[$i] eq '(i|o)') {
              $enumname = "subgate";
           }

           else {
              die "Error: unrecognized enum $atttypes[$i]\n";
           }
        }
        else {
           $argtype = "const char *";
        }
        push(@varnames,$varname);
        push(@ucvarnames,$ucvarname);
        push(@attrids,$attrid);
        push(@argtypes,$argtype);
        push(@enumnames,$enumname);
    }

    $att_varnames{$element} = [ @varnames ];
    $att_ucvarnames{$element} = [ @ucvarnames ];
    $att_attrids{$element} = [ @attrids ];
    $att_argtypes{$element} = [ @argtypes ];
    $att_enumnames{$element} = [ @enumnames ];
}

foreach $element (('files', 'comment', 'property', 'property-key', 'literal', 'import', 'unknown')) {
    $sharedElements{$element} = 1;
}


#------------------------------------------------------------------------
#
# Data classes
#

open(H,">$hfile") || die "*** cannot open output file $hfile";
open(CC,">$ccfile") || die "*** cannot open output file $ccfile";

print H "$copyright\n";
print H "#ifndef __OMNETPP_NEDXML_${NED}ELEMENTS_H\n";
print H "#define __OMNETPP_NEDXML_${NED}ELEMENTS_H\n\n";
print H "#include \"astnode.h\"\n\n";
print H "#include \"nedelements.h\"\n" if ($ned ne "ned");
print H "namespace omnetpp {\n";
print H "namespace nedxml {\n\n";

print CC "$copyright\n";
print CC "#include <cstring>\n";
print CC "#include \"errorstore.h\"\n";
print CC "#include \"exception.h\"\n";
print CC "#include \"$hfile\"\n\n";
print CC "namespace omnetpp {\n";
print CC "namespace nedxml {\n\n";

# forward declarations
foreach $element (@elements)
{
    print H "class $elementclass{$element};\n";
}
print H "\n\n";

# tag codes
print H "/**\n";
print H " * Tag codes\n";
print H " *\n";
print H " * \@ingroup Data\n";
print H " */\n";
print H "enum ${Ned}TagCode {\n";
print H "    NED_NULL = 0,  // 0 is reserved\n" if ($ned eq 'ned');
print H "    dummy = 100,\n" if ($ned eq 'msg');
foreach $element (@elements)
{
    next if ($ned eq 'msg' && exists($sharedElements{$element}));
    print H "    $enumname{$element},\n";
}
if ($ned eq 'msg') {
    foreach $element (keys(%sharedElements)) {
        my $nedEnumName = $enumname{$element};
        $nedEnumName =~ s/^MSG_/NED_/;
        print H "    $enumname{$element} = $nedEnumName,\n";
    }
}

print H "};\n\n";

if ($ned eq "ned") {
    print H "// Note: zero *must* be a valid value for all enums, because that gets set in the ctor if there's not default\n";
    print H "enum {GATETYPE_NONE, GATETYPE_INPUT, GATETYPE_OUTPUT, GATETYPE_INOUT};\n";
    print H "enum {PARTYPE_NONE, PARTYPE_DOUBLE, PARTYPE_INT, PARTYPE_STRING, PARTYPE_BOOL, PARTYPE_OBJECT, PARTYPE_XML};\n";
    print H "enum {LIT_DOUBLE, LIT_QUANTITY, LIT_INT, LIT_STRING, LIT_BOOL, LIT_SPEC};\n";
    print H "enum {SUBGATE_NONE, SUBGATE_I, SUBGATE_O};\n";
    print H "\n";

    print CC "static const char *gatetype_vals[] = {\"\", \"input\", \"output\", \"inout\"};\n";
    print CC "static int gatetype_nums[] = {GATETYPE_NONE, GATETYPE_INPUT, GATETYPE_OUTPUT, GATETYPE_INOUT};\n";
    print CC "static const int gatetype_n = 4;\n";
    print CC "\n";
    print CC "static const char *partype_vals[] = {\"\", \"double\", \"int\", \"string\", \"bool\", \"object\", \"xml\"};\n";
    print CC "static int partype_nums[] = {PARTYPE_NONE, PARTYPE_DOUBLE, PARTYPE_INT, PARTYPE_STRING, PARTYPE_BOOL, PARTYPE_OBJECT, PARTYPE_XML};\n";
    print CC "static const int partype_n = 7;\n";
    print CC "\n";
    print CC "static const char *littype_vals[] = {\"double\", \"quantity\", \"int\", \"string\", \"bool\", \"spec\"};\n";
    print CC "static int littype_nums[] = {LIT_DOUBLE, LIT_QUANTITY, LIT_INT, LIT_STRING, LIT_BOOL, LIT_SPEC};\n";
    print CC "static const int littype_n = 6;\n";
    print CC "\n";
    print CC "static const char *subgate_vals[] = {\"\", \"i\", \"o\"};\n";
    print CC "static int subgate_nums[] = {SUBGATE_NONE, SUBGATE_I, SUBGATE_O};\n";
    print CC "static const int subgate_n = 3;\n";
    print CC "\n";
}

foreach $element (@elements)
{
    next if ($ned eq 'msg' && exists($sharedElements{$element}));

    $elementclass = $elementclass{$element};
    $shortelementclass = $shortelementclass{$element};

    @attnames = @{$att_names{$element}};
    @atttypes = @{$att_types{$element}};
    @attvals = @{$att_vals{$element}};
    @children = @{$children{$element}};
    @childmult = @{$childmult{$element}};

    @childvars = @{$childvars{$element}};
    @varnames = @{$att_varnames{$element}};
    @ucvarnames = @{$att_ucvarnames{$element}};
    @attrids = @{$att_attrids{$element}};
    @argtypes = @{$att_argtypes{$element}};
    @enumnames = @{$att_enumnames{$element}};

    $attcount = $#attnames+1;
    $childcount = $#children+1;

    $decl = $elementdecl{$element}."\n".$attlistdecl{$element}."\n";
    $decl =~ s/^/ * /mg;
    $decl =~ s/</\\</mg;
    $decl =~ s/>/\\>/mg;
    $decl =~ s/#/\\#/mg;
    $decl =~ s/ *$//mg;

    print H "/**\n";
    print H " * \@brief GENERATED CLASS. Represents the \\<$element\\> XML element in memory.\n";
    print H " *\n";
    print H " * DTD declaration:\n";
    print H " * <pre>\n";
    print H $decl;
    print H " * </pre>\n";
    print H " *\n";
    print H " * \@ingroup Data\n";
    print H " */\n";
    print H "class NEDXML_API $elementclass : public ASTNode\n";
    print H "{\n";
    if ($attcount > 0) {
		print H "  public:\n";
		print H "    enum { ";
		for ($i=0; $i<$attcount; $i++)
		{
		    print H "$attrids[$i], ";
		}
		print H "};\n";
    }
    print H "  private:\n";
    for ($i=0; $i<$attcount; $i++)
    {
        $vartype = ($argtypes[$i] eq "const char *") ? "std::string" : $argtypes[$i];
        print H "    $vartype $varnames[$i];\n";
    }
    print H "  public:\n";
    print H "    /** \@name Constructors, destructor */\n";
    print H "    //\@{\n";
    print H "    $elementclass();\n";
    print H "    $elementclass(ASTNode *parent);\n";
    print H "    virtual ~$elementclass() {}\n";
    print H "    //\@}\n";
    print H "\n";
    print H "    /** \@name Redefined ASTNode methods, incl. generic access to attributes */\n";
    print H "    //\@{\n";
    print H "    virtual const char *getTagName() const override {return \"$element\";}\n";
    print H "    virtual int getTagCode() const override {return $enumname{$element};}\n";
    print H "    virtual int getNumAttributes() const override;\n";
    print H "    virtual const char *getAttributeName(int k) const override;\n";
    print H "    virtual const char *getAttribute(int k) const override;\n";
    print H "    virtual const char *getAttribute(const char *name) const override {return ASTNode::getAttribute(name);} // needed because of a C++ language quirk\n";
    print H "    virtual void setAttribute(int k, const char *val) override;\n";
    print H "    virtual void setAttribute(const char *name, const char *val) override {ASTNode::setAttribute(name, val);} // ditto\n";
    print H "    virtual const char *getAttributeDefault(int k) const override;\n";
    print H "    virtual $elementclass *dup() const override;\n";
    print H "    //\@}\n";

    print H "\n";
    print H "    /** \@name Typed access to attributes, children and siblings */\n";
    print H "    //\@{\n";
    for ($i=0; $i<$attcount; $i++)
    {
        if ($argtypes[$i] eq "const char *") {
            print H "    $argtypes[$i] get$ucvarnames[$i]() const  {return $varnames[$i].c_str();}\n";
        } else {
            print H "    $argtypes[$i] get$ucvarnames[$i]() const  {return $varnames[$i];}\n";
        }
        if ($argtypes[$i] eq "int") {
            print H "    void set$ucvarnames[$i]($argtypes[$i] val);\n";
            print CC "void $elementclass\:\:set$ucvarnames[$i]($argtypes[$i] val)\n";
            print CC "{\n";
            print CC "    validateEnum(val, $enumnames[$i]_vals, $enumnames[$i]_nums, $enumnames[$i]_n);\n";
            print CC "    $varnames[$i] = val;\n";
            print CC "}\n\n";
        } else {
            print H "    void set$ucvarnames[$i]($argtypes[$i] val)  {$varnames[$i] = val;}\n";
        }

    }

    print H "\n";
    print H "    virtual $elementclass *getNext${shortelementclass}Sibling() const;\n";
    for ($i=0; $i<$childcount; $i++)
    {
        print H "    virtual $elementclass{$children[$i]} *getFirst$childvars[$i]Child() const;\n";
    }
    print H "    //\@}\n";
    print H "};\n\n";

    print CC "$elementclass\:\:$elementclass()\n";
    print CC "{\n";
    for ($i=0; $i<$attcount; $i++)
    {
        print CC "    $varnames[$i] = false;\n" if ($argtypes[$i] eq "bool");
        print CC "    $varnames[$i] = 0;\n" if ($argtypes[$i] eq "int");
    }
    print CC "    applyDefaults();\n";
    print CC "}\n\n";

    print CC "$elementclass\:\:$elementclass(ASTNode *parent) : ASTNode(parent)\n";
    print CC "{\n";
    for ($i=0; $i<$attcount; $i++)
    {
        print CC "    $varnames[$i] = false;\n" if ($argtypes[$i] eq "bool");
        print CC "    $varnames[$i] = 0;\n" if ($argtypes[$i] eq "int");
    }
    print CC "    applyDefaults();\n";
    print CC "}\n\n";

    print CC "int $elementclass\:\:getNumAttributes() const\n";
    print CC "{\n";
    print CC "    return $attcount;\n";
    print CC "}\n\n";

    print CC "const char *$elementclass\:\:getAttributeName(int k) const\n";
    print CC "{\n";
    print CC "    switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        print CC "        case $attrids[$i]: return \"$attnames[$i]\";\n";
    }
    print CC "        default: return nullptr;\n";
    print CC "    }\n";
    print CC "}\n\n";

    print CC "const char *$elementclass\:\:getAttribute(int k) const\n";
    print CC "{\n";
    print CC "    switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        if ($argtypes[$i] eq "const char *") {
            print CC "        case $attrids[$i]: return $varnames[$i].c_str();\n";
        }
        elsif ($argtypes[$i] eq "bool") {
            print CC "        case $attrids[$i]: return boolToString($varnames[$i]);\n";
        }
        elsif ($argtypes[$i] eq "int") {
            print CC "        case $attrids[$i]: return enumToString($varnames[$i], $enumnames[$i]_vals, $enumnames[$i]_nums, $enumnames[$i]_n);\n";
        }
    }
    print CC "        default: return nullptr;\n";
    print CC "    }\n";
    print CC "}\n\n";

    print CC "void $elementclass\:\:setAttribute(int k, const char *val)\n";
    print CC "{\n";
    print CC "    switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        if ($argtypes[$i] eq "const char *") {
            print CC "        case $attrids[$i]: $varnames[$i] = val; break;\n";
        }
        elsif ($argtypes[$i] eq "bool") {
            print CC "        case $attrids[$i]: $varnames[$i] = stringToBool(val); break;\n";
        }
        elsif ($argtypes[$i] eq "int") {
            print CC "        case $attrids[$i]: $varnames[$i] = stringToEnum(val, $enumnames[$i]_vals, $enumnames[$i]_nums, $enumnames[$i]_n); break;\n";
        }
    }
    print CC "        default: ;\n";
    print CC "    }\n";
    print CC "}\n\n";

    print CC "const char *$elementclass\:\:getAttributeDefault(int k) const\n";
    print CC "{\n";
    print CC "    switch (k) {\n";
    for ($i=0; $i<$attcount; $i++)
    {
        $attval = $attvals[$i];
        $attval =~ s/\&#10;/\\n/g;
        if ($attval eq "#IMPLIED") {
            print CC "        case $attrids[$i]: return \"\";\n";
        } elsif ($attval eq "#REQUIRED") {
            print CC "        case $attrids[$i]: return nullptr;\n";
        } else {
            print CC "        case $attrids[$i]: return $attval;\n";
        }
    }
    print CC "        default: return nullptr;\n";
    print CC "    }\n";
    print CC "}\n\n";

    print CC "$elementclass *$elementclass\:\:dup() const\n";
    print CC "{\n";
    print CC "    $elementclass *element = new $elementclass();\n";
    for ($i=0; $i<$attcount; $i++)
    {
        print CC "    element->$varnames[$i] = this->$varnames[$i];\n"
    }
    print CC "    return element;\n";
    print CC "}\n\n";

    print CC "$elementclass *$elementclass\:\:getNext${shortelementclass}Sibling() const\n";
    print CC "{\n";
    print CC "    return static_cast<$elementclass *>(getNextSiblingWithTag($enumname{$element}));\n";
    print CC "}\n\n";

    for ($i=0; $i<$childcount; $i++)
    {
        $childclass = $elementclass{$children[$i]};
        print CC "$childclass *$elementclass\:\:getFirst$childvars[$i]Child() const\n";
        print CC "{\n";
        print CC "    return static_cast<$childclass *>(getFirstChildWithTag($enumname{$children[$i]}));\n";
        print CC "}\n\n";
    }
}


#-------------------------------------------------------------------------------------
#
# Factory class
#
print H "/**\n";
print H " * @brief GENERATED CLASS. Factory for ${NED} ASTNode subclasses.\n";
print H " *\n";
print H " * \@ingroup Data\n";
print H " */\n";
print H "class NEDXML_API ${Ned}AstNodeFactory : public ASTNodeFactory\n";
print H "{\n";
print H "  public:\n";
print H "    virtual ASTNode *createElementWithTag(const char *tagname);\n";
print H "    virtual ASTNode *createElementWithTag(int tagcode);\n";
print H "};\n\n";
print H "} // namespace nedxml\n";
print H "} // namespace omnetpp\n\n";
print H "#endif\n\n";

print CC "ASTNode *${Ned}AstNodeFactory::createElementWithTag(const char *tagname)\n";
print CC "{\n";
foreach $element (@elements)
{
    $element =~ /^(.)/;
    $startletter = $1;
    print CC "    if (tagname[0]=='$startletter' && !strcmp(tagname,\"$element\"))  return new $elementclass{$element}();\n";
}
print CC "    throw NedException(\"unknown tag '%s', cannot create object to represent it\", tagname);\n";
print CC "}\n\n";
print CC "ASTNode *${Ned}AstNodeFactory::createElementWithTag(int tagcode)\n";
print CC "{\n";
print CC "    switch (tagcode) {\n";
foreach $element (@elements)
{
    $element =~ /^(.)/;
    $startletter = $1;
    print CC "        case $enumname{$element}: return new $elementclass{$element}();\n";
}
print CC "    }\n";
print CC "    throw NedException(\"unknown tag code %d, cannot create object to represent it\", tagcode);\n";
print CC "}\n\n";
print CC "} // namespace nedxml\n";
print CC "} // namespace omnetpp\n\n";


#-------------------------------------------------------------------------------------
#
# Validator classes
#
open(VAL_H,">$validatorhfile") || die "*** cannot open output file $validatorhfile";
open(VAL_CC,">$validatorccfile") || die "*** cannot open output file $validatorccfile";

print VAL_H "$copyright\n";
print VAL_H "#ifndef __OMNETPP_NEDXML_${NED}VALIDATOR_H\n";
print VAL_H "#define __OMNETPP_NEDXML_${NED}VALIDATOR_H\n\n";
print VAL_H "#include \"errorstore.h\"\n";
print VAL_H "#include \"exception.h\"\n";
print VAL_H "#include \"${ned}elements.h\"\n\n";
print VAL_H "namespace omnetpp {\n";
print VAL_H "namespace nedxml {\n\n";

print VAL_CC "$copyright\n";
print VAL_CC "#include <cstdio>\n";
print VAL_CC "#include \"errorstore.h\"\n";
print VAL_CC "#include \"exception.h\"\n";
print VAL_CC "#include \"$validatorhfile\"\n\n";
print VAL_CC "namespace omnetpp {\n";
print VAL_CC "namespace nedxml {\n";

print VAL_H "/**\n";
print VAL_H " * @brief GENERATED CLASS. Abtract base class for ${NED} validators.\n";
print VAL_H " *\n";
print VAL_H " * \@ingroup Validation\n";
print VAL_H " */\n";
print VAL_H "class NEDXML_API ${Ned}ValidatorBase\n";
print VAL_H "{\n";
print VAL_H "  protected:\n";
print VAL_H "    ErrorStore *errors;\n";
print VAL_H "  public:\n";
print VAL_H "    /** \@name Constructor, destructor */\n";
print VAL_H "    //\@{\n";
print VAL_H "    ${Ned}ValidatorBase(ErrorStore *e) {errors = e;}\n";
print VAL_H "    virtual ~${Ned}ValidatorBase() {}\n";
print VAL_H "    //\@}\n\n";
print VAL_H "    /** Validates the node recursively */\n";
print VAL_H "    virtual void validate(ASTNode *node);\n";
print VAL_H "    /** Dispatches to the corresponding overloaded validateElement() function */\n";
print VAL_H "    virtual void validateElement(ASTNode *node);\n";
print VAL_H "\n";
print VAL_H "  protected:\n";
print VAL_H "    /** \@name Validation functions, to be implemented in subclasses */\n";
print VAL_H "    //\@{\n";
foreach $element (@elements)
{
    print VAL_H "    virtual void validateElement($elementclass{$element} *node) = 0;\n";
}
print VAL_H "    //\@}\n";
print VAL_H "};\n\n";
print VAL_H "} // namespace nedxml\n";
print VAL_H "} // namespace omnetpp\n\n";
print VAL_H "#endif\n\n";

print VAL_CC "void  ${Ned}ValidatorBase::validate(ASTNode *node)\n";
print VAL_CC "{\n";
print VAL_CC "    validateElement(node);\n";
print VAL_CC "    for (ASTNode *child=node->getFirstChild(); child; child=child->getNextSibling())\n";
print VAL_CC "        validate(child);\n";
print VAL_CC "}\n\n";

print VAL_CC "void  ${Ned}ValidatorBase::validateElement(ASTNode *node)\n";
print VAL_CC "{\n";
print VAL_CC "    try {\n";
print VAL_CC "        switch (node->getTagCode()) {\n";
foreach $element (@elements)
{
    $element =~ /^(.)/;
    $startletter = $1;
    print VAL_CC "            case $enumname{$element}: validateElement(static_cast<$elementclass{$element} *>(node)); break;\n";
}
print VAL_CC "            default: INTERNAL_ERROR1(node,\"validateElement(): unknown tag '%s'\", node->getTagName());\n";
print VAL_CC "        }\n";
print VAL_CC "    }\n";
print VAL_CC "    catch (NedException& e)\n";
print VAL_CC "    {\n";
print VAL_CC "        INTERNAL_ERROR1(node,\"validateElement(): NedException: %s\", e.what());\n";
print VAL_CC "    }\n";
print VAL_CC "}\n\n";
print VAL_CC "} // namespace nedxml\n";
print VAL_CC "} // namespace omnetpp\n\n";


#-------------------------------------------------------------------------------------
#
# DTD Validator classes
#
open(DTDVAL_H,">$dtdvalidatorhfile") || die "*** cannot open output file $dtdvalidatorhfile";
open(DTDVAL_CC,">$dtdvalidatorccfile") || die "*** cannot open output file $dtdvalidatorccfile";

print DTDVAL_H "$copyright\n";
print DTDVAL_H "#ifndef __OMNETPP_NEDXML_${NED}DTDVALIDATOR_H\n";
print DTDVAL_H "#define __OMNETPP_NEDXML_${NED}DTDVALIDATOR_H\n\n";
print DTDVAL_H "#include \"${ned}elements.h\"\n";
print DTDVAL_H "#include \"${ned}validator.h\"\n";
print DTDVAL_H "#include \"dtdvalidationutils.h\"\n\n";
print DTDVAL_H "namespace omnetpp {\n";
print DTDVAL_H "namespace nedxml {\n\n";

print DTDVAL_CC "$copyright\n";
print DTDVAL_CC "#include <stdio.h>\n";
print DTDVAL_CC "#include \"errorstore.h\"\n";
print DTDVAL_CC "#include \"$dtdvalidatorhfile\"\n\n";
print DTDVAL_CC "namespace omnetpp {\n";
print DTDVAL_CC "namespace nedxml {\n\n";

print DTDVAL_H "/**\n";
print DTDVAL_H " * @brief GENERATED CLASS. Validates an ASTNode tree by the DTD.\n";
print DTDVAL_H " *\n";
print DTDVAL_H " * \@ingroup Validation\n";
print DTDVAL_H " */\n";
print DTDVAL_H "class NEDXML_API ${Ned}DtdValidator : public ${Ned}ValidatorBase\n";
print DTDVAL_H "{\n";
print DTDVAL_H "  protected:\n";
print DTDVAL_H "      typedef DtdValidationUtils::Choice Choice;\n";
print DTDVAL_H "      DtdValidationUtils utils;\n";
print DTDVAL_H "  public:\n";
print DTDVAL_H "    ${Ned}DtdValidator(ErrorStore *e) : ${Ned}ValidatorBase(e), utils(e) {}\n";
print DTDVAL_H "    virtual ~${Ned}DtdValidator() {}\n";
print DTDVAL_H "\n";
print DTDVAL_H "  protected:\n";
print DTDVAL_H "    /** \@name Validation functions */\n";
print DTDVAL_H "    //\@{\n";
foreach $element (@elements)
{
    print DTDVAL_H "    virtual void validateElement($elementclass{$element} *node) override;\n";
}
print DTDVAL_H "    //\@}\n";
print DTDVAL_H "};\n\n";
print DTDVAL_H "} // namespace nedxml\n";
print DTDVAL_H "} // namespace omnetpp\n\n";
print DTDVAL_H "#endif\n\n";

foreach $element (@elements)
{
    print DTDVAL_CC "void ${Ned}DtdValidator\:\:validateElement($elementclass{$element} *node)\n";
    print DTDVAL_CC "{\n";
    if ($elementdef{$element} =~ /^\(([^|]*)\)$/) {  # in parens, does not contain "|"
       @a = split(',',$1);
       $tags='';
       $mult='';
       foreach $e (@a) {
          $e =~ /^(.*?)([*?+]?)$/;
          $tags .= $enumname{$1}.",";
          $mult .= "'".$2."'," if ($2 ne '');
          $mult .= "'1'," if ($2 eq '');
       }
       print DTDVAL_CC "    int tags[] = {$tags NED_NULL};\n";
       print DTDVAL_CC "    char mult[] = {$mult 0};\n";
       print DTDVAL_CC "    utils.checkSequence(node, tags, mult);\n";
    }
    elsif ($elementdef{$element} =~ /^\(\(([^,]*)\)([*?+]?)\)$/) { # like ((a|b|c)*)
       @a = split('\|',$1);
       $mult = $2;
       if ($mult eq '') {$mult='1';}
       $tags='';
       foreach $e (@a) {
          $tags .= $enumname{$e}.",";
       }
       print DTDVAL_CC "    int tags[] = {$tags NED_NULL};\n";
       print DTDVAL_CC "    utils.checkChoice(node, tags, \'$mult\');\n";
    }
    elsif ($elementdef{$element} =~ /^\((.*)\)$/) {  # fallback: in parens
       print DTDVAL_CC "    Choice choices[] = {\n";
       #print "DBG: parsing $1\n";
       @seqs = split(',',$1);
       foreach $seq (@seqs) {
          if ($seq =~ /^\((.*)\)([*?+]?$)/) {
              #print "DBG:   CHOICE=( $1 )  MUL=$2\n";
              @choicetags = split('\|',$1);
              $mul = $2;
              if ($mul eq '') {$mul = '1';}

              print DTDVAL_CC "        {{";
              foreach $choicetag (@choicetags) {
                  if (! $choicetag =~ /^[a-zA-Z0-9-_]+$/) {
                      die "Cannot parse element def for $element: $elementdef{$element}: $choicetag does not look like an element name\n";
                  }
                  print DTDVAL_CC $enumname{$choicetag}.", ";
              }
              print DTDVAL_CC "NED_NULL}, '$mul'},\n";
          }
          elsif ($seq =~ /^([a-zA-Z0-9-_]+)([*?+]?$)/) {
              #print "DBG:   NAME=$1  MUL=$2\n";
              $name = $1;
              $mul = $2;
              if ($mul eq '') {$mul = '1';}
              print DTDVAL_CC "        {{".$enumname{$name}.", NED_NULL}, '$mul'},\n";
          }
          else {
              die "Cannot parse $seq in element def for $element: $elementdef{$element}\n";
          }
       }
       print DTDVAL_CC "    };\n";
       print DTDVAL_CC "    utils.checkSeqOfChoices(node, choices, sizeof(choices)/sizeof(Choice));\n";
    }
    elsif ($elementdef{$element} eq 'EMPTY') {
       print DTDVAL_CC "    utils.checkEmpty(node);\n";
    }
    elsif ($elementdef{$element} eq 'ANY') {
       print DTDVAL_CC "    // ANY\n";
    }
    else {
       die "Cannot parse element def for $element: $elementdef{$element}\n";
    }

    @attnames = @{$att_names{$element}};
    @atttypes = @{$att_types{$element}};
    @attvals = @{$att_vals{$element}};
    $attcount = $#attnames+1;

    print DTDVAL_CC "\n";

    for ($i=0; $i<$attcount; $i++)
    {
        $attname = $attnames[$i];
        $attval = $attvals[$i];
        $atttype = $atttypes[$i];

        if ($attval eq "#REQUIRED") {
            print DTDVAL_CC "    utils.checkRequiredAttribute(node, \"$attname\");\n";
        }

        if ($atttype =~ /^\((.*)\)$/) {
            @a = split('\|',$1);
            $vals='';
            foreach $e (@a) {
               $vals .= "\"".$e."\",";
            }
            $vals =~ s/,$//;
            if ($attval eq "#IMPLIED") {
               $vals .= ",\"\"";
            }
            print DTDVAL_CC "    const char *vals${i}[] = {$vals};\n";
            print DTDVAL_CC "    utils.checkEnumeratedAttribute(node, \"$attname\", vals$i, sizeof(vals$i)/sizeof(const char *));\n";
        }
        elsif ($attname eq "comment" || $attname =~ /-comment$/) {
            print DTDVAL_CC "    utils.checkCommentAttribute(node, \"$attname\");\n";
        }
        elsif ($atttype eq "NMTOKEN") {
            print DTDVAL_CC "    utils.checkNameAttribute(node, \"$attname\");\n";
        }
    }
    print DTDVAL_CC "}\n\n";
}

print DTDVAL_CC "} // namespace nedxml\n";
print DTDVAL_CC "} // namespace omnetpp\n\n";

