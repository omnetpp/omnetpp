<#if supportExport>
<?xml version="1.0" encoding="UTF-8"?>
<xswt xmlns:x="http://sweet_swt.sf.net/xswt">

  <import xmlns="http://sweet_swt.sf.net/xswt">
    <package name="java.lang"/>
    <package name="org.eclipse.swt.widgets" />
    <package name="org.eclipse.swt.graphics" />
    <package name="org.eclipse.swt.layout" />
    <package name="org.omnetpp.common.wizard.support" />
    <package name="org.omnetpp.ned.editor.wizards.support" />
    <package name="org.omnetpp.cdt.wizard.support" />
  </import>
  <layout x:class="GridLayout" numColumns="2"/>
  <x:children>
  
    <label x:style="WRAP" text="This example wizard exports the list of all NED types into a text file.">
        <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </label>
  
    <label text="Export to file:"/>
    <saveAsChooser x:id="fileName">
      <layoutData x:class="GridData" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
      <setFocus/>
    </saveAsChooser>

  </x:children>
</xswt>
</#if>
