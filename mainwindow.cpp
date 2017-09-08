#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "CSVLoader.h"
#include "custom_size.h"

#include <QFileDialog>
#include <QProgressDialog>
#include <Qstring>
#include <fstream>
#include <QScrollArea>
#include <QListWidget>
#include <QHBoxLayout>
#include <QPrinter>
#include <QPrintDialog>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scrollArea = new QScrollArea;
    layout = new QVBoxLayout;
    print=new QWidget();
    print->setLayout(layout);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(print);

    list=new QListWidget;
    slide =new QSlider(Qt::Vertical);
    slide->setMinimum(0);
    slide->setMaximum(200);
    slide->setValue(100);

    QWidget *temp=new QWidget;
    QHBoxLayout *tlayout= new QHBoxLayout;
    temp->setLayout(tlayout);
    tlayout->addWidget(scrollArea);
    //tlayout->addWidget(slide);
    tlayout->addWidget(list);

    tlayout->setStretch(0,9);
    //tlayout->setStretch(1,0.5);
    tlayout->setStretch(1,1);
    this->setCentralWidget(temp);
    connect(slide,SIGNAL(valueChanged(int)),this,SLOT(slider_valuechanged(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initplot(int index){
    plot.clear();
    QLayoutItem* item;
    while ( ( item = layout->takeAt( 0 ) ) != NULL )
    {
        delete item->widget();
        delete item;
    }
    int max=0;
    int min=0;
    for(int i=0; i<dataSet[index].imag.size();i++){
        for(int j=0; j<dataSet[index].imag[i].second.size();j++){
            if(dataSet[index].imag[i].second.at(j)>max)
                max=dataSet[index].imag[i].second.at(j);
            if(dataSet[index].imag[i].second.at(j)<min)
                min=dataSet[index].imag[i].second.at(j);
        }
    }
    for(int i=0; i<dataSet[index].real.size();i++){
        for(int j=0; j<dataSet[index].real[i].second.size();j++){
            if(dataSet[index].real[i].second.at(j)>max)
                max=dataSet[index].real[i].second.at(j);
            if(dataSet[index].real[i].second.at(j)<min)
                min=dataSet[index].real[i].second.at(j);
        }
    }
    for(int i=0;i<dataSet[index].imag.size();i++){
        QwtPlot *p = new QwtPlot;
        p->enableAxis(QwtPlot::yRight,false);
        p->enableAxis(QwtPlot::xBottom, false);
        p->setAxisTitle(QwtPlot::yLeft,QString::number(dataSet[index].imag[i].first));
        p->setAxisAutoScale(QwtPlot::yLeft);
        p->setCanvasBackground(QBrush(Qt::white));
        p->setAxisScale(QwtPlot::yLeft,min,max,(max-min)/4);
        layout->addWidget(p);
        plot.push_back(p);
    }
}

void MainWindow::replot(int item){
    for(int i=0; i<dataSet[item].imag.size();i++){
        QPolygonF p;
        for(int j=0; j<dataSet[item].imag[i].second.size();j++){
            p<<QPointF(dataSet[item].Dist[j],dataSet[item].imag[i].second.at(j));
        }
        QwtPlotCurve *curve= new QwtPlotCurve("Quadrature");
        curve->setYAxis(QwtPlot::yLeft);
        curve->setSamples(p);
        curve->setPen(Qt::red,2);
        curve->attach(plot[i]);
    }
    for(int i=0; i<dataSet[item].real.size();i++){
        QPolygonF p;
        for(int j=0; j<dataSet[item].real[i].second.size();j++){
            p<<QPointF(dataSet[item].Dist[j],dataSet[item].real[i].second.at(j));
        }
        QwtPlotCurve *curve= new QwtPlotCurve("In-Phase");
        curve->setYAxis(QwtPlot::yLeft);
        curve->setSamples(p);
        curve->setPen(Qt::green,2);
        curve->attach(plot[i]);
    }
}

void MainWindow::initiallist(){
    for(int i=0;i<dataSet.size(); i++){
        list->addItem(dataSet[i].name);
    }
    connect(list,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(updateview(QListWidgetItem*)));
}

QStringList MainWindow::readFile(QString dir){
    QDir mDir(QFileInfo(dir).absoluteFilePath());
    QStringList files;
    foreach(QFileInfo m, mDir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoDotAndDotDot)){
        if(m.isDir()){
            files<<readFile(m.absoluteFilePath());
        }else if(m.isFile()){
            files<<m.absoluteFilePath();
        }
    }
    return files;
}

void MainWindow::on_actionopen_folder_triggered()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())return;
    QDir mDir(QFileInfo(dir).absoluteFilePath());
    QStringList qlist = readFile(dir);
    qlist=qlist.filter(QRegExp("Line[0-9]*_[A-Z]*[a-z]*_(Imag|Real)"));

    QProgressDialog progress("Opening files...", "Abort",0,qlist.size(),this);
    progress.setWindowModality(Qt::WindowModal);

    foreach(QString file, qlist){

        progress.setValue(qlist.indexOf(file));
        bool badFile=false;
        Data temp= Data();
        temp.name=QFileInfo(file).baseName();
        QString type= temp.name.split('_')[2];
        temp.name.remove(QRegExp("_(Imag|Real)"));
        if(dataSet.size()==0) dataSet.push_back(temp);
        for(int i=0;i<dataSet.size();i++){
            if (dataSet[i].name==temp.name){
                dataSet[i].Dist.clear();
                temp=dataSet[i];
            }
            else if(i==dataSet.size()-1)
                dataSet.push_back(temp);
        }

        std::vector <std::string> title;
        std::fstream fin(file.toStdString());
        CSVLoader *loader=new CSVLoader();
        loader->ReadFromStream(fin);
        CSVLoader::Lines &lines = loader->GetLines();
        bool firsttime=true;
        bool header=true;
        for(CSVLoader::Lines::iterator it = lines.begin(); it !=lines.end()&&badFile==false; ++it){
            CSVLoader::Line line = *it;
            if(header){
                if(line[0].AsString()=="BB1_Transfer_Function_File_Version_1"){
                    line=*(++it);
                    if(line[0].AsString()=="Frequencies"){
                        for(int i=1; i<line.Size(); i++){
                            std::vector<double> tvector;
                            if(type=="Imag")
                                temp.imag.push_back(make_pair(line[i].AsDouble(),tvector));
                            if(type=="Real")
                                temp.real.push_back(make_pair(line[i].AsDouble(),tvector));
                        }
                    }
                    line=*(++it);
                }else{
                    badFile=true;
                }
                header=false;
            }
            if(firsttime){
                firsttime=false;
                for(int i=0; i<line.Size(); i++)
                    if(!line[i].AsString().empty())
                        title.push_back((std::string)line[i].AsString());
                line=*(++it);
            }

            for(int i=0;i<line.Size()-1;i++){
                if(title[i]=="LineDist")
                    temp.Dist.push_back(line[i].AsDouble());
                if(i>5){
                    if(type=="Imag"){
                        for(int j=0;j<temp.imag.size();j++){
                            if(temp.imag[j].first==std::stod(title[i])){
                                temp.imag[j].second.push_back(line[i].AsDouble());
                                break;
                            }
                        }
                    }
                    if(type=="Real"){
                        for(int j=0; j<temp.real.size(); j++){
                            if(temp.real[j].first==std::stod(title[i])){
                                temp.real[j].second.push_back(line[i].AsDouble());
                                break;
                            }
                        }
                    }
                }
            }
        }

        for(int i=0;i<dataSet.size();i++){
            if (dataSet[i].name==temp.name)
                dataSet[i]=temp;
        }
    }
    initiallist();
}

