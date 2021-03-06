<?xml version="1.0"?> 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>
<xsl:template match="/message"><p>
<a>
<xsl:attribute name="href">msg://<xsl:value-of select="id"/></xsl:attribute>
<img border="0">
<xsl:attribute name="src">icon:<xsl:value-of select="icon"/></xsl:attribute>
</img>
<xsl:if test="@encrypted='1'">
<img src="icon:encrypted" border="0"/>
</xsl:if>
<xsl:if test="@urgent='1'">
<img src="icon:urgentmsg" border="0"/>
</xsl:if>
<xsl:if test="@list='1'">
<img src="icon:listmsg" border="0"/>
</xsl:if>
</a>
<xsl:text> </xsl:text>
<xsl:choose>
<xsl:when test="@direction='2'">
<font>
<xsl:attribute name="color">#808080</xsl:attribute>
<xsl:text> </xsl:text>
<xsl:value-of select="time/date"/>
<xsl:text> </xsl:text>
<xsl:value-of select="time/hour"/>:<xsl:value-of select="time/minute"/>:<xsl:value-of select="time/second"/>
<xsl:text> </xsl:text>
<xsl:value-of disable-output-escaping="yes" select="from"/>
</font>
</xsl:when>
<xsl:otherwise>
<span>
<xsl:if test="@unread='1'">
<xsl:attribute name="style">font-weight:600</xsl:attribute>
</xsl:if>
<xsl:text> </xsl:text>
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
<xsl:text> </xsl:text>
<font size="-1">
<xsl:text> </xsl:text>
<xsl:value-of select="time/date"/>
<xsl:text> </xsl:text>
<xsl:value-of select="time/hour"/>:<xsl:value-of select="time/minute"/>:<xsl:value-of select="time/second"/>
</font></span>
</xsl:otherwise>
</xsl:choose>
</p><p>
<xsl:attribute name="style"><xsl:if test="body/@bgcolor">background-color:<xsl:value-of select="body/@bgcolor"/>;</xsl:if><xsl:if test="body/@fgcolor">color:<xsl:value-of select="body/@fgcolor"/>;</xsl:if></xsl:attribute>
<xsl:value-of disable-output-escaping="yes" select="body"/>
</p>
</xsl:template>
</xsl:stylesheet>