# wizard properties
templateName = Newly Generated Wizard
templateDescription = Does not generate anything useful, but contains template code examples
templateCategory = Generated Wizards
supportedWizardTypes =

<#if wantJar>
ignoreResources = *.java
</#if>

<#if wantBasics>
# variables
boolVar = true
numericVar = 42
textVar = "lorem ipsum dolor"
</#if>

<#if wantJSON>
# array (list)
apps = ["ftp", "smtp", "news"]

# hash which contains list values too
layers = { "datalink":"ieee80211", "network":"ip", "transport":["tcp","udp"] }
</#if>