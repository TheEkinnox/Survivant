#ifndef __LIBMATH__MATRIX__MATRIX3_INL__
#define __LIBMATH__MATRIX__MATRIX3_INL__

#include "Matrix3.h"

namespace LibMath::Details
{
    template <typename DataT>
    DataT Determinant<3, 3, DataT>::compute(const TMatrix<3, 3, DataT>& mat)
    {
        // 0 1 2
        // 3 4 5
        // 6 7 8
        const DataT positive = mat[0] * mat[4] * mat[8] +
            mat[1] * mat[5] * mat[6] +
            mat[2] * mat[3] * mat[7];

        const DataT negative = mat[2] * mat[4] * mat[6] +
            mat[1] * mat[3] * mat[8] +
            mat[0] * mat[5] * mat[7];

        return positive - negative;
    }

    template <typename DataT>
    TMatrix<3, 3, DataT> Inverse<3, 3, DataT>::compute(const TMatrix<3, 3, DataT>& mat)
    {
        DataT oneOverDeterminant = static_cast<DataT>(1) / mat.determinant();

        // 0 1 2
        // 3 4 5
        // 6 7 8
        TMatrix<3, 3, DataT> inv;
        inv[0] = +(mat[4] * mat[8] - mat[5] * mat[7]) * oneOverDeterminant;
        inv[1] = -(mat[1] * mat[8] - mat[2] * mat[7]) * oneOverDeterminant;
        inv[2] = +(mat[1] * mat[5] - mat[2] * mat[4]) * oneOverDeterminant;
        inv[3] = -(mat[3] * mat[8] - mat[5] * mat[6]) * oneOverDeterminant;
        inv[4] = +(mat[0] * mat[8] - mat[2] * mat[6]) * oneOverDeterminant;
        inv[5] = -(mat[0] * mat[5] - mat[2] * mat[3]) * oneOverDeterminant;
        inv[6] = +(mat[3] * mat[7] - mat[4] * mat[6]) * oneOverDeterminant;
        inv[7] = -(mat[0] * mat[7] - mat[1] * mat[6]) * oneOverDeterminant;
        inv[8] = +(mat[0] * mat[4] - mat[1] * mat[3]) * oneOverDeterminant;

        return inv;
    }
}

#endif // !__LIBMATH__MATRIX__MATRIX3_INL__
