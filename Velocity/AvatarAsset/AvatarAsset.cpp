#include "AvatarAsset.h"

#include "FileIO.h"

AvatarAsset::AvatarAsset(const QString &assetPath) : ioPassedIn(false)
{
	metadata.gender = (AssetGender)0;
	customColors.entries = NULL;
	customColors.count = 0;
	animation.frameCount = 0;

    io = new FileIO(assetPath.toStdString());
	readHeader();
	readBlocks();
}

AvatarAsset::AvatarAsset(FileIO *io) : io(io), ioPassedIn(true)
{
	metadata.gender = (AssetGender)0;
	customColors.entries = NULL;
	customColors.count = 0;

	readHeader();
	readBlocks();
}

void AvatarAsset::readHeader()
{
	// seek to the begining of the file
	io->setPosition(0);

	// read in the magic to verify the file
	header.magic = io->readDword();
	if (header.magic != 0x53545242)
        throw QString("STRB: Invalid magic.\n");

	// read in the rest of the header
	header.blockAlignmentStored = (bool)io->readByte();
	header.littleEndian = (bool)io->readByte();
	io->setEndian((EndianType)header.littleEndian);

    io->readBytes(header.guid, 16);

	// read the rest of the header
	header.blockIDSize = io->readByte();
	header.blockSpanSize = io->readByte();
	header.unused = io->readWord();
	header.blockAlignment = (header.blockAlignmentStored) ? io->readByte() : 1;

	// calculate the stuff that needs to be calculated
	header.blockStartAddress = roundUpToBlockAlignment((header.blockAlignmentStored) ? 30 : 0x1A);
	header.blockHeaderSize = roundUpToBlockAlignment((header.blockIDSize + header.blockSpanSize) + header.blockSpanSize);
}

void AvatarAsset::readBlocks()
{
	DWORD prevAddress = header.blockStartAddress, prevDataLen = 0;
    io->setPosition(0, std::ios_base::end);
	DWORD fileSize = io->getPosition();

	for (;;)
	{
		// seek to the position of the next block
		DWORD curBlockAddress = prevAddress + roundUpToBlockAlignment(prevDataLen);
		io->setPosition(curBlockAddress);

		// if the io is at the end of the file, then break out
		if ((curBlockAddress + header.blockHeaderSize) > fileSize)
			break;

		// create a new block
		STRBBlock block;
		block.data = NULL;

		// read the block header
		block.id = (STRRBBlockId)io->readMultiByte(header.blockIDSize);
		block.dataLength = prevDataLen = (int)io->readMultiByte(header.blockSpanSize);
		block.fieldSize = (int)io->readMultiByte(header.blockSpanSize);

		if (block.id == STRBAssetMetadata)
			parseMetadataLegacyV1(curBlockAddress + header.blockHeaderSize);
		else if (block.id == STRBAssetMetadataVersioned)
			parseMetadata(curBlockAddress + header.blockHeaderSize);
		else if (block.id == STRBCustomColorTable)
			parseColorTable(curBlockAddress + header.blockHeaderSize);
		else if (block.id == STRBAnimation)
			readAnimationInfo(curBlockAddress + header.blockHeaderSize);

		// check the values read in
		if (block.dataLength < 0 || block.fieldSize == 0 || (block.dataLength % block.fieldSize) != 0)
            throw QString("STRB: Invalid block header value(s).\n");

		prevAddress = curBlockAddress + header.blockHeaderSize;
		block.dataAddress = prevAddress;

		blocks.push_back(block);
	}
}

void AvatarAsset::parseMetadataLegacyV1(DWORD pos)
{
	// seek to the position of the metadata
	io->setPosition(pos);

	// preserve the endian
	EndianType byteOrder = io->getEndian();
	io->setEndian(LittleEndian);

	// read in the metadata
	metadata.metadataVersion = 1;
	metadata.gender = (AssetGender)io->readByte();
	metadata.type = (BinaryAssetType)io->readDword();
	metadata.assetTypeDetails = io->readDword();
	metadata.category = (AssetSubcategory)io->readInt32();
	metadata.skeletonVersion = Nxe;

	// set the endian back
	io->setEndian(byteOrder);
}

