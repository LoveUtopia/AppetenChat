#ifndef CHAT_H
#define CHAT_H
#include <QDialog>
#include <QEvent>
namespace Ui {
class Chat;
}

class Chat : public QDialog
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = nullptr);
    void setChatInfo(QString name,int friendId);
    void setChatContent(QString content);
    void setFriendOffline();
    bool eventFilter(QObject *obj, QEvent *event);
    ~Chat();
signals:
    void SIG_chatMessage(QString content,int friendId);
private slots:
    void on_pb_send_clicked();

private:
    Ui::Chat *ui;
    int m_friendId;
    QString m_friendName;

};

#endif // CHAT_H
