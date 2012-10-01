<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>
  <xsl:template match="/">
    <xsl:for-each select="/messages/message">
<xsl:choose>
<xsl:when test="@direction='2'">
<font>
<prepend>
<xsl:attribute name="color">#808080</xsl:attribute>
[<xsl:value-of select="time/hour"/>:<xsl:value-of select="time/minute"/>:<xsl:value-of select="time/second"/>]
<xsl:text> &lt;</xsl:text>
<xsl:value-of disable-output-escaping="yes" select="from"/>
<xsl:text>&gt; </xsl:text>
</prepend>
<xsl:value-of disable-output-escaping="yes" select="body"/>
</font>
</xsl:when>
<xsl:otherwise>
<prepend>
<div>
<xsl:if test="@unread='1'">
<xsl:attribute name="style">font-weight:600</xsl:attribute>
</xsl:if>
[<xsl:value-of select="time/hour"/>:<xsl:value-of select="time/minute"/>:<xsl:value-of select="time/second"/>]
<xsl:text> &lt;</xsl:text>
<font>
<xsl:choose>
<xsl:when test="@direction='1'">
<xsl:attribute name="color">#800000</xsl:attribute>
</xsl:when>
<xsl:when test="@direction='0'">
<xsl:attribute name="color">#000080</xsl:attribute>
</xsl:when>
</xsl:choose>
<xsl:value-of disable-output-escaping="yes" select="from"/>
</font>
<xsl:text>&gt; </xsl:text>
</div>
</prepend>
<div>
<xsl:attribute name="style">
<xsl:if test="body/@bgcolor">
background-color:
<xsl:value-of select="body/@bgcolor"/>;
</xsl:if>
<xsl:if test="body/@fgcolor">
color:
<xsl:value-of select="body/@fgcolor"/>;
</xsl:if>
</xsl:attribute>
<xsl:value-of disable-output-escaping="yes" select="body"/>
</div>
</xsl:otherwise>
</xsl:choose>
    </xsl:for-each>
</xsl:template>
</xsl:stylesheet>
