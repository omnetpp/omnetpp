<#assign xswtFile=wizardFolder+"/wizardpage.xswt">
<#assign propsFile=wizardFolder+"/template.properties">
<#assign nedFile=wizardFolder+"/output.ned.ftl">
<@setoutput file=propsFile/>
# adjust the following
templateName = Generated Wizard
templateDescription = Wizard with a single input page
templateCategory = Generated Wizards
supportedWizardTypes = project, nedfile, simulation

# custom wizard pages
page.1.file = wizardpage.xswt
page.1.title = Generated Page
page.1.description = Select options below

# variables
<@setoutput file=xswtFile/>
<?xml version="1.0" encoding="UTF-8"?>
<xswt xmlns:x="http://sweet_swt.sf.net/xswt">

  <import xmlns="http://sweet_swt.sf.net/xswt">
    <package name="java.lang"/>
    <package name="org.eclipse.swt.widgets" />
    <package name="org.eclipse.swt.graphics" />
    <package name="org.eclipse.swt.layout" />
    <package name="org.omnetpp.common.wizard.support" />
    <package name="org.omnetpp.cdt.wizard.support" />
  </import>
  <layout x:class="GridLayout" x:numColumns="2"/>

  <x:children>
    <label x:text="This is an example wizard generated from the following spec:" x:style="WRAP">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </label>
    
    <label x:text="${spec}" x:style="WRAP">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </label>

<#list StringUtils.split(spec, ",") as i>
  <#assign field = i.trim()>
    <!-- ${field} -->
  <#assign name = field.replaceFirst("^([a-zA-Z0-9_]+).*", "$1")>
  <#assign suffix = field.replaceFirst("^[a-zA-Z0-9_]+", "")>
  <#assign defaultValue = "">
  <#if field.matches("([a-zA-Z0-9_]+/)+[a-zA-Z0-9_]+")>
    <#-- **** RADIOBUTTON GROUP **** -->
    <composite>
      <layoutData x:class="GridData" horizontalSpan="2"/>
      <x:children>
      <#list field.split("/") as j>
        <#assign option = j.trim()>
        <button x:id="${option}" text="${option}" x:style="RADIO"/>
        <@setoutput file=propsFile/>
${option} = false
        <@setoutput file=nedFile/>
// ${option}: <#noparse><#if </#noparse> ${option}<#noparse>>true<#else>false</#if></#noparse>
        <@setoutput file=xswtFile/>
      </#list>
      </x:children>
    </composite>
  <#elseif suffix=="">
    <#-- **** TEXT **** -->
    <label text="Enter ${name}:"/>
    <text x:id="${name}" x:style="BORDER">
      <layoutData x:class="GridData" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </text>
    <@setoutput file=propsFile/>
${name} = some text
    <@setoutput file=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput file=xswtFile/>
  <#elseif suffix=="$">
    <#-- **** MULTILINE **** -->
    <label text="Enter ${name}:"/>
    <text x:id="${name}" x:style="BORDER|WRAP|MULTI">
      <layoutData x:class="GridData" heightHint="60" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </text>
    <@setoutput file=propsFile/>
${name} = some more text
    <@setoutput file=nedFile/>
    <@setoutput file=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput file=xswtFile/>
  <#elseif suffix=="%">
    <#-- **** SPINNER **** -->
    <label text="Select ${name}:"/>
    <spinner x:id="${name}" minimum="0" maximum="100" x:style="BORDER"/>
    <@setoutput file=propsFile/>
${name} = 10
    <@setoutput file=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput file=xswtFile/>
  <#elseif suffix=="?">
    <#-- **** CHECKBOX **** -->
    <button x:id="${name}" text="${name}" x:style="CHECK">
      <layoutData x:class="GridData" horizontalSpan="2"/>
    </button>
    <@setoutput file=propsFile/>
${name} = false
    <@setoutput file=nedFile/>
// ${name}: <#noparse><#if </#noparse> ${name}<#noparse>>true<#else>false</#if></#noparse>
    <@setoutput file=xswtFile/>
  <#elseif suffix=="=">
    <#-- **** SCALE **** -->
    <label text="Choose ${name} (0..100):"/>
    <scale x:id="${name}" minimum="0" maximum="100"/>
    <@setoutput file=propsFile/>
${name} = 40
    <@setoutput file=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput file=xswtFile/>
  <#elseif suffix.startsWith(":")>
    <#-- **** COMBOBOX **** -->
    <label text="Choose ${name}:"/>
    <combo x:id="${name}" x:style="BORDER|READ_ONLY">
    <#list suffix.substring(1).split("/") as option>
      <#assign lastOption=option.trim()>
      <add x:p0="${option.trim()}"/>
    </#list>
    </combo>
    <@setoutput file=propsFile/>
${name} = ${lastOption}
    <@setoutput file=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput file=xswtFile/>
  </#if>

</#list>  
  
  </x:children>
</xswt>