void MainWindow::on_actionprint_PDF_triggered()
{
    QListWidgetItem* row=list->currentItem();
    QPrinter prn;
    prn.setOutputFileName(row->text()+".pdf");
    QSize size=print->size();
    qDebug()<<size;
    prn.setPaperSize(size,QPrinter::DevicePixel);
    print->render(&prn);
}

void MainWindow::updateview(QListWidgetItem* item){
    for(int i=0; i<dataSet.size(); i++){
        if(dataSet[i].name==item->text()){
            initplot(i);
            replot(i);
        }
    }
}

void MainWindow::on_actioncustom_scale_triggered()
{
    custom_size *csize= new custom_size;
    csize->setModal(true);
    csize->exec();
    if(csize->Accepted&&csize->getmin()!=csize->getmax()){
        foreach (QwtPlot *p, plot) {
            p->setAxisScale(QwtPlot::yLeft,csize->getmin(),csize->getmax(),(csize->getmax()-csize->getmin())/4);
            p->replot();
        }
    }

}

void MainWindow::slider_valuechanged(int i){
    QSize size=print->size();
    double h = size.height()*i/100;
    double w = size.width()*i/100;
    for(int j=0;j<plot.size();j++){
        plot[j]->resize(w,h/plot.size());
        plot[j]->update();
    }
}
