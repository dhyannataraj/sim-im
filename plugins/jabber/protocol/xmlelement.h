
#ifndef XMLELEMENT_H
#define XMLELEMENT_H

#include <memory>
#include <QList>
#include <QMap>
#include <QString>

class XmlElement
{
public:
    typedef std::shared_ptr<XmlElement> Ptr;
    typedef std::weak_ptr<XmlElement> WeakPtr;

    XmlElement(const QString& name, const XmlElement::Ptr& parent = XmlElement::Ptr());
    virtual ~XmlElement();

    static XmlElement::Ptr create(const QString& name, const XmlElement::Ptr& parent = XmlElement::Ptr());

    XmlElement::Ptr parent() const;
    QString name() const;

    void addChild(const XmlElement::Ptr& el);
    QList<XmlElement::Ptr> children() const;
    XmlElement::Ptr firstChild(const QString& name);

    void setAttribute(const QString& key, const QString& val);
    QString attribute(const QString& key);

    void appendText(const QString& chars);
    QString text() const;

private:
    QString m_name;
    XmlElement::WeakPtr m_parent;
    QList<XmlElement::Ptr> m_children;
    QMap<QString,QString> m_attributes;
    QString m_text;
};

#endif

