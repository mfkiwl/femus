
#include "FemusInit.hpp"
#include "MultiLevelSolution.hpp"
#include "MultiLevelProblem.hpp"
#include "VTKWriter.hpp"
#include "TransientSystem.hpp"
#include "NonLinearImplicitSystem.hpp"

#include "NumericVector.hpp"
#include "adept.h"

#include "CurrentElem.hpp"
#include "ElemType_template.hpp"

#include "petsc.h"
#include "petscmat.h"
#include "PetscMatrix.hpp"

#include "slepceps.h"

#include "../include/sfem_assembly.hpp"

using namespace femus;

double InitalValueU(const std::vector < double >& x)
{
  return x[0] + x[1];
}

bool SetBoundaryCondition(const std::vector < double >& x, const char SolName[], double& value, const int facename, const double time)
{
  bool dirichlet = true; //dirichlet
  value = 0.;
  return dirichlet;
}

void GetHsNorm(MultiLevelProblem& ml_prob, const int& numberOfEigPairs, std::vector < std::pair<double, double> >& eigenvalues);


unsigned numberOfUniformLevels = 3;

int main(int argc, char** argv)
{


  //quadr rule order
  const std::string fe_quad_rule_1 = "fifth";
  const std::string fe_quad_rule_2 = "sixth";


  //BEGIN deterministic FEM instances

  // init Petsc-MPI communicator
  FemusInit mpinit(argc, argv, MPI_COMM_WORLD);

  MultiLevelMesh mlMsh;
  double scalingFactor = 1.;
  unsigned numberOfSelectiveLevels = 0;
  mlMsh.ReadCoarseMesh("../input/square.neu", fe_quad_rule_1.c_str(), scalingFactor);
  mlMsh.RefineMesh(numberOfUniformLevels + numberOfSelectiveLevels, numberOfUniformLevels, NULL);

  // erase all the coarse mesh levels
//   mlMsh.EraseCoarseLevels(numberOfUniformLevels - 1);

  unsigned dim = mlMsh.GetDimension();

  MultiLevelSolution mlSol(&mlMsh);

  // add variables to mlSol
  mlSol.AddSolution("u", LAGRANGE, SECOND, 2);


  //mlSol.Initialize("All");

  mlSol.Initialize("u", InitalValueU);

  MultiLevelProblem ml_prob(&mlSol);

  ml_prob.SetQuadratureRuleAllGeomElems(fe_quad_rule_2);
  ml_prob.set_all_abstract_fe();

  GetHsNorm(ml_prob, numberOfEigPairs, eigenvalues); //solve the generalized eigenvalue problem and compute the eigenpairs


  // ******* Print solution *******
  mlSol.SetWriter(VTK);
  std::vector<std::string> print_vars;
  print_vars.push_back("All");
  mlSol.GetWriter()->SetDebugOutput(true);
  mlSol.GetWriter()->Write(DEFAULT_OUTPUTDIR, "biquadratic", print_vars, 0);

  //ierr = SlepcFinalize();
  //CHKERRQ(ierr);

  return 0;

} //end main

