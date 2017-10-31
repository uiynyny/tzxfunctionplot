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
#include <QMessageBox>
#include <QTextStream>
#include <QThread>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_legenditem.h>

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

    QWidget *temp=new QWidget;
    QHBoxLayout *tlayout= new QHBoxLayout;
    temp->setLayout(tlayout);

    tlayout->addWidget(scrollArea);
    tlayout->addWidget(list);
    tlayout->setStretch(0,9);
    tlayout->setStretch(1,1);
    this->setCentralWidget(temp);

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
    for(int i=0; i<dataSet[index].imag.size();i++){
        for(int j=0; j<dataSet[index].imag[i].second.size();j++){
            if(dataSet[index].imag[i].second.at(j)>dataSet[index].max)
                dataSet[index].max=dataSet[index].imag[i].second.at(j);
            if(dataSet[index].imag[i].second.at(j)<dataSet[index].min)
                 dataSet[index].min=dataSet[index].imag[i].second.at(j);
        }
    }
    for(int i=0; i<dataSet[index].real.size();i++){
        for(int j=0; j<dataSet[index].real[i].second.size();j++){
            if(dataSet[index].real[i].second.at(j)>dataSet[index].max)
                 dataSet[index].max=dataSet[index].real[i].second.at(j);
            if(dataSet[index].real[i].second.at(j)<dataSet[index].min)
                 dataSet[index].min=dataSet[index].real[i].second.at(j);
        }
    }
    for(int i=0;i<dataSet[index].imag.size();i++){
        QwtPlot *p = new QwtPlot;
        p->enableAxis(QwtPlot::yRight,false);
        p->enableAxis(QwtPlot::xBottom, false);
        if(i+1==dataSet[index].imag.size()){
            p->enableAxis(QwtPlot::xBottom,true);
        }
        p->setAxisTitle(QwtPlot::yLeft,QString::number(dataSet[index].imag[i].first));
        p->setAxisAutoScale(QwtPlot::yLeft);
        p->setCanvasBackground(QBrush(Qt::white));
        if(!ui->actionauto_scale->isChecked()){
            p->setAxisScale(QwtPlot::yLeft,(int)dataSet[index].min,(int)dataSet[index].max,(int)(dataSet[index].max-dataSet[index].min)/4);
        }
        QwtPlotPicker *picker = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,
                                                   QwtPlotPicker::CrossRubberBand, QwtPicker::AlwaysOn,
                                                   p->canvas());
        picker->setMousePattern(QwtEventPattern::MouseSelect1,
                                Qt::RightButton, Qt::NoModifier );
        picker->setStateMachine( new QwtPickerDragPointMachine() );
        picker->setRubberBandPen( QColor( Qt::green ) );
        picker->setRubberBand( QwtPicker::CrossRubberBand );
        picker->setTrackerPen( QColor( Qt::blue ) );

        if(i==0){
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
        }
        layout->addWidget(p);
        plot.push_back(p);
    }
}

