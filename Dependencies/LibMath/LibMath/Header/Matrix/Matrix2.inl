#ifndef __LIBMATH__MATRIX__MATRIX2_INL__
#define __LIBMATH__MATRIX__MATRIX2_INL__

#include "Matrix2.h"

namespace LibMath::Details
{
    template <typename DataT>
    DataT Determinant<2, 2, DataT>::compute(const TMatrix<2, 2, DataT>& mat)
    {
        // 0 1
        // 2 3
        return mat[0] * mat[3] - mat[1] * mat[2];
    }

    template <typename DataT>
    TMatrix<2, 2, DataT> Inverse<2, 2, DataT>::compute(const TMatrix<2, 2, DataT>& mat)
    {
        DataT detInv = static_cast<DataT>(1) / mat.determinant();

        // 0 1
        // 2 3
        TMatrix<2, 2, DataT> inv;
        inv[0] = mat[3] * detInv;
        inv[1] = -mat[2] * detInv;
        inv[2] = -mat[1] * detInv;
        inv[3] = mat[0] * detInv;

        return inv;
    }
}

#endif // !__LIBMATH__MATRIX__MATRIX2_INL__
