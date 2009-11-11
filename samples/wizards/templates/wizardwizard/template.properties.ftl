<#function iif condition value>
  	<#if condition><#return value><#else><#return ""></#if>
</#function>
templateName = ${wizardTemplateName}
templateDescription = ${wizardTemplateDescription}
templateCategory = ${wizardTemplateCategory}
<#assign wizartSupportedWizardTypes = 
    iif(supportProject, "project,")+
    iif(supportSimulation, "simulation,") + 
	iif(supportSimplemodule, "simplemodule,") +
	iif(supportCompoundmodule, "compoundmodule,") +
	iif(supportNetwork, "network,") +
	iif(supportNedfile, "nedfile,") +
	iif(supportMsgfile, "msgfile,") +
	iif(supportInifile, "inifile,")>
supportedWizardTypes = ${StringUtils.removeEnd(wizartSupportedWizardTypes, ",").replace(",", ", ")}


# template variables

# custom wizard pages
page.1.file = intro.xswt
page.2.file = supportedwizardtypes.xswt
page.3.file = properties.xswt
page.4.file = selectfeatures.xswt
page.5.file = introcontent.xswt
page.6.file = formcontents.xswt

