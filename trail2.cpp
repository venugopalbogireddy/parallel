#include <bits/stdc++.h>
#include <bitset>
#include <string>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>

#define _ ios_base::sync_with_stdio(false);cin.tie(0);
using namespace std;

#define pb push_back
#define pob pop_back
#define pf push_front
#define pof pop_front
#define mp make_pair
#define all(a) a.begin(),a.end()
#define bitcnt(x) __builtin_popcountll(x)
#define MOD 1000000007
#define PI 3.14159265
#define BLOCK 20000
#define MAXN 500005
#define N (1<<28)

typedef unsigned long long int uint64;
typedef long long int int64;

int a[N];

void par_prefix_sum(int* b, int n) {
	for (int i=1; i<n; i++) {
		b[i] = b[i]+b[i-1];
	}
}

int par_partition(int q, int r, int x) {
	int n = r-q+1;
	if (n==1)
		return q;
	int b[n], lt[n], gt[n];
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

int partition (int low, int high) 
{ 
    int pivot = a[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
  
    for (int j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than or 
        // equal to pivot 
        if (a[j] <= pivot) 
        { 
            i++;    // increment index of smaller element 
            int t = a[i];
	    a[i] = a[j];
	    a[j] = t; 
        } 
    }
    int t=a[i+1];
    a[i+1]=a[high];
    a[high]=t; 
    return (i + 1); 
}
/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(low, pi - 1); 
        quickSort(pi + 1, high); 
    } 
} 

void par_randomized_quicksort(int q, int r) {
	int n=r-q+1;
	cout<<"Quicksort "<<q<<" "<<r<<" "<<n<<endl;
	if (n<=32)
		quickSort(q, r);
	else {
		// select a random number
		int x = rand() % n + q;
		cout<<"Random Index "<<x<<"~"<<a[x]<<endl;
		int k = par_partition(q, r, a[x]);
		cout<<"Random Index position "<<x<<"~"<<k<<endl;
		cilk_spawn par_randomized_quicksort(q, k-1);
		par_randomized_quicksort(k+1, r);
		cilk_sync;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2)
	{
		cout<<"Usage: /quicksort-par N"<<endl;
	}
        __cilkrts_set_param("nworkers",argv[2]);
	// initializing random numbers to the array
	int n = atoi(argv[1]);
        cout<<"Array size "<<N<<" input "<<n<<endl;
	for (int i = 0; i < n; ++i)
	{
		a[i] = rand() % 5000000;
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
    par_randomized_quicksort(0, n-1);
    time_t end = time(NULL);
    double time_taken = end-start;

    printf("Time Taken for Base size %d is %.6lf\n",n,time_taken);
	/*for (int i=0; i<n; i++)
	{
		cout<<a[i]<<",";
	}
	cout<<endl;*/
}
