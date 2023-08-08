#include "fuzzy.h"
fuzzy::fuzzy()
{
}

fuzzy::~fuzzy()
{
    /* clean up memory */
	fuzzyTool.fisFreeFisNode(fis);
	fuzzyTool.fisFreeMatrix((void **)dataMatrix, data_row_n);
	fuzzyTool.fisFreeMatrix((void **)fisMatrix, fis_row_n);
	fuzzyTool.fisFreeMatrix((void **)outputMatrix, data_row_n);
}

char *fuzzy::setFisPath(char *path)
{
    fis_file = path;
    genFisMatrix(path);
    fis = (FIS *)fuzzyTool.fisCalloc(1, sizeof(FIS));
	fuzzyTool.fisBuildFisNode(fis, fisMatrix, fis_col_n, MF_POINT_N);
    return fis_file;
}

DOUBLE **fuzzy::genFisMatrix(char *fis_file)
{
    fisMatrix = fuzzyTool.returnFismatrix(fis_file, &fis_row_n, &fis_col_n);
    return fisMatrix;
}

DOUBLE **fuzzy::genOutMatrix()
{
    outputMatrix = (DOUBLE **)fuzzyTool.fisCreateMatrix(data_row_n, fis->out_n, sizeof(DOUBLE));
    return outputMatrix;
}

DOUBLE **fuzzy::calculate(DOUBLE **dataArry, int row, int col)
{
    data_row_n = row;
    data_col_n = col;
    dataMatrix = dataArry;
    genOutMatrix();
    for (int i = 0; i < data_row_n; i++)
		fuzzyTool.getFisOutput(dataMatrix[i], fis, outputMatrix[i]);
    return outputMatrix;
}