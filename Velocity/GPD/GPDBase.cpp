#include "GPDBase.h"


GPDBase::GPDBase(string path) : ioPassedIn(false)
{
    io = new FileIO(path);
    xdbf = new XDBF(io);

    init();
}

GPDBase::GPDBase(FileIO *io) : ioPassedIn(true), io(io)
{
    xdbf = new XDBF(io);

    init();
}

void GPDBase::init()
{
    // read all the images
    for (DWORD i = 0; i < xdbf->images.size(); i++)
    {
        // set up the information
        ImageEntry image;
        image.entry = xdbf->images.at(i);
        image.initialLength = image.length = image.entry.length;
        image.image = new BYTE[image.length];

        // read in the image
        io->setPosition(xdbf->GetRealAddress(image.entry.addressSpecifier));
        io->readBytes(image.image, image.length);

        // add the image to the vector
        images.push_back(image);
    }

    // read all the settings
    for (DWORD i = 0; i < xdbf->settings.entries.size(); i++)
        settings.push_back(readSettingEntry(xdbf->settings.entries.at(i)));

    // read all the strings
    for (DWORD i = 0; i < xdbf->strings.size(); i++)
    {
        // read in the string entry
        StringEntry entry;
        entry.entry = xdbf->strings.at(i);
        entry.ws = readStringEntry(entry.entry);
        entry.initialLength = entry.ws.length();

        // add the entry to the vector
        strings.push_back(entry);
    }
}

wstring GPDBase::readStringEntry(XDBFEntry entry)
{
    // ensure that the entry is a string entry
    if (entry.type != String)
        throw string("XDBF: Error reading string entry. Specified entry isn't a string.\n");

    // seek to the entry's position
    io->setPosition(xdbf->GetRealAddress(entry.addressSpecifier));

    // read the string
    return io->readWString();
}

SettingEntry GPDBase::readSettingEntry(XDBFEntry entry)
{
    // ensure the entry is a setting entry
    if (entry.type != Setting)
        throw string("XDBF: Error reading setting entry. The entry specified isn't a setting.\n");

    SettingEntry toReturn;
    toReturn.entry = entry;

    // seek to the position of the setting entry, skip past the id
    DWORD entryAddr = xdbf->GetRealAddress(entry.addressSpecifier);
    io->setPosition(entryAddr + 8);

    // read the setting entry type
    toReturn.type = (SettingEntryType)io->readByte();
    if (toReturn.type <= 0 || toReturn.type > 7)
    {
        printf("%llX\n", entry.id);
        throw string("XDBF: Error reading setting entry. Invalid setting entry type.\n");
    }

    // skip past the nonsense
    io->setPosition(entryAddr + 0x10);

    switch (toReturn.type)
    {
        case Int32:
            toReturn.int32 = io->readInt32();
            break;
        case Int64:
            toReturn.int64 = io->readInt64();
            break;
        case Double:
        {
            toReturn.doubleData = io->readDouble();
            break;
        }
        case UnicodeString:
        {
            DWORD strLen = io->readDword();
            io->setPosition(entryAddr + 0x18);
            toReturn.str = new wstring(io->readWString(strLen));
            break;
        }
        case Float:
        {
            toReturn.floatData = io->readFloat();
            break;
        }
        case Binary:
            toReturn.binaryData.length = io->readDword();
            io->setPosition(entryAddr + 0x18);
            toReturn.binaryData.data = new BYTE[toReturn.binaryData.length];
            io->readBytes(toReturn.binaryData.data, toReturn.binaryData.length);
            break;
        case TimeStamp:
        {
            FILETIME time = { io->readDword(), io->readDword() };
            toReturn.timeStamp = XDBFHelpers::FILETIMEtoTimeT(time);
            break;
        }
    }

    return toReturn;
}

void GPDBase::DeleteSettingEntry(SettingEntry setting)
{
    // remove the entry from the list
    DWORD i;
    for (i = 0; i < settings.size(); i++)
    {
        if (settings.at(i).entry.id == setting.entry.id)
        {
            settings.erase(settings.begin() + i);
            break;
        }
    }
    if (i > settings.size())
        throw string("GPD: Error deleting setting entry. Setting doesn't exist.\n");

    // delete the entry from the file
    xdbf->DeleteEntry(setting.entry);
}

void GPDBase::DeleteImageEntry(ImageEntry image)
{
    // remove the entry from the list
    DWORD i;
    for (i = 0 ; i < images.size(); i++)
    {
        if (images.at(i).entry.id == image.entry.id)
        {
            images.erase(images.begin() + i);
            break;
        }
    }
    if (i > settings.size())
        throw string("GPD: Error deleting image entry. Image doesn't exist.\n");

    // delete the entry from the file
    xdbf->DeleteEntry(image.entry);
}

