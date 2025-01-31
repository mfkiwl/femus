/*=========================================================================

 Program: FEMUS
 Module: FemusInit
 Authors: Simone Bnà

 Copyright (c) FEMTTU
 All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "FemusInit.hpp"

#include "UqQuadratureTypeEnum.hpp"


#include <iostream>



namespace femus {


// === Automatic differentiation - BEGIN =================
adept::Stack FemusInit::_adeptStack;
// === Automatic differentiation - END =================


// === UQ Quadratures - BEGIN =================
uq FemusInit::_uqHermite(UQ_HERMITE); 
uq FemusInit::_uqLegendre(UQ_LEGENDRE); 
// === UQ Quadratures - END =================


// === Constructors / Destructor  - BEGIN =================


/// This function initializes the libraries if it is parallel
FemusInit::FemusInit(
    int & argc,            // integer program input
    char** & argv,         // char program input
    MPI_Comm comm_world_in // communicator for MPI direct
) {

#ifdef HAVE_PETSC

  int ierr = PetscInitialize (&argc, &argv, NULL, NULL);    CHKERRABORT(PETSC_COMM_WORLD,ierr);

#endif

#ifdef HAVE_MPI
    // redirect out to nothing on all
    // other processors unless explicitly told
    // not to via the --keep-cout command-line argument.
    int i;
    MPI_Comm_rank(MPI_COMM_WORLD, &i);


    if ( i != 0) {
        std::cout.rdbuf(NULL);
    }
#endif

   std::cout << " FemusInit(): PETSC_COMM_WORLD initialized" << std::endl << std::endl;

    return;
}


FemusInit::~FemusInit() {

#ifdef HAVE_PETSC
    PetscFinalize();
    std::cout << std::endl << " ~FemusInit(): PETSC_COMM_WORLD ends" << std::endl;
#endif

    return;
}


// === Constructors / Destructor  - END =================



} //end namespace femus


