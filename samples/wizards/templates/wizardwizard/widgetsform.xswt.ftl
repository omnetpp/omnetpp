<@setoutput file=wizardFolder+"/widgetsform.xswt"/>
<#if wantWidgetsForm>
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
  <layout x:class="GridLayout" x:numColumns="1"/>

  <x:children>
  <#if wantLabel>  
    <!-- Label -->  
    <label text="This is a label"/>
  </#if>
  <#if wantWrappingLabel>  
    <!-- Wrapping label -->
    <label text="This is a very long label. Its text surely does not fit on a single line on the screen, so we want it to wrap automatically, depending on how much available space it has." x:style="WRAP">
      <layoutData x:class="GridData" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </label>
  </#if>
  <#if wantReadonlyText>  
    <!-- Message (selectable multi-line label) -->
    <text x:style="READ_ONLY|WRAP" text="This is a multi-line readonly wrapping text widget. It looks very much like a label, but its content is selectable, so it makes a good candidate for long descriptions.">
      <layoutData x:class="GridData" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </text>
  </#if>
  <#if wantText>  
    <!-- Text edit field -->
    <text x:id="exampleTextVar" x:style="BORDER">
      <layoutData x:class="GridData" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </text>
  </#if>
  <#if wantMultilineText>  
    <!-- Multi-line text editor -->
    <text x:id="exampleMultilineTextVar" x:style="BORDER|MULTI|WRAP">
      <layoutData x:class="GridData" heightHint="100" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
    </text>
  </#if>
  <#if wantCombo>  
    <!-- Combobox -->
    <combo x:id="exampleTextVar2" x:style="BORDER|READ_ONLY">
      <add x:p0="grid"/>
      <add x:p0="random"/>
      <add x:p0="sinewave"/>
      <add x:p0="unpinned"/>
    </combo>
    <!-- Combobox, editable version -->
    <combo x:id="exampleTextVar3" x:style="BORDER">
      <add x:p0="grid"/>
      <add x:p0="random"/>
      <add x:p0="sinewave"/>
      <add x:p0="unpinned"/>
    </combo>
  </#if>
  <#if wantList>  
    <!-- Listbox (remove MULTI for single-select behavior) -->
    <list x:id="exampleTextVar4" x:style="BORDER|MULTI">
      <add x:p0="red"/>
      <add x:p0="green"/>
      <add x:p0="blue"/>
      <add x:p0="alpha"/>
    </list>
  </#if>
  <#if wantCheckbox>  
    <!-- Checkbox -->
    <button x:id="exampleBooleanVar1" text="Check this" x:style="CHECK"/>
  </#if>
  <#if wantRadioButtons>  
    <!-- Radio Buttons -->
    <button x:id="exampleBooleanVar2a" text="Plan 1" x:style="RADIO"/>
    <button x:id="exampleBooleanVar2b" text="Plan 2" x:style="RADIO"/>
    <button x:id="exampleBooleanVar2c" text="Plan 9" x:style="RADIO"/>
  </#if>
  <#if wantGroup>  
    <!-- Labelled group -->
    <!-- Labelled group -->
    <group text="Group">
      <layoutData x:class="GridData" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
      <layout x:class="GridLayout" x:numColumns="2"/>
      <x:children>
          <label text="Type something:"/>
          <text x:id="exampleTextVar10" x:style="BORDER"/>
          <button x:id="exampleBooleanVar10" text=" And check this" x:style="CHECK">
              <layoutData x:class="GridData" horizontalSpan="2"/>
          </button>
      </x:children>
    </group>
  </#if>
  <#if wantComposite>  
    <!-- Panel (composite) -->
    <composite x:style="BORDER">
      <layoutData x:class="GridData" horizontalAlignment="FILL" x:grabExcessHorizontalSpace="true"/>
      <layout x:class="GridLayout" x:numColumns="2"/>
      <x:children>
          <label text="Type something:"/>
          <text x:id="exampleTextVar11" x:style="BORDER"/>
          <button x:id="exampleBooleanVar11" text=" And check this" x:style="CHECK">
              <layoutData x:class="GridData" horizontalSpan="2"/>
          </button>
      </x:children>
    </composite>
  </#if>
  <#if wantHSep>  
    <!-- Horizontal separator -->
    <label x:style="SEPARATOR|HORIZONTAL"/>
  </#if>
  <#if wantVSep>  
    <!-- Vertical separator -->
    <label x:style="SEPARATOR|VERTICAL"/>
  </#if>
  <#if wantTabs>  
    <!-- Tabbed panel -->
    <tabFolder>
      <layoutData x:class="gridData" grabExcessHorizontalSpace="true" grabExcessVerticalSpace="true" horizontalAlignment="FILL" verticalAlignment="FILL"/>
      <x:children>
        <composite x:id="page1">
          <layoutData x:class="gridData" grabExcessHorizontalSpace="true" horizontalAlignment="GridData.FILL"/>
          <layout x:class="gridLayout" numColumns="2"/>
          <x:children>
            <label text="Enter something:"/>
            <text x:style="BORDER"/>
          </x:children>
        </composite>

        <composite x:id="page2">
          <layoutData x:class="gridData" grabExcessHorizontalSpace="true" horizontalAlignment="GridData.FILL"/>
          <layout x:class="gridLayout" numColumns="2"/>
          <x:children>
            <button text="Check this" x:style="CHECK"/>
          </x:children>
        </composite>

        <tabItem text="Tab 1" control="page1"/>
        <tabItem text="Tab 2" control="page2"/>
        
      </x:children>
    </tabFolder>
  </#if>
  <#if wantScale>  
    <!-- Scale -->
    <scale x:id="exampleNumericVar10" x:minimum="-100" x:maximum="100" x:style="NONE"/>
  </#if>
  <#if wantSlider>  
    <!-- Slider ("scrollbar") -->
    <slider x:id="exampleNumericVar11" x:minimum="1" x:maximum="100" x:style="BORDER"/>
  </#if>
  <#if wantSpinner>  
    <!-- Spinner (numeric edit field) -->
    <spinner x:id="exampleNumericVar12" x:minimum="1" x:maximum="100" x:style="BORDER"/>
  </#if>

  </x:children>

</xswt>
</#if>
