#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class chatDialog; }
QT_END_NAMESPACE

class chatDialog : public QDialog
{
    Q_OBJECT

public:
    chatDialog(QWidget *parent = nullptr);
    ~chatDialog();
    void closeEvent(QCloseEvent *);
    QString getIp();
private slots:
    void on_pb_clear_register_clicked();

    void on_pb_commit_register_clicked();

    void on_pb_clear_clicked();

    void on_pb_commit_clicked();

private:
    Ui::chatDialog *ui;
signals:
    void SIG_sendRegisterData(QString name,QString tel, QString psd);
    void SIG_sendLoginData(QString tel, QString psd);
    void SIG_closeEvent();

};
#endif // CHATDIALOG_H
