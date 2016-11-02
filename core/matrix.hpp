#ifndef YBLAS_MATRIX_HPP
#define YBLAS_MATRIX_HPP

#include <cstddef> // std::size_t
#include <cstring> // std::memcpy()
#include <new> // std::bad_alloc()
#include <stdexcept>

// NO TRY...CATCH... : This should be done in runtime

namespace yblas {
    namespace core{

        template<class T>
        class matrix_ref;

        template<class T>
        class matrix {

            public:
                friend matrix_ref<T>;

                matrix() {
                    this->sztWidth = this->sztHeight = this->sztCapacity = 0;
                    this->ptrArray = nullptr;
                }

                matrix(std::size_t sztHeight, std::size_t sztWidth) {
                    this->sztHeight = sztHeight;
                    this->sztWidth = sztWidth;
                    this->sztCapacity = sztWidth * sztHeight; // overflow??
                    this->ptrArray = new float [sztCapacity];
                }

                ~matrix() {
                    if (this->sztCapacity != 0) {
                        delete [] this->ptrArray;
                    }
                }

                matrix(const matrix& mtrxInput) {
                    this->sztWidth = mtrxInput.sztWidth;
                    this->sztHeight = mtrxInput.sztHeight;
                    this->sztCapacity = mtrxInput.sztCapacity; // overflow??
                    this->ptrArray = new float[sztCapacity]; // std::bad_allocator ??
                    std::memcpy(this->ptrArray, mtrxInput.ptrArray, sztCapacity * sizeof(T));
                }

                matrix(std::size_t sztHeight, std::size_t sztWidth, T* ptrInput) {
                    this->sztHeight = sztHeight;
                    this->sztWidth = sztWidth;
                    this->sztCapacity = sztHeight * sztWidth;
                    ptrArray = ptrInput;
                }

                void resize(std::size_t sztHeight, std::size_t sztWidth) {
                    if ((sztHeight == 0) ^ (sztWidth == 0)) {
                        throw std::bad_alloc();
                    }

                    std::size_t sztNew = sztWidth * sztHeight; // new capacity // overflow??
                    
                    if (sztNew == 0) {
                        // if new matrix is void, just clear
                        this->clear();
                    }
                    else if (sztNew != sztCapacity) { 
                        // do copy only when new matrix is not void matrix
                        T* ptrTemp = new T [sztNew];
                        T* ptrRow = ptrTemp;
                        T* ptrSrc = ptrArray;
                        if (this->sztCapacity != 0) {
                            for (std::size_t i = 0; i < this->sztHeight; i++) {
                                // copy by row
                                std::memcpy(ptrRow, ptrSrc, sztWidth * sizeof(T));
                                ptrRow += sztWidth;
                                ptrSrc += this->sztWidth;
                            }
                        }
                        delete [] ptrArray;
                        ptrArray = ptrTemp;
                        
                    }
                    

                    // update member data
                    this->sztWidth = sztWidth;
                    this->sztHeight = sztHeight;
                    this->sztCapacity = sztNew;
                }