void GetHsNorm(MultiLevelProblem& ml_prob, const int& numberOfEigPairs, std::vector < std::pair<double, double> >& eigenvalues)
{
//void GetEigenPair(MultiLevelProblem & ml_prob, Mat &CCSLEPc, Mat &MMSLEPc) {

//   LinearImplicitSystem* mlPdeSys  = &ml_prob.get_system<LinearImplicitSystem> ("UQ");   // pointer to the linear implicit system named "Poisson"

  unsigned level = numberOfUniformLevels - 1;

  double varianceInput = stdDeviationInput * stdDeviationInput;

  Mesh*                    msh = ml_prob._ml_msh->GetLevel(level);    // pointer to the mesh (level) object
  elem*                     el = msh->el;  // pointer to the elem object in msh (level)

  MultiLevelSolution*    mlSol = ml_prob._ml_sol;  // pointer to the multilevel solution object
  Solution*                sol = ml_prob._ml_sol->GetSolutionLevel(level);    // pointer to the solution (level) object

//   LinearEquationSolver* pdeSys = mlPdeSys->_LinSolver[level]; // pointer to the equation (level) object
//   SparseMatrix*             MM = pdeSys->_KK;  // pointer to the global stifness matrix object in pdeSys (level)

  const unsigned  dim = msh->GetDimension(); // get the domain dimension of the problem
  const unsigned maxSize = static_cast< unsigned >(ceil(pow(3, dim)));          // conservative: based on line3, quad9, hex27

  unsigned    iproc = msh->processor_id(); // get the process_id (for parallel computation)
  unsigned    nprocs = msh->n_processors(); // get the process_id (for parallel computation)


  CurrentElem < double > geom_element1(dim, msh);            // must be adept if the domain is moving, otherwise double
  CurrentElem < double > geom_element2(dim, msh);            // must be adept if the domain is moving, otherwise double

  constexpr unsigned int space_dim = 3;
//***************************************************
  //prepare Abstract quantities for all fe fams for all geom elems: all quadrature evaluations are performed beforehand in the main function
//***************************************************
  std::vector < std::vector < double > >  JacI_qp(space_dim);
  std::vector < std::vector < double > >  Jac_qp(dim);
  for(unsigned d = 0; d < dim; d++) {
    Jac_qp[d].resize(space_dim);
  }
  for(unsigned d = 0; d < space_dim; d++) {
    JacI_qp[d].resize(dim);
  }

  double detJac_qp;
  std::vector < std::vector < /*const*/ elem_type_templ_base< double, double > *  > > elem_all;
  ml_prob.get_all_abstract_fe(elem_all);
//***************************************************

  //solution variable
  unsigned soluIndex;
  soluIndex = mlSol->GetIndex("u");    // get the position of "u" in the ml_sol object
  unsigned solType = mlSol->GetSolutionType(soluIndex);    // get the finite element type for "u"

  std::vector < double > solu1;
  std::vector < double > solu2;

//   unsigned soluPdeIndex;
//   soluPdeIndex = mlPdeSys->GetSolPdeIndex("u");    // get the position of "u" in the pdeSys object

  unsigned xType = 2; // get the finite element type for "x", it is always 2 (LAGRANGE QUADRATIC)

  vector < vector < double > > x1(dim);    // local coordinates
  vector < vector < double > > x2(dim);    // local coordinates
  for(unsigned k = 0; k < dim; k++) {
    x1[k].reserve(maxSize);
    x2[k].reserve(maxSize);
  }

  vector <double> phi_x; // local test function first order partial derivatives

  phi_x.reserve(maxSize * dim);

//   /*vector< int > l2GMap1; // local to global mapping
//   vector< int > l2GMap2; // local to global mapping
//   l2GMap1.reserve(maxSize);
//   l2GMap2.reserve(maxSize);*/

//   vector < double > MMlocal;
//   MMlocal.reserve(maxSize * maxSize);
//
//   vector < double > CClocal;
//   CClocal.reserve(maxSize * maxSize);

//   MM->zero(); // Set to zero all the entries of the Global Matrix

//   int MM_size = msh->_dofOffset[solType][nprocs];
//   int MM_local_size = msh->_dofOffset[solType][iproc + 1] - msh->_dofOffset[solType][iproc];

//   SparseMatrix* CC;
//   CC = SparseMatrix::build().release();
//   CC->init(MM_size, MM_size, MM_local_size, MM_local_size, MM_local_size, MM_size - MM_local_size);
//   CC->zero();

  double integral_iproc = 0.;


  for(int kproc = 0; kproc < nprocs; kproc++) {
    for(int jel = msh->_elementOffset[kproc]; jel < msh->_elementOffset[kproc + 1]; jel++) {

      short unsigned ielGeom2;
      unsigned nDof2;
      unsigned nDofx2;
      unsigned nDofu2;

      if(iproc == kproc) {
        ielGeom2 = msh->GetElementType(jel);
        nDof2  = msh->GetElementDofNumber(jel, solType);    // number of solution element dofs
        nDofx2 = msh->GetElementDofNumber(jel, xType);    // number of coordinate element dofs
      }

      MPI_Bcast(&ielGeom2, 1, MPI_UNSIGNED_SHORT, kproc, MPI_COMM_WORLD);
      MPI_Bcast(&nDof2, 1, MPI_UNSIGNED, kproc, MPI_COMM_WORLD);
      MPI_Bcast(&nDofx2, 1, MPI_UNSIGNED, kproc, MPI_COMM_WORLD);

      // resize local arrays
//       l2GMap2.resize(nDof2);


      for(int k = 0; k < dim; k++) {
        x2[k].resize(nDofx2);
      }
      solu2.resize(nDof2);

      // local storage of global mapping and solution ********************
//       if(iproc == kproc) {
//         for(unsigned j = 0; j < nDof2; j++) {
//           l2GMap2[j] = pdeSys->GetSystemDof(soluIndex, soluPdeIndex, j, jel);  // global to global mapping between solution node and pdeSys dof
//         }
//       }
//       MPI_Bcast(&l2GMap2[0], nDof2, MPI_UNSIGNED, kproc, MPI_COMM_WORLD);
      // ******************************************************************

      // local storage of coordinates  #######################################
      if(iproc == kproc) {
        for(unsigned j = 0; j < nDofx2; j++) {
          unsigned xDof  = msh->GetSolutionDof(j, jel, xType);  // global to global mapping between coordinates node and coordinate dof
          for(unsigned k = 0; k < dim; k++) {
            x2[k][j] = (*msh->_topology->_Sol[k])(xDof);  // global extraction and local storage for the element coordinates
          }
        }
        for(unsigned j = 0; j < nDof2; j++) {
          unsigned jDof  = msh->GetSolutionDof(j, jel, solType);  // global to global mapping between coordinates node and coordinate dof
          solu2[j] = (*sol->_Sol[soluIndex])(jDof);  // global extraction and local storage for the element coordinates
        }
      }
      for(unsigned k = 0; k < dim; k++) {
        MPI_Bcast(& x2[k][0], nDofx2, MPI_DOUBLE, kproc, MPI_COMM_WORLD);
      }
      MPI_Bcast(& solu2[0], nDof2, MPI_DOUBLE, kproc, MPI_COMM_WORLD);
      // ######################################################################

      // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
      if(iproc == kproc) {
        geom_element2.set_coords_at_dofs_and_geom_type(jel, xType);
      }
      for(unsigned k = 0; k < dim; k++) {
        MPI_Bcast(& geom_element2.get_coords_at_dofs()[k][0], nDofx2, MPI_DOUBLE, kproc, MPI_COMM_WORLD);
      }
      for(unsigned k = 0; k < space_dim; k++) {
        MPI_Bcast(& geom_element2.get_coords_at_dofs_3d()[k][0], nDofx2, MPI_DOUBLE, kproc, MPI_COMM_WORLD);
      }
      // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

//       const unsigned jgNumber = msh->_finiteElement[ielGeom2][solType]->GetGaussPointNumber();
      const unsigned jgNumber = ml_prob.GetQuadratureRule(ielGeom2).GetGaussPointsNumber();

      vector < vector < double > > xg2(jgNumber);
      vector <double> weight2(jgNumber);
      vector < vector <double> > phi2(jgNumber);  // local test function
      std::vector< double > solY(jgNumber, 0.);

      for(unsigned jg = 0; jg < jgNumber; jg++) {

//          msh->_finiteElement[ielGeom2][solType]->Jacobian(x2, jg, weight2[jg], phi2[jg], phi_x);

        elem_all[ielGeom2][xType]->JacJacInv(/*x2*/geom_element2.get_coords_at_dofs_3d(), jg, Jac_qp, JacI_qp, detJac_qp, space_dim);
        weight2[jg] = detJac_qp * ml_prob.GetQuadratureRule(ielGeom2).GetGaussWeightsPointer()[jg];
        elem_all[ielGeom2][solType]->shape_funcs_current_elem(jg, JacI_qp, phi2[jg], phi_x /*boost::none*/, boost::none /*phi_u_xx*/, space_dim);

        xg2[jg].assign(dim, 0.);

        for(unsigned j = 0; j < nDof2; j++) {
          solY[jg] += solu2[j] * phi2[jg][j];
          for(unsigned k = 0; k < dim; k++) {
            xg2[jg][k] += x2[k][j] * phi2[jg][j];
          }
        }
      }

      // element loop: each process loops only on the elements that owns
      for(int iel = msh->_elementOffset[iproc]; iel < msh->_elementOffset[iproc + 1]; iel++) {

        short unsigned ielGeom1 = msh->GetElementType(iel);
        unsigned nDof1  = msh->GetElementDofNumber(iel, solType);    // number of solution element dofs
        unsigned nDofx1 = msh->GetElementDofNumber(iel, xType);    // number of coordinate element dofs

        // resize local arrays
//         l2GMap1.resize(nDof1);
        //std::vector<bool>bdcDirichlet(nDof1);

        for(int k = 0; k < dim; k++) {
          x1[k].resize(nDofx1);
        }
        solu1.resize(nDof1);

        // local storage of global mapping and solution
//         for(unsigned i = 0; i < nDof1; i++) {
//           l2GMap1[i] = pdeSys->GetSystemDof(soluIndex, soluPdeIndex, i, iel);    // global to global mapping between solution node and pdeSys dof
//           //unsigned solDof = msh->GetSolutionDof(i, iel, solType);    // global to global mapping between solution node and solution dof
//           //bdcDirichlet[i] = ( (*sol->_Bdc[soluIndex])(solDof) < 1.5)? false:false;
//         }

        // local storage of coordinates
        for(unsigned i = 0; i < nDofx1; i++) {
          unsigned xDof  = msh->GetSolutionDof(i, iel, xType);    // global to global mapping between coordinates node and coordinate dof
          for(unsigned k = 0; k < dim; k++) {
            x1[k][i] = (*msh->_topology->_Sol[k])(xDof);  // global extraction and local storage for the element coordinates
          }
        }
        for(unsigned i = 0; i < nDof1; i++) {
          unsigned iDof  = msh->GetSolutionDof(i, iel, solType);  // global to global mapping between coordinates node and coordinate dof
          solu1[i] = (*sol->_Sol[soluIndex])(iDof);  // global extraction and local storage for the element coordinates
        }

//         if(iel == jel) MMlocal.assign(nDof1 * nDof1, 0.);  //resize
//         CClocal.assign(nDof1 * nDof2, 0.);   //resize

        // *** Gauss point loop ***
        const unsigned igNumber = msh->_finiteElement[ielGeom1][solType]->GetGaussPointNumber();
//         const unsigned igNumber = ml_prob.GetQuadratureRule(ielGeom1).GetGaussPointsNumber();

        double weight1;
        vector < double > phi1;  // local test function
        std::vector< double > solX(igNumber, 0.);

        for(unsigned ig = 0; ig < igNumber; ig++) {

          msh->_finiteElement[ielGeom1][solType]->Jacobian(x1, ig, weight1, phi1, phi_x);

          // evaluate the solution, the solution derivatives and the coordinates in the gauss point
          vector < double > xg1(dim, 0.);
          for(unsigned i = 0; i < nDof1; i++) {
            solX[ig] += solu1[i] * phi1[i];
            for(unsigned k = 0; k < dim; k++) {
              xg1[k] += x1[k][i] * phi1[i];
            }
          }

//           if(iel == jel) {
//
//             for(unsigned i = 0; i < nDof1; i++) {
//               for(unsigned i1 = 0; i1 < nDof1; i1++) {
//                 MMlocal[ i * nDof1 + i1 ] += phi1[i] * phi1[i1] * weight1;
//               }
//             }
//
//           }

          for(unsigned jg = 0; jg < jgNumber; jg++) {
            double dist = 0;
            for(unsigned k = 0; k < dim; k++) {
              dist += fabs(xg1[k] - xg2[jg][k]);
            }

            double sol_diff = weight1 * solX[ig] - weight2[jg] * solY[jg];
            double denom = pow(dist, (dim / 2.) + 0.);

//             integral_iproc +=  weight1 *  weight2[jg];
            integral_iproc += (sol_diff * sol_diff) / denom;

//             double C = varianceInput * exp(- dist / L);

//             for(unsigned i = 0; i < nDof1; i++) {
//               for(unsigned j = 0; j < nDof2; j++) {
//                 CClocal[i * nDof2 + j] += weight1 * phi1[i] * C * phi2[jg][j] * weight2[jg];
//               }//endl j loop
//             } //endl i loop



          } //endl jg loop
        } //endl ig loop
        //  if(iel == jel) MM->add_matrix_blocked(MMlocal, l2GMap1, l2GMap1);
        // CC->add_matrix_blocked(CClocal, l2GMap1, l2GMap2);
      } // end iel loop
    } //end jel loop
  } //end kproc loop

//   MM->close();
//   CC->close();

  ////////////////////////////////////////

  printf("integral on processor %d = %f \n", iproc, integral_iproc);

  double J = 0.;
  MPI_Allreduce(&integral_iproc, &J, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);     //THIS IS THE RIGHT ONE!!

  std::cout << "integral after Allreduce: " << sqrt(J) << std::endl;

  //return;                                                  //ignore the rest

  // ***************** END ASSEMBLY *******************
}