void MainWindow::replot(int item){
    double min,max;
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
        min=curve->minYValue();
        max=curve->maxYValue();
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
    dataSet.clear();
    list->clear();

    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    QString(),
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    if(dir.isEmpty())return;
    QDir mDir(QFileInfo(dir).absoluteFilePath());
    QStringList qlist = readFile(dir);

    bool controlfile=false;
    if(qlist.filter(QRegExp("frequencies")).length()!=0){
        controlfile=true;
        QFile file(qlist.filter(QRegExp("frequencies")).at(0));
        if(!file.open(QIODevice::ReadOnly)){
            qDebug()<<"failed to open file";
            return;
        }
        QTextStream in(&file);

        while(!in.atEnd()) {
            QString line = in.readLine();
            QStringList  fields = line.split(",",QString::SkipEmptyParts);
            frequency+=fields;
        }
    }
    qlist=qlist.filter(QRegExp("Line[0-9]*_[A-Z]*[a-z]*_(Imag|Real)"));

    QProgressDialog progress("Loading files...", "Abort",0,qlist.size(),this);
    progress.setWindowModality(Qt::WindowModal);

    foreach(QString file, qlist){

        progress.setValue(qlist.indexOf(file));
        if(progress.wasCanceled()){
            dataSet.clear();
            list->clear();
            break;
        }
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
                            if(controlfile){
                                if(frequency.contains(QString::fromStdString(line[i].AsString()))){
                                    if(type=="Imag")
                                        temp.imag.push_back(make_pair(line[i].AsDouble(),tvector));
                                    if(type=="Real")
                                        temp.real.push_back(make_pair(line[i].AsDouble(),tvector));
                                }
                            }else{
                                if(type=="Imag")
                                    temp.imag.push_back(make_pair(line[i].AsDouble(),tvector));
                                if(type=="Real")
                                    temp.real.push_back(make_pair(line[i].AsDouble(),tvector));
                            }
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
    if(list->currentRow()==-1){
        QMessageBox *error= new QMessageBox(QMessageBox::Warning,"Warning","no file was choosen");
        error->show();
        return;
    }
    QPrinter prn;
    prn.setOutputFileName(list->currentItem()->text()+".pdf");
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


void MainWindow::on_actioncreate_pdf_for_all_triggered()
{
    for (int i=0; i<dataSet.size(); i++) {

        QWidget *temp=new QWidget();
        QVBoxLayout *templayout=new QVBoxLayout;
        temp->setLayout(templayout);

        for(int j=0;j<dataSet[i].imag.size();j++){
            //plot
            QwtPlot *p = new QwtPlot;
            p->enableAxis(QwtPlot::yRight,false);
            p->enableAxis(QwtPlot::xBottom, false);
            p->setAxisTitle(QwtPlot::yLeft,QString::number(dataSet[i].imag[j].first));
            p->setAxisAutoScale(QwtPlot::yLeft);
            p->setCanvasBackground(QBrush(Qt::white));

            //curve
            QPolygonF pi;
            for(int k=0; k<dataSet[i].imag[j].second.size();k++){
                pi<<QPointF(dataSet[i].Dist[k],dataSet[i].imag[j].second.at(k));
            }
            QwtPlotCurve *curvei= new QwtPlotCurve("Quadrature");
            curvei->setYAxis(QwtPlot::yLeft);
            curvei->setSamples(pi);
            curvei->setPen(Qt::red,2);
            curvei->attach(p);

            if(dataSet[i].min>curvei->minYValue())
                dataSet[i].min=curvei->minYValue();
            if(dataSet[i].max<curvei->maxYValue())
                dataSet[i].max=curvei->maxYValue();

            QPolygonF pr;
            for(int k=0; k<dataSet[i].real[j].second.size();k++){
                pr<<QPointF(dataSet[i].Dist[k],dataSet[i].real[j].second.at(k));
            }
            QwtPlotCurve *curver= new QwtPlotCurve("In-Phase");
            curver->setYAxis(QwtPlot::yLeft);
            curver->setSamples(pr);
            curver->setPen(Qt::green,2);
            curver->attach(p);

            if(dataSet[i].min>curver->minYValue())
                dataSet[i].min=curver->minYValue();
            if(dataSet[i].max<curver->maxYValue())
                dataSet[i].max=curver->maxYValue();

            if(!ui->actionauto_scale->isChecked()){
                p->setAxisScale(QwtPlot::yLeft,(int)dataSet[i].min,(int)dataSet[i].max,(int)(dataSet[i].max-dataSet[i].min)/4);
                p->replot();
            }
            templayout->addWidget(p);
        }
        temp->resize(3000,5000);
        QPrinter prn;
        prn.setOutputFileName(list->item(i)->text()+".pdf");
        QSize size=temp->size();
        prn.setPaperSize(size,QPrinter::DevicePixel);
        temp->render(&prn);
    }
}

void MainWindow::on_actionauto_scale_triggered()
{
    foreach (QwtPlot *p, plot){
        p->setAxisAutoScale(QwtPlot::yLeft,true);
        p->replot();
    }
}
