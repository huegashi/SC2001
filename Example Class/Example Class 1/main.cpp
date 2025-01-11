#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <string>
#include <sstream>
#include <omp.h>

using namespace std;

int mergearr(vector<int>& arr, int left, int middle, int right) {
    int n1 = middle - left + 1;
    int n2 = right - middle;

    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[middle + 1 + j];

    int i = 0, j = 0, k = left, comparisons = 0;

    while (i < n1 && j < n2) {
        comparisons++;
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }

    return comparisons;
}

int mergeSort(vector<int>& arr, int left, int right) {
    int comparisons = 0;
    if (left < right) {
        int middle = left + (right - left) / 2;

        #pragma omp parallel sections
        {
            #pragma omp section
            {
                comparisons += mergeSort(arr, left, middle);
            }
            #pragma omp section
            {
                comparisons += mergeSort(arr, middle + 1, right);
            }
        }

        comparisons += mergearr(arr, left, middle, right);
    }

    return comparisons;
}

int insertionSort(vector<int>& arr, int left, int right) {
    int comparisons = 0;
    for (int i = left; i <= right; ++i) {
        int key = arr[i];
        int j = i - 1;

        comparisons++;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
            comparisons++;
        }
        arr[j + 1] = key;
    }
    return comparisons;
}

int insertmergeSort(vector<int>& arr, int left, int right, int S) {
    int comparisons = 0;

    if (left < right) {
        if (right - left <= S) {
            comparisons += insertionSort(arr, left, right);
        } else {
            int middle = left + (right - left) / 2;

            #pragma omp parallel sections
            {
                #pragma omp section
                {
                    comparisons += insertmergeSort(arr, left, middle, S);
                }
                #pragma omp section
                {
                    comparisons += insertmergeSort(arr, middle + 1, right, S);
                }
            }

            comparisons += mergearr(arr, left, middle, right);
        }
    }

    return comparisons;
}

vector<int> generateRandomArrays(int s, int maxElement) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(1, maxElement);

    vector<int> data(s);
    for (int j = 0; j < s; j++) {
        data[j] = distrib(gen);
    }
    return data;
}

void evaluatei(int minSize, int maxSize, int maxElement, string filename, int input_size, int S) {
    cout << "parti\n";
    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error opening file: " << endl;
    }
    csvFile << "ArraySize,Average Comparisons,Performance (ns)\n";

    for (int i = minSize; i <= maxSize; i *= 10) {
        int total_comparisons = 0;
        long long total_time = 0;
        cout << "evaluating " << i << "\n";

        #pragma omp parallel for reduction(+:total_comparisons, total_time)
        for (int j = 0; j < input_size; j++) {
            vector<int> data = generateRandomArrays(i, maxElement);

            auto start = chrono::high_resolution_clock::now();  // Start time
            total_comparisons += insertmergeSort(data, 0, i - 1, S);
            auto end = chrono::high_resolution_clock::now();    // End time

            total_time += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }

        csvFile << i << "," << total_comparisons / input_size << "," << total_time / input_size << "\n";
    }
    csvFile.close();
}

void evaluateii(int minS, int maxS, int maxElement, string filename, int input_size, int array_size) {
    cout << "partii\n";
    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error opening file: " << endl;
    }
    csvFile << "S,Average Comparisons,Performance (ns)\n";

    for (int i = minS; i <= maxS; i++) {
        int total_comparisons = 0;
        long long total_time = 0;
        cout << "evaluating " << i << "\n";

        #pragma omp parallel for reduction(+:total_comparisons, total_time)
        for (int j = 0; j < input_size; j++) {
            vector<int> data = generateRandomArrays(array_size, maxElement);

            auto start = chrono::high_resolution_clock::now();  // Start time
            total_comparisons += insertmergeSort(data, 0, array_size - 1, i);
            auto end = chrono::high_resolution_clock::now();    // End time

            total_time += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
        }

        csvFile << i << "," << total_comparisons / input_size << "," << total_time / input_size << "\n";
    }
    csvFile.close();
}

