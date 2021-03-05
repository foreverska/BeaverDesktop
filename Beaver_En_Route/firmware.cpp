#include <fcntl.h>

#include "firmware.h"

Firmware::Firmware()
{

}

int Firmware::load(QString filename)
{
    int fd = open(filename.toStdString().c_str(), O_RDONLY);
    if (fd == -1)
    {
        return FIRMWARE_NONEXIST;
    }

    try {
        std::shared_ptr<elf::loader> pLoader = elf::create_mmap_loader(fd);
        m_pElf = std::make_shared<elf::elf>(pLoader);
    }  catch (const elf::format_error &any) {
        return FIRMWARE_INCORRECT;
    } catch (const std::exception &any) {
        return FIRMWARE_NOK;
    }

    m_pSections = std::make_shared<SectionIterator>(m_pElf);

    return FIRMWARE_OK;
}

SectionIterator Firmware::sections()
{
    return *m_pSections;
}

size_t Firmware::numSections()
{
    return m_pSections->end().pos();
}
