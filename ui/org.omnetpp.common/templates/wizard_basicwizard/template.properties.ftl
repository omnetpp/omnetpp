<@setoutput path="template.properties"/>
# wizard properties
templateName = ${wizardTemplateName}
templateDescription = ${wizardTemplateDescription}
templateCategory = ${wizardTemplateCategory}
<#assign wizardSupportedWizardTypes =
    iif(supportProject, "project,") +
    iif(supportSimulation, "simulation,") +
	iif(supportSimplemodule, "simplemodule,") +
	iif(supportCompoundmodule, "compoundmodule,") +
	iif(supportNetwork, "network,") +
	iif(supportNedfile, "nedfile,") +
	iif(supportMsgfile, "msgfile,") +
	iif(supportInifile, "inifile,") +
	iif(supportImport, "import,")>
<#if supportExport><#assign wizardSupportedWizardTypes = "export"></#if>
supportedWizardTypes = ${StringUtils.removeEnd(wizardSupportedWizardTypes, ",").replace(",", ", ")}

<#if supportProject && !supportExport>
# project creation options
addProjectReference = ${iif(wizardAddProjectReference,"true", "false")}
<#if wizardSourceFolders!="">sourceFolders = ${wizardSourceFolders}</#if>
<#if wizardNedSourceFolders!="">nedSourceFolders = ${wizardNedSourceFolders}</#if>
<#if wizardMakemakeOptions!="">makemakeOptions = ${wizardMakemakeOptions}</#if>
</#if>

<#if wantBasicForm && !supportExport>
# template variables
columns = 8
rows = 6
</#if>

<#if supportExport>
# template variables
fileName =
</#if>

# custom wizard pages
<#if wantIntroPage>
page.1.file = intro.xswt
page.1.title = Introduction
page.1.description = Read this carefully
</#if>
<#if wantBasicForm && !supportExport>
page.2.file = basicform.xswt
page.2.title = Enter Grid Dimensions
page.2.description = Enter Width/Height of Grid To Generate
</#if>
<#if supportExport>
page.2.file = exportform.xswt
page.2.title = Export
</#if>
