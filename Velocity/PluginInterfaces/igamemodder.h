#ifndef IGAMEMODDER_H
#define IGAMEMODDER_H

#include <QWidget>
#include <Stfs/StfsPackage.h>

class IGameModder
{
public:
    virtual IGameModder(StfsPackage *package, QWidget *parent = NULL) { }
    virtual ~IGameModder() { }

    virtual QString ToolAuthor() const = 0;
    virtual QString ToolName() const = 0;
    virtual DWORD TitleID() const = 0;

    virtual bool Save() = 0;
};

Q_DECLARE_INTERFACE (IGameModder, "com.velocity.GameModders")

#endif // IGAMEMODDER_H
