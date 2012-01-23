/* FiniteFieldCal.h
 * Jing Wang
 * June, 2010

ZetaSim - A NS2 based Simulation suit for Zeta Protocol
Copright (C) 2010 Jing Wang

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

Jing Wang
Department of Electronics and Information Engineering
Huazhong University of Science and Technology
Wuhan, Hubei, 430074
hbhzwj@gmail.com

*/
#ifndef FINITE_FIELD_CAL_H
#define FINITE_FIELD_CAL_H

#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "galois.h"

typedef char BYTE;
typedef unsigned char CodeWord_t;

struct Parameter {
	Parameter () {
		codeWordSize = 0;
		blockSize = 0;
		dataSize = 0;
	}
	Parameter(int codeWordSize0, int blockSize0, int dataSize0) {
		codeWordSize = codeWordSize0;
		blockSize = blockSize0;
		dataSize = dataSize0;
		wordNumInBlock = blockSize0 / codeWordSize0; // Error Prone, need to be revised in later version
		blockNumInData = dataSize0 / blockSize0;
	}
	void Update() {
		wordNumInBlock = blockSize / codeWordSize;
		blockNumInData = dataSize / blockSize;
	}
        int codeWordSize; // unit is byte, fieldsize = codeWordSize * sizeof(BYTE);
        int blockSize;
        int dataSize;
        int wordNumInBlock;
        int blockNumInData;
};

struct Block {
        CodeWord_t* data_;
        CodeWord_t* coef_;

	Block () {
		data_ = NULL;
		coef_ = NULL;
	}
        Block ( int blockNumInData, int wordNumInBlock ) {
                data_ = new CodeWord_t[wordNumInBlock];
		//data_ =  (CodeWord_t*) malloc( wordNumInBlock * sizeof(CodeWord_t) );
		if ( data_ == NULL) {
			std::cout << "Memory Not Enough" << std::endl;
			exit(1);
		}
                memset ( data_, 0, wordNumInBlock * sizeof ( CodeWord_t ) );
                coef_ = new CodeWord_t[blockNumInData];
		if ( coef_ == NULL) {
			std::cout << "Memory Not Enough" << std::endl;
			exit(1);
		}
                memset ( coef_, 0, blockNumInData * sizeof ( CodeWord_t ) );
        }
        ~Block () {
                //delete [] data_;
                //delete [] coef_;
        }
};

typedef std::vector<Block*> BlockPVec_t;

struct Matrix {
// Constructor and Deconstruct
        Matrix ( int colSize, int rowSize ) {
                COL_SIZE = colSize;
                ROW_SIZE = rowSize;
                col = colSize;
                row = 0;
                mx = new CodeWord_t[COL_SIZE * ROW_SIZE];
        }
// variable
        int COL_SIZE;
        int ROW_SIZE;
        int col;
        int row;
        CodeWord_t* mx;
// method
        void InsertRow ( CodeWord_t* r, int len );
        void EraseLastRow() { row--; }
        CodeWord_t& Access ( int i, int j ) { // start from 0.
                return * ( ( CodeWord_t * ) ( mx + i*COL_SIZE + j ) );
        }
        void SwapCol ( int m, int n );
        void AddRow ( int desRow, int srcRow, CodeWord_t Coef );

        bool IsRelated();
};



struct File {
// Constructor and Deconstruct
	File ( int blockNumInData )
                        : coefMat_( blockNumInData, blockNumInData ), invMat_( blockNumInData, blockNumInData ){
                for ( int i = 0; i < blockNumInData; i++ ) {
                        //printf("invMat_ %d ", i);
                        invMat_.Access ( i, i ) = 1;
                }
        }
        //~File() { }
// variable
        BlockPVec_t data_;
        Matrix coefMat_;
        Matrix invMat_;
// method
        void PreDecode();
};

CodeWord_t Multiply ( CodeWord_t x, CodeWord_t y );
CodeWord_t Divide ( CodeWord_t& x, CodeWord_t& y );
void RegionMultiply ( CodeWord_t* data, CodeWord_t coef, int dataSize, CodeWord_t* res, int add, int codeWordSize );
CodeWord_t GenRand ( int fieldSize );
// for debug
void PrintCodeWord ( CodeWord_t word );
void PrintCodeWord ( CodeWord_t word, char op);
void PrintCodeWord ( CodeWord_t* wordP, int n );
void PrintCoef ( Matrix mat );

#endif