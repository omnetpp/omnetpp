<#-- main file -->

<#noparse>
<#-- 
 IDE Wizards use the FreeMarker templating engine (freemarker.org),
 and the full power of its template language is available.
 
 See FreeMarker documentation at http://freemarker.org/docs 
-->
</#noparse>

<#if wantBasics><#include "basics.fti"/></#if>
<#if wantControl><#include "control.fti"/></#if>
<#if wantExec><#include "exec.fti"/></#if>
<#if wantFunction><#include "function.fti"/></#if>
<#if wantJar><#include "jar.fti"/></#if>
<#if wantJava><#include "java.fti"/></#if>
<#if wantJSON><#include "json.fti"/></#if>
<#if wantMacro><#include "macro.fti"/></#if>
<#if wantStrings><#include "strings.fti"/></#if>
 