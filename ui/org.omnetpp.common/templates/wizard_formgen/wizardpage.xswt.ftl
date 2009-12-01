<#assign xswtFile="wizardpage.xswt">
<#assign propsFile="template.properties">
<#assign nedFile="output.ned.ftl">
<@setoutput path=propsFile/>
# adjust the following
templateName = Newly Generated Wizard, with an Input Page
templateDescription = Wizard with a single input page
templateCategory = Generated Wizards
supportedWizardTypes =

# custom wizard pages
page.1.file = wizardpage.xswt
page.1.title = Generated Page
page.1.description = Select options below

# variables
<@setoutput path=nedFile/>
<#noparse><@setoutput path=targetFileName?default("")/>
${bannerComment}

<#if nedPackageName!="">package ${nedPackageName};</#if>

</#noparse>
<@setoutput path=xswtFile/>
<?xml version="1.0" encoding="UTF-8"?>
<xswt xmlns:x="http://sweet_swt.sf.net/xswt">

  <import xmlns="http://sweet_swt.sf.net/xswt">
    <package name="java.lang"/>
    <package name="org.eclipse.swt.widgets" />
    <package name="org.eclipse.swt.graphics" />
    <package name="org.eclipse.swt.layout" />
    <package name="org.eclipse.swt.custom" />
    <package name="org.omnetpp.common.wizard.support" />
    <package name="org.omnetpp.ned.editor.wizards.support" />
    <package name="org.omnetpp.cdt.wizard.support" />
  </import>
  <layout x:class="GridLayout" numColumns="2"/>

  <x:children>

    <label x:text="This is a generated wizard page, to be refined manually. It was created with the &quot;${templateName}&quot; wizard." x:style="WRAP">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </label>

    <!-- page generated from the following spec:
<#if !spec?contains("\n")>      </#if>${spec}    
    -->
    
<#function makeLabel rawname>
  <#assign label = rawname?replace("([a-z0-9_])([A-Z])", "$1 $2", "r")>
  <#assign label = StringUtils.capitalize(label?lower_case)>
  <#return label> 
</#function>
<#function makeIdentifier rawname>
  <#return StringUtils.uncapitalize(StringUtils.makeValidIdentifier(WordUtils.capitalize(rawname?trim)),1)>
</#function>

<#assign spec = spec?replace("{", "{,")?replace("}", ",},")?replace(",,", ",")>
<#list StringUtils.split(spec, ",") as i>
  <#assign field = i.trim()>
    <!-- ${field} -->
  <#assign rawname = field.replaceFirst("^([-a-zA-Z0-9_ \t]*).*", "$1")>
  <#assign suffix = StringUtils.removeStart(field,rawname)?trim>
  <#assign label = makeLabel(rawname?trim)>
  <#assign name = makeIdentifier(rawname?trim)>
  <#if field=="">
    <#-- nothing, skip it -->
  <#elseif rawname?trim=="--">
    <#-- **** SEPARATOR **** -->
    <label x:style="SEPARATOR|HORIZONTAL">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </label>
  <#elseif field.matches("[^:]*/.*")>
    <#-- **** RADIOBUTTON GROUP **** -->
    <composite>
      <layoutData x:class="GridData" horizontalSpan="2"/>
      <layout x:class="GridLayout"/>
      <x:children>
      <#assign first=true>
      <#list field.split("/") as j>
        <#assign option = makeIdentifier(j?trim)>
        <button x:id="${option}" text="${makeLabel(j?trim)}" x:style="RADIO"/>
        <@setoutput path=propsFile/>
${option} = <#if first>true<#else>false</#if>
        <@setoutput path=nedFile/>
// ${option}: <#noparse><#if</#noparse> ${option}<#noparse>>true<#else>false</#if></#noparse>
        <@setoutput path=xswtFile/>
        <#assign first=false>
      </#list>
      </x:children>
    </composite>
  <#elseif suffix=="">
    <#-- **** TEXT **** -->
    <label text="${label}:"/>
    <text x:id="${name}" x:style="BORDER">
      <layoutData x:class="GridData" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </text>
    <@setoutput path=propsFile/>
${name} = some text
    <@setoutput path=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput path=xswtFile/>
  <#elseif suffix=="\"">
    <#-- **** LABEL **** -->
    <label text="${label}">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </label>
  <#elseif suffix=="<" || suffix=="<<">
    <#-- **** FILECHOOSER **** -->
    <#if suffix=="<"><#assign chooserWidget="fileChooser"><#else><#assign chooserWidget="externalFileChooser"></#if>
    <label text="${label}:"/>
    <${chooserWidget} x:id="${name}">
      <layoutData x:class="GridData" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </${chooserWidget}>
    <@setoutput path=propsFile/>
${name} = foo.txt
    <@setoutput path=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput path=xswtFile/>
  <#elseif suffix=="$">
    <#-- **** MULTILINE **** -->
    <label text="${label}:"/>
    <text x:id="${name}" x:style="BORDER|WRAP|MULTI">
      <layoutData x:class="GridData" heightHint="60" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </text>
    <@setoutput path=propsFile/>
${name} = some more text
    <@setoutput path=nedFile/>
    <@setoutput path=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput path=xswtFile/>
  <#elseif suffix=="%">
    <#-- **** SPINNER **** -->
    <label text="${label}:"/>
    <spinner x:id="${name}" minimum="0" maximum="100" x:style="BORDER"/>
    <@setoutput path=propsFile/>
${name} = 10
    <@setoutput path=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput path=xswtFile/>
  <#elseif suffix=="?">
    <#-- **** CHECKBOX **** -->
    <button x:id="${name}" text="${label}" x:style="CHECK">
      <layoutData x:class="GridData" horizontalSpan="2"/>
    </button>
    <@setoutput path=propsFile/>
${name} = false
    <@setoutput path=nedFile/>
// ${name}: <#noparse><#if</#noparse> ${name}<#noparse>>true<#else>false</#if></#noparse>
    <@setoutput path=xswtFile/>
  <#elseif suffix=="=">
    <#-- **** SCALE **** -->
    <label text="${label} (0..100):"/>
    <scale x:id="${name}" minimum="0" maximum="100"/>
    <@setoutput path=propsFile/>
${name} = 40
    <@setoutput path=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput path=xswtFile/>
  <#elseif suffix?starts_with(":")>
    <#-- **** COMBOBOX **** -->
    <label text="${label}:"/>
    <combo x:id="${name}" x:style="BORDER|READ_ONLY">
    <#list suffix?substring(1)?split("/") as option>
      <#assign lastOption=option?trim>
      <add x:p0="${option?trim}"/>
    </#list>
    </combo>
    <@setoutput path=propsFile/>
${name} = ${lastOption}
    <@setoutput path=nedFile/>
// ${name}: ${"${" + name + "}"}
    <@setoutput path=xswtFile/>
  <#elseif suffix=="{">
    <#-- **** GROUP START **** -->
    <group text="${label}">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
      <layout x:class="GridLayout" numColumns="2"/>
      <x:children>
  <#elseif suffix=="}">
    <#-- **** GROUP END **** -->
      </x:children>
    </group>
  <#else>
    <!-- unrecognized suffix "${suffix}" -->
  </#if>

</#list>

  </x:children>
</xswt>
