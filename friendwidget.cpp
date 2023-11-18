#include "friendwidget.h"
#include "ui_friendwidget.h"
#include "inet/packDef.h"
#include <QBitmap>
#include <QDebug>
#include <QMessageBox>
friendWidget::friendWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::friendWidget)
{
    ui->setupUi(this);
}

friendWidget::~friendWidget()
{
    delete ui;
}

void friendWidget::setInfo(int friendId,int icon, QString name, QString feeling, int status)
{
    m_friendId = friendId;
    m_icon = icon;
    m_name = name;
    m_feeling = feeling;
    m_status = status;

    QString iconPath = QString(":/src/resource/tx/%1.png").arg(icon);
    ui->lb_name->setText(name);
    ui->lb_feeling->setText(feeling);

    if(m_status == _status_online){
        ui->pb_icon->setIcon(QIcon(iconPath));
    }else if(m_status == _status_offline){
        QBitmap bitmap;
        bitmap.load(iconPath);
        ui->pb_icon->setIcon(bitmap);
    }

    //重绘
    this->repaint();
}

void friendWidget::setOffline()
{
    //设置下线状态
    m_status = _status_offline;
    //头像设为暗显
    QString iconPath = QString(":/src/resource/tx/%1.png").arg(m_icon);
    QBitmap bitmap;
    bitmap.load(iconPath);
    ui->pb_icon->setIcon(bitmap);
    //重绘
    this->repaint();

}

void friendWidget::on_pb_icon_clicked()
{
    qDebug()<<__func__<<endl;
    emit SIG_showChat(m_friendId);
}

const QString &friendWidget::name() const
{
    return m_name;
}

void friendWidget::hideDeleteUi()
{
    ui->pushButton->hide();
}


void friendWidget::on_pushButton_clicked()
{
    if(QMessageBox::Yes == QMessageBox::question(this,"删除好友",m_name)){
            emit SIG_deleteFriend(m_friendId);
    }
}