void evaluateiii(int minSize, int maxSize, int minS, int maxS, int maxElement, string filename, int input_size) {
    cout << "partiii\n";
    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error opening file: " << endl;
    }
    csvFile << "ArraySize,S,Average Comparisons,Performance (ns)\n";

    for (int i = minSize; i <= maxSize; i *= 10) {
        for (int k = minS; k <= maxS; k++) {
            int total_comparisons = 0;
            long long total_time = 0;
            cout << "evaluating size " << i << " S " << k << "\n";

            #pragma omp parallel for reduction(+:total_comparisons, total_time)
            for (int j = 0; j < input_size; j++) {
                vector<int> data = generateRandomArrays(i, maxElement);

                auto start = chrono::high_resolution_clock::now();  // Start time
                total_comparisons += insertmergeSort(data, 0, i - 1, k);
                auto end = chrono::high_resolution_clock::now();    // End time

                total_time += chrono::duration_cast<chrono::nanoseconds>(end - start).count();
            }

            csvFile << i << "," << k << "," << total_comparisons / input_size << "," << total_time / input_size << "\n";
        }
    }
    csvFile.close();
}

void evaluate_d(int arrsize, int maxElement, string filename, int input_size, int S) {
    cout << "partd\n";
    ofstream csvFile(filename);
    if (!csvFile.is_open()) {
        cerr << "Error opening file: " << endl;
    }
    csvFile << "Type,ArraySize,Average Comparisons,Performance (ns)\n";

    int total_comparisons_ms = 0, total_comparisons_ims = 0;
    long long duration_ms = 0, duration_ims = 0;

    #pragma omp parallel for reduction(+:total_comparisons_ms, total_comparisons_ims, duration_ms, duration_ims)
    for (int i = 0; i < input_size; i++) {
        vector<int> data = generateRandomArrays(arrsize, maxElement);

        // Insert MergeSort
        auto start_ims = chrono::high_resolution_clock::now(); // Start time for InsertMergeSort
        total_comparisons_ims += insertmergeSort(data, 0, arrsize - 1, S);
        auto end_ims = chrono::high_resolution_clock::now();   // End time for InsertMergeSort
        duration_ims += chrono::duration_cast<chrono::nanoseconds>(end_ims - start_ims).count();

        // Regular MergeSort
        vector<int> data_copy = data; // Reset the data
        auto start_ms = chrono::high_resolution_clock::now();  // Start time for MergeSort
        total_comparisons_ms += mergeSort(data_copy, 0, arrsize - 1);
        auto end_ms = chrono::high_resolution_clock::now();    // End time for MergeSort
        duration_ms += chrono::duration_cast<chrono::nanoseconds>(end_ms - start_ms).count();
    }

    csvFile << "MergeSort" << "," << arrsize << "," << total_comparisons_ms / input_size << "," << duration_ms / input_size << "\n";
    csvFile << "Insert MergeSort" << "," << arrsize << "," << total_comparisons_ims / input_size << "," << duration_ims / input_size << "\n";

    csvFile.close();
}

int main() {
    int minSize = 1000;
    int maxSize = 10000000;
    int maxElement = 1000000;

    // Test using 2 types of mergesort on fixed S and fixed array size
    evaluate_d(1000000, maxElement, "partd.csv", 3, 28);  // Example for array size 1 million

    // Evaluate with fixed S and varying array size
    evaluatei(1000, 10000000, maxElement, "partci.csv", 5, 10);

    // Evaluate with fixed array size and varying S
    evaluateii(1, 100, maxElement, "partcii.csv", 3, 1000000);

    // Evaluate with both S and array size varying
    evaluateiii(1000, 10000000, 1, 100, maxElement, "partciii.csv", 1);

    return 0;
}
