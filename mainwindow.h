#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "model.h"
#include <qwt_plot.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void openFile(QString fname);
    void initplot(int i,QColor color);

private slots:
    void on_actionimage_file_triggered();

    void on_actionreal_file_triggered();

private:
    Ui::MainWindow *ui;
    Data* data;
    QwtPlot *plot;
};

#endif // MAINWINDOW_H
