#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "load_dialog.h"

#include "CSVLoader.h"
#include <QFileDialog>
#include <Qstring>
#include <fstream>
#include <qwt_plot_grid.h>
#include <QScrollArea>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    data=new Data();
    QScrollArea *scrollArea = new QScrollArea;
    layout = new QVBoxLayout;
    QWidget *temp=new QWidget;
    temp->setLayout(layout);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(temp);
    this->setCentralWidget(scrollArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::on_actionimage_file_triggered()
//{
//    QString files = QFileDialog::getOpenFileName(this,tr("Open image file"),QString(),"csv file (*.csv);; all file(*)");
//    if(files.isEmpty())return;
//    openFile(files,"imag");

//    QLayoutItem *c;
//    while((c=layout->takeAt(0))!=0)
//        delete c;

//    initplot();
//    replot();
//}

//void MainWindow::on_actionreal_file_triggered()
//{
//    QString files = QFileDialog::getOpenFileName(this,tr("Open real file"),QString(),"csv file (*.csv);; all file(*)");
//    if(files.isEmpty())return;
//    openFile(files,"real");
//}

void MainWindow::openFile(QString fname,QString type){
    if(type=="imag"){
        data->imag.clear();
        data->Dist.clear();
    }else if(type=="real"){
        data->real.clear();
        data->Dist.clear();
    }
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
                        if(type=="imag")
                            data->imag.push_back(make_pair(title[i],valueVector));
                        else
                            data->real.push_back(make_pair(title[i],valueVector));
                    }
                }
            }else{
            for(int i=0;i<line.Size();i++){
                if(title[i]=="LineDist")
                    data->Dist.push_back(line[i].AsDouble());
                if(i>5){
                    if(type=="imag")
                        for(int j=0;j<data->imag.size();j++){
                            if(data->imag[j].first==title[i]){
                                data->imag[j].second.push_back(line[i].AsDouble());
                            }
                        }
                    else
                        for(int j=0;j<data->real.size();j++){
                            if(data->real[j].first==title[i]){
                                data->real[j].second.push_back(line[i].AsDouble());
                            }
                        }
                }
            }
            }
        }
    }
}

void MainWindow::initplot(){
    plot.clear();
    QLayoutItem* item;
        while ( ( item = layout->takeAt( 0 ) ) != NULL )
        {
            delete item->widget();
            delete item;
        }
    for(int i=0;i<data->imag.size();i++){
        QwtPlot *p = new QwtPlot;
        p->enableAxis(QwtPlot::yRight,false);
        p->setAxisTitle(QwtPlot::yLeft,QString::fromStdString(data->imag[i].first));
        p->setAxisScale(QwtPlot::yLeft, -1.0,1.0);
        p->setCanvasBackground( QBrush(Qt::white));

        QwtPlotLegendItem *legendItem = new QwtPlotLegendItem();
        legendItem->setRenderHint(QwtPlotItem::RenderAntialiased);
        legendItem->setTextPen(QColor(Qt::black));
        legendItem->setBorderPen(QColor(Qt::black));
        QColor c(Qt::gray);
        c.setAlpha(200);
        legendItem->setBackgroundBrush(c);
        legendItem->setMaxColumns(1);
        legendItem->setAlignment(Qt::AlignRight|Qt::AlignBottom);
        legendItem->attach(p);
        layout->addWidget(p);
        plot.push_back(p);
    }
}

void MainWindow::replot(){
    for(int i=0; i<data->imag.size();i++){
        QPolygonF p;
        for(int j=0; j<data->imag[i].second.size();j++){
            p<<QPointF(data->Dist[j],data->imag[i].second.at(j));
        }
        QwtPlotCurve *curve= new QwtPlotCurve("Quadrature");
        curve->setYAxis(QwtPlot::yLeft);
        curve->setSamples(p);
        curve->setPen(Qt::red,2);
        curve->attach(plot[i]);
    }
    for(int i=0; i<data->real.size();i++){
        QPolygonF p;
        for(int j=0; j<data->real[i].second.size();j++){
            p<<QPointF(data->Dist[j],data->real[i].second.at(j));
        }
        QwtPlotCurve *curve= new QwtPlotCurve("Quadrature");
        curve->setYAxis(QwtPlot::yLeft);
        curve->setSamples(p);
        curve->setPen(Qt::green,2);
        curve->attach(plot[i]);
    }
}

void MainWindow::on_actionopen_file_triggered()
{
    load_Dialog *loadpage=new load_Dialog(NULL, data);
    loadpage->setModal(true);
    loadpage->exec();

    initplot();
    replot();
}
