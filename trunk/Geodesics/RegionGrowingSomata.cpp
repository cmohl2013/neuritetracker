//**********************************************************
//Copyright 2012 Fethallah Benmansour
//
//Licensed under the Apache License, Version 2.0 (the "License"); 
//you may not use this file except in compliance with the License. 
//You may obtain a copy of the License at
//
//http://www.apache.org/licenses/LICENSE-2.0 
//
//Unless required by applicable law or agreed to in writing, software 
//distributed under the License is distributed on an "AS IS" BASIS, 
//WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
//See the License for the specific language governing permissions and 
//limitations under the License.
//**********************************************************

#include "RegionGrowingSomata.h"

void mexFunction(	int nlhs, mxArray *plhs[],
        int nrhs, const mxArray*prhs[] )
{
    //------------------------------------------------------------------
    /* retrive arguments */
    //------------------------------------------------------------------
    if( nrhs!=8 )
    {
        mexErrMsgTxt("7 arguments are required.");
    }
    //------------------------------------------------------------------
    // first argument : spacing and dimensions
    const mwSize* dim_h = mxGetDimensions(prhs[0]);
    if ( (dim_h[0]!=2) || (dim_h[1]!=1) )
    {
        mexErrMsgTxt("Library error: h must be a 2x1 array list.");
    }
    hx = mxGetPr(prhs[0])[0]; hy = mxGetPr(prhs[0])[1];
    hx2 = hx*hx; hy2 = hy*hy;
    hx2hy2 = hx*hx*hy*hy;
    hx2_plus_hy2 = hx*hx + hy*hy;
    //------------------------------------------------------------------
    // Second argument : Input Image (green channel, supposed to be normalized)
    if( (mxGetClassID(prhs[1]) != mxDOUBLE_CLASS) || (mxGetNumberOfDimensions(prhs[1])!= 2) )
    {
        mexErrMsgTxt("Input image must be a 2D double array AND \n must be of class double") ;
    }
    
    WW = mxGetPr(prhs[1]);
    nx = mxGetDimensions(prhs[1])[0];
    ny = mxGetDimensions(prhs[1])[1];
    Nx = nx+2; Ny = ny+2;
    size = Nx*Ny;
    // Third argument : Nuclei detections
    if( (mxGetClassID(prhs[2]) != mxDOUBLE_CLASS) || (mxGetNumberOfDimensions(prhs[2])!= 2) )
    {
        mexErrMsgTxt("Nuclei detections must be a 2D double array AND \n must be of class double") ;
    }
    Nuclei = mxGetPr(prhs[2]);
    // Fourth argument : array of [mean_i, std_i] for intensities for each region
    if( (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS) || (mxGetNumberOfDimensions(prhs[3])!= 2)
    ||    mxGetDimensions(prhs[3])[0] != 2 )
    {
        mexErrMsgTxt("tforth input shoud be an array containing the [mean_i, std_i] intensities of the nuclei regions") ;
    }
    mean_std = mxGetPr(prhs[3]);
    number_of_regions = mxGetDimensions(prhs[3])[1];
    
    multFactor = mxGetScalar(prhs[4]);
    
    meanGlobalInt = mxGetScalar(prhs[5]);
    stdGlobalInt  = mxGetScalar(prhs[6]);
    Lmax          = mxGetScalar(prhs[7]);
    //------------------------------------------------------------------
    //==================================================================
    // Outputs
    mwSize dims[2] = {Nx,Ny};
    // First output : minimal action map
    plhs[0] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL );
    U = (double*) mxGetPr(plhs[0]);
    plhs[1] = mxCreateNumericArray(2, dims, mxINT16_CLASS, mxREAL );
    V = (short*) mxGetPr(plhs[1]);
    plhs[2] = mxCreateNumericArray(2, dims, mxDOUBLE_CLASS, mxREAL );
    L = (double*) mxGetPr(plhs[2]);
    //==================================================================
    InitializeNeighborhoods();
    //------------------------------------------------------------------
    InitializeArrays();
    //------------------------------------------------------------------
    InitializeOpenHeap();
    //------------------------------------------------------------------
    RunPropagation();
    //==================================================================
    resize();
    dims[0] = Nx-2; dims[1] = Ny-2;
    mxSetDimensions(plhs[0], dims, 2);
    mxSetDimensions(plhs[1], dims, 2);
    mxSetDimensions(plhs[2], dims, 2);
    //==================================================================
    mxFree(NeighborhoodSmall);
    mxFree(S);
    mxFree(W);
    mxFree(Trial);
    mxFree(Tree);
    //==================================================================
    return;
}