#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>


void printMatrix(double *a, int n) {
  printf("***************** Matrix %d x %d *********************\n", n, n);
  int i = 0;
  for (i = 0; i < n * n; ++i) {
    printf("%8.5f ", a[i]);
    if ((i+1) % n == 0)
      printf("\n");
  }
  return;
}

int checkEqual(double *a, double *b, int n) {
  int i, j;
  for (i = 0; i < n*n; i++)
    if (abs(a[i] - b[i]) > 0.00001)
      return -1;
  return 0;
}

void transpose(double *a, int n) {
  int i, j;
  for (i = 0; i < n; ++i) {
    for (j = i + 1; j < n; ++j) {
      double tmp = a[i*n + j];
      a[i*n + j] = a[j*n + i];
      a[j*n + i] = tmp;
    }
  }
  return;
}

double* multiply(double *a, double *b, int p, int q, int r) {
  double *result = (double*)malloc(p*r*sizeof(double));
  int i, j, k;
  for (i = 0; i < p; i++) {
    for (j = 0; j < r; j++) {
      double sum = 0;
      for (k = 0; k < q; k++)
        sum += a[i*q+k] * b[k*r+j];
      result[i*r+j] = sum;
    }
  }
  return result;
}
void assignIdentity(double *a, int n) {
  int r, c;
  for (r = 0; r < n; ++r) {
    for (c = 0; c < n; ++c) {
      if (r == c)
	a[r*n+c] = 1;
      else
	a[r*n+c] = 0;
    }
  }
  return;
}

void givens_rotation(double *b, int n, int r, int eigen, double *paras) {

  double x = b[(r-1)*n+r];
  double y = b[r*n+r];

  double sina = x/sqrt(x*x+y*y);
  double cosa = y/sqrt(x*x+y*y);

  paras[r*2] = sina;
  paras[r*2+1] = cosa;


  int i;

  //Q * B -> B
  for (i = eigen; i < n; i++) {
	double r1i = b[(r-1)*n+i];
	double ri = b[r*n+i];

	b[(r-1)*n+i] = cosa*r1i - sina*ri;
	b[r*n+i] = sina*r1i + cosa*ri;
  }


  return;
}

void adjoints_givens_rotation(double *b, int n, int r, int eigen, double *paras) {
  double sina = paras[r*2];
  double cosa = paras[r*2+1];

  int i;
  for (i = eigen; i < n; i++) {
	double ir1 = b[i*n+r-1];
	double ir = b[i*n+r];
	
	b[i*n+r-1] = cosa*ir1 - sina*ir;
	b[i*n+r] = sina*ir1 + cosa*ir;
  }
  return;
}

void qr_symmetric(double *a, int n, double *b) {

  
  memcpy(b, a, n*n*sizeof(double));

  //store the sina and cosa from step 3 to perform step 4

  int eigen;
  for (eigen = 0; eigen < n-1; eigen++) {
	//some of the paras space will not be used
	double *paras = (double *)malloc(2*n*sizeof(double));


	int round = 0;
	while (fabs(b[eigen*n+eigen+1]) > 0.0001) {
	  int i;
	  //step 2: b shift
	  double miu = b[eigen*n+eigen];
	  for (i = eigen; i < n; i++)
		b[i*n+i] -= miu;


	  //step 3: givens rotation
	  for (i = n-1; i > eigen; --i)
		//memcpy(q, multiply(givens_rotation(b, n, i), q, n, n, n), n*n*sizeof(double));
		givens_rotation(b, n, i, eigen, paras);
  

	  //step 4: adjoints of givens rotation
	  for (i = n-1; i > eigen; --i)
		adjoints_givens_rotation(b, n, i, eigen, paras);

	  
	  //step 5: add shift back
	  for (i = eigen; i < n; i++)
		b[i*n+i] += miu;

	  //printf("Eigen: %d, Round: %d\n", eigen, round);
	  //printf("Check Flag: %f\n", fabs(b[eigen*n+eigen+1]));
	  //printMatrix(b, n);
	  //getchar();
	  round++;
	  
	}
  }
  return;
}

int problem(int n) {
  srand(time(NULL));
  
  int i, j;

  double *a = (double *)malloc(n*n*sizeof(double));
  double *b = (double *)malloc(n*n*sizeof(double));

  for (i = 0; i < n; i++)
	for (j = 0; j < n; j++)
	  a[i*n+j] = 0;
  for (i = 0; i < n; i++)
	a[i*n+i] = (double)rand()/(double)RAND_MAX * 10;
  for (i = 0; i < n-1; i++) {
	a[i*n+i+1] = (double)rand()/(double)RAND_MAX * 10;
	a[(i+1)*n+i] = a[i*n+i+1];
  }
  
  //double a[25] = {5,1,2,0,4,1,4,2,1,3,2,2,5,4,0,0,1,4,1,3,4,3,0,3,4};
  
  qr_symmetric(a, n, b);

  printf("Original Test:\n");
  printMatrix(a, n);
  printf("Final Result:\n");
  printMatrix(b, n);



  return 0;
}

int main() {
  srand(time(NULL));

  problem(7);
  return 0;
}




