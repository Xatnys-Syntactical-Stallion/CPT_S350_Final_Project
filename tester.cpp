#include <fstream>//for 
#include <iostream>//for cout
#include <random>//for mt19937 and uniform_int_distribution
#include <cstdlib>//for atoi
#include <chrono>//for
#include <cassert>
#include <limits>
//g++ -pedantic-errors -Wall -Weffc++ -Wextra -Wconversion -Wsign-conversion -O0 -o radn.exe tester.cpp
template<typename T> void radixSortn(T a[], int length){
	bool sorted = true;//flag to check if a sort even needs to be performed
	T max = a[0];//maximum value
	for(int i=0;i<length;i++){//check if the array is sorted and find the maximum value
		if(i&&sorted&&a[i-1]>a[i]) sorted = false;
		if(max < a[i]) max = a[i];
	}
	if(sorted) return;//early exit case
	int maxRecursions = 1;
	for(T i = max; i >>= 1; maxRecursions++);//find the number of recursions required (most significant bit that is 1)
	T* oarr = new T[length];//other array for sorting
	T *dest = oarr, *source = a;//destination and source arrays while sorting (they are swapped every round)
	T cbit = 1;//the current bit to inspect
	int numones = 0;
	for(int i=0;i<length;i++) if(cbit&a[i]) numones++;//increment if a one is found
	for(int j = 0; j < maxRecursions; j++){//loop through all bits we need
		int pnumones = 0;
		int k = 0, l = length-numones;//start indices of the 0 subarray and the 1 subarray
		for(int i=0;i<length;i++){
			if((cbit<<1)&source[i]) pnumones++;//increment if a one is found
			if(cbit&source[i]) dest[l++] = source[i];
			else dest[k++] = source[i];
		}
		T* tmp = dest;//I should learn XOR swap (swap source and destination)
		dest = source;
		source = tmp;
		cbit <<= 1;//next bit
		numones = pnumones;
	}
	if(dest == a) for(int i = 0; i < length;i++) a[i] = oarr[i];//swap all values one final time if they are in the wrong spot
	delete[] oarr;//fix memory leaks
}
template <typename T> void msdrSort(T a[], int n){
	if(n <= 1) return;
    T mx = a[0];
    bool sorted = true;
    for(int i = 1; i < n; i++){//check if it is already sorted, find the largest number
        if(a[i] > mx) mx = a[i];
        if(a[i-1] > a[i]) sorted = false;
    }
    if(sorted) return;
    int digits = 0;
    for(T i = mx; i >>= 1; digits++);//find the largest bit
    int* midArr = new int[digits];//contain the end indices for each significance
    for(int i=0;i<digits;i++) midArr[i] = 0;
    T cbit = 1 << digits;//current bit
    int b = 0, e = n;//begining index, end index
	for(int recurs = 0;;){
        int i = b, j = -1;
        for(;i<e-1&&j<e;i++){
            while(!(cbit&a[i])) {
                i++;
                if(i>=e) goto ol;//early exit case 1: all radices are 0
            }
            if(i == e-1) goto ol;//early exit case 2: only 1 radix is 1
            if(j == -1) j = i+1;
            while(cbit&a[j]){
                j++;
                if(j>=e) goto ol;//early exit case 3: all remaining radices are 1
            }
            a[i] ^= a[j];//swap i and j
            a[j] ^= a[i];
            a[i] ^= a[j];
            j++;
        }//last 0 is i-1, j = e
        ol: cbit >>= 1;//go to next bit (ol stands for outer-loop, used as a reference for break statements (goto) from a sub-loop)
        if(b == e) {//jump to a transition step if there is nothing in the sublist (0-over or 1-up)
			cbit = 0;
			for(;recurs!=digits;midArr[recurs++] = e);
		}
        if(!cbit){
            cbit = 1;
            if(!(2&a[b])&&b-e > 1){//0-over (you need to evaluate the 1s sublist at the same significance as the current sublist (only occurs when recurs == digits
                if(!recurs) break;//0-break, I don't remember what it does
                b = e;
                e = midArr[recurs-1];
            }
            else{//1-up, go up in significance until you find an unsorted sublist
                recurs--;
                while(e == midArr[recurs]){
                    cbit <<= 1;
                    if(!(recurs--)) goto bl;//list is sorted
                }
                b = recurs==digits-1?e:midArr[recurs+1];
                e = midArr[recurs++];
            }
        }
        else {//recurse, go to a bit of lesser significances
            midArr[recurs++] = e;
            e = i;
        }
    }
    bl: delete [] midArr;//beginning loop, used as a reference for break statements (goto) from a sub-loop
}
void quicksort(int *A, int len) {//https://rosettacode.org/wiki/Sorting_algorithms/Quicksort#C
  if (len < 2) return;

  int pivot = A[len / 2];

  int i, j;
  for (i = 0, j = len - 1; ; i++, j--) {
    while (A[i] < pivot) i++;
    while (A[j] > pivot) j--;

    if (i >= j) break;

    int temp = A[i];
    A[i]     = A[j];
    A[j]     = temp;
  }

  quicksort(A, i);
  quicksort(A + i, len - i);
}
int main(int argc, char** argv){
	if(argc != 2){
		std::cout << "Use "<< argv[0] << " <number of elements>\n";
		return -1;
	}
	int n = atoi(argv[1]);
	if(n <=1){
		std::cout << "The number of elements must be greater than one";
		return -1;
	}
	int* list = new int[n];
	std::random_device randev{};
	std::seed_seq ss {randev(), randev(), randev(), randev(), randev(), randev(), randev(), randev()};
	std::mt19937 mt {ss};
	std::uniform_int_distribution urand {0, 99};//0 - 100
	using std::chrono::high_resolution_clock;
	auto testSort = [&](void (*sort)(int[], int)) -> void {
		for(int i=0;i<n;i++) list[i] = static_cast<int>(mt())&(std::numeric_limits<int>::max()^(1<<sizeof(int)));
		std::chrono::time_point<high_resolution_clock> start {high_resolution_clock::now()};
		sort(list, n);
		std::chrono::time_point<high_resolution_clock> end {high_resolution_clock::now()};
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << '\n';
		for(int i=1;i<n;i++) assert(list[i] >= list[i-1]&&"List not sorted");
		for(int i=0;i<n;i++) list[i] = urand(mt);
		start = high_resolution_clock::now();
		sort(list, n);
		end = high_resolution_clock::now();
		std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << '\n';
		for(int i=1;i<n;i++) assert(list[i] >= list[i-1]&&"List not sorted");
		
	};
	testSort(radixSortn);
	testSort(msdrSort);
	testSort(quicksort);
	delete [] list;
	return 0;
}