void GPDBase::CreateSettingEntry(SettingEntry *setting, UINT64 entryID)
{
    DWORD entryLen = 0;
    switch (setting->type)
    {
    case Int32:
    case Float:
    case Int64:
    case Double:
    case TimeStamp:
        entryLen = 0x18;
        break;
    case UnicodeString:
        entryLen = 0x18 + ((setting->str->size() + 1) * 2);
        break;
    case Binary:
        entryLen = 0x18 + (setting->binaryData.length * 2);
        break;
    default:
        throw string("GPD: Error creating setting entry. Invalid setting entry type.\n");
    }

    // create the xdbf entry
    setting->entry = xdbf->CreateEntry(Setting, entryID, entryLen);

    // add the new setting entry to the list
    settings.push_back(*setting);

    // write the setting
    WriteSettingEntry(*setting);
}

void GPDBase::CreateImageEntry(ImageEntry *image, UINT64 entryID)
{
    // create XDBF entry
    image->initialLength = image->length;
    image->entry = xdbf->CreateEntry(Image, entryID, image->length);

    // add the new entry to the list
    images.push_back(*image);

    // write the image
    WriteImageEntry(*image);
}

void GPDBase::WriteSettingEntry(SettingEntry setting)
{
    // get the address of the entry in the file
    DWORD entryAddr = xdbf->GetRealAddress(setting.entry.addressSpecifier);

    // write the setting entry header
    io->setPosition(entryAddr);
    io->write((DWORD)setting.entry.id);
    io->setPosition(entryAddr + 8);
    io->write((BYTE)setting.type);

    io->setPosition(entryAddr + 0x10);
    io->flush();

    // write setting
    switch (setting.type)
    {
        case Int32:
        case Float:
            io->write((DWORD)setting.int32);
            break;
        case Int64:
        case Double:
            io->write((UINT64)setting.int64);
            break;
        case TimeStamp:
        {
            FILETIME time = XDBFHelpers::TimeTtoFILETIME(setting.timeStamp);
            io->write(time.dwHighDateTime);
            io->write(time.dwLowDateTime);
            break;
        }
        case UnicodeString:
        {
            DWORD calculatedLength = 0x18 + ((setting.str->size() + 1) * 2);
            if (setting.entry.length != calculatedLength)
            {
                // adjust the memory if the length changed
                xdbf->DeallocateMemory(xdbf->GetRealAddress(setting.entry.addressSpecifier), setting.entry.length);
                setting.entry.length = calculatedLength;
                entryAddr = xdbf->AllocateMemory(setting.entry.length);
                setting.entry.addressSpecifier = xdbf->GetSpecifier(entryAddr);

                io->setPosition(entryAddr);
                io->write((UINT64)setting.entry.id);
                io->write((BYTE)setting.type);
                io->setPosition(entryAddr + 0x10);
            }
            io->write((DWORD)((setting.str->size() + 1) * 2));
            io->setPosition(entryAddr + 0x18);
            io->write(*setting.str);
            break;
        }
        case Binary:
            DWORD calculatedLength = 0x18 + setting.binaryData.length;
            if (setting.entry.length != calculatedLength)
            {
                // adjust the memory if the length changed
                xdbf->DeallocateMemory(xdbf->GetRealAddress(setting.entry.addressSpecifier), setting.entry.length);
                setting.entry.length = calculatedLength;
                entryAddr = xdbf->AllocateMemory(setting.entry.length);
                setting.entry.addressSpecifier = xdbf->GetSpecifier(entryAddr);

                io->setPosition(entryAddr);
                io->write((DWORD)setting.entry.id);
                io->setPosition(entryAddr + 8);
                io->write((BYTE)setting.type);
                io->setPosition(entryAddr + 0x10);
            }
            io->write(setting.binaryData.length);
            io->setPosition(entryAddr + 0x18);
            io->write(setting.binaryData.data, setting.binaryData.length);
            break;
    }

    xdbf->UpdateEntry(&setting.entry);

    io->flush();
}

void GPDBase::WriteImageEntry(ImageEntry image)
{
    // allocate memory if needed
    if (image.length != image.initialLength)
    {
        xdbf->DeallocateMemory(xdbf->GetRealAddress(image.entry.addressSpecifier), image.entry.length);
        image.entry.length = image.length;
        image.entry.addressSpecifier = xdbf->GetSpecifier(xdbf->AllocateMemory(image.entry.length));
    }

    // write the image
    io->setPosition(xdbf->GetRealAddress(image.entry.addressSpecifier));
    io->write(image.image, image.length);
}

void GPDBase::Close()
{
    io->close();
}

GPDBase::~GPDBase(void)
{
    // deallocate all of the image memory
    for (DWORD i = 0; i < images.size(); i++)
        delete[] images.at(i).image;

    // deallocate all of the setting memory
    for (DWORD i = 0; i < settings.size(); i++)
    {
        if (settings.at(i).type == Binary)
            delete[] settings.at(i).binaryData.data;
        else if (settings.at(i).type == UnicodeString)
            delete settings.at(i).str;
    }

    delete io;
    delete xdbf;
}
