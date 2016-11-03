#include <iostream>
#include <string>
#include <unordered_map>
// #include <stack>
#include <vector>

#include "immintrin.h"

#include "../core/matrix.hpp"

using namespace yblas::core;
using namespace std;

namespace yblas {
namespace lang {
class VMprototype {
    public:

        struct stkObj { // stack object

            stkObj(mfloat* pInput, bool blCopy) {
                p_mfSrc = pInput;
                copy = blCopy;
            }

            mfloat* p_mfSrc;
            bool copy;
        };

        struct pmfrd { 
            
            // pointer to mfloat as operand
            // "smart-pointer" that will free all its memory when deconstructs
            // if the matrix p_mfSrc points is a deep copy


            pmfrd(const stkObj& objInput) {
                this->p_mfSrc = objInput.p_mfSrc;
                this->copy = objInput.copy;
            }

            ~pmfrd() {
                if (copy) {
                    p_mfSrc->clear();
                    delete p_mfSrc;
                }
            }

            mfloat* move() {
                copy = false;
                return p_mfSrc;
            }

            void clear() {
                if (copy) {
                    copy = false;
                    p_mfSrc->clear();
                    delete p_mfSrc;
                }
            }

            mfloat* p_mfSrc;
            bool copy;
        };

        VMprototype(ostream& ostrInput) {
            this->outMsg = &ostrInput;
        }

        ~VMprototype() {
            // cerr << "DBG:\tMemory deconstructor called.\n";
            this->clear();
            while (!stkOperand.empty()) {
                pmfrd p = pmfrd(stkOperand.back());
                p.clear();
                stkOperand.pop_back(); 
            }
        }

        void clear() {
            for (auto it = mpMain.begin(); it != mpMain.end(); it = mpMain.begin()) {
                // cerr << "DBG:\tVariable " << it->first << " removed.\n"; 
                it->second->clear();
                delete it->second;
                mpMain.erase(it);
            }
        }

        void clearvars(size_t sztCount, const string* p_strNames) {
            for (size_t i = 0; i < sztCount; i++) {
                this->remove(p_strNames[i]);
            }
        }

        void set(const string& strName) {
            if (stkOperand.empty()) {
                *outMsg << "ERR:\tOperand stack is empty. No value to assign. Skipping.\n";
                return;
            }
            pmfrd pSrc = pmfrd(stkOperand.back());
            stkOperand.pop_back();
            auto got = mpMain.find(strName);
            if (got == mpMain.end()) {
                mpMain[strName] = (pSrc.copy) ? pSrc.move() : new mfloat(*(pSrc.p_mfSrc));
            }
            else {
                mfloat* p_mfDst = got->second;
                // the memory of target matrix is managed by the matrix itself
                p_mfDst->operator=(*(pSrc.p_mfSrc)); 
            }
            // pSrc will free its memory itself in deconstructor if it's a deep copy
        }   

        void litmat(size_t sztHeight, size_t sztWidth, float* p_fEntry) {
            mfloat *p_matLit = new mfloat(sztHeight, sztWidth, p_fEntry);
            this->push_operand(p_matLit, true);
        }

        void part(const string& strName, size_t sztRowSt, size_t sztRowEd, 
                                         size_t sztColSt, size_t sztColEd) {
            mfloat *p_mSrc = this->query(strName);
            if (!p_mSrc) {
                *outMsg << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                return;
            }
            mfloat *p_mPush = &(p_mSrc->part(sztRowSt, sztRowEd, sztColSt, sztColEd));
            this->push_operand(p_mPush, true);
        }

        void pushref(const string& strName) {
            mfloat *p_mSrc = this->query(strName);
            if (!p_mSrc) {
                *outMsg << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                return;
            }
            this->push_operand(p_mSrc, false);
        }

