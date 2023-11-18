#include "chatdialog.h"
#include "inetMediator/TcpClientMediator.h"
#include <QApplication>
#include <QMessageBox>
#include <iostream>
#include "ckernel.h"
using namespace std;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    chatDialog w;
//    w.show();
    CKernel kernel;
    return a.exec();
}
