#ifndef SECTORITERATOR_H
#define SECTORITERATOR_H

#include <iterator>
#include <memory>

#include <elf/elf++.hh>

#include "section.h"

class SectionIterator : public std::iterator<std::input_iterator_tag, Section>
{
    std::shared_ptr<elf::elf> m_pElf;
    uint32_t m_curSection;

public:
    SectionIterator(std::shared_ptr<elf::elf> newElf);
    SectionIterator& operator++();
    SectionIterator begin();
    SectionIterator end();
    Section operator *();
    bool operator!=(const SectionIterator& rhs) const;
    size_t pos();
    void setPos(size_t pos);
};

#endif // SECTORITERATOR_H
