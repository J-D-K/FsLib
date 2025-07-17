#include "Stream.hpp"

bool fslib::Stream::is_open() const { return m_isOpen; }

int64_t fslib::Stream::tell() const { return m_offset; }

int64_t fslib::Stream::get_size() const { return m_streamSize; }

bool fslib::Stream::end_of_stream() const { return m_offset >= m_streamSize; }

void fslib::Stream::seek(int64_t offset, uint8_t origin)
{
    switch(origin)
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

    Stream::ensure_offset_is_valid();
}

fslib::Stream::operator bool() const { return m_isOpen; }

void fslib::Stream::ensure_offset_is_valid()
{
    if(m_offset < 0) { m_offset = 0; }
    else if(m_offset > m_streamSize) { m_offset = m_streamSize; }
}
