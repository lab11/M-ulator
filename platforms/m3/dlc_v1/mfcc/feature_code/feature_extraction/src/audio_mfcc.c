#include "audio_mfcc.h"

void mfcc_init(mfcc_params_t* mfccparams, global_params_t* gbparams, int nfilters, int ncoeff, float minhz, float maxhz) {
  mfccparams->samprate = gbparams->samprate;
  mfccparams->specsize = (gbparams->fftsize/2)+1;
  mfccparams->nfilters = nfilters;
  mfccparams->ncoeff = ncoeff;
  mfccparams->minhz = minhz;
  mfccparams->maxhz = maxhz;
  mfccparams->coeffs = (float*) malloc(ncoeff*sizeof(float));


  float minmel = HZ2MEL(minhz);
  float maxmel = HZ2MEL(maxhz);
  
  float* filtcents;
  filtcents = (float*) malloc((nfilters+2)*sizeof(float));
  float filtwidth = (maxmel-minmel)/(nfilters+1);
  for (int i=0; i<nfilters+2; i++) {
    float tmp = minmel + (filtwidth*i);
    filtcents[i] = MEL2HZ(tmp);
  }
  int specsize = (gbparams->fftsize/2)+1;
  float* filtvals;
  filtvals = (float*) malloc(specsize*sizeof(float));
  for (int i=0; i<specsize; i++) {
    filtvals[i] = ((float) i)*gbparams->samprate/gbparams->fftsize;
  }
  //////////////////////
  ///caogao
// printf("%i\n", gbparams->samprate);
// printf("test: fixed:%f float:%f", fixed_point_mul(1.35, 3.56, 2, 2, 4, 4), 1.35*3.56);
  //////////////////////
  mfccparams->fbankmatrix = (float*) malloc(specsize*nfilters*sizeof(float));
  for (int i=0; i<nfilters; i++) {
    for (int j=0; j<specsize; j++) {
      if(filtvals[j]>=filtcents[i] && filtvals[j]<filtcents[i+1]) {
	mfccparams->fbankmatrix[(i*specsize)+j] = (filtvals[j]-filtcents[i])/(filtcents[i+1]-filtcents[i]);
      }
      else if(filtvals[j]>=filtcents[i+1] && filtvals[j]<filtcents[i+2]) {
	mfccparams->fbankmatrix[(i*specsize)+j] = (filtvals[j]-filtcents[i+2])/(filtcents[i+1]-filtcents[i+2]);
      }
      else {
	mfccparams->fbankmatrix[(i*specsize)+j] = 0;
      }
    }
  }
  mfccparams->dctmatrix = (float*) malloc (nfilters*ncoeff*sizeof(float));
  for (int i=0; i<ncoeff; i++) {
    for (int j=0; j<nfilters; j++) {
      mfccparams->dctmatrix[(i*nfilters)+j] = cos((i+1)*(PI/nfilters*(j+0.5)));
    }
  }
  free(filtvals);
  free(filtcents);
}

void mfcc_extract(mfcc_params_t* mfccparams, frame_t* frame) {
  int specsize = mfccparams->specsize;
  int ncoeff = mfccparams->ncoeff;
  int nfilters = mfccparams->nfilters;
  float* temp;
  temp = (float*) malloc(nfilters*sizeof(float));
  memset(temp,0,nfilters*sizeof(float));
  memset(mfccparams->coeffs,0,ncoeff*sizeof(float));
  /////////////////////////
  /// caogao
  int mfcc_parameter_precision = 8;
  float* temp_fixed = (float*) calloc(nfilters, sizeof(float));
  float* mfcc_result = (float*) calloc(nfilters, sizeof(float));
  float* log_result = (float*) calloc(nfilters, sizeof(float));
  float* dct_result = (float*) calloc(ncoeff, sizeof(float));
  float* mfcc_fixed_result = (float*) calloc(nfilters, sizeof(float));
  float* log_fixed_result = (float*) calloc(nfilters, sizeof(float));
  float* dct_fixed_result = (float*) calloc(ncoeff, sizeof(float));
//  printf("mfcc_parameter_precision:%i\n", mfcc_parameter_precision);
  /////////////////////////
  for (int i=0; i<nfilters; i++) {
    for (int j=0; j<specsize; j++) {
      temp[i]+= frame->spec[j]*mfccparams->fbankmatrix[(i*specsize)+j];
      /////////////////////
      // caogao
      float input = frame->spec[j];  
      float params = mfccparams->fbankmatrix[(i*specsize) + j];
      temp_fixed[i] += fixed_point_mul(params, input, 0, mfcc_parameter_precision);
      /////////////////////
    }
    /////////////////////////
    /// caogao
    mfcc_result[i] = temp[i];
    mfcc_fixed_result[i] = temp_fixed[i];
//    printf("i: %i, mfcc_result:%f, mfcc_fixed_result%f", i, mfcc_result[i], mfcc_fixed_result[i]);
    /////////////////////////

    if (temp[i] > 0) {
      temp[i] = log(temp[i]);
    }
    else {
      temp[i] = 0;
    }

    /////////////////////////
    /// caogao
    if (temp_fixed[i] > 0) {
      temp_fixed[i] = log(temp_fixed[i]);
    }
    else {
      temp_fixed[i] = 0;
    }
    /// 
    log_result[i] = temp[i];
    log_fixed_result[i] = temp_fixed[i];
//    printf("log_result:%f, log_fixed_result%f", log_result[i], log_fixed_result[i]);
    /////////////////////////
  }
  for (int i=0; i<ncoeff; i++) {
    for (int j=0; j<nfilters; j++) {
      mfccparams->coeffs[i] += temp[j]*mfccparams->dctmatrix[(i*nfilters)+j];
    }
    /////////////////////////
    /// caogao
    for (int j=0; j<nfilters; j++) {
      float input = temp_fixed[j]; 
      float params = mfccparams->dctmatrix[(i*nfilters)+j];
      dct_fixed_result[i] += params * input;
    }
    ///
    dct_result[i] = mfccparams->coeffs[i];
    /////////////////////////
  }
  /////////////////////////
  /// caogao
  printf("## mfcc parameter ##\n");
  for (int i=0; i<nfilters; i++)
    for (int j=0; j<specsize; j++)
      printf("%f\t", mfccparams->fbankmatrix[(i*specsize)+j]);
  printf("\n");

  printf("## mfcc result ##\n");
  for (int i=0; i<nfilters; i++) {
    printf("%f\t", mfcc_result[i]);
  }
  printf("\n");

  printf("## mfcc fixed result ##\n");
  for (int i=0; i<nfilters; i++)
    printf("%f\t", mfcc_fixed_result[i]);
  printf("\n");

  printf("## log result ##\n");
  for (int i=0; i<nfilters; i++)
    printf("%f\t", log_result[i]);
  printf("\n");
  printf("## log fixed result ##\n");
  for (int i=0; i<nfilters; i++)
    printf("%f\t", log_fixed_result[i]);
  printf("\n");

  printf("## dct parameter ##\n");
  for (int i=0; i<ncoeff; i++)
    for (int j=0; j<nfilters; j++)
      printf("%f\t", mfccparams->dctmatrix[(i*nfilters)+j]);
  printf("\n");

  printf("## dct fixed result ##\n");
  for (int i=0; i<ncoeff; i++)
    printf("%f\t", dct_fixed_result[i]);
  printf("\n");
  printf("## dct result ##\n");
  // printed in example.c
  /////////////////////////
}
