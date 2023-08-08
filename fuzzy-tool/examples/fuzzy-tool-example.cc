#include "ns3/fuzzy-tool-module.h"

using namespace ns3;

int main()
{
	FIS *fis;
	int i, j;
	int debug = 1;

	DOUBLE **dataMatrix, **fisMatrix, **outputMatrix;
	char *fis_file, *data_file;
	int data_row_n, data_col_n, fis_row_n, fis_col_n;

	data_file = (char*)"dataMatrixFile.txt";  //测试用数据（）
	fis_file = (char*)"cluster.fis";    //fis文件
	FuzzyTool fuzzy;
	/* obtain data matrix and FIS matrix */
	dataMatrix = fuzzy.returnDataMatrix(data_file, &data_row_n, &data_col_n);
	fisMatrix = fuzzy.returnFismatrix(fis_file, &fis_row_n, &fis_col_n);

	/* build FIS data structure */
	fis = (FIS *)fuzzy.fisCalloc(1, sizeof(FIS));
	fuzzy.fisBuildFisNode(fis, fisMatrix, fis_col_n, MF_POINT_N);

	/* error checking */
	if (data_col_n < fis->in_n) {
		printf("Given FIS is a %d-input %d-output system.\n",
			fis->in_n, fis->out_n);
		printf("Given data file does not have enough input entries.\n");
		fuzzy.fisFreeMatrix((void **)dataMatrix, data_row_n);
		fuzzy.fisFreeMatrix((void **)fisMatrix, fis_row_n);
		fuzzy.fisFreeFisNode(fis);
		fisError((char*)"Exiting ...");
	}

	/* debugging */
	if (debug)
		fuzzy.fisPrintData(fis);

	/* create output matrix */
	outputMatrix = (DOUBLE **)fuzzy.fisCreateMatrix(data_row_n, fis->out_n, sizeof(DOUBLE));

	/* evaluate FIS on each input vector */
	for (i = 0; i < data_row_n; i++)
		fuzzy.getFisOutput(dataMatrix[i], fis, outputMatrix[i]);

	/* print output vector */
	for (i = 0; i < data_row_n; i++) {
		for (j = 0; j < fis->out_n; j++)
			printf("%.12f ", outputMatrix[i][j]);
		printf("\n");
	}

	/* clean up memory */
	fuzzy.fisFreeFisNode(fis);
	fuzzy.fisFreeMatrix((void **)dataMatrix, data_row_n);
	fuzzy.fisFreeMatrix((void **)fisMatrix, fis_row_n);
	fuzzy.fisFreeMatrix((void **)outputMatrix, data_row_n);
	//exit(0);
	return 0;

}
