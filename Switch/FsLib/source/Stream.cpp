#include "Stream.hpp"

bool fslib::Stream::isOpen(void) const
{
    return m_isOpen;
}

int64_t fslib::Stream::tell(void) const
{
    return m_offset;
}

int64_t fslib::Stream::getSize(void) const
{
    return m_streamSize;
}

bool fslib::Stream::endOfStream(void) const
{
    return m_offset >= m_streamSize;
}

void fslib::Stream::seek(int64_t offset, uint8_t origin)
{
    switch (origin)
    {
        case Stream::beginning:
        {
            m_offset = offset;
        }
        break;

        case Stream::current:
        {
            m_offset += offset;
        }
        break;

        case Stream::end:
        {
            m_offset = m_streamSize + offset;
        }
        break;
    }
    // Just to be sure.
    Stream::ensureOffsetIsValid();
}

fslib::Stream::operator bool(void) const
{
    return m_isOpen;
}

void fslib::Stream::ensureOffsetIsValid(void)
{
    if (m_offset < 0)
    {
        m_offset = 0;
    }
    else if (m_offset >= m_streamSize)
    {
        m_offset = m_streamSize - 1;
    }
}
