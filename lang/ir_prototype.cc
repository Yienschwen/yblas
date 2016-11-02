#include <iostream>
#include <string>
#include <unordered_map>
#include <stack>

#include "../core/matrix.hpp"

using namespace yblas::core;
using namespace std;

class memory {
    public:
        ~memory() {
            cerr << "DBG:\tMemory deconstructor called.\n";
            this->clear();
            while (!stkOperand.empty()) {
                mfloat* p_mTop = stkOperand.top();
                p_mTop->clear();
                delete p_mTop;
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
            mfloat* p_mSet = stkOperand.top();
            stkOperand.pop();
            mpMain[strName] = p_mSet;
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

        void push_operand(mfloat* p_matPush) {
            stkOperand.push(p_matPush);
        }

    private:
        unordered_map<string, mfloat*> mpMain;
        stack<mfloat*> stkOperand;
};

// enum op {}

int main() {
    string strInstr; // instruction
    memory mmrMain;
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
            mmrMain.push_operand(p_matLit);
        }
        else if (strInstr == "set") {
            string strName;
            cin >> strName;
            mmrMain.set(strName);
        }
        else if (strInstr == "part") {
            string strName;
            cin >> strName;
            size_t Rst, Red, Cst, Ced;
            cin >> Rst >> Red >> Cst >> Ced;
            mfloat *p_mSrc = mmrMain.query(strName);
            if (!p_mSrc) {
                cerr << "ERR:\tVariable " << strName << " not found. Skipping.\n";
                continue;
            }
            mfloat *p_mPush = &(p_mSrc->part(Rst, Red, Cst, Ced));
            mmrMain.push_operand(p_mPush);
        }
        // else if (strInstr == "ref") {
        //     string strName;
        //     cin >> strName;
        //     mfloat *p_mSrc = mmrMain.query(strName);
        //     if (!p_mSrc) {
        //         cerr << "ERR:\tVariable " << strName << " not found. Skipping.\n";
        //         continue;
        //     }

        // }
        else if (strInstr == "clear") {
            mmrMain.clear();
        }
        else if (strInstr == "clearvars") {
            size_t N;
            cin >> N;
            for (size_t i = 0; i < N; i++) {
                string strName;
                cin >> strName;
                mmrMain.remove(strName);
            }
        }
        else if (strInstr == "print") {
            string strName;
            cin >> strName;
            mfloat *p_mSrc = mmrMain.query(strName);
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