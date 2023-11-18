#include "chatdialog.h"
#include "ui_chatdialog.h"
#include <QMessageBox>
#include "inet/packDef.h"
#include <QMovie>
chatDialog::chatDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::chatDialog)
{
    ui->setupUi(this);
    ui->lb_icon->setScaledContents(true);
    QMovie *movie = new QMovie(":/src/resource/images/cyberlogoWord.png_swing.gif");
    ui->lb_icon->setMovie(movie);
    movie->start();

}

chatDialog::~chatDialog()
{
    delete ui;
}

void chatDialog::closeEvent(QCloseEvent *)
{
    emit SIG_closeEvent();
    this->hide();
}

QString chatDialog::getIp()
{
    QString ip = ui->le_ip->text();
    return ip;
}


void chatDialog::on_pb_clear_register_clicked()
{
    ui->le_name_register_->setText("");
    ui->le_password_register->setText("");
    ui->le_tel_register->setText("");
}


void chatDialog::on_pb_commit_register_clicked()
{
    //设置ip

    //获取内容
    QString name = ui->le_name_register_->text();
    QString tel = ui->le_tel_register->text();
    QString psd = ui->le_password_register->text();
    QString namet = name;
    QString telt = tel;
    QString psdt = psd;
    //校验合法性
    //1.是否为空或全空格
    if(name.isEmpty()||tel.isEmpty()||psd.isEmpty()||
            namet.remove(" ").isEmpty()||
            telt.remove(" ").isEmpty()||
            psdt.remove(" ").isEmpty()){
       QMessageBox::about(this,"error","内容不能为空");
       return;
    }
    //2.判断长度
    if(name.length()>50||tel.length()!=11||psd.length()>50){
        QMessageBox::about(this,"error","字符数不符要求");
        return;
    }
    //3.判断内容合法 正则表达式

    //发送
    emit SIG_sendRegisterData(name,tel,psd);
}


void chatDialog::on_pb_clear_clicked()
{
    ui->le_tel->setText("");
    ui->le_password->setText("");
}


void chatDialog::on_pb_commit_clicked()
{
    //获取内容
    QString tel = ui->le_tel->text();
    QString psd = ui->le_password->text();
    QString telt = tel;
    QString psdt = psd;
    //校验合法性
    //1.是否为空或全空格
    if(tel.isEmpty()||psd.isEmpty()||
            telt.remove(" ").isEmpty()||
            psdt.remove(" ").isEmpty()){
       QMessageBox::about(this,"error","内容不能为空");
       return;
    }
    //2.判断长度
    if(tel.length()!=11||psd.length()>50){
        QMessageBox::about(this,"error","字符数不符要求");
        return;
    }
    //3.判断内容合法 正则表达式

    //发送
    emit SIG_sendLoginData(tel,psd);
}

