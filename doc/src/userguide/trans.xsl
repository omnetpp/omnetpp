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
      <xsl:attribute name="id"><xsl:value-of select="@file"/></xsl:attribute>
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

<xsl:template match="icon">
   <guibutton>
      <inlinegraphic scale="60">
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

</xsl:stylesheet>
