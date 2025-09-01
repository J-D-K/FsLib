#pragma once
#include <switch.h>

namespace fslib
{
    class SaveInfoReader;

    class SaveInfoIterator final
    {
        public:
            /// @brief SaveInfoIterator constructor.
            /// @param infoReader SaveInfoReader to construct from.
            SaveInfoIterator(const fslib::SaveInfoReader *infoReader);

            /// @brief Returns the first element of the infoReader array.
            FsSaveDataInfo *begin() const noexcept;

            /// @brief Returns the final valid element of the infoReadArray.
            FsSaveDataInfo *end() const noexcept;

        private:
            /// @brief Pointer to the creating SaveInfoReader.
            const fslib::SaveInfoReader *m_infoReader{};

            /// @brief Internal index.
            int m_index{};
    };
}