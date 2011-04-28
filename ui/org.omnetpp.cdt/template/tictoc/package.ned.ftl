<@setoutput path=srcFolder+"/package.ned"/>
<#if srcPackage!="">
// designate the NED package of this folder
package ${srcPackage};

</#if>
<#if namespace!="">
// namespace of module C++ classes
@namespace(${namespace});

</#if>
<#if licenseCode!="">
@license(${licenseCode});
</#if>

