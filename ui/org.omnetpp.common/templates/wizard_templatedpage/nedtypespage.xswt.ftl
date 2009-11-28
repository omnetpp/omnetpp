<#noparse>
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

<#list submoduleNames?split(",") as name>
    <label text="Type of ${name?trim}:"/>
    <nedTypeChooser x:id="${name?trim}Type" acceptedTypes="NedTypeChooser.MODULE">
      <layoutData x:class="GridData" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </nedTypeChooser>
</#list>

  </x:children>
</xswt>
</#noparse>
