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

#include "FiniteFieldCal.h"
//#include "wj_galois.h"

void Matrix::InsertRow ( CodeWord_t* r, int len )
{
	//printf("Enter Matrix::InsertRow()\n");
        if ( len != COL_SIZE ) {
                std::cout << "Error " << std::cout;
                exit ( 1 );
        }
        //printf("Access(%d, 0): ", row);

        CodeWord_t& cp = Access ( row, 0 ); // Get the address of header of the next line
        //PrintCodeWord(&cp, col);
        memcpy ( &cp, r, len );
        row++;
}

void Matrix::SwapCol ( int m, int n )
{
	//printf("Enter SwapCol\n");
        CodeWord_t tmp;
        for ( int i = 0 ; i < row; i++ ) {
                tmp = Access ( i, m );
                Access ( i, m ) = Access ( i, n );
                Access ( i, n ) = Access ( i, m );
        }
}

void Matrix::AddRow ( int desRow, int srcRow, CodeWord_t Coef )
{
        CodeWord_t* desAddr = ( CodeWord_t * ) & Access ( desRow, 0 );
        CodeWord_t* srcAddr = ( CodeWord_t * ) & Access ( srcRow, 0 );
        RegionMultiply ( srcAddr, ( int ) Coef, COL_SIZE, desAddr, 1, sizeof ( CodeWord_t ) );
}

bool Matrix::IsRelated()
{
        //printf ( "Enter Matrix::IsRelate()\n" );
        // Since in Each step, we use the precode rule. So the Coeffeicnet row, except last one
        // forms a up-triangle form. We make use of this propety to accelarate the
        // program
        bool upTriFlag = true; // the flag to check whether the Coeffeicnet , except last row forms a up-triangle form or not
        int i, j;

        for ( i = 0; i < ( row-1 ); i++ ) { // don't include last row
                for ( j = 0; j < ( row-1 ); j++ ) {

                        if ( ( i == j ) && ( Access ( i, j ) == 0 ) ) {
                                upTriFlag = false;
                                break;
                        }
                        if ( ( i > j ) && ( Access ( i, j ) !=0 ) ) {
				printf("i: %d j: %d\n", i, j);
                                upTriFlag = false;
                                break;
                        }
                }

                if ( upTriFlag == false ) {
                        break;
                }
        }
        if ( upTriFlag == false ) { // flag is flase means there is some problems in the last PreDecode function
                std::cerr << "[ERROR] in Matrix::IsRelated()" << std::endl;
		std::cerr << "i: " << i << "j: " << j << std::endl;
                exit ( 1 );
        }
        bool relatedFlag = true;
        for ( j = ( row-1 ); j < col ; j++ ) {
                if ( Access ( row-1, j ) != 0 ) {
                        relatedFlag = false;
                        break;
                }
        }
        return relatedFlag;
}


void File::PreDecode()
{
        //printf ( "Enter File::PreDecode()\n" );
        int row = coefMat_.row;
        int col = coefMat_.col;
        int i = 0, j = 0;



        // transform the Matrix into a UpTriangle Form
        for ( j = 0; j < ( row-1 ); j++ ) {
                CodeWord_t coef = Divide ( coefMat_.Access ( row-1, j ),
                                           coefMat_.Access ( j, j ) );
                coefMat_.AddRow ( row-1, j, coef );
        }

        if ( coefMat_.Access ( row-1, row-1 ) == 0 ) {
                // find the first nonzelo element in row and  column with A(i,j)!=0
                for ( j = row; j < col; j ++ ) {
			if ( coefMat_.Access(row-1, j) != 0 ) {
				break;
			}
		}
                if ( j == col ) { // could not find such a column
                        std::cerr << "Matrix::PreDeocde(), the matrix is not full rank" << std::endl;
                        exit ( 1 );
                } else { // swap the two columns
                        coefMat_.SwapCol ( row-1, j );
                        invMat_.SwapCol ( row-1, j );
                }
        }
}



CodeWord_t GenRand ( int fieldSize )
{
        if ( fieldSize > sizeof ( unsigned int ) * 8 ) {
                std::cerr << "fieldSize is twoo large" << std::endl;
                return NULL;
        }
        unsigned int i = rand();
        CodeWord_t randNum;
        memcpy ( ( BYTE* ) &randNum, ( BYTE* ) &i, fieldSize / 8 );
        return randNum;
}

