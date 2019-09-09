#include "lib.h"

void sortVector (double *v, int n){
    int i, k;
    double vp[n];
    int minIndex=0;

    while (check(v,n) == -1){
        for (i=0; i<n; i++)
        {
            minIndex=i; //you suppose that the minimum is the first element in each loop
            for (k=i+1; k<n; k++)
            {
                if (v[k] < v[minIndex])
                  minIndex=k; // if there is another element lower you preserve its index in the variable
            }
           //after finishing the loop above you have the lowest variable in the array  which has the index stored in minIndex
           vp[i] = v[minIndex]; // put it in vp array
           v[minIndex]=v[i];//put it in treated elements zone
           v[i]=-1;// make it -1
        }
    }

    for (i=0; i<n; i++)
        v[i] = vp[i];
}

int check (double *v, int n){
    int i;
    for (i=0; i<n; i++)
    {
        if (v[i] != -1)
          return -1;
    }
    return 1;
}

double* unique2(double **X, int nl, int *KK){
        int i, k, K=0, a;
        double *vals=calloc(nl, sizeof(double));

        for(i=0; i<nl; i++){
               a=0;
               k=0;
               while(a==0 && k<K){
                    if(X[i][1] == vals[k]) a++;
                    k++;
               }
               if(a == 0) {//if a>1 error!!
                    vals[K] = X[i][1];
                    K++;
               }
        }
        double *vals2=calloc(K, sizeof(double));
        for(i=0; i<K; i++) vals2[i]=vals[i];
        free(vals);
        sortVector(vals2, K);
        *KK=K;
        return vals2;
}

double* unique(double **X, int nrows, int ncols, int *KK){
        int i, j, k, K=0, a, nl=nrows*ncols;
        double *vals=calloc(nl, sizeof(double));

        for(i=0; i<nrows; i++)
               for(j=0; j<ncols; j++){
               if(X[i][j]>=0){
                    a=0;
                    k=0;
                    while(a==0 && k<K){
                         if(X[i][j] == vals[k]) a++;
                         k++;
                    }
                    if(a == 0) {//if a>1 error!!
                         vals[K] = X[i][j];
                         K++;
                    }
               }
        }
        double *vals2=calloc(K, sizeof(double));
        for(i=0; i<K; i++) vals2[i]=vals[i];
        free(vals);
        sortVector(vals2, K);
        *KK=K;
        return vals2;
}

double** hierarchy_area(double **area, int nrows, int ncols, int *nl){
     int i, j, k;
     double value, index;

     double **new_area=(double**)calloc(nrows*ncols, sizeof(double*));
     for(i=0; i<nrows*ncols; i++) new_area[i]=(double*)calloc(2, sizeof(double));

     k=0;
     for(i=0; i<nrows; i++){
          for(j=0; j<ncols; j++)
               if(area[i][j]>=0){
                    index = i+j*nrows;
                    value = area[i][j];
                    new_area[k][0]=index;
                    new_area[k][1]=value;
                    k++;
               }
     }

     double **new_new_area=(double**)calloc(k, sizeof(double*));
     for(i=0; i<k; i++) {new_new_area[i]=(double*)calloc(2, sizeof(double));
                         new_new_area[i][0]=new_area[i][0];
                         new_new_area[i][1]=new_area[i][1];
                         free(new_area[i]);}
     *nl=k;
     return new_new_area;
}
