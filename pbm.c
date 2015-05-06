#include "pbm.h"

void *d_Sampling_img(const char *image_data, int rows, int cols){

	int i, j, flag;
	char *pix;
	char *ycbcr;
	char *ycbcr420;
	struct image_header ih;

//	image_data = (char *)malloc(3 * rows * cols + sizeof(struct image_header));

	//Convert RGB to YCbCr
	for (j = 0; j < rows; ++j)
		for (i = 0; i < cols; ++i) {
			pix = &image_data[sizeof(struct image_header) + j*(cols * 3) + i * 3];
			ycbcr = &image_data[sizeof(struct image_header) + j*(cols * 3) + i * 3];

			ycbcr[0] =((19595*pix[0] + 38470*pix[1] + 7471*pix[2])>>16);
			ycbcr[1] =((36962*(pix[2]-ycbcr[0]))>>16)+128;
			ycbcr[2] =((46727*(pix[0]-ycbcr[0]))>>16)+128;

		}

	//downSampling
	for (j = 0; j < rows; ++j)
		for (i = 0; i < cols; ++i) {
			ycbcr = &image_data[sizeof(struct image_header) +   j *(cols * 3) + i * 6];
			if(j%2==0 || j ==0){
				if(j==0) flag = 0;
				else flag = -1;
				ycbcr420 = &image_data[sizeof(struct image_header)
									   + (j+flag) *(cols * 3) + i * 6];
				ycbcr420[0] = ycbcr[0];//y420[0,0]
				ycbcr420[1] = ycbcr[0+3];//y420[0,1]
				ycbcr420[2] = ycbcr[0+(cols*3)+0];//y420[1,0]
				ycbcr420[3] = ycbcr[0+(cols*3)+3];//y420[1,1]
				ycbcr420[4] = (ycbcr[1]+ycbcr[1+(cols*3)]+ycbcr[1+ 3]+ycbcr[1+(cols*3)+ 3])/4;//cb420
				ycbcr420[5] = (ycbcr[2]+ycbcr[2+(cols*3)]+ycbcr[2+ 3]+ycbcr[2+(cols*3)+ 3])/4;//cr420
			}
		}

//	printf("d_sam_fin rows : %d, cols : %d\n",rows,cols);
//	free(image_data);
}

void *u_Sampling_img(const char *image_data, int rows, int cols){

	int i, j, flag;
	char *pix;
	char *ycbcr;
	char *ycbcr420;
	struct image_header ih;
	printf("w1 rows : %d, cols : %d\n",rows,cols);

//	image_data = (char *)malloc(3 * rows * cols + sizeof(struct image_header));

	//upSampling
	for (j = 0; j < rows; ++j)
		for (i = 0; i < cols; ++i) {
			ycbcr = &image_data[sizeof(struct image_header) +   j *(cols * 3) + i * 6];
			if(j%2==0 || j ==0){
				if(j==0) flag = 0;
				else flag = -1;
				ycbcr420 = &image_data[sizeof(struct image_header)
									   + (j+flag) *(cols * 3) + i * 6];

				ycbcr[0] = ycbcr420[0];
				ycbcr[1] = ycbcr420[4];
				ycbcr[2] = ycbcr420[5];

				ycbcr[0+ 3] = ycbcr420[1];
				ycbcr[1+ 3] = ycbcr420[4];
				ycbcr[2+ 3] = ycbcr420[5];

				ycbcr[0+ (cols*3)+0] = ycbcr420[2];
				ycbcr[1+ (cols*3)+0] = ycbcr420[4];
				ycbcr[2+ (cols*3)+0] = ycbcr420[5];

				ycbcr[0+ (cols*3)+3] = ycbcr420[3];
				ycbcr[1+ (cols*3)+3] = ycbcr420[4];
				ycbcr[2+ (cols*3)+3] = ycbcr420[5];

			}
		}

	//Convert YCbCr to RGB
	for (j = 0; j < rows; ++j)
		for (i = 0; i < cols; ++i) {
			pix = &image_data[sizeof(struct image_header) + j*(cols * 3) + i * 3];
			ycbcr = &image_data[sizeof(struct image_header) + j*(cols * 3) + i * 3];

			pix[0] = ycbcr[0] + ((91881*ycbcr[2])>>16)-179;
			pix[1] = ycbcr[0] - ((46793*ycbcr[2] + 22544*ycbcr[1])>>16)+135;
			pix[2] = ycbcr[0] + ((116129*ycbcr[1])>>16)-226;
		}
//	free(image_data);
}

