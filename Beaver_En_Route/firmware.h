#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <QObject>
#include <QString>

#include "elf/elf++.hh"

#include "section.h"
#include "sectioniterator.h"

#define FIRMWARE_NONEXIST   -3
#define FIRMWARE_INCORRECT  -2
#define FIRMWARE_NOK        -1
#define FIRMWARE_OK         0

class Firmware
{
    std::shared_ptr<elf::elf> m_pElf;
    std::shared_ptr<SectionIterator> m_pSections;

public:
    explicit Firmware();
    int load(QString filename);

    SectionIterator sections();
    size_t numSections();
};

#endif // FIRMWARE_H
