<#if makeSrcAndSimulationsFolders>
<@setoutput path=simulationsFolder+"/run"/>
#!/bin/sh
cd `dirname $0`
../${srcFolder}/${projectname} -n .:../${srcFolder} $*
# for shared lib, use: opp_run -l ../${srcFolder}/${projectname} -n .:../${srcFolder} $*
</#if>
