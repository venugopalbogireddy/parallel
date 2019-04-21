#include<bits/stdc++.h>
#include <time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>


using namespace std;

int* par_prefix_sum(int* array, int n){
	int* sum = new int[n];
	if(n==1){
		sum[0] = array[0];
	}
	else{
		int* left = new int[n/2];
		int* right = new int[n/2];
		cilk_for(int i = 0 ; i < n/2; i++ )
				left[i] = array[2*i] + array[2*i+1];
		right = par_prefix_sum(left,n/2);
		sum[0] = array[0];
		cilk_for( int i = 1 ; i < n ; i++ ){
			if(i%2)
				sum[i] = right[i/2];
			else
				sum[i] = right[(i-1)/2]+array[i];
		}
	}
	return sum;
}

void par_counting( int s[], int n, int d, int r[]){
	int p = 64;
	int f[1<<d][p];
	int r1[1<<d][p];
	int js[p];
	int je[p];
	int ofs[p];
	cilk_for( int i = 0 ; i <p ; i++ ){
		for( int j = 0 ; j < (1<<d); j++ )
			f[j][i] = 0;
		js[i] = i*ceil(n/p);
		je[i] = (i+1<p)?((i+1)*ceil(n/p))-1:n-1;
		for( int j = js[i]; j <= je[i]; j++ )
			f[s[j]][i] = f[s[j]][i]+1;
		}
		for( int j = 0 ; j < (1<<d); j++ ){
			int* prefix_sum = par_prefix_sum(f[j],p);
			cilk_for( int i = 0 ; i < p ; i++ )
				f[j][i] = prefix_sum[i];
		}
		cilk_for( int i = 0 ; i <p; i++ ){
			ofs[i]=1;
			for( int j = 0 ; j < (1<<d); j++ ){
				r1[j][i] = (i==0)?ofs[i]:ofs[i]+f[j][i-1];
				ofs[i] = ofs[i] + f[j][p-1];
			}
			for( int j = js[i]; j<= je[i]; j++ ){
				r[j] = r1[s[j]][i];
				r1[s[j]][i] = r1[s[j]][i]+1;
			}
		}
}

int extract_bit( int a, int k, int l){
	return (((1 << l) - 1) & (a >> k));	
}

void par_radix_sort(int a[], int n, int b){
	int* s = new int[n];
	int* r = new int[n];
	int* B = new int[n];
	int p = 64;
	int d = ceil(log(ceil(n/(p*log(n)))));
	for( int k = 0 ; k < b; k = k+d ){
		int q = (k+d<=b)?d:(b-k);
		cilk_for( int i = 0 ; i <n; i++ )
			s[i] = extract_bit(a[i],k,q);
		par_counting(s,n,q,r);
		cilk_for( int i = 0 ; i <n ; i++ )
			B[r[i]-1]=a[i];
		cilk_for( int i = 0 ; i <n ; i++ )
			a[i] = B[i];
	}	
}

void radix_sort(int n){
	//__cilkrts_set_param("nworkers","10");
	//int worker = __cilkrts_get_nworkers();
	//cout<<"Computing with N worker "<<worker<<endl;
	n = (1<<n);
	int b = ceil(log(n));
	int *array = new int[n];
	int *serial_sort = new int[n];
	for( int i = 0 ; i < n ; i++ ){
		array[i] = rand()%100;
		serial_sort[i] = array[i];
	}
	struct timespec start, end;
	//clock_t start = clock();
	clock_gettime(CLOCK_MONOTONIC, &start);
	par_radix_sort(array,n,b);
	//clock_t end = clock();
	clock_gettime(CLOCK_MONOTONIC, &end);
	sort(serial_sort,serial_sort+n);
	//for( int i = 0 ; i < n ;i++ ){
	//	if(array[i]!=serial_sort[i])
	//		cout<<"Mismatch at index "<<i<<" expected "<<serial_sort[i]<<" found "<<array[i]<<endl;
	//}
	double time_taken = (end.tv_sec - start.tv_sec);
	printf("Time Taken %.6lf\n",time_taken);
}

int main(int argc, char* argv[]){
	if(argc>2) __cilkrts_set_param("nworkers",argv[2]);
	int n=atoi(argv[1]);
	radix_sort(n);
	return 0;
}
