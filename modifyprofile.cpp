#include "modifyprofile.h"
#include "ui_modifyprofile.h"

ModifyProFile::ModifyProFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ModifyProFile)
{
    ui->setupUi(this);
}

ModifyProFile::~ModifyProFile()
{
    delete ui;
}
