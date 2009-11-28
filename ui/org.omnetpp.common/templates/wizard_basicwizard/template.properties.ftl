<@setoutput path="template.properties"/>
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

<#if wantBasicForm>
# template variables
columns = 8
rows = 6
</#if>

# custom wizard pages
<#if wantIntroPage>
page.1.file = intro.xswt
page.1.title = Introduction
page.1.description = Read this carefully
</#if>
<#if wantBasicForm>
page.2.file = basicform.xswt
page.2.title = Enter Grid Dimensions
page.2.description = Enter Width/Height of Grid To Generate
</#if>
