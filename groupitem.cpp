#include "groupitem.h"
#include "ui_groupitem.h"
#include <QDebug>
groupItem::groupItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::groupItem)
{
    ui->setupUi(this);
}

groupItem::~groupItem()
{
    delete ui;
}

void groupItem::setInfo(int iconId, QString name,int groupId)
{
    ui->lb_name->setText(name);
    m_name = name;
    QString iconPath = QString(":/src/resource/tx/%1.png").arg(iconId);
    ui->pb_icon->setIcon(QIcon(iconPath));

    m_groupId = groupId;
}

const QString &groupItem::name() const
{
    return m_name;
}

void groupItem::on_pb_icon_clicked()
{
    qDebug()<<__func__<<endl;
    emit SIG_showChatGroup(m_groupId);
}

