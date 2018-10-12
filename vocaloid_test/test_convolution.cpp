#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;

void DoConvolution(vector<float> a, int a_len,
                   vector<float> b, int b_len,
                   vector<float> &output){
    auto output_len = a_len + b_len - 1;
    for (int i = 0; i < output_len; i++) {
        output[i] = 0;
        int j_min = (i >= b_len - 1) ? i - (b_len - 1) : 0;
        int j_max = (i < a_len - 1) ? i : a_len - 1;
        for (int j = j_min; j <= j_max; j++) {
            output[i] += a[j] * b[i - j];
        }
    }
}

int main(){
    vector<float> IS = {2, 4, 3, 6};
    vector<float> IR = {1, 5, 2, 3, 4};
    vector<float> output(8);
    fill(output.begin(), output.end(), 0);
    DoConvolution(IS, 4, IR, 5, output);
    cout << output.size() << endl;
    for(auto o : output){
        cout << o << endl;
    }
    return 0;
}