        ostream& print(const string& strName, ostream& os) {
            mfloat *p_mSrc = this->query(strName);
            if (!p_mSrc) {
                *outMsg << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                return os;
            }
            os << strName << " =\n";
            size_t M, N;
            M = p_mSrc->height();
            N = p_mSrc->width();
            for (size_t i = 1; i <= M; i++) {
                for (size_t j = 1; j <= N; j++) {
                    os << '\t' << p_mSrc->value(i, j);
                }
                os << '\n';
            }
            os << '\n';
            return os;
        }

        void mfaddavx() {
            if (!mfbicheck("mfaddavx")) {
                return;
            }
            avxInstr inst = add;
            mfavxbilinear(inst);
        }

        void mfsubavx() {
            if (!mfbicheck("mfsubavx")) {
                return;
            }
            avxInstr inst = sub;
            mfavxbilinear(inst);
        }

        void mfdmulavx() {
            if (!mfbicheck("mfdmulavx")) {
                return;
            }
            avxInstr inst = mul;
            mfavxbilinear(inst);
        }

        void mfddivavx() {
            if (!mfbicheck("mfddivavx")) {
                return;
            }
            avxInstr inst = div;
            mfavxbilinear(inst);
        }

        void mfset1avx(size_t sztHeight, size_t sztWidth, float k) {
            size_t total = sztWidth * sztHeight;
            float *ptrTemp = new float[total];
            size_t sztLoop = total / 8, sztRemain = total % 8;
            __m256 c = _mm256_set1_ps(k);
            float *ptrDst = ptrTemp;
            for (size_t i = 0; i < sztLoop; i++) {
                _mm256_storeu_ps(ptrDst, c);
                ptrDst += 8;
            }
            for (size_t i = 0; i < sztRemain; i++) {
                ptrDst[i] = k;
            }
            mfloat *p_matLit = new mfloat(sztHeight, sztWidth, ptrTemp);
            this->push_operand(p_matLit, true);
        }
        

    private:

        enum avxInstr {add, sub, mul, div};

        void push_operand(mfloat* p_matPush, bool blCopy) {
            stkOperand.push_back(stkObj(p_matPush, blCopy));
        }

        mfloat* query(const string& strName) {
            auto got = mpMain.find(strName);
            if (got == mpMain.end()) {
                return nullptr;
            }
            return got->second;
        }

        void remove(const string& strName) {
            auto got = mpMain.find(strName);
            if (got == mpMain.end()) {
                *outMsg << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                return;
            }
            // cerr << "DBG:\tVariable " << got->first << " removed.\n"; 
            mfloat* p_mDel = got->second;
            p_mDel->clear();
            delete p_mDel;
            mpMain.erase(got);
        }

        bool mfbicheck(const char* strInstr) {
            if (stkOperand.size() < 2) {
                *outMsg << "ERR:\t" << strInstr << ": Not enough operands in stack. Skipping.\n";
                return false;
            }
            // check if two matrix matches
            mfloat* p_mfR = stkOperand.back().p_mfSrc;
            mfloat* p_mfL = stkOperand[stkOperand.size() - 2].p_mfSrc;
            if ((p_mfL->sztWidth != p_mfR->sztWidth) || (p_mfL->sztHeight != p_mfR->sztHeight)) {
                *outMsg << "ERR:\t" << strInstr << ": Dimensions of two operands don't match. Skipping.\n";
                return false;
            }
            return true;
        }

