#include "chatgroup.h"
#include "ui_chatgroup.h"
#include <QTime>
ChatGroup::ChatGroup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ChatGroup)
{
    ui->setupUi(this);
    m_pLayout = new QVBoxLayout;
    //设置垂直距离
    m_pLayout->setSpacing(3);
    //设置外边框的距离
    m_pLayout->setContentsMargins(0,0,0,0);
    //设置到大控件上
    ui->wdg_member->setLayout(m_pLayout);
}

ChatGroup::~ChatGroup()
{
    delete ui;
}

void ChatGroup::setChatInfo(int id, QString name)
{
    m_id = id;
    m_name = name;
    this->setWindowTitle(name);
}

void ChatGroup::setChatContent(QString content,QString name)
{

    ui->tb_chat->append(QString("[%1] %2").arg(name).arg(QTime::currentTime().toString("hh:mm::ss")));
    ui->tb_chat->append(content);

}

void ChatGroup::addMember(friendWidget *friendw)
{
   m_pLayout->addWidget(friendw);
}



void ChatGroup::on_pb_send_clicked()
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
    ui->tb_chat->append(QString("[我] %1").arg(QTime::currentTime().toString("hh:mm:ss")));
    ui->tb_chat->append(content);
    //5.发给kernel
    emit SIG_chatGroupMessage(content,m_id);
}

