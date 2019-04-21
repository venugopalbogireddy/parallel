#include <bits/stdc++.h>
#include <bitset>
#include <string>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#define _ ios_base::sync_with_stdio(false);cin.tie(0);
using namespace std;

void par_prefix_sum(int* b, int n) {
	for (int i=1; i<n; i++) {
		b[i] = b[i]+b[i-1];
	}
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
	par_prefix_sum(lt,n);
	par_prefix_sum(gt,n);
	int k=q+lt[n-1];
	a[k]=x;
	cilk_for(int i=0; i<n; i++) {
		if (b[i]<x)
			a[q+lt[i]-1]=b[i];
		else if (b[i] > x)
			a[k+gt[i]] = b[i];
	}
	return k;
} 

void par_randomized_quicksort(int* a, int q, int r) {
	int n=r-q+1;
	//cout<<"Quicksort "<<q<<" "<<r<<" "<<n<<endl;
	if (n<=32)
		sort(a+q, a+r+1);
	else {
		// select a random number
		int x = (rand() % n) + q;
		//cout<<"Random Index "<<x<<"~"<<a[x]<<endl;
		int k = par_partition(a, q, r, a[x]);
		//cout<<"Random Index position "<<x<<"~"<<k<<endl;
		cilk_spawn par_randomized_quicksort(a, q, k-1);
		par_randomized_quicksort(a, k+1, r);
		cilk_sync;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2)
	{
		cout<<"Usage: /quicksort-par input-array-size #workers"<<endl;
	}
        //__cilkrts_set_param("nworkers",argv[2]);
	// initializing random numbers to the array
	int n = atoi(argv[1]);
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
	// starting the time
	srand(time(NULL));
    time_t start = time(NULL);
    //calling function
    //
    cout<<"Calling Quicksort"<<endl;
    par_randomized_quicksort(a, 0, n-1);
    time_t end = time(NULL);
    double time_taken = end-start;

	for(int i=0; i<n; i++) {
		if(a[i]!=i) {
			cout<<"NO-SORT "<<i<<endl;
			break;
		}
	}

    printf("Time Taken for Base size %d is %.6lf\n",n,time_taken);
	/*for (int i=0; i<n; i++)
	{
		cout<<a[i]<<",";
	}
	cout<<endl;*/
}
