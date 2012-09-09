/*
 *   Copyright (c) 2012, Michael Lehn, Klaus Pototzky
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

#ifndef FLENS_MATRIXTYPES_TRIANGULAR_IMPL_TPMATRIX_TCC
#define FLENS_MATRIXTYPES_TRIANGULAR_IMPL_TPMATRIX_TCC 1

#include <flens/auxiliary/auxiliary.h>
#include <flens/blas/level1/copy.h>
#include <flens/matrixtypes/triangular/impl/tpmatrix.h>
#include <flens/typedefs.h>

namespace flens {

template <typename FS>
TpMatrix<FS>::TpMatrix()
{
}

template <typename FS>
TpMatrix<FS>::TpMatrix(IndexType dim, Diag diag)
    : _engine(dim), _diag(diag)
{
}


template <typename FS>
TpMatrix<FS>::TpMatrix(const Engine &engine, Diag diag)
    : _engine(engine), _diag(diag)
{
}

template <typename FS>
TpMatrix<FS>::TpMatrix(const TpMatrix &rhs)
    : TriangularMatrix<TpMatrix<FS> >(),
      _engine(rhs.engine()), _diag(rhs.diag())
{
}

template <typename FS>
template <typename RHS>
TpMatrix<FS>::TpMatrix(const TpMatrix<RHS> &rhs)
    : _engine(rhs.engine()), _diag(rhs.diag())
{
}

template <typename FS>
template <typename RHS>
TpMatrix<FS>::TpMatrix(TpMatrix<RHS> &rhs)
    : _engine(rhs.engine()), _diag(rhs.diag())
{
}

template <typename FS>
template <typename RHS>
TpMatrix<FS>::TpMatrix(const Matrix<RHS> &rhs)
{
    assign(rhs, *this);
}

// -- operators ----------------------------------------------------------------

template <typename FS>
TpMatrix<FS> &
TpMatrix<FS>::operator=(const TpMatrix &rhs)
{
    if (this!=&rhs) {
        assign(rhs, *this);
    }
    return *this;
}

template <typename FS>
template <typename RHS>
TpMatrix<FS> &
TpMatrix<FS>::operator=(const Matrix<RHS> &rhs)
{
    assign(rhs, *this);
    return *this;
}


template <typename FS>
TpMatrix<FS> &
TpMatrix<FS>::operator=(const ElementType &alpha)
{
    ASSERT(_diag==NonUnit);
    
    if (upLo()==Upper) {
        for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = i; j <= lastIndex(); ++j)
                (*this)(i,j) = alpha;
 
    } else {
      for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = firstIndex(); j <= i; ++j)
                (*this)(i,j) = alpha;
    }
    return *this;
}

template <typename FS>
TpMatrix<FS> &
TpMatrix<FS>::operator+=(const ElementType &alpha)
{
    ASSERT(_diag==NonUnit);
    
    if (upLo()==Upper) {
        for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = i; j <= lastIndex(); ++j)
                (*this)(i,j) += alpha;
 
    } else {
      for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = firstIndex(); j <= i; ++j)
                (*this)(i,j) += alpha;
    }
    return *this;
}

template <typename FS>
TpMatrix<FS> &
TpMatrix<FS>::operator-=(const ElementType &alpha)
{
    ASSERT(_diag==NonUnit);
    
    if (upLo()==Upper) {
        for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = i; j <= lastIndex(); ++j)
                (*this)(i,j) -= alpha;
 
    } else {
      for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = firstIndex(); j <= i; ++j)
                (*this)(i,j) -= alpha;
    }
    return *this;
}

template <typename FS>
TpMatrix<FS> &
TpMatrix<FS>::operator*=(const ElementType &alpha)
{
    ASSERT(_diag==NonUnit);
    
    if (upLo()==Upper) {
        for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = i; j <= lastIndex(); ++j)
                (*this)(i,j) *= alpha;
 
    } else {
      for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = firstIndex(); j <= i; ++j)
                (*this)(i,j) *= alpha;
    }
    return *this;
}

template <typename FS>
TpMatrix<FS> &
TpMatrix<FS>::operator/=(const ElementType &alpha)
{
    ASSERT(_diag==NonUnit);
    
    if (upLo()==Upper) {
        for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = i; j <= lastIndex(); ++j)
                (*this)(i,j) /= alpha;
 
    } else {
      for (IndexType i = firstIndex(); i<= lastIndex(); ++i)
            for(IndexType j = firstIndex(); j <= i; ++j)
                (*this)(i,j) /= alpha;
    }
    return *this;
}

template <typename FS>
const typename TpMatrix<FS>::ElementType &
TpMatrix<FS>::operator()(IndexType row, IndexType col) const
{
#   ifndef NDEBUG
    if (upLo()==Upper) {
        ASSERT(col>=row);
    } else {
        ASSERT(col<=row);
    }
    ASSERT(!((_diag==Unit) && (col==row)));
#   endif
    return _engine(row, col);
}

template <typename FS>
typename TpMatrix<FS>::ElementType &
TpMatrix<FS>::operator()(IndexType row, IndexType col)
{
#   ifndef NDEBUG
    if (upLo()==Upper) {
        ASSERT(col>=row);
    } else {
        ASSERT(col<=row);
    }
    ASSERT(!((_diag==Unit) && (col==row)));
#   endif
    return _engine(row, col);
}











// -- views ------------------------------------------------------------

// hermitian views
template <typename FS>
const typename TpMatrix<FS>::ConstHermitianView
TpMatrix<FS>::hermitian() const
{
    return ConstHermitianView(_engine, upLo());
}

template <typename FS>
typename TpMatrix<FS>::HermitianView
TpMatrix<FS>::hermitian()
{
    return HermitianView(_engine);
}

// symmetric views
template <typename FS>
const typename TpMatrix<FS>::ConstSymmetricView
TpMatrix<FS>::symmetric() const
{
    return ConstSymmetricView(_engine);
}

template <typename FS>
typename TpMatrix<FS>::SymmetricView
TpMatrix<FS>::symmetric()
{
    return SymmetricView(_engine);
}

// triangular views
template <typename FS>
const typename TpMatrix<FS>::ConstView
TpMatrix<FS>::triangular() const
{
    return ConstView(_engine);
}

template <typename FS>
typename TpMatrix<FS>::View
TpMatrix<FS>::triangular()
{
    return View(_engine);
}

// -- methods ------------------------------------------------------------------
template <typename FS>
typename TpMatrix<FS>::IndexType
TpMatrix<FS>::dim() const
{
    return _engine.dim();
}

template <typename FS>
typename TpMatrix<FS>::IndexType
TpMatrix<FS>::firstIndex() const
{
    return _engine.firstIndex();
}

template <typename FS>
typename TpMatrix<FS>::IndexType
TpMatrix<FS>::lastIndex() const
{
    return _engine.lastIndex();
}

template <typename FS>
const typename TpMatrix<FS>::ElementType *
TpMatrix<FS>::data() const
{
    return _engine.data();
}

template <typename FS>
typename TpMatrix<FS>::ElementType *
TpMatrix<FS>::data()
{
    return _engine.data();
}


template <typename FS>
StorageOrder
TpMatrix<FS>::order() const
{
    return _engine.order;
}

template <typename FS>
template <typename RHS>
bool
TpMatrix<FS>::resize(const TpMatrix<RHS> &rhs,
                     const ElementType &value)
{
    return _engine.resize(rhs.engine(), value);
}

template <typename FS>
bool
TpMatrix<FS>::resize(IndexType dim,
                     IndexType indexBase,
                     const ElementType &value)
{
    return _engine.resize(dim, indexBase,
                          value);
}

template <typename FS>
bool
TpMatrix<FS>::fill(const ElementType &value)
{
    return _engine.fill(value);
}

template <typename FS>
bool
TpMatrix<FS>::fillRandom()
{
    return _engine.fillRandom();
}

// -- implementation -----------------------------------------------------------
template <typename FS>
const typename TpMatrix<FS>::Engine &
TpMatrix<FS>::engine() const
{
    return _engine;
}

template <typename FS>
typename TpMatrix<FS>::Engine &
TpMatrix<FS>::engine()
{
    return _engine;
}

template <typename FS>
StorageUpLo
TpMatrix<FS>::upLo() const
{
    return FS::upLo;
}

template <typename FS>
Diag
TpMatrix<FS>::diag() const
{
    return _diag;
}

template <typename FS>
Diag &
TpMatrix<FS>::diag()
{
    return _diag;
}

} // namespace flens

#endif // FLENS_MATRIXTYPES_TRIANGULAR_IMPL_TPMATRIX_TCC
