#pragma once

#include "AvatarAssetDefinintions.h"

class FileIO;

#include <QVector>
#include <QString>

class AvatarAsset
{
public:
    AvatarAsset(const QString &assetPath);
	AvatarAsset(FileIO *io);
	~AvatarAsset(void);

	// Description: retrieve the metadata that describes the asset
	AssetMetadata GetAssetMetadata();

	// Description: retrieve the custom color table
	ColorTable GetCustomColorTable();

	// Description: retrieve the animation info for the asset
	struct Animation GetAnimation();

	// Description: read the block's data in
	void ReadBlockData(STRBBlock *block);

    const QVector<STRBBlock> &GetBlocks();
private:

    QVector<STRBBlock> blocks;

    FileIO *io;
	bool ioPassedIn;
	STRBHeader header;
	AssetMetadata metadata;
	struct Animation animation; // TODO: read this crap in
	ColorTable customColors;

	// Description: read the STRB header
	void readHeader();
	
	// Description: read in all of the block information except for the actual data
	void readBlocks();

	// Description: parse v1 asset metadata
	void parseMetadataLegacyV1(DWORD pos);

	// Description: parse either version of the metadata
	void parseMetadata(DWORD pos);

	// Description: parse a color table
	void parseColorTable(DWORD pos);

	// Description: read some of the basic animation information
	void readAnimationInfo(DWORD pos);

	// Description: round the number to a block boundry
	DWORD roundUpToBlockAlignment(DWORD valueToRound);
};

