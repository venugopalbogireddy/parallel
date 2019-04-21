#include <bits/stdc++.h>
#include <bitset>
#include <string>
#include <time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#define _ ios_base::sync_with_stdio(false);cin.tie(0);
using namespace std;

int thr=32;
int* par_prefix_sum(int* b, int n) {
	int* s=new int[n];
	if (n==1) {
		s[0]=b[0];
	} else {
		int *z, *y=new int[n/2];
		cilk_for(int i=0; i<n/2; i++){
			y[i] = b[2*i]+b[2*i+1];
		}
		z = par_prefix_sum(y, n/2);
		s[0] = b[0];
		cilk_for(int i=1; i<n; i++){
			if(i%2 != 0){
				s[i]=z[i/2];
			} else {
				s[i]=z[(i-1)/2]+b[i];
			}
		}
	}
	return s;
}

int par_partition(int* a, int q, int r, int x) {
	int n = r-q+1;
	if (n==1)
		return q;
	int *b = new int[n];
	int *lt = new int[n];
	int *gt = new int[n];
	cilk_for(int i=0; i<n; i++) {
		b[i] = a[q+i];
		if(b[i]<x) lt[i]=1;
		else lt[i]=0;
		if(b[i]>x) gt[i]=1;
		else gt[i]=0;
	}
	int *lt_p = par_prefix_sum(lt,n);
	int *gt_p = par_prefix_sum(gt,n);
	int k=q+lt_p[n-1];
	a[k]=x;
	cilk_for(int i=0; i<n; i++) {
		if (b[i]<x)
			a[q+lt_p[i]-1]=b[i];
		else if (b[i] > x)
			a[k+gt_p[i]] = b[i];
	}
	return k;
}
 

void par_randomized_quicksort(int* a, int q, int r) {
	int n=r-q+1;
	//cout<<"Quicksort "<<q<<" "<<r<<" "<<n<<endl;
	if (n<=thr)
		sort(a+q, a+r+1);
	else {
		// select a random number
		int x = q+(rand() % n);
		//cout<<"Random Index "<<x<<"~"<<a[x]<<endl;
		int k = par_partition(a, q, r, a[x]);
		//cout<<"Random Index position "<<x<<"~"<<k<<endl;
		cilk_spawn par_randomized_quicksort(a, q, k-1);
		par_randomized_quicksort(a, k+1, r);
		cilk_sync;
	}
}

int main(int argc, char* argv[]) {
	if (argc < 2)
	{
		cout<<"Usage: /quicksort-par input-array-size threshold"<<endl;
	}
	if(argc>=3){
		thr=atoi(argv[2]);
		thr = (1<<thr);
	}
	//int w=atoi(argv[3]);
        if(argc>3) __cilkrts_set_param("nworkers",argv[3]);
	// initializing random numbers to the array
	int n = atoi(argv[1]);
	n = (1<<n);
	int *a = new int[n];
        cout<<"Array size "<<"input "<<n<<endl;
	for (int i = 0; i < n; ++i)
	{
		a[i] = n-1-i;
	}
	cout<<"Array created"<<endl;
	/*for (int i=0; i<n; i++)
	{
		cout<<a[i]<<endl;
	}*/
	//int w = __cilkrts_get_nworkers();
	//cout<<"Workers = "<<w<<endl;
	// starting the time
//	srand(time(NULL));
	struct timespec start, end;
	double elapsed;
    //clock_t start = clock();
    clock_gettime(CLOCK_MONOTONIC, &start);
    //calling function
    //
    cout<<"Calling Quicksort"<<endl;
    par_randomized_quicksort(a, 0, n-1);
    //clock_t end = clock();
    clock_gettime(CLOCK_MONOTONIC, &end);
    //double time_taken = end-start;
    //time_taken/=CLOCKS_PER_SEC;
    elapsed = (end.tv_sec - start.tv_sec);
   
	//for(int i=0; i<n; i++) {
	//	if(a[i]!=i) {
	//		cout<<"NO-SORT "<<i<<endl;
	//		break;
	//	}
	//}

    printf("Time Taken for Base size %d, thr %d is %.6lf\n",n,thr,elapsed);
	/*for (int i=0; i<n; i++)
	{
		cout<<a[i]<<",";
	}
	cout<<endl;*/
}
