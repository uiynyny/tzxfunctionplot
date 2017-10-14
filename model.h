#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <QString>

class Data{
public:
    QString name;
    std::vector<double> Dist;
    std::vector<std::pair<double, std::vector<double>>> real;
    std::vector<std::pair<double, std::vector<double>>> imag;
    double max=0,min=0;

    void clear(){
        Dist.clear();
        imag.clear();
        real.clear();
    }
};

#endif // MODEL_H
