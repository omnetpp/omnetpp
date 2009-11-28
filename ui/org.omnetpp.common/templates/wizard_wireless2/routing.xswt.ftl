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
  <layout x:class="GridLayout"/>
  <x:children>
    <label text="Choose routing protocol:"/>
    <combo x:id="routingProtocol" x:style="BORDER">
        <add x:p0="AODV"/>
        <add x:p0="DSDV"/>
        <add x:p0="DYMO"/>
    </combo>
  </x:children>
</xswt>
