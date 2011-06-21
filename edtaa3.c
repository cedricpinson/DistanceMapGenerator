/*
 * edtaa3.c - compute the Euclidean distance transform of an image,
 * with more accurate handling of 1 pixel wide anti-aliased edges.
 *
 * This is a MEX-file for MATLAB.
 * MATLAB is a product of The MathWorks, Inc.
 *
 * Code in "edtaa3func.c" originally by Stefan Gustavson 1994,
 * implemented from a verbal description in the PhD dissertation
 * of Ingemar Ragnemalm, dept of EE, Linkoping University.
 *
 * Modification to handle antialiased edges and
 * this Matlab MEX wrapper by Stefan Gustavson,
 * (stefan.gustavson@gmail.com) 2009-05-15
 */

  /*
 
Copyright (C) 2009 Stefan Gustavson (stefan.gustavson@gmail.com)

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

The GNU General Public License is available on <http://www.gnu.org/licenses/>.
 */

#include "mex.h"
#include <math.h>

/* Lazy code: include the actual function definition instead of
 * linking the object file. This makes compilation easier, since
 * this source file becomes the only compilation unit required.
 * This way, you can compile the entire DLL with "mex edtaa2.c"
 */
#include "edtaa3func.c"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
  double *img, *Dout;
  double *gx, *gy;
  int *Iout;
  short *xdist, *ydist;
  int mrows,ncols;
  int i;
  
  /* Check for the proper number of arguments. */
  if(nrhs<1) {
    mexErrMsgTxt("Too few input arguments.");
  } else if(nrhs>1) {
    mexErrMsgTxt("Too many input arguments.");
  } else if(nlhs>2) {
    mexErrMsgTxt("Too many output arguments.");
  }
  
  /* The input must be a noncomplex, double 2-D array. */
  mrows = mxGetM(prhs[0]);
  ncols = mxGetN(prhs[0]);
  if( !mxIsDouble(prhs[0]) || mxIsComplex(prhs[0])) {
    mexErrMsgTxt("Input must be a noncomplex double array.");
  }

  /* Get input data pointer */
  img = mxGetPr(prhs[0]);

  /* Create a new complex array, set output data pointers to it. */
  plhs[0] = mxCreateDoubleMatrix(mrows, ncols, mxREAL);
  Dout = mxGetPr(plhs[0]);

  /* Call the C subroutine. */
  xdist = (short*)mxMalloc(mrows*ncols*sizeof(short)); // local data
  ydist = (short*)mxMalloc(mrows*ncols*sizeof(short));
  gx = (double*)mxCalloc(mrows*ncols, sizeof(double));
  gy = (double*)mxCalloc(mrows*ncols, sizeof(double));
  computegradient(img, mrows, ncols, gx, gy);
  edtaa3(img, gx, gy, mrows, ncols, xdist, ydist, Dout);
  // Pixels with grayscale>0.5 will have a negative distance.
  // This is correct, but we don't want values <0 returned here.
    for(i=0; i<mrows*ncols; i++) {
      if(Dout[i] < 0) Dout[i]=0.0;
    }

  if(nlhs > 1) {
  /* Create a new int array, set output data pointer to it. */
    plhs[1] = mxCreateNumericMatrix(mrows, ncols, mxINT32_CLASS, mxREAL);
    Iout = mxGetData(plhs[1]);
    // Compute output data for optional 'index to closest object pixel'
    for(i=0; i<mrows*ncols; i++) {
      Iout[i] = i+1 - xdist[i] - ydist[i]*mrows;
    }
  }

  mxFree(xdist); // Local data allocated with mxMalloc()
  mxFree(ydist); // (Would be automatically deallocated, but be tidy)
  mxFree(gx);
  mxFree(gy);

  /* That's it. We're done. */
}
