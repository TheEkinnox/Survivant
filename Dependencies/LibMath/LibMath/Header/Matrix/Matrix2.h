#ifndef __LIBMATH__MATRIX__MATRIX2_H__
#define __LIBMATH__MATRIX__MATRIX2_H__

#include "TMatrix.h"

namespace LibMath
{
    template <typename DataT>
    struct Details::Determinant<2, 2, DataT>
    {
        static DataT compute(const TMatrix<2, 2, DataT>& mat);
    };

    template <typename DataT>
    struct Details::Inverse<2, 2, DataT>
    {
        static TMatrix<2, 2, DataT> compute(const TMatrix<2, 2, DataT>& mat);
    };

    using Matrix2x2 = TMatrix<2, 2, float>;
    using Matrix2 = Matrix2x2;

    using Matrix2x3 = TMatrix<2, 3, float>;

    using Matrix2x4 = TMatrix<2, 4, float>;
}

#include "Matrix2.inl"

#endif // !__LIBMATH__MATRIX__MATRIX2_H__
