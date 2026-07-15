/*
 * Copyright (C) 2010-2014 Intel Corporation. All Rights Reserved.
 *
 * The source code contained or described herein and all
 * documents related to the source code ("Material") are owned by 
 * Intel Corporation or its suppliers or licensors. Title to the
 * Material remains with Intel Corporation or its suppliers and
 * licensors. The Material is protected by worldwide copyright
 * laws and treaty provisions.  No part of the Material may be
 * used, copied, reproduced, modified, published, uploaded,
 * posted, transmitted, distributed,  or disclosed in any way
 * except as expressly provided in the license provided with the
 * Materials.  No license under any patent, copyright, trade
 * secret or other intellectual property right is granted to or
 * conferred upon you by disclosure or delivery of the Materials,
 * either expressly, by implication, inducement, estoppel or
 * otherwise, except as expressly provided in the license
 * provided with the Materials. 
 *
 * Part of the vec_samples tutorial. See "Tutorial: Auto-vectorization"
 * in the Intel(R) C++ Compiler Tutorials document
 */

#include "Multiply.h"

// The "restrict" qualifier tells the compiler that pointer b does
// not alias with pointers a and x meaning that pointer b does not point
// to the same memory location as a or x and there is no overlap between
// the corresponding arrays.
// Without the "restrict" qualifer the compiler may try to generate
// additional runtime checks to determine whether or not b and a or x are
// aliased and multiple versions of the inner loop. Avoiding the runtime
// checks will increase the performance of the generated code.
// When using the "restrict" qualifier, you'll need to compile with
// the "-restrict" compiler option

#ifdef NOALIAS
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[restrict], FTYPE x[])
#else
void matvec(int size1, int size2, FTYPE a[][size2], FTYPE b[], FTYPE x[])
#endif
{
	int i, j;

	for (i = 0; i < size1; i++) {
		b[i] = 0;

#ifdef ALIGNED
// The pragma vector aligned below tells the compiler to assume that the data in
// the loop is aligned on 16-byte boundary so the vectorizer can use
// aligned instructions to generate faster code.
#pragma vector aligned
#endif

		for (j = 0;j < size2; j++) {
			b[i] += a[i][j] * x[j];
		}
	}
}
