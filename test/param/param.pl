# This program generates NED and INI files to test the param assignment fallback mechanism in the OMNeT++ IDE.

open INI, ">omnetpp.ini";
print INI "[General]\n";
print INI "network = Network\n";
print INI "user-interface = Cmdenv\n";
print INI "cmdenv-interactive = true\n\n";

$singleParamIndex = 0;
$vectorParamIndex = 0;

foreach $submoduleCardinality ("single", "vector") {
  @patternEnum = $submoduleCardinality eq "single" ? ("") : ("[*]", "[0]", "[1]");

  foreach $iniKeyPattern (@patternEnum) {
    foreach $nedParamDeclarationValue ("", "default(\"NED declaration\")", "\"NED declaration\"") {
      foreach $nedSubmoduleParamAssignmentValue ("", "default(\"NED submodule\")", "\"NED submodule\"") {
        foreach $nedModuleParamAssignmentValue ("", "default(\"NED module\")", "\"NED module\"") {
          foreach $nedModuleParamAssignmentPattern ($nedModuleParamAssignmentValue eq "" ? ("") : @patternEnum) {
            foreach $iniValue ("", "default", "ask", "\"INI\"") {

  $paramName = $submoduleCardinality eq "single" ? "s".$singleParamIndex++ : "v".$vectorParamIndex++;
  $paramComment = $submoduleCardinality."_".$nedParamDeclarationValue."_".$nedSubmoduleParamAssignmentValue."_".$nedModuleParamAssignmentValue."_".$iniValue;

  # INI file
  if ($iniValue ne "") {
    print INI "**.".$submoduleCardinality.$iniKeyPattern.".".$paramName." = ".$iniValue."\n";
  }

  # NED declaration
  $paramDeclarations = "\n";
  $paramDeclarations .= "        // <pre>\n";
  $paramDeclarations .= "        // nedParamDeclarationValue         = ".$nedParamDeclarationValue."\n";
  $paramDeclarations .= "        // nedSubmoduleParamAssignmentValue = ".$nedSubmoduleParamAssignmentValue."\n";

  if ($submoduleCardinality eq "vector") {
    $paramDeclarations .= "        // nedModuleParamAssignmentPattern  = ".$nedModuleParamAssignmentPattern."\n";
  }

  $paramDeclarations .= "        // nedModuleParamAssignmentValue    = ".$nedModuleParamAssignmentValue."\n";
  $paramDeclarations .= "        // iniKeyPattern                    = ".$iniKeyPattern."\n";
  $paramDeclarations .= "        // iniValue                         = ".$iniValue."</pre>\n";
  $paramDeclarations .= "        string ".$paramName;

  if ($nedParamDeclarationValue ne "") {
    $paramDeclarations .= " = ".$nedParamDeclarationValue;
  }

  $paramDeclarations .= ";\n";

  if ($submoduleCardinality eq "single") {
    $singleParamDeclarations .= $paramDeclarations;
  }
  else {
    $vectorParamDeclarations .= $paramDeclarations;
  }

  # NED submodule
  if ($submoduleCardinality eq "single") {
    if ($nedSubmoduleParamAssignmentValue ne "") {
      $singleSubmoduleParamAssignments .= "                ".$paramName." = ".$nedSubmoduleParamAssignmentValue.";\n";
    }
  }
  else {
    if ($nedSubmoduleParamAssignmentValue ne "") {
      $vectorSubmoduleParamAssignments .= "                ".$paramName." = ".$nedSubmoduleParamAssignmentValue.";\n";
    }
  }

  # NED module
  if ($nedModuleParamAssignmentValue ne "") {
    $moduleParamAssignments .= "        ".$submoduleCardinality.$nedModuleParamAssignmentPattern.".".$paramName." = ".$nedModuleParamAssignmentValue.";\n";
  }
            }
          }
        }
      }
    }
  }
}

close INI;

open NED, ">param.ned";
print NED "package param;\n\n";
print NED "module Single {\n";
print NED "    parameters:\n";
print NED $singleParamDeclarations;
print NED "}\n\n";
print NED "module Vector {\n";
print NED "    parameters:\n";
print NED $vectorParamDeclarations;
print NED "}\n\n";
print NED "network Network {\n";
print NED "    parameters:\n";
print NED $moduleParamAssignments;
print NED "    submodules:\n";
print NED "        dump: Dump;\n";
print NED "        single: Single {\n";
print NED "            parameters:\n";
print NED $singleSubmoduleParamAssignments;
print NED "        }\n";
print NED "        vector[2]: Vector {\n";
print NED "            parameters:\n";
print NED $vectorSubmoduleParamAssignments;
print NED "        }\n";
print NED "}\n";
close NED;
