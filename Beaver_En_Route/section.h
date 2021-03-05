#ifndef SECTOR_H
#define SECTOR_H

#include <stdint.h>

#include <QString>

#include "elf/elf++.hh"

class Section
{
    QString m_name;
    uint32_t m_addr;
    const void *m_pData;
    size_t m_size;

public:
    Section();
    Section(const elf::section &section);
    bool operator ==(const Section &rhs) const;
    bool operator !=(const Section &rhs) const;
    QString name() const;
    void setName(const QString &name);
    uint32_t addr() const;
    void setAddr(const uint32_t &addr);
    const void *pData() const;
    size_t size() const;
    void setSize(const size_t &size);
};

#endif // SECTOR_H
