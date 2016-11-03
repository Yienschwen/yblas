#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>

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

        ~VMprototype() {
            cerr << "DBG:\tMemory deconstructor called.\n";
            this->clear();
            while (!stkOperand.empty()) {
                pmfrd p = pmfrd(stkOperand.top());
                p.clear();
                stkOperand.pop(); 
            }
        }

        void clear() {
            for (auto it = mpMain.begin(); it != mpMain.end(); it = mpMain.begin()) {
                cerr << "DBG:\tVariable " << it->first << " removed.\n"; 
                it->second->clear();
                delete it->second;
                mpMain.erase(it);
            }
        }

        void set(const string& strName) {
            if (stkOperand.empty()) {
                cerr << "ERR:\tOperand stack is empty. No value to assign. Skipping.\n";
                return;
            }
            pmfrd pSrc = pmfrd(stkOperand.top());
            stkOperand.pop();
            auto got = mpMain.find(strName);
            if (got != mpMain.end()) {
                mfloat* p_mfDst = got->second;
                // the memory of target matrix is managed by the matrix itself
                p_mfDst->operator=(*(pSrc.p_mfSrc)); 
            }
            else {
                mpMain[strName] = pSrc.move();
            }
            // pSrc will free its memory itself in deconstructor if it's a deep copy
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
                cerr << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                return;
            }
            cerr << "DBG:\tVariable " << got->first << " removed.\n"; 
            mfloat* p_mDel = got->second;
            p_mDel->clear();
            delete p_mDel;
            mpMain.erase(got);
        }

        void push_operand(mfloat* p_matPush, bool blCopy) {
            stkOperand.push(stkObj(p_matPush, blCopy));
        }

    private:
        unordered_map<string, mfloat*> mpMain;
        stack<stkObj> stkOperand;
};
}
}

// enum op {}

using namespace yblas::lang;

int main(int argc, char** argv) {
    string strInstr; // instruction
    VMprototype vmMain;
    while (cin >> strInstr) {
        if (strInstr == "litmat") {
            size_t M, N, total;
            cin >> M >> N;
            total = M * N;
            float *ptrTemp = new float[total]; 
            for (size_t i = 0; i < total; i++) {
                cin >> ptrTemp[i];
            }
            // automatic push
            mfloat *p_matLit = new mfloat(M, N, ptrTemp);
            // delete [] ptrTemp;
            vmMain.push_operand(p_matLit, true);
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
            mfloat *p_mSrc = vmMain.query(strName);
            if (!p_mSrc) {
                cerr << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                continue;
            }
            mfloat *p_mPush = &(p_mSrc->part(Rst, Red, Cst, Ced));
            vmMain.push_operand(p_mPush, true);
        }
        else if (strInstr == "clear") {
            vmMain.clear();
        }
        else if (strInstr == "clearvars") {
            size_t N;
            cin >> N;
            for (size_t i = 0; i < N; i++) {
                string strName;
                cin >> strName;
                vmMain.remove(strName);
            }
        }
        else if (strInstr == "print") {
            string strName;
            cin >> strName;
            mfloat *p_mSrc = vmMain.query(strName);
            if (!p_mSrc) {
                cerr << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                continue;
            }
            cout << strName << " =\n";
            size_t M, N;
            M = p_mSrc->height();
            N = p_mSrc->width();
            for (size_t i = 1; i <= M; i++) {
                for (size_t j = 1; j <= N; j++) {
                    cout << '\t' << p_mSrc->value(i, j);
                }
                cout << '\n';
            }
            cout << '\n';
        }
        else {
            // unknown instruction
            cerr << "ERR:\tUnknown instruction " << strInstr << ". Skipping.\n";
        }

    }
    return 0;
}