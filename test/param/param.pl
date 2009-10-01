# This program generates NED and INI files to test the parameter assignment fallback mechanism in the OMNeT++ IDE.

open INI, ">omnetpp.ini";
print INI "[General]\n";
print INI "network = Network\n";
print INI "user-interface = Cmdenv\n";
print INI "cmdenv-interactive = true\n\n";

open NED, ">param.ned";
print NED "package param;\n\n";
print NED "module Param {\n";
print NED "    parameters:\n";

$paramIndex = 0;

foreach $submoduleCardinality ("single", "vector") {
  @patternEnum = ($submoduleCardinality eq "single") ? ("") : ("[*]", "[0]", "[1]");

  foreach $iniKeyPattern (@patternEnum) {
    foreach $nedParamDeclarationValue ("", "default(\"NED declaration\")", "\"NED declaration\"") {
      foreach $nedSubmoduleParamAssignmentValue ("", "default(\"NED submodule\")", "\"NED submodule\"") {
        foreach $nedModuleParamAssignmentPattern (@patternEnum) {
          foreach $nedModuleParamAssignmentValue ("", "default(\"NED module\")", "\"NED module\"") {
            foreach $iniValue ("", "default", "ask", "\"INI\"") {

  $paramName = "p".$paramIndex++;
  $paramComment = $submoduleCardinality."_".$nedParamDeclarationValue."_".$nedSubmoduleParamAssignmentValue."_".$nedModuleParamAssignmentValue."_".$iniValue;

  # INI file
  if ($iniValue ne "") {
    print INI "**.".$submoduleCardinality.$iniKeyPattern.".".$paramName." = ".$iniValue."\n";
  }

  # NED declaration
  print NED "\n";
  print NED "        // <pre>\n";
  print NED "        // submoduleCardinality             = ".$submoduleCardinality."\n";
  print NED "        // nedParamDeclarationValue         = ".$nedParamDeclarationValue."\n";
  print NED "        // nedSubmoduleParamAssignmentValue = ".$nedSubmoduleParamAssignmentValue."\n";
  print NED "        // nedModuleParamAssignmentPattern  = ".$nedModuleParamAssignmentPattern."\n";
  print NED "        // nedModuleParamAssignmentValue    = ".$nedModuleParamAssignmentValue."\n";
  print NED "        // iniKeyPattern                    = ".$iniKeyPattern."\n";
  print NED "        // iniValue                         = ".$iniValue."\n";
  print NED "        // </pre>\n";
  print NED "        string ".$paramName;
  if ($nedParamDeclarationValue ne "") {
    print NED " = ".$nedParamDeclarationValue;
  }
  print NED ";\n";

  # NED submodule
  if ($submoduleCardinality eq "single") {
    if ($nedSubmoduleParamAssignmentValue ne "") {
      $singleSubmoduleParameterAssignments .= "                ".$paramName." = ".$nedSubmoduleParamAssignmentValue.";\n";
    }
  }
  else {
    if ($nedSubmoduleParamAssignmentValue ne "") {
      $vectorSubmoduleParameterAssignments .= "                ".$paramName." = ".$nedSubmoduleParamAssignmentValue.";\n";
    }
  }

  # NED module
  if ($nedModuleParamAssignmentValue ne "") {
    $moduleParameterAssignments .= "        ".$submoduleCardinality.$nedModuleParamAssignmentPattern.".".$paramName." = ".$nedModuleParamAssignmentValue.";\n";
  }
            }
          }
        }
      }
    }
  }
}

print NED "}\n\n";
print NED "network Network {\n";
print NED "    parameters:\n";
print NED $moduleParameterAssignments;
print NED "    submodules:\n";
print NED "        dump: Dump;\n";
print NED "        single: Param {\n";
print NED "            parameters:\n";
print NED $singleSubmoduleParameterAssignments;
print NED "        }\n";
print NED "        vector[2]: Param {\n";
print NED "            parameters:\n";
print NED $vectorSubmoduleParameterAssignments;
print NED "        }\n";
print NED "}\n";

close INI;
close NED;
