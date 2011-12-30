<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  
  <xsl:template match="/">

    <xsl:for-each select="/messages/message">

      <p>
        <a>
          <xsl:attribute name="href">
            msg://<xsl:value-of select="id"/>
          </xsl:attribute>
          <img>
            <xsl:attribute name="src">sim:icons/<xsl:value-of select="icon"/></xsl:attribute>
          </img>

         
          <font style="font-size:14px;">
            <xsl:if test="@direction='0'">
              <xsl:attribute name="bgcolor">#e5e5e5</xsl:attribute>
              <!-- outgoing -->
            </xsl:if>
            <xsl:if test="@direction='1'">
              <xsl:attribute name="bgcolor">#b0b0b0</xsl:attribute>
              <!-- incoming -->
            </xsl:if>
            <xsl:attribute name="href">
              contact://<xsl:value-of select="source/contact_id" />
            </xsl:attribute>
        
            <xsl:value-of select="source/contact_name" />

            <xsl:text> </xsl:text>
            <xsl:value-of select="time/date"/>
            <xsl:text> </xsl:text>
            <xsl:value-of select="time/hour"/>:<xsl:value-of select="time/minute"/>:<xsl:value-of select="time/second"/>
          </font>
        </a>
      </p>

      <p>
        <xsl:value-of select="messagetext" />
      </p>

    </xsl:for-each>

  </xsl:template>
</xsl:stylesheet>