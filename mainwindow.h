#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "model.h"
#include <qwt_plot.h>
#include <QVBoxLayout>
#include <QListWidget>
#include <QScrollArea>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QStringList readFile(QString dir);
    void initplot(int);
    void replot(int);
    void initiallist();

private slots:

    void on_actionopen_folder_triggered();

    void on_actionprint_PDF_triggered();

    void updateview(QListWidgetItem*);

    void on_actioncustom_scale_triggered();

    void slider_valuechanged(int);

private:
    Ui::MainWindow *ui;
    std::vector<Data> dataSet;
    QVBoxLayout *layout;
    std::vector<QwtPlot*> plot;
    QWidget *print;
    QListWidget *list;
    QScrollArea *scrollArea;
    QSlider *slide;
};

#endif // MAINWINDOW_H
