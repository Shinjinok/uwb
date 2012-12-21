/****************************************************************************
 *
 *   Copyright (C) 2012 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file Matrix.h
 *
 * matrix code
 */

#pragma once


#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "Vector.hpp"

//#define MATRIX_ASSERT

#define ARM_MATH

#ifdef ARM_MATH
#include "arm_math.h"
#endif

namespace math
{

template<class T>
class __EXPORT Matrix {
public:
    typedef Matrix<T> MatrixType;
    typedef Vector<T> VectorType;
    // constructor
    Matrix(size_t rows, size_t cols) :
        _rows(rows),
        _cols(cols),
        _data((T*)calloc(rows*cols,sizeof(T)))
    {
    }
    Matrix(size_t rows, size_t cols, const T * data) :
        _rows(rows),
        _cols(cols),
        _data((T*)malloc(getSize()))
    {
        memcpy(getData(),data,getSize());
    }
    // deconstructor
    virtual ~Matrix()
    {
        delete [] getData();
    }
    // copy constructor (deep)
    Matrix(const MatrixType & right) :
        _rows(right.getRows()),
        _cols(right.getCols()),
        _data((T*)malloc(getSize()))
    {
        memcpy(getData(),right.getData(),
                    right.getSize());
    }
    // assignment
    inline MatrixType & operator=(const MatrixType & right)
    {
#ifdef MATRIX_ASSERT
        ASSERT(getRows()==right.getRows());
        ASSERT(getCols()==right.getCols());
#endif
        if (this != &right)
        {
            memcpy(getData(),right.getData(),
                        right.getSize());
        }
        return *this;
    }
    // element accessors
    inline T & operator()(size_t i, size_t j)
    {
#ifdef MATRIX_ASSERT
        ASSERT(i<getRows());
        ASSERT(j<getCols());
#endif
        return getData()[i*getCols() + j];
    }
    inline const T & operator()(size_t i, size_t j) const
    {
#ifdef MATRIX_ASSERT
        ASSERT(i<getRows());
        ASSERT(j<getCols());
#endif
        return getData()[i*getCols() + j];
    }
    // output
    inline void print() const
    {
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                float sig;
                int exp;
                float num = (*this)(i,j);
                float2SigExp(num,sig,exp);
                printf ("%6.3fe%03.3d,", (double)sig, exp);
            }
            printf("\n");
        }
    }
    // boolean ops
    inline bool operator==(const T & right) const
    {
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                if ((*this)(i,j) != right(i,j))
                    return false;
            }
        }
        return true;
    }
    // scalar ops
    inline MatrixType operator+(const T & right) const
    {
        MatrixType result(getRows(), getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i,j) = (*this)(i,j) + right;
            }
        }
        return result;
    }
    inline MatrixType operator-(const T & right) const
    {
        MatrixType result(getRows(), getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i,j) = (*this)(i,j) - right;
            }
        }
        return result;
    }
    inline MatrixType operator*(const T & right) const
    {
        MatrixType result(getRows(), getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i,j) = (*this)(i,j) * right;
            }
        }
        return result;
    }
    inline MatrixType operator/(const T & right) const
    {
        MatrixType result(getRows(), getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i,j) = (*this)(i,j) / right;
            }
        }
        return result;
    }
    // vector ops
    inline VectorType operator*(const VectorType & right) const
    {
#ifdef MATRIX_ASSERT
        ASSERT(getCols()==right.getRows());
#endif
        VectorType result(getRows());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i) += (*this)(i,j) * right(j);
            }
        }
        return result;
    }
    // matrix ops
    inline MatrixType operator+(const MatrixType & right) const
    {
#ifdef MATRIX_ASSERT
        ASSERT(getRows()==right.getRows());
        ASSERT(getCols()==right.getCols());
#endif
        MatrixType result(getRows(), getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i,j) = (*this)(i,j) + right(i,j);
            }
        }
        return result;
    }
    inline MatrixType operator-(const MatrixType & right) const
    {
#ifdef MATRIX_ASSERT
        ASSERT(getRows()==right.getRows());
        ASSERT(getCols()==right.getCols());
#endif
        MatrixType result(getRows(), getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<getCols(); j++)
            {
                result(i,j) = (*this)(i,j) - right(i,j);
            }
        }
        return result;
    }
    inline MatrixType operator*(const MatrixType & right) const
    {
#ifdef MATRIX_ASSERT
        ASSERT(getCols()==right.getRows());
#endif
        MatrixType result(getRows(), right.getCols());
        for (size_t i=0; i<getRows(); i++)
        {
            for (size_t j=0; j<right.getCols(); j++)
            {
                for (size_t k=0; k<right.getRows(); k++)
                {
                    result(i,j) += (*this)(i,k) * right(k,j);
                }
            }
        }
        return result;
    }
    inline MatrixType operator/(const MatrixType & right) const
    {
#ifdef MATRIX_ASSERT
        ASSERT(right.getRows()==right.getCols());
        ASSERT(getCols()==right.getCols());
#endif
        return (*this)*right.inverse();
    }
    // other functions
    inline MatrixType transpose() const
    {
        MatrixType result(getCols(),getRows());
        for(size_t i=0;i<getRows();i++) {
            for(size_t j=0;j<getCols();j++) {
                result(j,i) = (*this)(i,j);
            }
        }
        return result;
    }
    inline void swapRows(size_t a, size_t b)
    {
        if (a==b) return;
        for(size_t j=0;j<getCols();j++) {
            T tmp = (*this)(a,j);
            (*this)(a,j) = (*this)(b,j);
            (*this)(b,j) = tmp;
        }
    }
    inline void swapCols(size_t a, size_t b)
    {
        if (a==b) return;
        for(size_t i=0;i<getRows();i++) {
            T tmp = (*this)(i,a);
            (*this)(i,a) = (*this)(i,b);
            (*this)(i,b) = tmp;
        }
    }
    /**
     * inverse based on LU factorization with partial pivotting
     */
    MatrixType inverse() const
    {
#ifdef MATRIX_ASSERT
        ASSERT(getRows()==getCols());
#endif
        size_t N = getRows();
        MatrixType L = identity(N);
        const MatrixType & A = (*this);
        MatrixType U = A;
        MatrixType P = identity(N);

        //printf("A:\n"); A.print();

        // for all diagonal elements
        for (size_t n=0; n<N; n++) {

            // if diagonal is zero, swap with row below
            if (fabs(U(n,n))<1e-8) {
                //printf("trying pivot for row %d\n",n);
                for (size_t i=0; i<N; i++) {
                    if (i==n) continue;
                    //printf("\ttrying row %d\n",i);
                    if (fabs(U(i,n))>1e-8) {
                        //printf("swapped %d\n",i);
                        U.swapRows(i,n);
                        P.swapRows(i,n);
                    }
                }
            }
#ifdef MATRIX_ASSERT
            //printf("A:\n"); A.print();
            //printf("U:\n"); U.print();
            //printf("P:\n"); P.print();
            //fflush(stdout);
            ASSERT(fabs(U(n,n))>1e-8);
#endif
            // failsafe, return zero matrix
            if (fabs(U(n,n))<1e-8)
            {
                return MatrixType::zero(n);
            }

            // for all rows below diagonal
            for (size_t i=(n+1); i<N; i++) {
                L(i,n) = U(i,n)/U(n,n);
                // add i-th row and n-th row
                // multiplied by: -a(i,n)/a(n,n)
                for (size_t k=n; k<N; k++) {
                    U(i,k) -= L(i,n) * U(n,k);       
                }
            }
        }

        //printf("L:\n"); L.print();
        //printf("U:\n"); U.print();

        // solve LY=P*I for Y by forward subst
        MatrixType Y = P;
        // for all columns of Y
        for (size_t c=0; c<N; c++) {
            // for all rows of L
            for (size_t i=0; i<N; i++) {
                // for all columns of L
                for (size_t j=0; j<i; j++) {
                    // for all existing y
                    // subtract the component they 
                    // contribute to the solution
                    Y(i,c) -= L(i,j)*Y(j,c); 
                }
                // divide by the factor 
                // on current
                // term to be solved
                // Y(i,c) /= L(i,i);
                // but L(i,i) = 1.0
            }
        }

        //printf("Y:\n"); Y.print();

        // solve Ux=y for x by back subst
        MatrixType X = Y;
        // for all columns of X
        for (size_t c=0; c<N; c++) {
            // for all rows of U
            for (size_t k=0; k<N; k++) {
                // have to go in reverse order
                size_t i = N-1-k; 
                // for all columns of U
                for (size_t j=i+1; j<N; j++) {
                    // for all existing x
                    // subtract the component they 
                    // contribute to the solution
                    X(i,c) -= U(i,j)*X(j,c); 
                }
                // divide by the factor 
                // on current
                // term to be solved
                X(i,c) /= U(i,i);
            }
        }
        //printf("X:\n"); X.print();
        return X;
    }
    inline void setAll(const T & val)
    {
        for (size_t i=0;i<getRows();i++) {
            for (size_t j=0;j<getCols();j++) {
                (*this)(i,j) = val;
            }
        }
    }
    inline void set(const T * data)
    {
        memcpy(getData(),data,getSize());
    }
    inline size_t getRows() const { return _rows; }
    inline size_t getCols() const { return _cols; }
    inline static MatrixType identity(size_t size) {
        MatrixType result(size,size);
        for (size_t i=0; i<size; i++) {
            result(i,i) = 1.0f; 
        }
        return result;
    }
    inline static MatrixType zero(size_t size) {
        MatrixType result(size,size);
        result.setAll(0.0f);
        return result;
    }
    inline static MatrixType zero(size_t m, size_t n) {
        MatrixType result(m,n);
        result.setAll(0.0f);
        return result;
    }
protected:
    inline size_t getSize() const { return sizeof(T)*getRows()*getCols(); }
    inline T * getData() { return _data; }
    inline const T * getData() const { return _data; }
    inline void setData(T * data) { _data = data; }
private:
    size_t _rows;
    size_t _cols;
    T * _data;
};

typedef Matrix<float> MatrixFloat;

int matrixTest();
int matrixAddTest();
int matrixSubTest();
int matrixMultTest();
int matrixInvTest();
int matrixDivTest();
int matrixArmTest();

} // namespace math
