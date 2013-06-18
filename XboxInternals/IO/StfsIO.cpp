#include "StfsIO.h"

StfsIO::StfsIO(BaseIO *io, StfsPackage *package, StfsFileEntry entry) :
    io(io), package(package), entry(entry), didChangeSize(false)
{
    package->GenerateBlockChain(&this->entry);
    blocksAllocated = this->entry.blocksForFile;
    SetPosition(0);
}

StfsIO::~StfsIO()
{
    Flush();
}

StfsFileEntry StfsIO::GetStfsFileEntry()
{
    return entry;
}

void StfsIO::SetPosition(UINT64 position, ios_base::seek_dir dir)
{
    if (dir == std::ios_base::cur)
        position += this->entryPosition;
    else if (dir == std::ios_base::end)
        position = (Length() + this->entryPosition);

    // preserve the virtual address
    this->entryPosition = position;

    // calculate at which block index the data will appear
    DWORD blockNum = entry.blockChain.at(position / 0x1000);
    UINT64 packagePosition = this->package->BlockToAddress(blockNum) + (position % 0x1000);

    io->SetPosition(packagePosition);
}

UINT64 StfsIO::GetPosition()
{
    return this->entryPosition;
}

UINT64 StfsIO::Length()
{
    return this->entry.fileSize;
}

void StfsIO::ReadBytes(BYTE *outBuffer, DWORD len)
{
    DWORD maxRead = 0x1000 - (this->entryPosition % 0x1000);

    while (len >= maxRead)
    {
        io->ReadBytes(outBuffer, maxRead);
        len -= maxRead;
        outBuffer += maxRead;
        SetPosition(maxRead, std::ios_base::cur);
    }

    if (len != 0)
        io->ReadBytes(outBuffer, len);
}

void StfsIO::WriteBytes(BYTE *buffer, DWORD len)
{
    // calculate the ending position
    DWORD endingPosition = GetPosition() + len;
    if (endingPosition > Length())
    {
        didChangeSize = true;

        // see if we need to allocate memory
        if ((endingPosition + 0xFFF) / 0x1000 > (Length() + 0xFFF) / 0x1000)
        {
            // get the amount of blocks we need to allocate
            DWORD amountOfBlocksToAllocate = ((endingPosition - entry.fileSize) + 0xFFF) / 0x1000;

            // allocate the memory
            DWORD previousBlock = this->entry.blockChain.back();
            while (amountOfBlocksToAllocate-- > 0)
            {
                // allocate a new block
                DWORD currentBlock = this->package->AllocateBlock();

                // link the current block and previous block
                this->package->SetNextBlock(previousBlock, currentBlock);

                this->entry.blockChain.push_back(currentBlock);
                previousBlock = currentBlock;
                blocksAllocated++;
            }

            this->package->SetNextBlock(previousBlock, INT24_MAX);
        }

        // set the new size
        entry.fileSize = endingPosition;
    }

    WORD maxWrite = 0x1000 - (this->entryPosition % 0x1000);
    while (len >= maxWrite)
    {
        io->WriteBytes(buffer, maxWrite);
        len -= maxWrite;
        buffer += maxWrite;
        SetPosition(maxWrite, std::ios_base::cur);
    }

    if (len != 0)
        io->WriteBytes(buffer, len);
}

void StfsIO::Flush()
{
    if (didChangeSize)
    {
        // update the blocks allocated
        this->io->SetPosition(this->entry.fileEntryAddress + 0x29);
        this->io->Write((INT24)blocksAllocated, LittleEndian);
        this->io->Write((INT24)blocksAllocated, LittleEndian);

        // update the file size
        this->io->SetPosition(this->entry.fileEntryAddress + 0x34);
        this->io->Write((DWORD)entry.fileSize);
    }

    this->io->Flush();
}

void StfsIO::Close()
{
    Flush();
}
