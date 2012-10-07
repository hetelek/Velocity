#ifndef IGAMEMODDER_H
#define IGAMEMODDER_H

#include <QString>
#include <QWidget>

#include "Stfs/StfsPackage.h"

class IGameModder
{
public:
    virtual IGameModder() { }
    virtual ~IGameModder() { }

    virtual void LoadPackage(StfsPackage *package, QWidget *parent = NULL) const = 0;
    virtual QWidget* GetDialog() const = 0;
    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;
};

Q_DECLARE_INTERFACE (IGameModder, "com.exetelek.Veloctiy.IGameModder/0.1")

#endif
