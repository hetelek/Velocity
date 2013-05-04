#ifndef IGPDMODDER_H
#define IGPDMODDER_H

#include <QString>
#include <QWidget>

// xbox360
#include "Gpd/GameGpd.h"

class IGPDModder
{
public:
    const void *Arguments;

    IGPDModder() { }
    virtual ~IGPDModder() { }

    virtual void LoadGPD(GameGpd * /* gpd*/, bool */*ok */= NULL, void * /*args*/ = NULL) { }
    virtual QWidget* GetDialog() { return NULL; }
    virtual QString Version() const = 0;
    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;

};

Q_DECLARE_INTERFACE (IGPDModder, "com.exetelek.Veloctiy.IGPDModder/0.1")

#endif
