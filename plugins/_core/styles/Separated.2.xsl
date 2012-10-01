<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" />
  <xsl:template match="/">
    <xsl:for-each select="/messages/message">
      <!--line between messages -->
      <hr />
      <!-- message header :: BEGIN -->
      <table width="100%" align="center" border="0" cellpadding="4" cellspacing="0">
        <xsl:if test="@direction='1'">
          <xsl:attribute name="bgcolor">#cccccc</xsl:attribute>
          <!-- incoming -->
        </xsl:if>
        <!-- background color of incoming/outgoing message -->
        <xsl:if test="@direction='0'">
          <xsl:attribute name="color">#fafafa</xsl:attribute>
          <!-- outgoing -->
        </xsl:if>
        <tr>
          <xsl:choose>
            <!-- header with changed user state :: BEGIN -->
            <xsl:when test="@direction='2'">
              <!--dummy :); change of user state are single messages, becose of ??parser?? bug -->
            </xsl:when>
            <!-- header with changed user state :: END -->
            <xsl:otherwise>
              <!-- Icons :: BEGIN -->
              <td width="60" nowrap="yes">
                <!-- message link & message icon :: BEGIN -->
                <a>
                  <xsl:attribute name="href">
                    msg://<xsl:value-of select="id" />
                  </xsl:attribute>
                  <img>
                    <xsl:attribute name="src">
                      sim:icons/<xsl:value-of select="icon" />
                    </xsl:attribute>
                  </img>
                </a>
                <xsl:if test="@encrypted='1'">
                  <xsl:text> </xsl:text>
                  <img src="sim:icons/encrypted" />
                </xsl:if>
                <xsl:if test="@urgent='1'">
                  <xsl:text> </xsl:text>
                  <img src="sim:icons/urgentmsg" />
                </xsl:if>
                <xsl:if test="@list='1'">
                  <xsl:text> </xsl:text>
                  <img src="sim:icons/listmsg" />
                </xsl:if>
              </td>
              <td nowrap="yes">
                <span>
                  <xsl:if test="@unread='1'">
                    <xsl:attribute name="style">font-weight:600</xsl:attribute>
                  </xsl:if>
                  <font>
                    <xsl:choose>
                      <xsl:when test="@direction='0'">
                        <xsl:attribute name="color">#660000</xsl:attribute>
                      </xsl:when>
                      <xsl:when test="@direction='1'">
                        <xsl:attribute name="color">#000066</xsl:attribute>
                      </xsl:when>
                      <xsl:value-of disable-output-escaping="yes" select="from" />
                    </xsl:choose>
                  </font>
                </span>
              </td>
              <td align="right" nowrap="yes">
                <font color="#666666">
                  <xsl:if test="@direction='2'">
                    <xsl:attribute name="color">#ffffff</xsl:attribute>
                  </xsl:if>
                  <xsl:text> </xsl:text>
                  <xsl:value-of select="time/date" />
                  <xsl:text> </xsl:text>
                  <xsl:value-of select="time/hour" />:<xsl:value-of select="time/minute" />:<xsl:value-of select="time/second" />
                </font>
              </td>
            </xsl:otherwise>
          </xsl:choose>
        </tr>
        <tr>
          <td colspan="3">
            <xsl:if test="body/@bgcolor">
              <xsl:attribute name="bgcolor">
                <xsl:value-of select="body/@bgcolor" />
              </xsl:attribute>
            </xsl:if>
            <span>
              <xsl:attribute name="style">
                <xsl:if test="body/@fgcolor">
                  color:<xsl:value-of select="body/@fgcolor" />;
                </xsl:if>
              </xsl:attribute>
              <xsl:value-of disable-output-escaping="yes" select="body" />
            </span>
          </td>
        </tr>
      </table>
    </xsl:for-each>
  </xsl:template>
</xsl:stylesheet>