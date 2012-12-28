
#include "xmlelement.h"
#include <algorithm>

XmlElement::XmlElement(const QString& name, const XmlElement::Ptr& parent) :
    m_name(name),
    m_parent(parent)
{
}

XmlElement::~XmlElement()
{
}

XmlElement::Ptr XmlElement::create(const QString& name, const XmlElement::Ptr& parent)
{
    auto el = std::make_shared<XmlElement>(name, parent);
    if(parent)
        parent->addChild(el);
    return el;
}

XmlElement::Ptr XmlElement::parent() const
{
    if(m_parent.expired())
        return XmlElement::Ptr();
    return m_parent.lock();
}

QString XmlElement::name() const
{
    return m_name;
}

void XmlElement::addChild(const XmlElement::Ptr& el)
{
    m_children.append(el);
}

QList<XmlElement::Ptr> XmlElement::children() const
{
    return m_children;
}

XmlElement::Ptr XmlElement::firstChild(const QString& name)
{
    auto it = std::find_if(m_children.begin(), m_children.end(), [&](const XmlElement::Ptr& e) { return e->name() == name; });
    if(it == m_children.end())
        return XmlElement::Ptr();
    return *it;
}

void XmlElement::setAttribute(const QString& key, const QString& val)
{
    m_attributes[key] = val;
}

QString XmlElement::attribute(const QString& key)
{
    return m_attributes[key];
}

void XmlElement::appendText(const QString& chars)
{
    m_text.append(chars);
}

QString XmlElement::text() const
{
    return m_text;
}