void AvatarAsset::parseMetadata(DWORD pos)
{
	// seek to the position of the metadata
	io->setPosition(pos);

	switch (io->readByte())
	{
	case 1:
		parseMetadataLegacyV1(pos + 1);
		return;
	case 2:
		// preserve the endian
		EndianType byteOrder = io->getEndian();
		io->setEndian(LittleEndian);

		// read in the metadata
		metadata.gender = (AssetGender)io->readByte();
		metadata.type = (BinaryAssetType)io->readInt32();
		metadata.assetTypeDetails = io->readDword();
		metadata.category = (AssetSubcategory)io->readInt32();
		metadata.skeletonVersion = (SkeletonVersion)io->readByte();

		// set the endian back
		io->setEndian(byteOrder);
		return;
	}
}

void AvatarAsset::parseColorTable(DWORD pos)
{
	// seek to the color table position
	io->setPosition(pos);

	// preserve the endian
	EndianType byteOrder = io->getEndian();
	io->setEndian(LittleEndian);

	// read the entry count
	customColors.count = io->readDword();

	// allocate enough memory for all the entries
	customColors.entries = new ColorTableEntry[customColors.count];

	// read the entries
	for (DWORD i = 0; i < customColors.count; i++)
	{
        customColors.entries[i].color1.color.blue = io->readByte();
        customColors.entries[i].color1.color.green = io->readByte();
        customColors.entries[i].color1.color.red = io->readByte();
        customColors.entries[i].color1.color.alpha = io->readByte();
		customColors.entries[i].color1.unknown = io->readDword();

        customColors.entries[i].color2.color.blue = io->readByte();
        customColors.entries[i].color2.color.green = io->readByte();
        customColors.entries[i].color2.color.red = io->readByte();
        customColors.entries[i].color2.color.alpha = io->readByte();
        customColors.entries[i].color2.unknown = io->readDword();

        customColors.entries[i].color3.color.blue = io->readByte();
        customColors.entries[i].color3.color.green = io->readByte();
        customColors.entries[i].color3.color.red = io->readByte();
        customColors.entries[i].color3.color.alpha = io->readByte();
        customColors.entries[i].color3.unknown = io->readDword();
	}

	// set the endian back
	io->setEndian(byteOrder);
}

void AvatarAsset::ReadBlockData(STRBBlock *block)
{
	// allocated enough memory for the data
	block->data = new BYTE[block->dataLength];

	// seek to the positon of the data
	io->setPosition(block->dataAddress);

	// read in the block data
    io->readBytes(block->data, block->dataLength);
}

const QVector<STRBBlock> &AvatarAsset::GetBlocks()
{
    return blocks;
}

void AvatarAsset::readAnimationInfo(DWORD pos)
{
	// seek to the position of the animation information
	io->setPosition(pos);

	// preserve the endian
	EndianType byteOrder = io->getEndian();
	io->setEndian(LittleEndian);

	// read the frame count
    animation.frameCount = io->readDword();

	// read the fps
    animation.framesPerSecond =  io->readFloat();

    // round dat, yo
    animation.framesPerSecond = (float)(((int)((animation.framesPerSecond + .05f) * 100)) / 100);

	// calculate the duration of the animation
	animation.duration = (float)animation.frameCount / animation.framesPerSecond;
	
	io->setEndian(byteOrder);
}

DWORD AvatarAsset::roundUpToBlockAlignment(DWORD valueToRound)
{
    if (((header.blockAlignment & (header.blockAlignment - 1)) != 0) || (header.blockAlignment == 0))
        throw QString("STRB: Error when rounding.\n");
    int blockAlignment = header.blockAlignment;
    return (((valueToRound + blockAlignment) - 1) & ~(blockAlignment - 1));
}

ColorTable AvatarAsset::GetCustomColorTable()
{
	if (customColors.entries == NULL)
        throw QString("Asset: No color table found for asset.\n");

	return customColors;
}

struct Animation AvatarAsset::GetAnimation()
{
	if (animation.frameCount == 0)
        throw QString("Asset: No animation found for asset.\n");

	return animation;
}

AssetMetadata AvatarAsset::GetAssetMetadata()
{
	// if the metadata doesn't exist then we need to throw an error
	if (metadata.gender == 0)
        throw QString("Asset: No metadata available for asset.\n");

	return metadata;
}

AvatarAsset::~AvatarAsset(void)
{
	// cleanup the io
	if (!ioPassedIn)
	{
		io->close();
		delete io;
	}

	// cleanup the colors
	if (customColors.entries != NULL)
		delete customColors.entries;

	// cleanup the blocks
    for (int i = 0; i < blocks.size(); i++)
		if (blocks.at(i).data != NULL)
			delete blocks.at(i).data;
}
