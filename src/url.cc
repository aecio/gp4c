#include "url.h"

const URL::Sum URL::kSum;

URL::Sum::Sum() {
    for (int w_i = 0; w_i <= MAX_SUM; ++w_i) {
        sum_aad[w_i] = 0;
        for (int i = 1; i <= w_i; ++i) {
            sum_aad[w_i] += i;
        }
//        std::cout << "sum_aad[i=" << w_i << "] = " << sum_aad[w_i] << std::endl;
    }
    for (int w_i = 0; w_i <= MAX_SUM; ++w_i) {
        sum_gad[w_i] = 0;
        for (int i = 1; i <= w_i; ++i) {
            sum_gad[w_i] += pow(2, i-1);
        }
//        std::cout << "sum_gad[i=" << w_i << "] = " << sum_gad[w_i] << std::endl;
    }

    for (int i = 0; i <= MAX_SUM; ++i) {
        pow_2_i_minus_1[i] = pow(2, i-1);
//        std::cout << "pow(2,"<<i<<"-1) = " << pow_2i_minus1[i] << std::endl;
    }
}
