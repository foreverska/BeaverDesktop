#include "sectioniterator.h"

SectionIterator::SectionIterator(std::shared_ptr<elf::elf> newElf)
{
    m_curSection = 0;
    m_pElf = newElf;
}

SectionIterator &SectionIterator::operator++()
{
    m_curSection++;
    return *this;
}

void SectionIterator::setPos(size_t pos)
{
    m_curSection = pos;
}

size_t SectionIterator::pos()
{
    return m_curSection;
}

SectionIterator SectionIterator::begin()
{
    m_curSection = 0;
    return *this;
}

SectionIterator SectionIterator::end()
{
    SectionIterator end(m_pElf);
    end.setPos(m_pElf->sections().size()-1);
    return end;
}

Section SectionIterator::operator*()
{
    Section newSection(m_pElf->get_section(m_curSection));
    return newSection;
}

bool SectionIterator::operator!=(const SectionIterator &rhs) const
{
    bool equal = ((m_pElf == rhs.m_pElf) & (m_curSection == rhs.m_curSection));

    return !equal;
}