char *readImage(const char *file_name, unsigned long *length, struct image_header *ih){
	FILE *pgmFile;
	int i, j;
	int pixel;

	char *image_data;
	char *pix;   //sizeof(pix) = 6   64bit

	pgmFile = fopen(file_name, "rb"); //rb(read binary)
	if (pgmFile == NULL) {
		fprintf(stderr, "cannot open file to read");
		fclose(pgmFile);
		return NULL;
	}

	//check the format
	fgets(ih->format, sizeof(ih->format), pgmFile);
	if (strcmp(ih->format, "P5") != 0 && strcmp(ih->format, "P6") != 0) {
		fprintf(stderr, "Wrong file type!\n");
		fclose(pgmFile);
		return NULL;
	}
	//read header
	fscanf(pgmFile, "%d", &ih->cols);  //file read is fscanf
	fscanf(pgmFile, "%d", &ih->rows);
	fscanf(pgmFile, "%d", &ih->levels);
	fgetc(pgmFile); //space

	if (strcmp(ih->format, "P5") == 0){
		image_data = (char *)malloc(ih->rows * ih->cols + sizeof(struct image_header));
	}else{ //define the image_data size
		image_data = (char *)malloc(3 * ih->rows * ih->cols + sizeof(struct image_header));
	}


	if (strcmp(ih->format, "P5") == 0){
		for (j = 0; j < ih->rows; ++j)
			for (i = 0; i < ih->cols; ++i) {
				pixel = (char)fgetc(pgmFile);  // fgetc is get char by char
				image_data[sizeof(struct image_header) + j*ih->cols + i] = pixel;
			}
	}else{
		for (j = 0; j < ih->rows; ++j)
			for (i = 0; i < ih->cols; ++i) {
				pix = &image_data[sizeof(struct image_header) + j*(ih->cols * 3) + i * 3];   //pointer    follow the address
				pix[0] = (char)fgetc(pgmFile);	//R
				pix[1] = (char)fgetc(pgmFile);	//G
				pix[2] = (char)fgetc(pgmFile);	//B

			}
	}


	if (strcmp(ih->format, "P5") == 0){
		*length = sizeof(struct image_header) + ih->rows * ih->cols;
	}else{
		*length = sizeof(struct image_header) + 3 * ih->rows * ih->cols;
	}

	d_Sampling_img(image_data, ih->rows, ih->cols);
//	u_Sampling_img(file_name, &ih);


	fclose(pgmFile);
	return image_data;
}
//---------------------------------------------------------------------------------------------
//reverse the readImage
void writeImage(const char *file_name, const char *image_data, const struct image_header ih){
	FILE *pgmFile;
	int i, j;
	const char *pix;

	pgmFile = fopen(file_name, "wb");
	if (pgmFile == NULL) {
		perror("cannot open file to write");
		exit(EXIT_FAILURE);
	}

	fprintf(pgmFile, "%s ", ih.format);
	fprintf(pgmFile, "%d %d ", ih.cols, ih.rows);
	fprintf(pgmFile, "%d ", ih.levels);
	printf("writeimage rows : %d, cols : %d\n",ih.rows,ih.cols);

	u_Sampling_img(image_data, ih.rows,ih.cols);

	if(strcmp(ih.format, "P5") == 0){
		for (j = 0; j < ih.rows; ++j)
			for (i = 0; i < ih.cols; ++i) {
				pix = &image_data[sizeof(struct image_header) + j*ih.cols + i];
				fputc(pix[0], pgmFile);
			}
	}else{
		for (j = 0; j < ih.rows; ++j)
			for (i = 0; i < ih.cols; ++i) {
				pix = &image_data[sizeof(struct image_header) + j * ih.cols * 3 + i * 3];
				fputc(pix[0], pgmFile);
				fputc(pix[1], pgmFile);
				fputc(pix[2], pgmFile);
			}
	}

	fclose(pgmFile);
}
