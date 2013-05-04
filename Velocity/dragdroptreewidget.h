#ifndef DRAGDROPLISTWIDGET_H
#define DRAGDROPLISTWIDGET_H

#include <QTreeWidget>
#include <QDragEnterEvent>

class DragDropTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    explicit DragDropTreeWidget(QObject *parent = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    
signals:
    void dragDropped(QDropEvent *event);
    void dragEntered(QDragEnterEvent *event);
    void dragLeft(QDragLeaveEvent *event);
};

#endif // DRAGDROPLISTWIDGET_H
