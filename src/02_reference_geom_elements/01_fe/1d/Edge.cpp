/*=========================================================================

  Program: FEMUS
  Module: Line
  Authors: Eugenio Aulisa and Sara Calandrini
 
  Copyright (c) FEMTTU
  All rights reserved. 

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notice for more information.

  =========================================================================*/



#include "Edge.hpp"


namespace femus {

  
  //******** C0 LAGRANGE - BEGIN ****************************************************
  
  const double line_lag::Xc[3][1]= {{-1},{1},{0}/*,{-0.5},{0.5}*/};

  const int line_lag::IND[3][1]= {{0},{2},{1}};

  
  const int line_lag::KVERT_IND[ LAGRANGE_EDGE_NDOFS_MAXIMUM_FINE ][2]= {{0,0},{1,1},{0,1},{0,2},{1,2}};
  
  const unsigned line_lag::fine2CoarseVertexMapping[2][2]={
  {0,2},
  {2,1} };
  
  
  //************************************************************

  double LineLinear::eval_phi(const int *I,const double* x) const {
    return lagLinear(x[0],I[0]);
  }

  double LineLinear::eval_dphidx(const int *I,const double* x) const {
    return dlagLinear(x[0],I[0]);
  }

  //************************************************************

  double LineBiquadratic::eval_phi(const int *I,const double* x) const {
    return lagBiquadratic(x[0],I[0]);
  }

  double LineBiquadratic::eval_dphidx(const int *I,const double* x) const {
    return dlagBiquadratic(x[0],I[0]);
  }

  double LineBiquadratic::eval_d2phidx2(const int *I,const double* x) const {
    return d2lagBiquadratic(x[0],I[0]);
  }
  
  
  //******** C0 LAGRANGE - END ****************************************************
  
  
  //******** DISCONTINUOUS POLYNOMIAL - BEGIN ****************************************************
  

  //************************************************************
  
  const int line_const::IND[2][1]= {{0},{1}};

  const double line_const::X[ DISCPOLY_EDGE_NDOFS_MAXIMUM_FINE ][1]={ 
    {-0.5},{0.5},
    {-0.5},{0.5}
  };
  
  const int line_const::KVERT_IND[ DISCPOLY_EDGE_NDOFS_MAXIMUM_FINE ][2]={ 
    {0,0},{1,0},
    {0,1},{1,1}
  };
  
  //************************************************************
  
  /// { 1, x}  the non-const functions are zero at the center of the element
  double linepwLinear::eval_phi(const int *I,const double* x) const {
    return (1.-I[0]) + x[0]*eval_dphidx(I,x);
  }

  double linepwLinear::eval_dphidx(const int *I,const double* x) const {
    return I[0];
  }

  //******** DISCONTINUOUS POLYNOMIAL - END ****************************************************
  
  
  
} //end namespace femus

