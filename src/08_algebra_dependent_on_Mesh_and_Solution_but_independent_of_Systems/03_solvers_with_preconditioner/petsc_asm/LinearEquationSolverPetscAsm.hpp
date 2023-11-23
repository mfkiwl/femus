/*=========================================================================

 Program: FEMUS
 Module: PetscLinearEquationSolver
 Authors: Eugenio Aulisa, Simone Bnà

 Copyright (c) FEMTTU
 All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __femus_algebra_LinearEquationSolverPetscAsm_hpp__
#define __femus_algebra_LinearEquationSolverPetscAsm_hpp__

#include "FemusConfig.hpp"

#ifdef HAVE_PETSC

#ifdef HAVE_MPI
#include <mpi.h>
#endif

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------
#include "LinearEquationSolverPetsc.hpp"
#include "PetscVector.hpp"

#include "Mesh.hpp"

#include "FSIenum.hpp"

namespace femus {

  /**
   * This class inherits the abstract class LinearEquationSolver. In this class the solver is implemented using the PETSc package
   **/

  class LinearEquationSolverPetscAsm : public LinearEquationSolverPetsc {

    public:

      /**  Constructor. Initializes Petsc data structures */
      LinearEquationSolverPetscAsm(const unsigned &igrid, Solution *other_solution);

      /** Destructor */
      ~LinearEquationSolverPetscAsm();

    private:

      /** To be Added */
      void SetElementBlockNumber(const unsigned & block_elemet_number);
      void SetElementBlockNumberSolid(const unsigned & block_elemet_number, const unsigned & overlap);
      void SetElementBlockNumberFluid(const unsigned & block_elemet_number, const unsigned & overlap);
      void SetElementBlockNumberPorous(const unsigned& block_elemet_number, const unsigned& overlap) ;

      /** To be Added */
      void SetElementBlockNumber(const char all[], const unsigned & overlap = 1);

      /** To be Added */
      void SetNumberOfSchurVariables(const unsigned short & NSchurVar) {
        _NSchurVar = NSchurVar;
      };

      /** To be Added */
      void BuildASMIndex(const std::vector <unsigned> &variable_to_be_solved);

      void BuildBdcIndex(const std::vector <unsigned> &variable_to_be_solved) {
        if(!_standardASM){
          BuildASMIndex(variable_to_be_solved);
        }
        LinearEquationSolverPetsc::BuildBdcIndex(variable_to_be_solved);
      }
      
      void SetPreconditioner(KSP& subksp, PC& subpc);

      // data member
    private:
      
      unsigned _elementBlockNumber[ FSIMaterialIndex_SolidPorousFluid_count ];
      
      unsigned short _NSchurVar;

      std::vector< std::vector <PetscInt> > _overlappingIsIndex;
      std::vector< std::vector <PetscInt> > _localIsIndex;
      std::vector <IS> _overlappingIs;
      std::vector <IS> _localIs;

      PetscInt  _nlocal, _first;
      bool _standardASM;
      unsigned _overlap;

      std::vector <unsigned> _blockTypeRange;

  };

// =================================================

  inline LinearEquationSolverPetscAsm::LinearEquationSolverPetscAsm(const unsigned &igrid, Solution *other_solution)
    : LinearEquationSolverPetsc(igrid, other_solution) {

    unsigned dim = GetMeshFromLinEq()->GetDimension();
    unsigned base = pow(2, dim);
    unsigned exponent = 5 - dim;
    _elementBlockNumber[ SOLID_FLAG_INDEX ] = pow(base, exponent);
    _elementBlockNumber[ POROUS_FLAG_INDEX ] = pow(base, exponent);
    _NSchurVar = 1;
    _standardASM = 1;
    _overlap = 0;

  }

// =============================================

  inline LinearEquationSolverPetscAsm::~LinearEquationSolverPetscAsm() {

    for(unsigned i = 0; i < _localIs.size(); i++) {
      ISDestroy(&_localIs[i]);
    }
    for(unsigned i = 0; i < _overlappingIs.size(); i++) {
      ISDestroy(&_overlappingIs[i]);
    }

  }

} //end namespace femus


#endif
#endif
