#ifndef CHATGROUP_H
#define CHATGROUP_H
#include "friendwidget.h"
#include <QDialog>
#include <QMap>
#include <QVBoxLayout>
namespace Ui {
class ChatGroup;
}

class ChatGroup : public QDialog
{
    Q_OBJECT
signals:
    void SIG_chatGroupMessage(QString content,int groupId);
public:
    explicit ChatGroup(QWidget *parent = nullptr);
    ~ChatGroup();
    void setChatInfo(int id,QString name);
    void setChatContent(QString content,QString name);
    void addMember(friendWidget *friendw);
    QMap<int,friendWidget*> m_mapIdToFriendwidget;

private slots:
    void on_pb_send_clicked();

private:
    Ui::ChatGroup *ui;
    int m_id;
    QString m_name;
    QVBoxLayout* m_pLayout;

};

#endif // CHATGROUP_H
