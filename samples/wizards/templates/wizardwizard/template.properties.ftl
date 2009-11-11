<@setoutput file=wizardFolder+"/template.properties"/>
<#function iif condition truevalue falsevalue="">
  	<#if condition><#return truevalue><#else><#return falsevalue></#if>
</#function>
# wizard properties
templateName = ${wizardTemplateName}
templateDescription = ${wizardTemplateDescription}
templateCategory = ${wizardTemplateCategory}
<#assign wizardSupportedWizardTypes = 
    iif(supportProject, "project,")+
    iif(supportSimulation, "simulation,") + 
	iif(supportSimplemodule, "simplemodule,") +
	iif(supportCompoundmodule, "compoundmodule,") +
	iif(supportNetwork, "network,") +
	iif(supportNedfile, "nedfile,") +
	iif(supportMsgfile, "msgfile,") +
	iif(supportInifile, "inifile,")>
supportedWizardTypes = ${StringUtils.removeEnd(wizardSupportedWizardTypes, ",").replace(",", ", ")}

<#if supportProject>
# project creation options
addProjectReference = ${iif(wizardAddProjectReference,"true", "false")}
<#if wizardSourceFolders!="">sourceFolders = ${wizardSourceFolders}</#if>
<#if wizardNedSourceFolders!="">nedSourceFolders = ${wizardNedSourceFolders}</#if>
<#if wizardMakemakeOptions!="">makemakeOptions = ${wizardMakemakeOptions}</#if>
</#if>

# template variables

# custom wizard pages
<#if wantIntroPage>
page.1.file = intro.xswt
</#if>
<#if wantBasicForm>
page.2.file = basicform.xswt
</#if>
<#if wantWidgetsForm>
page.3.file = widgetsform.xswt
</#if>
<#if wantComplexForm>
page.4.file = complexform.xswt
</#if>
<#if wantFileGeneration>
page.5.file = generatefile.xswt
</#if>
<#if wantComputations>
page.6.file = computations.xswt
</#if>
<#if wantJarLoading>
page.7.file = jarloading.xswt
</#if>
<#if wantProgramLaunch>
page.8.file = programlaunch.xswt
</#if>
