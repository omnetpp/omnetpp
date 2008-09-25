#!/usr/bin/perl
#
# read a SWIG-generated .java file with native methods, and create a table
# for the RegisterNatives() JNI call.
#
# See SWIG issue 1522417: "Java/JNI: Generate method that registers methods with a VM"
#

$verbose = 0;

$javafile = $ARGV[0];
die "no Java file specified" if ($javafile eq '');

$cppfile = $javafile;
$cppfile =~ s|\.java|_registerNatives.cc|;
$cppfile =~ s|^.*[/\\]||;
$cppfile = $ARGV[1] . "\\" . $cppfile;
print $cppfile;

open(INFILE, $javafile) || die "cannot open $javafile";
read(INFILE, $java, 1000000) || die "cannot read $javafile";

# extract package and class name
$java =~ /\bpackage +(.*?);/;
$packagename = $1;
$packagenameslash = $packagename;
$packagenameslash =~ s|\.|/|g;

$java =~ /\bclass +(\w+)/;
$classname = $1;

# parse file
$n = 0;
$java =~ s|([^;{}]*) +([[a-zA-Z0-9_[]+) +(\w+)\((.*?)\);|$mmods{$n}=$1; $mret{$n}=$2; $mname{$n}=$3; $margs{$n}=$4; $n++; ""|gse;

# process
for ($i=0; $i<$n; $i++) {
    #print $mmods{$i}, "|", $mret{$i}, "|", $mname{$i}, "|", $margs{$i}, "\n";

    # remove arg names and commas
    $sig = $margs{$i};
    $sig =~ s/(\w+) +(\w+)/\1/g;
    $sig =~ s/(\w+\[\]) +(\w+)/\1/g;
    $sig =~ s/,/ /g;

    # translate arg types and assemble signature string
    $sig =~ s/(\w+)/toJNI($1);/gse;
    $sig =~ s/ +//g;
    $sig = "($sig)" . toJNI($mret{$i});

    $msig{$i} = $sig;

    $prefix = $packagename;
    $prefix =~ s/\./_/g;

    $quotedmname = $mname{$i};
    $quotedmname =~ s/_/_1/g;

    $mfunc{$i} = "Java_${prefix}_${classname}_$quotedmname";

    #print $mname{$i}, "\t", $sig, "\n";
}

# write C++ file
open(OUT, ">$cppfile") || die "cannot open $cppfile";
print OUT "// generated using registernatives.pl\n\n";
print OUT "#include <stdio.h>\n";
print OUT "#include <stdlib.h>\n";
print OUT "#include <jni.h>\n\n";
print OUT "extern \"C\" {\n";
for ($i=0; $i<$n; $i++) {
    print OUT "void $mfunc{$i}(JNIEnv *jenv, jclass jcls,...);\n";
}
print OUT "};\n\n";

print OUT "static JNINativeMethod ${classname}_methods[] = {\n";
for ($i=0; $i<$n; $i++) {
    print OUT "    { \"$mname{$i}\", \"$msig{$i}\", (void *)$mfunc{$i} },\n";
}
print OUT "};\n\n";
print OUT "void ${classname}_registerNatives(JNIEnv *jenv)\n";
print OUT "{\n";
print OUT "    jclass clazz = jenv->FindClass(\"$packagenameslash/$classname\");\n";
print OUT "    if (!clazz) {\n";
print OUT "        fprintf(stderr, \"ERROR: Cannot find ${classname} class\\n\");\n";
print OUT "        exit(1);\n";
print OUT "    }\n";
print OUT "    int ret = jenv->RegisterNatives(clazz, ${classname}_methods, $n);\n";
print OUT "    if (ret!=0) {\n";
print OUT "        fprintf(stderr, \"ERROR: Cannot register native methods for ${classname}: RegisterNatives() returned %d\\n\", ret);\n";
print OUT "        exit(1);\n";
print OUT "    }\n";
print OUT "}\n\n";

print "$cppfile created.\n";

sub toJNI()
{
    my $t = shift;
    $a = "";
    if ($t =~ s/\[\]$//) {$a = "[";}
    return $a."V" if ($t eq "void");
    return $a."Z" if ($t eq "boolean");
    return $a."B" if ($t eq "byte");
    return $a."C" if ($t eq "char");
    return $a."S" if ($t eq "short");
    return $a."I" if ($t eq "int");
    return $a."J" if ($t eq "long");
    return $a."F" if ($t eq "float");
    return $a."D" if ($t eq "double");
    return $a."Ljava/lang/String;" if ($t eq "String");
    return $a."Ljava/lang/Object;" if ($t eq "Object");
    return $a."Lorg/omnetpp/simkernel/$t;";
}

