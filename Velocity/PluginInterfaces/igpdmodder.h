#ifndef IGPDMODDER_H
#define IGPDMODDER_H

#include <QString>
#include <QWidget>

// xbox360
#include "GPD/GameGPD.h"

class IGPDModder
{
public:
    const void *Arguments;

    IGPDModder() { }
    virtual ~IGPDModder() { }

    virtual void LoadGPD(GameGPD *gpd, void *arg = NULL) { }
    virtual QWidget* GetDialog() { }
    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;
};

Q_DECLARE_INTERFACE (IGPDModder, "com.exetelek.Veloctiy.IGPDModder/0.1")

#endif
