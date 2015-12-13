#include "IXContentHeader.h"


FileSystem IXContentHeader::GetFileSystem(std::string file)
{
    FileIO fileIO(file);
    fileIO.SetPosition(0x3AC);
    FileSystem fileSystem = (FileSystem)fileIO.ReadByte();
    fileIO.Close();

    return fileSystem;
}

std::string IXContentHeader::GetFatxFilePath()
{
    //std::ostringstream ss;
    //ss << std::hex << std::uppercase << std::setfill('0');
    //for (int i = 0; i < 8; i++)
    //    ss << std::setw(2) << (int)package.metaData->profileID[i];
    std::string profileID = Utils::ConvertToHexString(metaData->profileID, 8);

    //ss.str("");
    //ss << std::hex << std::uppercase << package.metaData->titleID;
    std::string titleID = Utils::ConvertToHexString(metaData->titleID);

    //ss.str("");
    //ss << std::hex << package.metaData->contentType;
    std::string contentType = Utils::ConvertToHexString(metaData->contentType);

    // the content type is padded with zeros at the beginning
    while (contentType.size() < 8)
        contentType = "0" + contentType;

    // get the path of the file on the device
    return "Drive:\\Content\\Content\\" + profileID + "\\" + titleID + "\\" + contentType + "\\";
}
