<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:output doctype-public="-//OASIS//DTD DocBook XML V4.5//EN"
            doctype-system="http://www.oasis-open.org/docbook/xml/4.5/docbookx.dtd"/>

<!-- "html" or "pdf" -->
<xsl:param name="output" select="'html'"/>

<xsl:template match="@*|node()">
   <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
   </xsl:copy>
</xsl:template>

<xsl:template match="commercial">
   <!-- <xsl:apply-templates select="@*|node()"/> -->
</xsl:template>

<xsl:template match="picture">
   <figure float="0">
      <xsl:attribute name="id"><xsl:value-of select="@file"/></xsl:attribute>
      <title><xsl:apply-templates select="node()"/></title>
      <screenshot>
         <mediaobject>
            <imageobject>
               <xsl:choose>
                  <xsl:when test="@width">
                     <imagedata fileref="{@file}" width="{@width}" align="center"/>
                  </xsl:when>
                  <xsl:when test="$output='pdf'">
                     <imagedata fileref="{@file}" scale="50" align="center"/>
                  </xsl:when>
                  <xsl:otherwise>
                     <imagedata fileref="{@file}" align="center"/>
                  </xsl:otherwise>
               </xsl:choose>
            </imageobject>
            <textobject>
               <phrase></phrase>
            </textobject>
         </mediaobject>
      </screenshot>
   </figure>
</xsl:template>

<xsl:template match="icon">
   <guibutton>
      <inlinegraphic>
         <xsl:if test="$output='pdf'">
            <xsl:attribute name="scale">60</xsl:attribute>
         </xsl:if>
         <xsl:attribute name="fileref">
            <xsl:choose>
               <xsl:when test="@file">
                  <xsl:value-of select="@file"/>
               </xsl:when>
               <xsl:otherwise>
                  <xsl:value-of select="concat('icons/',@name)"/>
               </xsl:otherwise>
            </xsl:choose>
         </xsl:attribute>
      </inlinegraphic>
   </guibutton>
</xsl:template>

<xsl:template match="eventnumber">
   event #<xsl:apply-templates select="node()"/>
</xsl:template>

<xsl:template match="inifile">
   <screen><xsl:apply-templates select="node()"/></screen>
</xsl:template>

<xsl:template match="eventlog">
   <screen><xsl:apply-templates select="node()"/></screen>
</xsl:template>

<xsl:template match="chart-properties">
  <table>
    <title><xsl:apply-templates/></title>
    <!-- Aaaah. If I put more groups here, there won't be rowsep between them. -->
    <tgroup cols='2' rowsep='1'>
      <colspec colname='name' colwidth='1.5in'/>
      <colspec colname='description' colwidth='3*'/>
      <spanspec spanname='category' namest='name' nameend='description' align='left'/>
      <tbody>
        <xsl:call-template name="chart.properties.groups">
          <xsl:with-param name="groups" select="normalize-space(@groups)"/>
        </xsl:call-template>
      </tbody>
    </tgroup>
  </table>
</xsl:template>

<!-- Iterating on a list of tokens is not a one-liner in XSLT -->
<xsl:template name="chart.properties.groups">
  <xsl:param name="groups"/>
  <xsl:choose>
    <xsl:when test="contains($groups, ' ')">
      <xsl:call-template name="chart.properties.group">
        <xsl:with-param name="group" select="substring-before($groups, ' ')"/>
      </xsl:call-template>
      <xsl:call-template name="chart.properties.groups">
        <xsl:with-param name="groups" select="substring-after($groups, ' ')"/>
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:call-template name="chart.properties.group">
        <xsl:with-param name="group" select="$groups"/>
      </xsl:call-template>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

<xsl:template name="chart.properties.group">
  <xsl:param name="group"/>
  <xsl:apply-templates select="document('chart-properties.xml')//tgroup[@id=$group]/tbody/row"/>
</xsl:template>


</xsl:stylesheet>
