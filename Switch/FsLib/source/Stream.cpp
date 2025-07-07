#include "Stream.hpp"

bool fslib::Stream::is_open() const noexcept
{
    return m_isOpen;
}

int64_t fslib::Stream::tell() const noexcept
{
    return m_offset;
}

int64_t fslib::Stream::get_size() const noexcept
{
    return m_streamSize;
}

bool fslib::Stream::end_of_stream() const noexcept
{
    return m_offset >= m_streamSize;
}

void fslib::Stream::seek(int64_t offset, uint8_t origin) noexcept
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

    // This will make sure the offset is actually valid.
    Stream::ensure_offset_is_valid();
}

fslib::Stream::operator bool() const noexcept
{
    return m_isOpen;
}

void fslib::Stream::ensure_offset_is_valid() noexcept
{
    if (m_offset < 0)
    {
        m_offset = 0;
    }
    // To do: Decide if this is the right way to approach this...
    else if (m_offset > m_streamSize)
    {
        m_offset = m_streamSize;
    }
}
