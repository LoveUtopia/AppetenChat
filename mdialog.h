#ifndef MDIALOG_H
#define MDIALOG_H
#include "friendwidget.h"
#include "groupitem.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QMenu>
namespace Ui {
class mDialog;
}

class mDialog : public QDialog
{
    Q_OBJECT
signals:
    void SIG_offline();
    void SIG_addFriend();
    void SIG_createGroup();
    void SIG_addGroup();
public:
    explicit mDialog(QWidget *parent = nullptr);
    ~mDialog();
    void addFriend(friendWidget* friendw);
    void addGroup(groupItem* Item);
    void setSelfInfo(QString name,QString feeling,int iconId);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pb_set_clicked();
    void slot_addFriend(QAction* action);

private:
    Ui::mDialog *ui;
    QVBoxLayout* m_pLayout;
    QVBoxLayout* m_pLayoutGroup;
    QMenu* m_set;

};

#endif // MDIALOG_H
