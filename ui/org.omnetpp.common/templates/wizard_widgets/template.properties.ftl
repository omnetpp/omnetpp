<@setoutput path="template.properties"/>
<#function iif condition truevalue falsevalue="">
  	<#if condition><#return truevalue><#else><#return falsevalue></#if>
</#function>
# wizard properties
templateName = Newly Generated Wizard, with Custom Page
templateDescription = Wizard with a page that contains selected widgets
templateCategory = Generated Wizards
supportedWizardTypes =

# template variables
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

page.1.file = widgetsform.xswt
page.1.title = Enter Data
page.1.desctiption = This is a generated dummy form
