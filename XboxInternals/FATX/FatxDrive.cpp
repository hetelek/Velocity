#include "FatxDrive.h"

FatxDrive::FatxDrive(std::wstring drivePath)
{
    io = new DeviceIO(drivePath);

    // parse the security blob
    io->SetPosition(0x2000);
    securityBlob.serialNumber = io->ReadString(0x14);
    securityBlob.firmwareRevision = io->ReadString(8);
    securityBlob.modelNumber = io->ReadString(0x28);
    io->ReadBytes(securityBlob.msLogoHash, 0x14);
    securityBlob.userAddressableSectors = io->ReadDword();
    io->ReadBytes(securityBlob.rsaSignature, 0x100);

    // TODO: verify the signature

    // seek to the next sector
    io->SetPosition(0x2200);
    securityBlob.msLogoSize = io->ReadDword();
    securityBlob.msLogo = new BYTE[securityBlob.msLogoSize];
    io->ReadBytes(securityBlob.msLogo, securityBlob.msLogoSize);

}

FatxDrive::~FatxDrive()
{
    delete[] securityBlob.msLogo;

    io->Close();
    delete io;
}
