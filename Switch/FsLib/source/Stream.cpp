#include "Stream.hpp"

bool fslib::Stream::is_open(void) const
{
    return m_isOpen;
}

int64_t fslib::Stream::tell(void) const
{
    return m_offset;
}

int64_t fslib::Stream::get_size(void) const
{
    return m_streamSize;
}

bool fslib::Stream::end_of_stream(void) const
{
    return m_offset >= m_streamSize;
}

void fslib::Stream::seek(int64_t offset, uint8_t origin)
{
    switch (origin)
    {
        case Stream::BEGINNING:
        {
            m_offset = offset;
        }
        break;

        case Stream::CURRENT:
        {
            m_offset += offset;
        }
        break;

        case Stream::END:
        {
            m_offset = m_streamSize + offset;
        }
        break;
    }
    // Just to be sure.
    Stream::ensure_offset_is_valid();
}

fslib::Stream::operator bool(void) const
{
    return m_isOpen;
}

void fslib::Stream::ensure_offset_is_valid(void)
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
