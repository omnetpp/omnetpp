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

    <label x:text="To enable the second wizard page, tick the checkbox and select a nonzero value below:" x:style="WRAP">
      <layoutData x:class="GridData" horizontalSpan="2" horizontalAlignment="FILL" grabExcessHorizontalSpace="true"/>
    </label>

    <button x:id="isAdvanced" text="Advanced" x:style="CHECK">
      <layoutData x:class="GridData" horizontalSpan="2"/>
    </button>

    <label text=Value:"/>
    <spinner x:id="someValue" minimum="0" maximum="10" x:style="BORDER"/>

  </x:children>
</xswt>
