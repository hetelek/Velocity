#pragma once

#include <iostream>
#include "XDBF.h"
#include "XDBFDefininitions.h"
#include <vector>

using std::string;
using std::vector;

class GPDBase
{
public:
	GPDBase(FileIO *io);
	GPDBase(string gpdPath);

	~GPDBase(void);
    XDBF *xdbf;

	vector<ImageEntry> images;
	vector<StringEntry> strings;
	vector<SettingEntry> settings;

	// Description: delete the setting entry passed in from the xdbf file
	void DeleteSettingEntry(SettingEntry setting);

	// Description: create a new setting entry, leave the entry field of the setting entry struct blanc, this function will set that info
	void CreateSettingEntry(SettingEntry *setting, UINT64 entryID);

	// Description: write the setting entry to the file
	void WriteSettingEntry(SettingEntry setting);

    // Description: close all open resources attatched to the file
    void Close();

protected:
	bool ioPassedIn;
	FileIO *io;

private:
	// Description: read the string entry passed in
	wstring readStringEntry(XDBFEntry entry);

	// Description: read the setting entry passed in
	SettingEntry readSettingEntry(XDBFEntry entry);

	// read in all of the images, settings, and strings
	void init();
};