                T& value(std::size_t sztRow, std::size_t sztColumn) {
                    // Matlab index starts from 1
                    if ((sztRow == 0) || (sztColumn == 0)) {
                        throw std::out_of_range("Matrix index starts from 1.");
                    }
                    // out of index
                    if ((sztRow  > sztHeight) || (sztColumn > sztWidth)) {
                        throw std::out_of_range("Index out of height or width.");
                    }
                    sztRow--;
                    sztColumn--;
                    return this->ptrArray[sztRow * sztWidth + sztColumn];
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

                matrix& part(std::size_t sztRowSt, std::size_t sztRowEd, 
                                std::size_t sztColSt, std::size_t sztColEd) {
                    // error_check
                    /* make sure that:
                        1 <= sztRowSt < sztRowEd <= sztHeight;
                        1 <= sztColSt < sztColEd <= sztWIdth;
                    */

                    if ((0 > sztRowSt) || (sztRowEd > sztHeight)) {
                        throw std::out_of_range("Index out of range.");
                    }
                    if ((0 > sztColSt) || (sztColEd > sztWidth)) {
                        throw std::out_of_range("Index out of range.");
                    }
                    if ((sztRowSt >= sztRowEd) && (sztColSt >= sztColEd)) {
                        throw std::invalid_argument("Matrix reference: coord1 should be upleft to coord2.");
                    }

                    // make a copy of self within the given boundary
                    std::size_t sztWidthNew = sztColEd - sztColSt + 1;
                    std::size_t sztHeightNew = sztRowEd - sztRowSt + 1;
                    std::size_t sztCapacityNew = sztWidthNew * sztHeightNew;
                    T *ptrSrc = this->ptrArray + sztColSt - 1;
                    T *ptrNew = new T [sztCapacityNew];
                    T *ptrDst = ptrNew;
                    // copy by row 
                    for (std::size_t i = 0; i < sztHeightNew; i++) {
                        std::memcpy(ptrDst, ptrSrc, sztWidthNew * sizeof(T));
                        ptrDst += sztWidthNew;
                        ptrSrc += this->sztWidth;
                    }
                    matrix *p_mReturn = new matrix(sztHeightNew, sztWidthNew, ptrNew);
                    return *p_mReturn;
                }

                std::size_t width() { return sztWidth; }
                std::size_t height() { return sztHeight; }
                
                void clear() {
                    if (this->sztCapacity != 0) {
                        delete [] this->ptrArray;
                    }
                    this->ptrArray = nullptr;
                    this->sztWidth = this->sztHeight = this->sztCapacity = 0;
                }

            private:
                
                T* ptrArray;
                std::size_t sztWidth, sztHeight, sztCapacity;
        };

        
        /*
        template <class T>
        class matrix_ref : matrix<T> { 
            public:
                // matrix_ref() {
                //     ptrMatrix = ptrArray = nullptr;
                //     sztWidth = sztHeight = sztRowOffset = sztColOffset = 0;
                // }

                // ~matrix_ref() {

                // }

                // matrix_ref(matrix<T>  *p_mtxInput) {
                //     ptrMatrix = p_mtxInput;
                //     sztWidth = p_mtxInput->sztWidth;
                //     sztHeight = p_mtxInput->sztHeight;
                //     sztRowOffset = sztColOffset = 0;
                //     ptrArray = p_mtxInput->ptrArray;
                // }

                matrix_ref(matrix<T> *p_mtxInput, 
                            std::size_t sztRowSt, std::size_t sztRowEd, 
                            std::size_t sztColSt, std::size_t sztColEd) {
                    // input check
                    // 0 < sztRow1 <= sztRow2 < sztHeight;
                    // 0 < sztColumn1 <= sztColumn2 < sztWidth;
                    // 
                    
                    if ((0 > sztRowSt) || (sztRowEd > p_mtxInput->sztHeight)) {
                        throw std::out_of_range("Index out of range.");
                    }
                    if ((0 > sztColSt) || (sztColEd > p_mtxInput->sztWidth)) {
                        throw std::out_of_range("Index out of range.");
                    }
                    if ((sztRowSt >= sztRowEd) && (sztColSt >= sztColEd)) {
                        throw std::invalid_argument("Matrix reference: coord1 should be upleft to coord2.");
                    }

                    // error check finished
                    ptrMatrix = p_mtxInput;
                    sztWidth = sztColEd - sztColSt; 
                    sztHeight = sztRowEd - sztRowSt;
                    sztWidthSrc = p_mtxInput->sztWidth;
                    sztOffset = (sztRowSt - 1) * sztWidthSrc + (sztColSt - 1);
                }

                T& value(std::size_t sztRow, std::size_t sztColumn) {
                    // Matlab index starts from 1
                    if ((sztRow == 0) || (sztColumn == 0)) {
                        throw std::out_of_range("Matrix index starts from 1.");
                    }
                    // out of index
                    if ((sztRow  > sztHeight) || (sztColumn > sztWidth)) {
                        throw std::out_of_range("Index out of height or width.");
                    }
                    sztRow--;
                    sztColumn--;
                    return this->ptrArray[sztOffset + sztRow * sztWidthSrc + sztColumn];
                }


            private:
                std::size_t sztOffset, sztWidthSrc;
                std::size_t sztWidth, sztHeight;
                matrix<T>* ptrMatrix;
                T* ptrArray;
        };
        */
        

        typedef matrix<float> mfloat;
        // typedef matrix<double> mdouble;
        // typedef matrix<int> mint;
        // typedef matrix<unsigned> musg;
    }
}

#endif