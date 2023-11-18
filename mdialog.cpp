#include "mdialog.h"
#include "ui_mdialog.h"
#include "ckernel.h"
#include <QCloseEvent>
mDialog::mDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mDialog)
{
    ui->setupUi(this);
    m_pLayout = new QVBoxLayout;
    m_pLayoutGroup = new QVBoxLayout;
    //设置垂直距离
    m_pLayout->setSpacing(3);
    m_pLayoutGroup->setSpacing(3);
    //设置外边框的距离
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayoutGroup->setContentsMargins(0,0,0,0);
    //设置到大控件上
    ui->wdg_list->setLayout(m_pLayout);
    ui->wdg_group->setLayout(m_pLayoutGroup);

    m_set = new QMenu;
    m_set->addAction("添加好友");
    m_set->addAction("设置");
    m_set->addAction("创建群聊");
    m_set->addAction("添加群聊");
    connect(m_set,SIGNAL(triggered(QAction*)),this,SLOT(slot_addFriend(QAction*)));
}

mDialog::~mDialog()
{
    if(m_pLayout){
        m_pLayout = nullptr;
        delete m_pLayout;
    }
    delete ui;
}

void mDialog::addFriend(friendWidget *friendw)
{
    m_pLayout->addWidget(friendw);
}

void mDialog::addGroup(groupItem *Item)
{
    m_pLayoutGroup->addWidget(Item);
}

void mDialog::setSelfInfo(QString name, QString feeling, int iconId)
{

    ui->le_name->setText(name);
    ui->le_feeling->setText(feeling);
    QString iconPath = QString(":/src/resource/tx/%1.png").arg(iconId);
    ui->pb_profile->setIcon(QIcon(iconPath));
}

void mDialog::closeEvent(QCloseEvent *event)
{
    //忽略不走父类函数
    event->ignore();
    if(QMessageBox::Yes == QMessageBox::question(this,"询问","是否退出？")){
        emit SIG_offline();
    }
}


void mDialog::on_pb_set_clicked()
{
    //获取鼠标位置
    QPoint pos = QCursor::pos();
    //获取框大小
    QSize size = m_set->sizeHint();
    //显示
    m_set->exec(QPoint(pos.x(),pos.y()-size.height()));
}

void mDialog::slot_addFriend(QAction *action)
{
    qDebug()<<__func__;
    if("添加好友" == action->text()){
            emit SIG_addFriend();
        }else if("创建群聊" == action->text()){
            emit SIG_createGroup();
    }else if("添加群聊" == action->text()){
            emit SIG_addGroup();
    }

}

