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
#define N (1<<13)

typedef unsigned long long int uint64;
typedef long long int int64;

int a[N];

int* par_prefix_sum(int a[]) {
	for (int i=1; i<a.size(); i++) {
		a[i] = a[i]+a[i-1];
	}
	return a;
}

int par_partition(int q, int r, int x) {
	int n = r-q+1;
	if (n==1)
		return q;
	int b[n], lt[n], gt[n];
	cilk_for(int i=0; i<n; i++) {
		b[i] = a[q+i];
		lt[i] = b[i]<x ? 1:0;
		gt[i] = b[i]>x ? 1:0;
	}
	lt = par_prefix_sum(lt);
	gt = par_prefix_sum(gt);
	int k=q+lt[n-1];
	a[k]=x;
	cilk_for(int i=0; i<n; i++) {
		if (b[i]<x)
			a[q+lt[i]-1]=b[i];
		else if (b[i] > x)
			a[q+gt[i]] = b[i];
	}
	return k;
}

void swap(int* a, int* b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
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
            swap(&a[i], &a[j]); 
        } 
    } 
    swap(&a[i + 1], &a[high]); 
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
	if (n<=32)
		quicksort(q, r);
	else {
		// select a random number
		int x = rand() % n + q;
		int k = par_partition(q, r, x);
		cilk_spawn par_randomized_quicksort(q, k-1);
		par_randomized_quicksort(k, r);
		cilk_sync;
	}
}

int main(int argc, char* argv[]) {
	if (argc != 2)
	{
		cout<<"Usage: /quicksort-par N"<<endl;
	}

	// initializing random numbers to the array
	int n = atoi(argv[1]);
	for (int i = 0; i < n; ++i)
	{
		a[i] = rand();
	}

	// starting the time
	srand(time(NULL));
    time_t start = time(NULL);
    //calling function
    par_randomized_quicksort(0, n-1);
    time_t end = time(NULL);
    double time_taken = end-start;

    printf("Time Taken for Base size %d is %.6lf\n",n,time_taken);
}