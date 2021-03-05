#include "elf/elf++.hh"

#include "section.h"

QString Section::name() const
{
    return m_name;
}

void Section::setName(const QString &name)
{
    m_name = name;
}

uint32_t Section::addr() const
{
    return m_addr;
}

void Section::setAddr(const uint32_t &addr)
{
    m_addr = addr;
}

const void *Section::pData() const
{
    return m_pData;
}

size_t Section::size() const
{
    return m_size;
}

void Section::setSize(const size_t &size)
{
    m_size = size;
}

Section::Section()
{
}


Section::Section(const elf::section &section)
{
    m_name = QString(section.get_name().c_str());
    m_addr = section.get_hdr().addr;
    m_size = section.size();
    m_pData = section.data();
}

bool Section::operator==(const Section& rhs) const
{
    if (m_name == rhs.m_name)
    {
        return true;
    }

    return false;
}

bool Section::operator!=(const Section& rhs) const
{
    if (m_name == rhs.m_name)
    {
        return false;
    }

    return true;
}
