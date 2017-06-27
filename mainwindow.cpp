#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "CSVLoader.h"
#include <QFileDialog>
#include <Qstring>
#include <fstream>
#include <qwt_plot_grid.h>
#include <QColor>
#include <QScrollBar>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    data=new Data();
    ui->verticalLayout->add
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionimage_file_triggered()
{
    QString files = QFileDialog::getOpenFileName(this,tr("Open image file"),QString(),"csv file (*.csv);; all file(*)");
    if(files.isEmpty())return;
    openFile(files);
    for(int i=0;i<data->freqs.size();i++){
        initplot(i,QColor("Red"));
        ui->verticalLayout->addWidget(plot);
    }
}

void MainWindow::on_actionreal_file_triggered()
{
    QString files = QFileDialog::getOpenFileName(this,tr("Open real file"),QString(),"csv file (*.csv);; all file(*)");
    if(files.isEmpty())return;
    openFile(files);
}

void MainWindow::openFile(QString fname){
    std::vector <std::string> title;
    std::fstream fin(fname.toStdString());
    CSVLoader *loader=new CSVLoader();
    loader->ReadFromStream(fin);
    CSVLoader::Lines &lines = loader->GetLines();
    bool firsttime=true;
    for(CSVLoader::Lines::iterator it = lines.begin();it !=lines.end();++it){
        CSVLoader::Line line = *it;
        if(line.Size()!=1){
            if(firsttime){
                firsttime=false;
                for(int i=0;i<line.Size();i++){
                    title.push_back((std::string)line[i].AsString());
                    std::vector<double> valueVector;
                    if(i>5 && !title[i].empty()){
                        data->freqs.push_back(make_pair(title[i],valueVector));
                    }
                }
            }else{
            for(int i=0;i<line.Size();i++){
                if(title[i]=="LineDist")
                    data->Dist.push_back(line[i].AsDouble());
                if(i>5){
                    for(int j=0;j<data->freqs.size();j++){
                        if(data->freqs[j].first==title[i]){
                            data->freqs[j].second.push_back(line[i].AsDouble());
                        }
                    }
                }
            }
            }
        }
    }
}

void MainWindow::initplot(int i,QColor color){
    plot = new QwtPlot();
    plot->enableAxis(QwtPlot::yRight,false);
    plot->setAxisTitle(QwtPlot::yLeft,QString::fromStdString(data->freqs[i].first));
    plot->setAxisScale(QwtPlot::yLeft, -1.0,1.0);
//    QwtPlotGrid *grid = new QwtPlotGrid;
//    grid->enableXMin(true);
//    grid->setXAxis(QwtPlot::xBottom);
//    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
//    grid->setMinorPen( Qt::darkGray, 0, Qt::DotLine );
//    grid->attach(plot);

}
