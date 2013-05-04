#include "dragdroptreewidget.h"
#include <QDebug>

DragDropTreeWidget::DragDropTreeWidget(QObject *parent) :
    QTreeWidget((QTreeWidget*)parent)
{
    setAcceptDrops(true);
}

void DragDropTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
    emit dragEntered(event);
}

void DragDropTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    event->accept();
}

void DragDropTreeWidget::dropEvent(QDropEvent *event)
{
    emit dragDropped(event);
}

void DragDropTreeWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    emit dragLeft(event);
}
