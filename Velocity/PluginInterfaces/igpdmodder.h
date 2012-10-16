#ifndef IGPDMODDER_H
#define IGPDMODDER_H

#include <QString>
#include <QWidget>

// xbox360
#include "GPD/XDBF.h"
#include "GPD/XDBFDefininitions.h"
#include "GPD/XDBFHelpers.h"
#include "GPD/GPDBase.h"
#include "GPD/GameGPD.h"
#include "FileIO.h"

class IGPDModder
{
public:
    IGPDModder() { }
    virtual ~IGPDModder() { }

    virtual void LoadGPD(GameGPD *gpd, QWidget *parent = NULL) const = 0;
    virtual QWidget* GetDialog() const = 0;
    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;
};

Q_DECLARE_INTERFACE (IGPDModder, "com.exetelek.Veloctiy.IGPDModder/0.1")

#endif
