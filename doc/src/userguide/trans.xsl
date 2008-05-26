<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version='1.0'>

<xsl:template match="@*|node()">
   <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
   </xsl:copy>
</xsl:template>

<xsl:template match="p">
   <para>
      <xsl:apply-templates select="@*|node()"/>
   </para>
</xsl:template>

<xsl:template match="picture">
<figure float="0">
   <title><xsl:apply-templates select="node()"/></title>
   <screenshot>
     <mediaobject>
       <imageobject>
          <imagedata fileref="{@file}" format=""/>
       </imageobject>
       <textobject>
          <phrase></phrase>
       </textobject>
     </mediaobject>
   </screenshot>
</figure>
</xsl:template>

</xsl:stylesheet>
	    
