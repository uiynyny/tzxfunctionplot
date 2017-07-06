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
#include <qwt_plot_marker.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    data=new Data();
    //set the main layout of the window
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



void MainWindow::initplot(){
    plot.clear();
    QLayoutItem* item;
        while ( ( item = layout->takeAt( 0 ) ) != NULL )
        {
            delete item->widget();
            delete item;
        }
    for(int i=0;i<data->imag.size();i++){
        //plot for each set of frequency
        QwtPlot *p = new QwtPlot;
        p->enableAxis(QwtPlot::yRight,false);
        p->setAxisTitle(QwtPlot::yLeft,QString::fromStdString(data->imag[i].first));
        p->setAxisScale(QwtPlot::yLeft, -2.0,2.0,1);
        p->setCanvasBackground( QBrush(Qt::white));

        //the legend for two lines in each plot
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

        //draw the horizontal line through y=0
        QwtPlotMarker *mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "y = 0" ) );
        mY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
        mY->setLineStyle( QwtPlotMarker::HLine );
        mY->setYValue( 0.0 );
        mY->attach( p );

        layout->addWidget(p);
        plot.push_back(p);
    }
}

void MainWindow::replot(){
    //read all points from image and make the curve
    for(int i=0; i<data->imag.size();i++){
        QPolygonF p;
        for(int j=0; j<data->imag[i].second.size();j++){
            p<<QPointF(data->Dist[j],data->imag[i].second.at(j));
        }
        QwtPlotCurve *curve= new QwtPlotCurve("Quadrature");
        curve->setYAxis(QwtPlot::yLeft);
        curve->setSamples(p);
        curve->setPen(Qt::green,2);
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setCurveAttribute(QwtPlotCurve::Fitted);
        curve->attach(plot[i]);
    }
    //read all points from the real and make the curve
    for(int i=0; i<data->real.size();i++){
        QPolygonF p;
        for(int j=0; j<data->real[i].second.size();j++){
            p<<QPointF(data->Dist[j],data->real[i].second.at(j));
        }
        QwtPlotCurve *curve= new QwtPlotCurve("In-Phase");
        curve->setYAxis(QwtPlot::yLeft);
        curve->setSamples(p);
        curve->setPen(Qt::red,2);
        curve->setStyle(QwtPlotCurve::Lines);
        curve->setCurveAttribute(QwtPlotCurve::Fitted);
        curve->attach(plot[i]);
    }
}

void MainWindow::on_actionopen_file_triggered()
{
    //open file dialog
    load_Dialog *loadpage=new load_Dialog(NULL, data);
    loadpage->setModal(true);
    loadpage->exec();

    initplot();
    replot();
}
