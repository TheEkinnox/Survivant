#ifndef __LIBMATH__MATRIX__MATRIX3_H__
#define __LIBMATH__MATRIX__MATRIX3_H__

#include "TMatrix.h"

namespace LibMath
{
    template <typename DataT>
    struct Details::Determinant<3, 3, DataT>
    {
        static DataT compute(const TMatrix<3, 3, DataT>& mat);
    };

    template <typename DataT>
    struct Details::Inverse<3, 3, DataT>
    {
        static TMatrix<3, 3, DataT> compute(const TMatrix<3, 3, DataT>& mat);
    };

    using Matrix3x2 = TMatrix<3, 2, float>;

    using Matrix3x3 = TMatrix<3, 3, float>;
    using Matrix3 = Matrix3x3;

    using Matrix3x4 = TMatrix<3, 4, float>;
}

#include "Matrix3.inl"

#endif // !__LIBMATH__MATRIX__MATRIX3_H__
