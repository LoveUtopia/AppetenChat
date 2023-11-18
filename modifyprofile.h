#ifndef MODIFYPROFILE_H
#define MODIFYPROFILE_H

#include <QDialog>

namespace Ui {
class ModifyProFile;
}

class ModifyProFile : public QDialog
{
    Q_OBJECT

public:
    explicit ModifyProFile(QWidget *parent = nullptr);
    ~ModifyProFile();

private:
    Ui::ModifyProFile *ui;
};

#endif // MODIFYPROFILE_H
