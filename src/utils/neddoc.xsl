<!--
==============================================================
 File: neddoc.xsl - part of OMNeT++
==============================================================

- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Copyright (C) 2002-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
-->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:output method="html" indent="yes" encoding="iso-8859-1"/>

<xsl:key name="module" match="//simple-module|//compound-module" use="@name"/>
<xsl:key name="channel" match="//channel" use="@name"/>

<xsl:template match="/">
   <html>
      <head>
         <style TYPE="text/css">
           * { font-family: sans-serif }
           h1 { font-size=18pt; text-align=center }
           h2 { font-size=14pt; background=#D0E0FF; padding-left=5pt; margin-top=36pt }
           h3 { font-size=12pt; margin-top=3pt; margin-bottom=0 }
           pre { font-size=10pt; background=#E0E0E0; padding-left=5pt }
           th { font-size=10pt; background=#E0E0E0 }
           td { font-size=10pt }
           p { font-size=10pt }
           ul { font-size=10pt }
         </style>
      </head>
      <body>
         <h1>Module Reference</h1>
         <xsl:call-template name="create-index"/>
         <xsl:call-template name="create-fileindex"/>
         <xsl:apply-templates/>
         <hr/>
         <font size="-1"><i>generated via XML by neddoc.xsl</i></font>
      </body>
   </html>
</xsl:template>

<xsl:template name="create-index">
   <h2>Index</h2>
   <h3>Channels</h3>
   <ul>
      <xsl:for-each select="//channel">
         <xsl:sort select="@name"/>
         <li>
            <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a>
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
         </li>
      </xsl:for-each>
   </ul>
   <h3>Modules</h3>
   <ul>
      <xsl:for-each select="//simple-module|//compound-module">
         <xsl:sort select="@name"/>
         <li>
            <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a>
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
         </li>
      </xsl:for-each>
   </ul>
   <h3>Networks</h3>
   <ul>
      <xsl:for-each select="//network">
         <xsl:sort select="@name"/>
         <li>
            <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a>
            <i> (<xsl:value-of select="ancestor::ned-file/@filename"/>)</i>
         </li>
      </xsl:for-each>
   </ul>
</xsl:template>

<xsl:template name="create-fileindex">
   <h2>File Index</h2>
   <xsl:for-each select="//ned-file">
      <xsl:sort select="@filename"/>
      <xsl:variable name="fileid" select="generate-id(.)"/>
      <a name="{generate-id(.)}"/>
      <h3><xsl:value-of select="@filename"/></h3>
      <ul>
         <xsl:for-each select="simple-module|compound-module|channel|network">
            <xsl:sort select="@name"/>
            <li>
               <a href="#{concat(@name,'-',$fileid)}"><xsl:value-of select="@name"/></a>
               <i> (<xsl:value-of select="local-name()"/>)</i>
            </li>
         </xsl:for-each>
      </ul>
   </xsl:for-each>
</xsl:template>

<xsl:template match="channel">
   <h2><a name="{concat(@name,'-',generate-id(ancestor::ned-file))}"/>Channel <i><xsl:value-of select="@name"/></i></h2>
   <xsl:call-template name="print-file"/>
   <xsl:call-template name="process-comment"/>
   <xsl:call-template name="print-attrs"/>
   <xsl:call-template name="print-channel-used-in"/>
</xsl:template>

<xsl:template match="simple-module">
   <a name="{concat(@name,'-',generate-id(ancestor::ned-file))}"/>
   <h2>Simple Module <i><xsl:value-of select="@name"/></i></h2>
   <xsl:call-template name="print-file"/>
   <xsl:call-template name="process-comment"/>
   <xsl:call-template name="print-params"/>
   <xsl:call-template name="print-gates"/>
   <xsl:call-template name="print-module-used-in"/>
</xsl:template>

<xsl:template match="compound-module">
   <a name="{concat(@name,'-',generate-id(ancestor::ned-file))}"/>
   <h2>Compound Module <i><xsl:value-of select="@name"/></i></h2>
   <xsl:call-template name="print-file"/>
   <xsl:call-template name="process-comment"/>
   <xsl:call-template name="print-params"/>
   <xsl:call-template name="print-gates"/>
   <xsl:call-template name="print-uses"/>
   <xsl:call-template name="print-module-used-in"/>
</xsl:template>

<xsl:template match="network">
   <a name="{concat(@name,'-',generate-id(ancestor::ned-file))}"/>
   <h2>Network <i><xsl:value-of select="@name"/></i></h2>
   <xsl:call-template name="print-file"/>
   <xsl:call-template name="process-comment"/>
   <xsl:call-template name="print-type"/>
   <xsl:call-template name="print-substparams"/>
</xsl:template>

<xsl:template name="print-file">
   <xsl:if test="ancestor::ned-file/@filename">
      <b>File: <a href="#{generate-id(ancestor::ned-file)}"><xsl:value-of select="ancestor::ned-file/@filename"/></a></b>
   </xsl:if>
</xsl:template>

<xsl:template name="print-params">
   <xsl:if test="params/param">
      <h3>Parameters:</h3>
      <table>
         <xsl:for-each select="params/param">
            <xsl:sort select="@name"/>
            <tr>
               <td width="10"></td>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="100">
                  <i>
                  <xsl:choose>
                     <xsl:when test="@data-type"><xsl:value-of select="@data-type"/></xsl:when>
                     <xsl:otherwise>numeric</xsl:otherwise>
                  </xsl:choose>
                  </i>
               </td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-gates">
   <xsl:if test="gates/gate">
      <h3>Gates:</h3>
      <table>
         <xsl:for-each select="gates/gate">
            <xsl:sort select="@name"/>
            <tr>
               <td width="10"></td>
               <td width="150"><xsl:value-of select="@name"/><xsl:if test="@is-vector='yes'"> [ ]</xsl:if></td>
               <td width="100"><i><xsl:value-of select="@direction"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-attrs">
   <xsl:if test="channel-attr">
      <h3>Attributes:</h3>
      <table>
         <xsl:for-each select="channel-attr">
            <xsl:sort select="@name"/>
            <tr>
               <td width="10"></td>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="300"><i><xsl:value-of select="@value"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-substparams">
   <xsl:if test="substparams/substparam">
      <h3>Parameter values:</h3>
      <table>
         <xsl:for-each select="substparams/substparam">
            <xsl:sort select="@name"/>
            <tr>
               <td width="10"></td>
               <td width="150"><xsl:value-of select="@name"/></td>
               <td width="300"><i><xsl:value-of select="@value"/></i></td>
               <td><xsl:call-template name="process-tablecomment"/></td>
            </tr>
         </xsl:for-each>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-uses">
   <xsl:if test="key('module',.//submodule/@type-name)">
      <h3>Uses modules:</h3>
      <table>
         <tr>
            <td width="10"></td>
            <td>
               <xsl:for-each select="key('module',.//submodule/@type-name)">
                  <xsl:sort select="@name"/>
                  <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
               </xsl:for-each>
            </td>
         </tr>
      </table>
   </xsl:if>
   <xsl:if test="key('channel',.//conn-attr[@name='channel']/@value)">
      <h3>Uses channels:</h3>
      <table>
         <tr>
            <td width="10"></td>
            <td>
               <xsl:for-each select="key('channel',.//conn-attr[@name='channel']/@value)">
                  <xsl:sort select="@name"/>
                  <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
               </xsl:for-each>
            </td>
         </tr>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-module-used-in">
   <xsl:variable name="name" select="@name"/>
   <xsl:if test="//compound-module[.//submodule[@type-name=$name]]">
      <h3>Used in compound modules:</h3>
      <table>
         <tr>
            <td width="10"></td>
            <td>
               <xsl:for-each select="//compound-module[.//submodule[@type-name=$name]]">
                  <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
               </xsl:for-each>
            </td>
         </tr>
      </table>
   </xsl:if>
   <xsl:if test="//network[@type-name=$name]">
      <h3>Used in networks:</h3>
      <table>
         <tr>
            <td width="10"></td>
            <td>
               <xsl:for-each select="//network[@type-name=$name]">
                  <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
               </xsl:for-each>
            </td>
         </tr>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-channel-used-in">
   <xsl:variable name="name" select="@name"/>
   <xsl:if test="//compound-module[.//conn-attr[@name='channel' and @value=$name]]">
      <h3>Used in compound modules:</h3>
      <table>
         <tr>
            <td width="10"></td>
            <td>
               <xsl:for-each select="//compound-module[.//conn-attr[@name='channel' and @value=$name]]">
                  <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
               </xsl:for-each>
            </td>
         </tr>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="print-type">
   <xsl:if test="key('module',@type-name)">
      <h3>Instance of:</h3>
      <table>
         <tr>
            <td width="10"></td>
            <td>
               <xsl:for-each select="key('module',@type-name)">
                  <xsl:sort select="@name"/>
                  <a href="#{concat(@name,'-',generate-id(ancestor::ned-file))}"><xsl:value-of select="@name"/></a><xsl:text> </xsl:text>
               </xsl:for-each>
            </td>
         </tr>
      </table>
   </xsl:if>
</xsl:template>

<xsl:template name="process-tablecomment">
   <xsl:param name="comment" select="@banner-comment|@right-comment"/>
   <xsl:choose>
      <xsl:when test="$comment"><xsl:value-of select="$comment"/></xsl:when>
      <xsl:otherwise> </xsl:otherwise>
   </xsl:choose>
</xsl:template>

<xsl:template name="process-comment">
   <xsl:param name="comment" select="@banner-comment"/>
   <pre><xsl:value-of select="$comment"/></pre>
</xsl:template>

</xsl:stylesheet>


