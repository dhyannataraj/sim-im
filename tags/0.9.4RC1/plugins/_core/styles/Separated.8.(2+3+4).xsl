<?xml version="1.0"?> 
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="html"/>
<xsl:template match="/message">

<table cellspacing="0" cellpadding="4" border="0" width="100%" align="center">
<xsl:if test="@direction='1'">
<xsl:attribute name="bgcolor">#cccccc</xsl:attribute>
</xsl:if>
<xsl:if test="@direction='0'">
<xsl:attribute name="color">#fafafa</xsl:attribute>
</xsl:if>
<tr>
<td nowrap="yes" align="left">
<span>
<xsl:if test="@unread='1'">
<xsl:attribute name="style">font-weight:600</xsl:attribute>
</xsl:if>
<font>
<xsl:if test="@direction='0'">
<xsl:attribute name="color">#660000</xsl:attribute>
</xsl:if>
<xsl:if test="@direction='1'">
<xsl:attribute name="color">#000066</xsl:attribute>
</xsl:if>
<a>
<xsl:attribute name="href">msg://<xsl:value-of select="id"/></xsl:attribute>
<xsl:value-of disable-output-escaping="yes" select="from"/>
</a>
</font></span>
</td>
<td align="right" nowrap="yes">
<font color="#666666">
<xsl:value-of select="time/date"/>
<xsl:text> </xsl:text>
<xsl:value-of select="time/hour"/>:<xsl:value-of select="time/minute"/>:<xsl:value-of select="time/second"/>
</font>
</td>
</tr>
<tr>
<td colspan="3">
<xsl:if test="body/@bgcolor">
<xsl:attribute name="bgcolor"><xsl:value-of select="body/@bgcolor"/></xsl:attribute>
</xsl:if>
<span>
<xsl:attribute name="style"><xsl:if test="body/@fgcolor">color:<xsl:value-of select="body/@fgcolor"/>;</xsl:if></xsl:attribute>
<xsl:value-of disable-output-escaping="yes" select="body"/>
</span>
</td>
</tr>
</table>
</xsl:template>
</xsl:stylesheet>
