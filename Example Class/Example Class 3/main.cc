#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

vector<vector<int>> knapsack(int W, vector<int>& weights, vector<int>& values, int n) {
    vector<vector<int>> dp(n + 1, vector<int>(W + 1, 0));

    for (int i = 1; i <= n; i++) {
        for (int w = 1; w <= W; w++) {
            if (weights[i - 1] <= w) {
                dp[i][w] = max(dp[i - 1][w], dp[i - 1][w - weights[i - 1]] + values[i - 1]);
            } else {
                dp[i][w] = dp[i - 1][w];
            }
        }
    }

    return dp;
}
void printmatrix(vector<vector<int>> dp, int n,int W){

    for (int i = 0; i < n+1; i++) {
        cout<< "["<<i<<"]";
        for (int j = 0; j < W+1; j++) {
            cout << setw(3) << dp[i][j]; // Set width to 3 for consistent spacing
        }
        cout << endl;
    }

}
int main() {

    vector<int> weights_1 = {4,6,8};
    vector<int> values_1 = {7,6,9};

    vector<int> weights_2 = {5,6,8};
    vector<int> values_2 = {7,6,9};
    int n = 3;

    vector<vector<int>> maxdp1 = knapsack(14, weights_1, values_1, n);
    printmatrix(maxdp1,n,14);


    vector<vector<int>> maxdp2 = knapsack(14, weights_2, values_2, n);
    cout<<endl;
    printmatrix(maxdp2,n,14);

    return 0;
}
