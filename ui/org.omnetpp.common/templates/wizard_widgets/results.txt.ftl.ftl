<@setoutput path="results.txt.ftl"/>
The form gave the following results:
<#if wantText>
exampleTextVar = <#noparse>${exampleTextVar}</#noparse>
</#if>
<#if wantMultilineText>
exampleMultilineTextVar = <#noparse>${exampleMultilineTextVar}</#noparse>
</#if>
<#if wantCombo>
exampleTextVar2 = <#noparse>${exampleTextVar2}</#noparse>
exampleTextVar3 = <#noparse>${exampleTextVar3}</#noparse>
</#if>
<#if wantList>
exampleListVar =<#noparse><#list exampleListVar as i> ${i}</#list></#noparse>
</#if>
<#if wantCheckbox>
exampleBooleanVar1 = <#noparse><#if exampleBooleanVar1>true<#else>false</#if></#noparse>
</#if>
<#if wantRadioButtons>
exampleBooleanVar2a = <#noparse><#if exampleBooleanVar2a>true<#else>false</#if></#noparse>
exampleBooleanVar2b = <#noparse><#if exampleBooleanVar2b>true<#else>false</#if></#noparse>
exampleBooleanVar2c = <#noparse><#if exampleBooleanVar2c>true<#else>false</#if></#noparse>
</#if>
<#if wantGroup>
exampleTextVar5 = <#noparse>${exampleTextVar5}</#noparse>
exampleBooleanVar5 = <#noparse><#if exampleBooleanVar5>true<#else>false</#if></#noparse>
</#if>
<#if wantComposite>
exampleTextVar6 = <#noparse>${exampleTextVar6}</#noparse>
exampleBooleanVar6 = <#noparse><#if exampleBooleanVar6>true<#else>false</#if></#noparse>
</#if>
<#if wantScale>
exampleNumericVar1 = <#noparse>${exampleNumericVar1}</#noparse>
</#if>
<#if wantSlider>
exampleNumericVar2 = <#noparse>${exampleNumericVar2}</#noparse>
</#if>
<#if wantSpinner>
exampleNumericVar3 = <#noparse>${exampleNumericVar3}</#noparse>
</#if>
