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

</xsl:stylesheet>
	    
