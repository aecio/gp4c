#include "url.h"

const URL::Sum URL::kSum;

URL::Sum::Sum() {
    for (int w = 1; w <= MAX_SUM; ++w) {
        sum_aad[w] = 0;
        for (int i = 1; i <= w; ++i) {
            sum_aad[w] += i;
//                    cout << "aad:" << i << " " << sum_aad[w] << endl;
        }
        sum_gad[w] = 0;
        for (int i = 1; i <= w+1; ++i) {
            sum_gad[w] += pow(2, i-1);
//                    cout << "gad:" << i << " " << sum_gad[w] << endl;
        }
    }
}
