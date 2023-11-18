#ifndef FRIENDWIDGET_H
#define FRIENDWIDGET_H

#include <QWidget>

namespace Ui {
class friendWidget;
}

class friendWidget : public QWidget
{
    Q_OBJECT
signals:
    void SIG_showChat(int friendId);
    void SIG_deleteFriend(int friendId);
public:
    explicit friendWidget(QWidget *parent = nullptr);
    ~friendWidget();
    void setInfo(int friendId,int icon,QString name,QString feeling,int status);
    void setOffline();
    const QString &name() const;
    void hideDeleteUi();
private slots:
    void on_pb_icon_clicked();

    void on_pushButton_clicked();

private:
    Ui::friendWidget *ui;
    //对端聊天id
    int m_friendId;
    int m_icon;
    QString m_name;
    QString m_feeling;
    int m_status;
};

#endif // FRIENDWIDGET_H