        void mfavxbilinear(avxInstr instIn) {
            pmfrd pL = stkOperand.back();
            stkOperand.pop_back();
            pmfrd pR = stkOperand.back();
            stkOperand.pop_back();
            float *p_fL = pL.p_mfSrc->ptrArray, *p_fR = pR.p_mfSrc->ptrArray;
            float *p_fResult = new float [pL.p_mfSrc->sztCapacity];
            size_t sztHeight = pL.p_mfSrc->sztHeight, sztWidth = pL.p_mfSrc->sztWidth;
            size_t sztCapacity = pL.p_mfSrc->sztCapacity;
            size_t sztLoop = sztCapacity / 8, sztRemain = sztCapacity % 8;
            float *p_fLCurr = p_fL, *p_fRCurr = p_fR, *p_fDst = p_fResult;
            for (size_t i = 0; i < sztLoop; i++) {
                __m256 l = _mm256_loadu_ps(p_fLCurr);
                __m256 r = _mm256_loadu_ps(p_fRCurr);
                __m256 rslt;
                switch (instIn) {
                    case add: rslt = _mm256_add_ps(l, r); break;
                    case sub: rslt = _mm256_sub_ps(l, r); break;
                    case mul: rslt = _mm256_mul_ps(l, r); break;
                    case div: rslt = _mm256_div_ps(l, r); break;
                };
                _mm256_storeu_ps(p_fDst, rslt);
                p_fLCurr += 8;
                p_fRCurr += 8;
                p_fDst += 8;
            }
            for (size_t i = 0; i < sztRemain; i++) {
                switch (instIn) {
                    case add: p_fDst[i] = p_fLCurr[i] + p_fRCurr[i]; break;
                    case sub: p_fDst[i] = p_fLCurr[i] - p_fRCurr[i]; break;
                    case mul: p_fDst[i] = p_fLCurr[i] * p_fRCurr[i]; break;
                    case div: p_fDst[i] = p_fLCurr[i] / p_fRCurr[i]; break;
                };
            }
            mfloat* p_mfRslt = new mfloat(sztHeight, sztWidth, p_fResult);
            this->push_operand(p_mfRslt, true);
            // pL and pR will free their memory themselves if necessary
        }

        unordered_map<string, mfloat*> mpMain;
        vector<stkObj> stkOperand;
        ostream* outMsg;
};
}
}

// enum op {}

using namespace yblas::lang;

int main(int argc, char** argv) {
    string strInstr; // instruction
    VMprototype vmMain = VMprototype(cerr);
    while (cin >> strInstr) {
        if (strInstr == "mfaddavx") {
            vmMain.mfaddavx();
        }
        else if (strInstr == "mfsubavx") {
            vmMain.mfsubavx();
        }
        else if (strInstr == "mfdmulavx") {
            vmMain.mfdmulavx();
        }
        else if (strInstr == "mfddivavx") {
            vmMain.mfddivavx();
        }
        else if (strInstr == "mfset1avx") {
            size_t M, N, total;
            cin >> M >> N;
            total = M * N;
            float k;
            cin >> k;
            vmMain.mfset1avx(M, N, k);
        }
        else if (strInstr == "litmat") {
            size_t M, N, total;
            cin >> M >> N;
            total = M * N;
            float *ptrTemp = new float[total]; 
            for (size_t i = 0; i < total; i++) {
                cin >> ptrTemp[i];
            }
            vmMain.litmat(M, N, ptrTemp);
        }
        else if (strInstr == "set") {
            string strName;
            cin >> strName;
            vmMain.set(strName);
        }
        else if (strInstr == "part") {
            string strName;
            cin >> strName;
            size_t Rst, Red, Cst, Ced;
            cin >> Rst >> Red >> Cst >> Ced;
            vmMain.part(strName, Rst, Red, Cst, Ced);
        }
        else if (strInstr == "pushref") {
            string strName;
            cin >> strName;
            vmMain.pushref(strName);
        }
        else if (strInstr == "clear") {
            vmMain.clear();
        }
        else if (strInstr == "clearvars") {
            size_t N;
            cin >> N;
            string* p_strVar = new string [N];
            for (size_t i = 0; i < N; i++) {
                cin >> p_strVar[i];
            }
            vmMain.clearvars(N, p_strVar);
        }
        else if (strInstr == "print") {
            string strName;
            cin >> strName;
            vmMain.print(strName, cout);
        }
        else {
            // unknown instruction
            cerr << "ERR:\tUnknown instruction " << strInstr << ". Skipping.\n";
        }

    }
    return 0;
}