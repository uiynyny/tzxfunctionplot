#ifndef CUSTOM_SIZE_H
#define CUSTOM_SIZE_H

#include <QDialog>

namespace Ui {
class custom_size;
}

class custom_size : public QDialog
{
    Q_OBJECT

public:
    explicit custom_size(QWidget *parent = 0);
    ~custom_size();
    double getmin();
    double getmax();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::custom_size *ui;
    double min;
    double max;
};

#endif // CUSTOM_SIZE_H
