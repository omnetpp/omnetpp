<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

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
      <title><xsl:apply-templates select="node()"/></title>
      <screenshot>
        <mediaobject>
          <imageobject>
             <xsl:choose>
                <xsl:when test="@width">
                   <imagedata fileref="{@file}" width="{@width}" align="center"/>
                </xsl:when>	
                <xsl:otherwise>	
                   <imagedata fileref="{@file}" scale="50" align="center"/>
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

<xsl:key name="icon" match="//icons/icon/@file" use="../@name"/>

<xsl:template match="icon">
   <guibutton>
      <inlinegraphic>
         <xsl:variable name="name" select="@name"/>
         <xsl:for-each select="document('icons.xml')">
            <xsl:attribute name="fileref">
               <xsl:value-of select="concat('../../../ui/',key('icon', $name))"/>
            </xsl:attribute>
         </xsl:for-each>
      </inlinegraphic>
   </guibutton>
</xsl:template>

<xsl:template match="inifile">
   <screen><xsl:apply-templates select="node()"/></screen>
</xsl:template>

<xsl:template match="eventlog">
   <screen><xsl:apply-templates select="node()"/></screen>
</xsl:template>

</xsl:stylesheet>
