#pragma once
#include "Stream.hpp"
#include <switch.h>

namespace fslib
{
    /**
     * @brief Class for reading from raw storage volumes.
     * @note This still needs a lot of work for the different storage types. This class is <b>read only</b> for a reason.
     * Even with that, it can still <b>read the most sensitive data</b> on the Switch.
    */
    class Storage : public fslib::Stream
    {
        public:
            /// @brief  Default storage constructor.
            Storage() = default;

            /// @brief Attempts to open PartitionID as raw storage.
            /// @param partitionID ID of partition to open.
            /// @note IsOpen can be used to see if the operation succeeded.
            Storage(FsBisPartitionId partitionID);

            /// @brief Closes storage handle at destruction.
            ~Storage();

            /// @brief Attempts to open PartitionID as raw storage.
            /// @param partitionID ID of partition to open.
            /// @note IsOpen can be used to see if the operation succeeded.
            void open(FsBisPartitionId partitionID);

            /// @brief Closes storage handle.
            void close();

            /**
             * @brief Attempts to read from storage.
             *
             * @param buffer Buffer to read into.
             * @param bufferSize Size of Buffer.
             * @return Number of bytes read. -1 on failure.
             * @note The underlying Switch storage reading functions have no way to really keep track of how much was read or where you
             * are located (offset). I've done the best I can to correct for this.
             */
            ssize_t read(void *buffer, size_t bufferSize);

            /// @brief Reads a single byte from storage.
            /// @return Byte read on success. -1 on failure.
            signed char read_byte();

        private:
            /// @brief Handle to storage opened.
            FsStorage m_storageHandle;
    };
} // namespace fslib
