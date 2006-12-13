# this inserts an extra statement into Body which cannot be done using SWIG.

$fileName = shift;

open(FILE, "$fileName");
@lines = <FILE>;
close(FILE);

open(FILE, ">$fileName");
foreach (@lines)
{
   s/this\(CommonEngineJNI\.new_Body__SWIG_([0-9]+)\(Variable.getCPtr\(variable\)(.*?), true\);/this(CommonEngineJNI.new_Body__SWIG_$1(Variable.getCPtr(variable)$2, true);\n    this.variable = variable; \/\/ added by replace.pl /;
   print(FILE);
}
close(FILE);