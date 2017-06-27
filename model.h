#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include <map>

class Data{
public:
std::vector<double> Dist;
typedef std::pair<std::string, std::vector<double>> freq;
std::vector<freq> freqs;
};

#endif // MODEL_H
