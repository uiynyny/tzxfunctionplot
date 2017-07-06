#include "load_dialog.h"
#include "ui_load_dialog.h"
#include "CSVLoader.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <fstream>
#include <QString>
#include <QDebug>

load_Dialog::load_Dialog(QWidget *parent, Data *d) :
    QDialog(parent),data(d),
    ui(new Ui::load_Dialog)
{
    ui->setupUi(this);
}

load_Dialog::~load_Dialog()
{
    delete ui;
}

void load_Dialog::on_imagefile_clicked()
{
    QString files = QFileDialog::getOpenFileName(this,tr("Open image file"),QString(),"csv file (*.csv);; all file(*)");
    if(files.isEmpty())return;
    ui->imagefileedit->setText(files);
    imageFile=files;
}

void load_Dialog::on_realfile_clicked()
{
    QString files = QFileDialog::getOpenFileName(this,tr("Open real file"),QString(),"csv file (*.csv);; all file(*)");
    if(files.isEmpty())return;
    ui->realfileedit->setText(files);
    realFile=files;
}

void load_Dialog::openFile(QString fname1,QString fname2){
    data->clear();
    std::vector <std::string> title;
    std::fstream fin(fname1.toStdString());
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
                            data->imag.push_back(make_pair(title[i],valueVector));
                            data->real.push_back(make_pair(title[i],valueVector));
                    }
                }
            }else{
                for(int i=0;i<line.Size();i++){
                    if(title[i]=="LineDist")
                        data->Dist.push_back(line[i].AsDouble());
                    if(i>5){
                        for(int j=0;j<data->imag.size();j++){
                            if(data->imag[j].first==title[i]){
                                data->imag[j].second.push_back(line[i].AsDouble());
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    fin=std::fstream(fname2.toStdString());
    loader->ReadFromStream(fin);
    lines=loader->GetLines();
    for(CSVLoader::Lines::iterator it = lines.begin(); it !=lines.end();++it){
        CSVLoader::Line line=*it;
        if(line.Size()==1) continue;
        for(int i=0;i<line.Size();i++){
            bool valid;
            QString temp=QString::fromStdString(line[i].AsString());
            double real=temp.toDouble(&valid);
            if(i>5&&valid){
                for(int j=0;j<data->real.size();j++){
                    if(data->real[j].first==title[i]){
                        data->real[j].second.push_back(line[i].AsDouble());
                        break;
                    }
                }
            }
        }
    }
}


void load_Dialog::on_buttonBox_accepted()
{
    if(!QFile(ui->imagefileedit->text()).exists()){
        QMessageBox *err = new QMessageBox(QMessageBox::Warning,"file not found","image file not found",QMessageBox::Ok);
        err->show();
        return;
    }
    if(!QFile(ui->realfileedit->text()).exists()){
        QMessageBox *err = new QMessageBox(QMessageBox::Warning,"file not found","real file not found",QMessageBox::Ok);
        err->show();
        return;
    }
    openFile(imageFile,realFile);
    this->close();
}
