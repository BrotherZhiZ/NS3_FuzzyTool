#include "ns3/fuzzy-tool.h"
class fuzzy
{
public:
    fuzzy();
    ~fuzzy();
    char * setFisPath(char *fis_file);
    DOUBLE ** genFisMatrix(char *fis_file);
    DOUBLE ** genOutMatrix();
    DOUBLE ** calculate(DOUBLE **dataArry, int row, int col);
    FuzzyTool fuzzyTool; // fuzzy tool class
    FIS *fis; //FIS data structure
    char *fis_file = 0;  // fis file path
    DOUBLE **dataMatrix, **fisMatrix, **outputMatrix;
    int data_row_n, data_col_n, fis_row_n, fis_col_n;
};
