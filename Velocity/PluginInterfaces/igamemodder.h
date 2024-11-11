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

    virtual void LoadPackage(StfsPackage * /*package*/, bool * /*ok*/ = nullptr,
            void * /*args*/ = nullptr) { }
    virtual QWidget* GetDialog()
    {
        return nullptr;
    }
    virtual QString Version() const = 0;
    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;
};

Q_DECLARE_INTERFACE (IGameModder, "com.exetelek.Veloctiy.IGameModder/0.1")

#endif
