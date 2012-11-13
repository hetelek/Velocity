#ifndef IGAMEMODDER_H
#define IGAMEMODDER_H

#include <QString>
#include <QWidget>

// xbox360
#include "Stfs/StfsPackage.h"

class IGameModder
{
public:
    const void *Arguments;

    IGameModder() { }
    virtual ~IGameModder() { }

    virtual void LoadPackage(StfsPackage * /*package*/, void * /*args*/ = NULL) { }
    virtual QDialog* GetDialog() { return NULL; }
    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;
};

Q_DECLARE_INTERFACE (IGameModder, "com.exetelek.Veloctiy.IGameModder/0.1")

#endif
