#include "chat.h"
#include "ui_chat.h"
#include <QTime>
#include <QKeyEvent>
#include <QDebug>
Chat::Chat(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    //绑定回车发送
    connect(ui->te_chat, SIGNAL(returnPressed()), ui->pb_send, SLOT(on_pb_send_clicked()));

}

void Chat::setChatInfo(QString name, int friendId)
{
    m_friendId = friendId;
    m_friendName = name;
    this->setWindowTitle(QString("与[%1]聊天").arg(name));
}

void Chat::setChatContent(QString content)
{
    ui->tb_chat->append(QString("[%1] %2").arg(m_friendName).arg(QTime::currentTime().toString("hh:mm::ss")));
    ui->tb_chat->append(content);
}

void Chat::setFriendOffline()
{
    ui->tb_chat->append(QString("您的好友[%1]已下线").arg(m_friendName));
}

Chat::~Chat()
{
    delete ui;
}

void Chat::on_pb_send_clicked()
{
    //1.获取纯文本内容，判断文本是否合法
    QString content = ui->te_chat->toPlainText();
    if(content.isEmpty()||content.remove(" ").isEmpty()){
        ui->te_chat->setText("");
        return;
    }
    //2.获取带格式的内容
    content = ui->te_chat->toHtml();
    //3.清空窗口
    ui->te_chat->setText("");
    //4.内容显示到浏览器
    ui->tb_chat->append(QString("[我] %1").arg(QTime::currentTime().toString("hh:mm::ss")));
    ui->tb_chat->append(content);
    //5.发给kernel
    emit SIG_chatMessage(content,m_friendId);
}

bool Chat::eventFilter(QObject *obj, QEvent *event)
{
    qDebug()<<__func__;
    if (event->type()==QEvent::KeyPress)
    {
        QKeyEvent* key = (QKeyEvent*)(event);
        if ((key->key() == Qt::Key_Enter) || (key->key() == Qt::Key_Return))
        {
            on_pb_send_clicked();
        }
    }
}
