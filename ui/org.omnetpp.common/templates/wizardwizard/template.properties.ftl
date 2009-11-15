<@setoutput file="template.properties"/>
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
<#if wantWidgetsForm>
# for the widgetsform page
<#if wantText>
exampleTextVar = some text
</#if>
<#if wantMultilineText>
exampleMultilineTextVar = some very long text, to be edited
</#if>
<#if wantCombo>  
exampleTextVar2 = random
exampleTextVar3 = random
</#if>
<#if wantList>  
exampleListVar = [ "red", "alpha" ]
</#if>
<#if wantCheckbox>  
exampleBooleanVar1 = false
</#if>
<#if wantRadioButtons>  
exampleBooleanVar2a = false
exampleBooleanVar2b = true
exampleBooleanVar2c = false
</#if>
<#if wantGroup>  
exampleTextVar5 = foo
exampleBooleanVar5 = false
</#if>
<#if wantComposite>  
exampleTextVar6 = foo
exampleBooleanVar6 = false
</#if>
<#if wantScale>  
exampleNumericVar1= 50
</#if>
<#if wantSlider>  
exampleNumericVar2 = 30
</#if>
<#if wantSpinner>  
exampleNumericVar3 = 20
</#if>
</#if>

# custom wizard pages
<#if wantIntroPage>
page.1.file = intro.xswt
page.1.title = Introduction 
page.1.description = Read this carefully 
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
