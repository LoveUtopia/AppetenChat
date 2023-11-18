#ifndef GROUPITEM_H
#define GROUPITEM_H

#include <QWidget>

namespace Ui {
class groupItem;
}

class groupItem : public QWidget
{
    Q_OBJECT
signals:
    void SIG_showChatGroup(int groupId);
public:
    explicit groupItem(QWidget *parent = nullptr);
    ~groupItem();
    void setInfo(int iconId,QString name,int groupId);
    const QString &name() const;

private slots:
    void on_pb_icon_clicked();

private:
    Ui::groupItem *ui;
    QString m_name;
    int m_groupId;
};

#endif // GROUPITEM_H
