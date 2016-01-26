#include "XexZeroBasedCompressionIO.h"

XexZeroBasedCompressionIO::XexZeroBasedCompressionIO(BaseIO *io, Xbox360Executable *xex) :
    io(io), xex(xex), position(0)
{

}

void XexZeroBasedCompressionIO::ReadBytes(BYTE *outBuffer, DWORD readLength)
{
    if (Length() < (position + readLength))
        throw std::string("XexZeroBasedCompressionIO: Cannot read beyond end of data.");

    if (GetPosition() + readLength >= 0x2E08000)
        std::cout << "yol" << std::endl;

    DWORD bytesRemaining = readLength;
    while (bytesRemaining > 0)
    {
        bool isData;
        XexCompressionBlock curBlock;
        DWORD offset;
        DWORD byteCounter = 0;

        // find the location in the zero compression table
        for (size_t i = 0; i < xex->compressionBlocks.size(); i++)
        {
            curBlock = xex->compressionBlocks.at(i);
            DWORD curBlockTotalSize = curBlock.size + curBlock.nullSize;

            // check to see if the position is in this compression block
            if ((position - byteCounter) < curBlockTotalSize)
            {
                // check if the position is in the data block
                if (position < (byteCounter + curBlock.size))
                {
                    isData = true;
                }
                // otherwise its in the null block
                else
                {
                    // advance past the data block
                    byteCounter += curBlock.size;

                    isData = false;
                }

                offset = position - byteCounter;
                break;
            }

            // advance past the data block
            byteCounter += curBlockTotalSize;
        }

        // read the data bytes in
        if (isData)
        {
            DWORD dataBytesToRead;
            if (bytesRemaining > curBlock.size - offset)
                dataBytesToRead = curBlock.size - offset;
            else
                dataBytesToRead = bytesRemaining;
            io->ReadBytes(outBuffer + (readLength - bytesRemaining), dataBytesToRead);

            bytesRemaining -= dataBytesToRead;
            position += dataBytesToRead;

            // recalculate the offset
            if (dataBytesToRead + offset == curBlock.size)
                offset = 0;
            else
                offset += dataBytesToRead;
        }

        // check to see if we need to copy over null bytes too
        if (bytesRemaining != 0)
        {
            // calculate the number of null bytes to copy
            DWORD nullBytesToCopy;
            if (bytesRemaining > curBlock.nullSize - offset)
                nullBytesToCopy = curBlock.nullSize - offset;
            else
                nullBytesToCopy = bytesRemaining;

            // allocate a null buffer
            BYTE *nullBuffer = new BYTE[XEX_NULL_BUFFER_SIZE];
            memset(nullBuffer, 0, XEX_NULL_BUFFER_SIZE);

            // calculate the number of copies to do
            DWORD copyIterations = nullBytesToCopy / XEX_NULL_BUFFER_SIZE;
            if (nullBytesToCopy % XEX_NULL_BUFFER_SIZE != 0)
                copyIterations++;

            // copy over the null bytes in chunks
            for (DWORD i = 0; i < copyIterations; i++)
            {
                // calculate the number of bytes to copy
                DWORD bytesToCopy = XEX_NULL_BUFFER_SIZE;
                if (i + 1 == copyIterations && nullBytesToCopy % XEX_NULL_BUFFER_SIZE != 0)
                    bytesToCopy = nullBytesToCopy % XEX_NULL_BUFFER_SIZE;

                memcpy(outBuffer + (readLength - bytesRemaining), nullBuffer, bytesToCopy);
                bytesRemaining -= bytesToCopy;
            }

            position += nullBytesToCopy;
            delete nullBuffer;
        }
    }
}

void XexZeroBasedCompressionIO::WriteBytes(BYTE *buffer, DWORD len)
{

}

void XexZeroBasedCompressionIO::SetPosition(UINT64 position, std::ios_base::seek_dir dir)
{
    if (position != 0 || dir != std::ios_base::beg)
        throw std::string("XexZeroBasedCompressionIO: Can only seek to the beginning.");

    xex->io->SetPosition(xex->header.dataAddress);
}

UINT64 XexZeroBasedCompressionIO::GetPosition()
{
    return position;
}

void XexZeroBasedCompressionIO::Flush()
{

}

void XexZeroBasedCompressionIO::Close()
{

}

UINT64 XexZeroBasedCompressionIO::Length()
{
    UINT64 length = 0;
    for (size_t i = 0; i < xex->compressionBlocks.size(); i++)
    {
        length += xex->compressionBlocks.at(i).size;
        length += xex->compressionBlocks.at(i).nullSize;
    }

    return length;
}