void RegionMultiply ( CodeWord_t* data, CodeWord_t coef, int dataSize, CodeWord_t* res, int add, int codeWordSize )
{
        switch ( codeWordSize ) {
        case sizeof ( BYTE ) :
                galois_w08_region_multiply ( ( char* ) data,
					     ( int ) coef,
					     dataSize * codeWordSize,
					     ( char* ) res,
					     add );
                break;

        case 2*sizeof ( BYTE ) :
		galois_w16_region_multiply ( ( char* ) data,
					     ( int ) coef,
					     dataSize * codeWordSize,
					     ( char* ) res, add );
                break;

        case 4*sizeof ( BYTE ) :
		galois_w32_region_multiply ( ( char* ) data,
					     ( int ) coef,
					     dataSize * codeWordSize,
					     ( char* ) res, add );
                break;

        default:
                std::cout << "Don't support CodeWordSize [" << codeWordSize<< "]" << std::endl;
        }
}

CodeWord_t Multiply ( CodeWord_t x, CodeWord_t y )
{
        //int res = galois_logtable_multiply ( ( int ) x, ( int ) y, sizeof ( CodeWord_t ) *8 );
        int res = galois_shift_multiply ( ( int ) x, ( int ) y, sizeof ( CodeWord_t ) *8 );
        return ( CodeWord_t ) res;
}

CodeWord_t Divide ( CodeWord_t& x, CodeWord_t& y )
{
        //printf ( "line25\n" );
        //int res = galois_logtable_divide ( ( int ) x, ( int ) y, sizeof ( CodeWord_t ) *8 );
	if ( (((int) x) < 0) || (((int) y) < 0) ) {
		std::cerr << "ERROR in Divide()" << std::endl;
		exit(1);
	}
        int res =  galois_single_divide ( ( int ) x, ( int ) y, sizeof ( CodeWord_t ) * 8 );
        return ( CodeWord_t ) res;
}

void PrintCodeWord ( CodeWord_t word )
{
        int k;
        for ( k = 8 * sizeof ( CodeWord_t ) -1; k >= 0 ; k-- ) {
                if ( ( word & ( 1 << k ) ) == 0 ) {
                        std::cout << "0";
                } else {
                        std::cout << "1";
                }
        }
        std::cout << "\n";
}

void PrintCodeWord ( CodeWord_t word, char op) {
        int k;
        for ( k = 8 * sizeof ( CodeWord_t ) -1; k >= 0 ; k-- ) {
                if ( ( word & ( 1 << k ) ) == 0 ) {
                        std::cout << "0";
                } else {
                        std::cout << "1";
                }
        }
        std::cout << op;
}

void PrintCodeWord ( CodeWord_t* wordP, int n )
{
        int j, k;
        for ( j = 0; j < n; j++ ) {
                CodeWord_t word = * ( wordP + j );
                for ( k = 8 * sizeof ( CodeWord_t ) -1; k >= 0 ; k-- ) {
                        if ( ( word & ( 1 << k ) ) == 0 ) {
                                std::cout << "0";
                        } else {
                                std::cout << "1";
                        }
                }
                std::cout << "\t";
        }
        std::cout << std::endl;
}



void PrintCoef ( Matrix mat )
{
        std::cout << "*****************************" << std::endl;
        std::cout << "row: " << mat.row << " col: " << mat.col << std::endl;
        int i, j, k;
	std::cout << "\t\t";
	for ( j = 0; j < mat.col; j ++) {
		std::cout << j << "\t\t";
	}
	std::cout << std::endl;
        for ( i = 0; i < mat.row; i++ ) {
		std::cout << "Row [" << i << "] ";
                for ( j = 0; j < mat.col; j++ ) {
                        for ( k = 8 * sizeof ( CodeWord_t ) -1; k >= 0 ; k-- ) {
                                CodeWord_t word = mat.Access ( i,j );
                                if ( ( word & ( 1 << k ) ) == 0 ) {
                                        std::cout << "0";
                                } else {
                                        std::cout << "1";
                                }
                        }
                        std::cout << "\t";
                }
                std::cout << std::endl;
        }
        std::cout << "*****************************" << std::endl << std::endl;

}

