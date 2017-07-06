#ifndef LOAD_DIALOG_H
#define LOAD_DIALOG_H

#include <QDialog>
#include "model.h"

namespace Ui {
class load_Dialog;
}

class load_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit load_Dialog(QWidget *parent = 0, Data *d=NULL);
    ~load_Dialog();
    void openFile(QString fname1, QString fname2);

private slots:
    void on_imagefile_clicked();

    void on_realfile_clicked();

    void on_buttonBox_accepted();

private:
    Ui::load_Dialog *ui;
    Data* data;
    QString imageFile;
    QString realFile;
};

#endif // LOAD_DIALOG_H
