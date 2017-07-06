#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "model.h"
#include <qwt_plot.h>
#include <QVBoxLayout>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    //void openFile(QString fname, QString type);
    void initplot();
    void replot();

private slots:
    void on_actionopen_file_triggered();

private:
    Ui::MainWindow *ui;
    Data* data;
    std::vector<QwtPlot *> plot;
    QVBoxLayout *layout;
};

#endif // MAINWINDOW_H
