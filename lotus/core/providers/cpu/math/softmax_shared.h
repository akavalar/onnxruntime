#pragma once

#include "core/common/status.h"

namespace Lotus {
/**
Calculate Softmax using CPU memory.
@param N Number of rows
@param D Number of elements in each row
@param Xdata Source data
@param Ydata Output data
@param scale Storage for scale calculation. Size must be >= N.
@param sum_multiplier Weights for each element. Size must be >= D.
@param logarithmic If true, compute LogSoftmax. If false compute Softmax.
@param rowmax Storage for calculation of maximum in each row. Size must be >= N.
*/
Common::Status SoftmaxCPU(const int64_t N,
                          const int64_t D,
                          const float* Xdata,
                          float* Ydata,
                          float* scale,
                          const float* sum_multiplier,
                          bool logarithmic,
                          float* rowmax);
}  // namespace Lotus