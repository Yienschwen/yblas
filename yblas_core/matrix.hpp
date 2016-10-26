#ifndef YBLAS_MATRIX_HPP
#define YBLAS_MATRIX_HPP

#include <cstddef> // std::size_t
#include <cstring> // std::memcpy()

#include "yblas_base.h"

namespace YBLAS_NAMESPACE {

    template<class T>
    class matrix {
        public:
            matrix() {
                this->sztWidth = this->sztHeight = this->sztCapacity = 0;
                this->ptrArray = nullptr;
            }

            ~matrix() {
                if (this->sztCapacity != 0) {
                    delete [] this->ptrArray;
                }
            }

            matrix(const matrix& mtrxInput) {
                this->sztWidth = mtrxInput.sztWidth;
                this->sztHeight = mtrxInput.sztHeight;
                this->sztCapacity = mtrxInput.sztCapacity;
                this->ptrArray = new float[sztCapacity]; // std::bad_allocator ??
                std::memcpy(this->ptrArray, mtrxInput.ptrArray, sztCapacity * sizeof(T));
            }

            matrix& operator=(const matrix& mtrxInput) {
                // if capacity doesn't match, then re-allocate memory
                if (this->sztCapacity != mtrxInput.sztCapacity) {
                    this->clear();
                    this->sztCapacity = mtrxInput.sztCapacity;
                    this->ptrArray = new T[sztCapacity]; // std::bad_allocator ??
                }
                this->sztWidth = mtrxInput.sztWidth;
                this->sztHeight = mtrxInput.sztHeight;
                std::memcpy(this->ptrArray, mtrxInput.ptrArray, sztCapacity * sizeof(T));
                return this;
            }

            matrix operator+(const matrix& mtrxInput) {
                
                
            }

            matrix operator-(const matrix& mtrxInput) {
                
            }
            
            // part copy
            // part reference

            // shift
            

        private:
            void clear() {
                if (this->sztCapacity != 0) {
                    delete [] this->ptrArray;
                }
                this->ptrArray = nullptr;
                this->sztWidth = this->sztHeight = this->sztCapacity = 0;
            }

            T* ptrArray;
            std::size_t sztWidth, sztHeight, sztCapacity;
    };
}

typedef matrix<float> mfloat;


//T operator+(const T &a, const T2 &b);
matrix<float> operator+(const matrix<float>& mtrxLeft, const matrix<float>& mtrxRight) {

}

matrix<float> operator-(const matrix<float>& mtrxLeft, const matrix<float>& mtrxRight) {
    
}

matrix<float> operator*(const matrix<float>& mtrxLeft, const matrix<float>& mtrxRight) {
    
}

/* type casting
template <class S, class T>
matrix<T> matrix::matrix_cast(const matrix<S>& mtrxInput) {
    
}
*/

#endif