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

    <label x:text="This wizard will generate a compound module, with the submodules you specify. Enter submodule names below, separated by comma. The next page will ask for the NED type of each." x:style="WRAP">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </label>

    <label text="Submodule names:"/>
    <text x:id="submoduleNames" x:style="BORDER">
      <layoutData x:class="GridData" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </text>

  </x:children>
</xswt>
