/*
 *   Copyright (c) 2009, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef FLENS_BLAS_LEVEL1_COPY_TCC
#define FLENS_BLAS_LEVEL1_COPY_TCC 1

#include <flens/blas/blas.h>
#include <flens/typedefs.h>

#ifdef FLENS_DEBUG_CLOSURES
#   include <flens/blas/blaslogon.h>
#else
#   include <flens/blas/blaslogoff.h>
#endif


namespace flens { namespace blas {

//-- copy
template <typename VX, typename VY>
void
copy(const DenseVector<VX> &x, DenseVector<VY> &y)
{
    FLENS_BLASLOG_SETTAG("--> ");
    FLENS_BLASLOG_BEGIN_COPY(x, y);

//
//  Resize left hand size if needed.  This is *usually* only alloweded
//  when the left hand side is an empty vector (such that it is no actual
//  resizing but rather an initialization).
//
    if (y.length()!=x.length()) {
#       ifndef FLENS_DEBUG_CLOSURES
        ASSERT(y.length()==0);
#       else
        if (y.length()!=0) {
            FLENS_BLASLOG_RESIZE_VECTOR(y, x.length());
        }
#       endif
        y.resize(x);
    }
    y.changeIndexBase(x.firstIndex());

#   ifdef HAVE_CXXBLAS_COPY
    cxxblas::copy(x.length(), x.data(), x.stride(), y.data(), y.stride());
#   else
    ASSERT(0);
#   endif

    FLENS_BLASLOG_END;
    FLENS_BLASLOG_UNSETTAG;
}

//-- gecopy
template <typename MA, typename MB>
void
copy(Transpose trans, const GeMatrix<MA> &A, GeMatrix<MB> &B)
{
//
//  check if this is an inplace transpose of A
//
    if (trans==Trans || trans==ConjTrans) {
        if (DEBUGCLOSURE::identical(A, B)) {
#           ifndef FLENS_DEBUG_CLOSURES
//
//          temporaries are not allowed
//
            ASSERT(A.numRows()==A.numCols());
            gecotr(MB::order, trans, B.numRows(), B.numCols(),
                   B.data(), B.leadingDimension());
#           else
//
//          temporaries are allowed: check if this requires a temporary
//
            if (A.numRows()!=A.numCols()) {
                typename Result<GeMatrix<MA> >::Type _A = A;
                FLENS_BLASLOG_TMP_ADD(_A);

                copy(trans, _A, B);

                FLENS_BLASLOG_TMP_REMOVE(_A, A);
                return;
            } else {
//
//              otherwise perform inplace transpose
//
                gecotr(MB::order, trans, B.numRows(), B.numCols(),
                       B.data(), B.leadingDimension());
                return;
            }
#           endif
        }
    }

//
//  Resize left hand size if needed.  This is *usually* only alloweded
//  when the left hand side is an empty matrix (such that it is no actual
//  resizing but rather an initialization).
//
    if (trans==NoTrans) {
        if ((A.numRows()!=B.numRows()) || (A.numCols()!=B.numCols())) {
#           ifndef FLENS_DEBUG_CLOSURES
            ASSERT(B.numRows()==0 || B.numCols()==0);
#           else
            if (B.numRows()!=0 || B.numCols()!=0) {
                FLENS_BLASLOG_RESIZE_MATRIX(B, A.numRows(), A.numCols());
            }
#           endif
            B.resize(A);
        }
    } else {
        if ((A.numRows()!=B.numCols())  || (A.numCols()!=B.numRows())) {
#           ifndef FLENS_DEBUG_CLOSURES
            ASSERT(B.numRows()==0 && A.numCols()==0);
#           else
            if (B.numRows()!=0 || B.numCols()!=0) {
                FLENS_BLASLOG_RESIZE_MATRIX(B, A.numCols(), A.numRows());
            }
#           endif
            B.resize(A.numCols(), A.numRows(),
                     A.firstCol(), A.firstRow());
        }
    }

    trans = (MA::order==MB::order)
          ? Transpose(trans ^ NoTrans)
          : Transpose(trans ^ Trans);

    FLENS_BLASLOG_SETTAG("--> ");
    FLENS_BLASLOG_BEGIN_MCOPY(trans, A, B);

#   ifdef HAVE_CXXBLAS_GECOPY
    cxxblas::gecopy(MB::order, trans,
                    B.numRows(), B.numCols(),
                    A.data(), A.leadingDimension(),
                    B.data(), B.leadingDimension());
#   else
    ASSERT(0);
#   endif

    FLENS_BLASLOG_END;
    FLENS_BLASLOG_UNSETTAG;
}

//-- trcopy
template <typename MA, typename MB>
void
copy(Transpose trans, const TrMatrix<MA> &A, TrMatrix<MB> &B)
{
//
//  Resize left hand size if needed.  This is *usually* only alloweded
//  when the left hand side is an empty matrix (such that it is no actual
//  resizing but rather an initialization).
//
    if (trans==NoTrans) {
        if ((A.numRows()!=B.numRows()) || (A.numCols()!=B.numCols())) {
#           ifndef FLENS_DEBUG_CLOSURES
            ASSERT(B.numRows()==0 || B.numCols()==0);
#           else
            if (B.numRows()!=0 && B.numCols()!=0) {
                FLENS_BLASLOG_RESIZE_MATRIX(B, A.numRows(), A.numCols());
            }
#           endif
            B.resize(A);
        }
    } else {
        if ((A.numRows()!=B.numCols()) || (A.numCols()!=B.numRows())) {
#           ifndef FLENS_DEBUG_CLOSURES
            ASSERT(B.numRows()==0 || B.numCols()==0);
#           else
            if (B.numRows()!=0 && B.numCols()!=0) {
                FLENS_BLASLOG_RESIZE_MATRIX(B, A.numCols(), A.numRows());
            }
#           endif
            B.resize(A.numCols(), A.numRows(),
                     A.firstCol(), A.firstRow());
        }
    }

#   ifndef NDEBUG
    if (trans==NoTrans || trans==Conj) {
        ASSERT(A.upLo()==B.upLo());
    } else {
        ASSERT(A.upLo()!=B.upLo());
    }
#   endif

    // TODO: make this assertion unnecessary
    ASSERT(A.order()==B.order());
    ASSERT(A.diag()==B.diag());

    FLENS_BLASLOG_SETTAG("--> ");
    FLENS_BLASLOG_BEGIN_MCOPY(trans, A, B);

#   ifdef HAVE_CXXBLAS_TRCOPY
    cxxblas::trcopy(B.order(), B.upLo(), trans, B.diag(),
                    B.numRows(), B.numCols(),
                    A.data(), A.leadingDimension(),
                    B.data(), B.leadingDimension());
#   else
    ASSERT(0);
#   endif

    FLENS_BLASLOG_END;
    FLENS_BLASLOG_UNSETTAG;
}

//-- sycopy
template <typename MA, typename MB>
void
copy(const SyMatrix<MA> &A, SyMatrix<MB> &B)
{
//
//  Resize left hand size if needed.  This is *usually* only alloweded
//  when the left hand side is an empty matrix (such that it is no actual
//  resizing but rather an initialization).
//
    if (B.dim()!=A.dim()) {
#       ifndef FLENS_DEBUG_CLOSURES
        ASSERT(B.dim()==0);
#       else
        if (B.dim()!=0) {
            FLENS_BLASLOG_RESIZE_MATRIX(B, A.dim(), A.dim());
        }
#       endif
        B.resize(A);
        B.upLo() = A.upLo();
    }

    ASSERT(A.upLo()==B.upLo());
    // TODO: make this assertion unnecessary
    ASSERT(A.order()==B.order());

    FLENS_BLASLOG_SETTAG("--> ");
    FLENS_BLASLOG_BEGIN_COPY(A, B);

#   ifdef HAVE_CXXBLAS_TRCOPY
    cxxblas::trcopy(B.order(), B.upLo(), NoTrans, NonUnit, B.dim(), B.dim(),
                    A.data(), A.leadingDimension(),
                    B.data(), B.leadingDimension());
#   else
    ASSERT(0);
#   endif
    FLENS_BLASLOG_END;
    FLENS_BLASLOG_UNSETTAG;
}

//-- extensions ----------------------------------------------------------------

//-- copy: TrMatrix -> GeMatrix
template <typename MA, typename MB>
void
copy(Transpose trans, const TrMatrix<MA> &A, GeMatrix<MB> &B)
{
    typename GeMatrix<MB>::ElementType  Zero(0);

    if (trans==NoTrans) {
        if (A.numRows()!=B.numRows() && A.numCols()!=B.numCols()) {
#           ifndef FLENS_DEBUG_CLOSURES
            ASSERT(B.numRows()==0 || B.numCols()==0);
#           else
            if (B.numRows()!=0 && B.numCols()!=0) {
                FLENS_BLASLOG_RESIZE_MATRIX(B, A.numRows(), A.numCols());
            }
#           endif
            B.resize(A.numRows(), A.numCols(),
                     A.firstRow(), A.firstCol());
        }
    } else {
        if (A.numRows()!=B.numCols() && A.numCols()!=B.numRows()) {
#           ifndef FLENS_DEBUG_CLOSURES
            ASSERT(B.numRows()==0 || B.numCols()==0);
#           else
            if (B.numRows()!=0 && B.numCols()!=0) {
                FLENS_BLASLOG_RESIZE_MATRIX(B, A.numCols(), A.numRows());
            }
#           endif
            B.resize(A.numCols(), A.numRows(),
                     A.firstCol(), A.firstRow());
         }
    }

    if (trans==NoTrans) {
        if (A.upLo()==Upper) {
            B.upper() = A;
            B.strictLower() = Zero;
        } else {
            B.lower() = A;
            B.strictUpper() = Zero;
        }
    } else if (trans==Trans) {
        if (A.upLo()==Upper) {
            B.lower() = transpose(A);
            B.strictUpper() = Zero;
        } else {
            B.upper() = transpose(A);
            B.strictLower() = Zero;
        }
    } else {
        ASSERT(0);
    }
}

//-- copy: SyMatrix -> GeMatrix
template <typename MA, typename MB>
void
copy(const SyMatrix<MA> &A, GeMatrix<MB> &B)
{
    if (A.numRows()!=B.numRows() && A.numCols()!=B.numCols()) {
#       ifndef FLENS_DEBUG_CLOSURES
        ASSERT(B.numRows()==0 || B.numCols()==0);
#       else
        if (B.numRows()!=0 && B.numCols()!=0) {
            FLENS_BLASLOG_RESIZE_MATRIX(B, A.numRows(), A.numCols());
        }
#       endif
        B.resize(A.numRows(), A.numCols(),
                 A.firstRow(), A.firstCol());
    }

    if (A.upLo()==Upper) {
        B.upper() = A.general().upper();
        B.strictLower() = transpose(A.general().strictUpper());
    } else {
        B.lower() = A.general().lower();
        B.strictUpper() = transpose(A.general().strictLower());
    }
}


} } // namespace blas, flens

#endif // FLENS_BLAS_LEVEL1_COPY_TCC
