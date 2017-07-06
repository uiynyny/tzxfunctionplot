#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <map>

class Data{
public:
std::vector<double> Dist;
std::vector<std::pair<std::string, std::vector<double>>> real;
std::vector<std::pair<std::string, std::vector<double>>> imag;

void clear(){
    Dist.clear();
    imag.clear();
    real.clear();
}
};

#endif // MODEL_H
