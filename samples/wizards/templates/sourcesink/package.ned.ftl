<#if makeSrcAndSimulationsFolders>
  <@setoutput file="src/package.ned"/>
</#if>
<#if rootPackage!="">
// designate the NED package of this folder
package ${rootPackage};
</#if>

<#if namespace!="">
// namespace of module C++ classes
@namespace(${namespace});
</#if>

<#if licenseCode!="">
@license(${licenseCode});
</#if>

