#include "FemusInit.hpp"
#include "MultiLevelProblem.hpp"
#include "NumericVector.hpp"
#include "VTKWriter.hpp"
#include "GMVWriter.hpp"
#include "NonLinearImplicitSystem.hpp"
#include "adept.h"
#include "LinearImplicitSystem.hpp"
#include "Parameter.hpp"
#include "Fluid.hpp"
#include "Solid.hpp"
#include "Files.hpp"
#include "FE_convergence.hpp"
#include "Assemble_jacobian.hpp"


#define MODEL "Linear_elastic"
// #define MODEL "Mooney-Rivlin" 
// #define MODEL "Neo-Hookean"

using namespace femus;

  
double Solution_set_initial_conditions(const MultiLevelProblem * ml_prob, const std::vector < double >& x, const char name[]) {
         
           double value = 0.;

             if(!strcmp(name,"DX")) {
                 value = 0.;
             }
             else if(!strcmp(name,"DY")) {
                 value = 0.;
             }
             else if(!strcmp(name,"DZ")) {
                 value = 0.;
             }
             else if(!strcmp(name,"P")) {
                 value = 0.;
             }

             else if(!strcmp(name,"DX_ADJ")) {
                 value = 0.;
             }
             else if(!strcmp(name,"DY_ADJ")) {
                 value = 0.;
             }
             else if(!strcmp(name,"DZ_ADJ")) {
                 value = 0.;
             }
             else if(!strcmp(name,"P_ADJ")) {
                 value = 0.;
             }
           
    
      return value;   
}  
  
  
  

bool Solution_set_boundary_conditions(const MultiLevelProblem * ml_prob, const std::vector < double >& x, const char SolName[], double& value, const int facename, const double time) {
  //1: bottom (y=y_min) //2: right (x=x_max)  //3: top (y=y_max)  //4: left (x=x_min)  (in 2D)
  //1: bottom (y=y_min) //2: right (x=x_max)  //3: top (y=y_max)  //4: left (x=x_min) //5: (z=z_min)  //6:  (z=z_max) (in 3D, I guess...)
  
  bool dirichlet;
  
      if (facename == 1) {
       if (!strcmp(SolName, "DX"))        { dirichlet = true; value = 0.; }
  else if (!strcmp(SolName, "DY"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DZ"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DX_ADJ"))    { dirichlet = true; value = 0.; }
  else if (!strcmp(SolName, "DY_ADJ"))    { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DZ_ADJ"))    { dirichlet = true; value = 0.; } 
  	
      }

      if (facename == 2) {
       if (!strcmp(SolName, "DX"))        { dirichlet = false/*true*/; value = 0.; }
  else if (!strcmp(SolName, "DY"))        { dirichlet = false/*true*/; value = 0.; } 
  else if (!strcmp(SolName, "DZ"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DX_ADJ"))    { dirichlet = false/*true*/; value = 0.; }
  else if (!strcmp(SolName, "DY_ADJ"))    { dirichlet = false/*true*/; value = 0.; } 
  else if (!strcmp(SolName, "DZ_ADJ"))    { dirichlet = true; value = 0.; } 
  	
      }

      if (facename == 3) {
       if (!strcmp(SolName, "DX"))        { dirichlet = true; value = 0.; }
  else if (!strcmp(SolName, "DY"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DZ"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DX_ADJ"))    { dirichlet = true; value = 0.; }
  else if (!strcmp(SolName, "DY_ADJ"))    { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DZ_ADJ"))    { dirichlet = true; value = 0.; } 
  	
      }

      if (facename == 4) {
       if (!strcmp(SolName, "DX"))        { dirichlet = false/*true*/; value = 0.; }
  else if (!strcmp(SolName, "DY"))        { dirichlet = false/*true*/; value = 0.; } 
  else if (!strcmp(SolName, "DZ"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DX_ADJ"))    { dirichlet = false/*true*/; value = 0.; }
  else if (!strcmp(SolName, "DY_ADJ"))    { dirichlet = false/*true*/; value = 0.; } 
  else if (!strcmp(SolName, "DZ_ADJ"))    { dirichlet = true; value = 0.; } 
  	
      }
      
      if (facename == 5) {
       if (!strcmp(SolName, "DX"))        { dirichlet = false; value = 0.; }
  else if (!strcmp(SolName, "DY"))        { dirichlet = false; value = 0.; } 
  else if (!strcmp(SolName, "DZ"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DX_ADJ"))    { dirichlet = true; value = 0.; }
  else if (!strcmp(SolName, "DY_ADJ"))    { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DZ_ADJ"))    { dirichlet = true; value = 0.; } 
     }
      
      if (facename == 6) {
       if (!strcmp(SolName, "DX"))        { dirichlet = false; value = 0.; }
  else if (!strcmp(SolName, "DY"))        { dirichlet = false; value = 0.; } 
  else if (!strcmp(SolName, "DZ"))        { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DX_ADJ"))    { dirichlet = true; value = 0.; }
  else if (!strcmp(SolName, "DY_ADJ"))    { dirichlet = true; value = 0.; } 
  else if (!strcmp(SolName, "DZ_ADJ"))    { dirichlet = true; value = 0.; } 
      }
      
 return dirichlet;
}


template < class system_type, class real_num, class real_num_mov >
void AssembleSolidMech(MultiLevelProblem& ml_prob);


template < class system_type, class real_num, class real_num_mov >
void AssembleSolidMech(MultiLevelProblem& ml_prob,
                       const Solid & solid_in,
                       system_type * mlPdeSys,
                       const std::vector< Unknown > &  unknowns);





 //Unknown definition  ==================
 const std::vector< Unknown >  provide_list_of_unknowns(const unsigned int dimension) {
     
     
  std::vector< FEFamily > feFamily;
  std::vector< FEOrder >   feOrder;

                        feFamily.push_back(LAGRANGE);   //state
                        feFamily.push_back(LAGRANGE);
  if (dimension == 3)   feFamily.push_back(LAGRANGE);
                        feFamily.push_back(LAGRANGE/*DISCONTINOUS_POLYNOMIAL*/);
                        feFamily.push_back(LAGRANGE);   //adjoint
                        feFamily.push_back(LAGRANGE);
  if (dimension == 3)   feFamily.push_back(LAGRANGE);
                        feFamily.push_back(LAGRANGE/*DISCONTINOUS_POLYNOMIAL*/);
 
                        feOrder.push_back(SECOND);
                        feOrder.push_back(SECOND);
  if (dimension == 3)   feOrder.push_back(SECOND);
                        feOrder.push_back(FIRST);
                        feOrder.push_back(SECOND);
                        feOrder.push_back(SECOND);
  if (dimension == 3)   feOrder.push_back(SECOND);
                        feOrder.push_back(FIRST);
 

  assert( feFamily.size() == feOrder.size() );
 
 std::vector< Unknown >  unknowns(feFamily.size());
 
  const int adj_pos_begin   = dimension + 1;

                                        unknowns[0]._name      = "DX";
                                        unknowns[1]._name      = "DY";
  if (dimension == 3)                   unknowns[2]._name      = "DZ";
                                unknowns[dimension]._name      = "P";
                        unknowns[adj_pos_begin + 0]._name      = "DX_ADJ";
                        unknowns[adj_pos_begin + 1]._name      = "DY_ADJ";
  if (dimension == 3)   unknowns[adj_pos_begin + 2]._name      = "DZ_ADJ";
                unknowns[adj_pos_begin + dimension]._name      = "P_ADJ";

     for (unsigned int u = 0; u < unknowns.size(); u++) {
         
              unknowns[u]._fe_family  = feFamily[u];
              unknowns[u]._fe_order   = feOrder[u];
              unknowns[u]._time_order = 0;
              unknowns[u]._is_pde_unknown = true;
              
     }
 
 
   return unknowns;
     
}




template < class real_num > 
class My_main_single_level : public Main_single_level {
    
public:
    
const MultiLevelSolution  run_on_single_level(const Files & files,
                                              MultiLevelProblem & ml_prob,
                                              const std::vector< Unknown > & unknowns,
                                              const MultiLevelSolution::BoundaryFuncMLProb SetBoundaryCondition_in,
                                              const MultiLevelSolution::InitFuncMLProb SetInitialCondition_in,
                                              MultiLevelMesh & ml_mesh,
                                              const unsigned i) const;
  
};
 






int main(int argc, char** args) {

  // ======= Init ==========================
  FemusInit mpinit(argc, args, MPI_COMM_WORLD);

  // ======= Files ==================
  Files files;
        files.CheckIODirectories();
        files.RedirectCout();

  // ======= Quad Rule ========================
  std::string quad_rule_order("seventh");

  // ======= Problem ========================
    MultiLevelProblem ml_prob;
  ml_prob.SetQuadratureRuleAllGeomElems(quad_rule_order);
  ml_prob.SetFilesHandler(&files);
    
  // ======= Mesh ==================
  const ElemType geom_elem_type = QUAD9;
  const std::vector< unsigned int > nsub = {2, 2, 0};
  const std::vector< double >      xyz_min = {0., 0., 0.};
  const std::vector< double >      xyz_max = {1., 1., 0.};

  MultiLevelMesh ml_mesh;
  ml_mesh.GenerateCoarseBoxMesh(nsub[0],nsub[1],nsub[2],xyz_min[0],xyz_max[0],xyz_min[1],xyz_max[1],xyz_min[2],xyz_max[2],geom_elem_type,quad_rule_order.c_str());

  // ======= Unknowns ========================
  const unsigned int dimension = ml_mesh.GetDimension();  
  std::vector< Unknown > unknowns = provide_list_of_unknowns(dimension);
  
  // ======= Normal run ========================   //if you don't want the convergence study
  My_main_single_level< adept::adouble > my_main;
//   const unsigned int n_levels = 1;
//   my_main.run_on_single_level(files, ml_prob, quad_rule_order, unknowns, Solution_set_boundary_conditions, Solution_set_initial_conditions, ml_mesh, n_levels); 
 
  
  
  // ======= Convergence study ========================
    
   //set coarse storage mesh (should write the copy constructor or "=" operator to copy the previous mesh) ==================
   MultiLevelMesh ml_mesh_all_levels;
   ml_mesh_all_levels.GenerateCoarseBoxMesh(nsub[0],nsub[1],nsub[2],xyz_min[0],xyz_max[0],xyz_min[1],xyz_max[1],xyz_min[2],xyz_max[2],geom_elem_type,quad_rule_order.c_str());
   //   ml_mesh_all_levels.ReadCoarseMesh(infile.c_str(),quad_rule_order.c_str(),1.);
 
   // convergence choices ================  
   unsigned int max_number_of_meshes;               // set total number of levels ================  

   if (nsub[2] == 0)   max_number_of_meshes = 3;
   else                max_number_of_meshes = 4;
  
//    My_exact_solution<> exact_sol;                //provide exact solution, if available ==============
   const unsigned conv_order_flag = 0;              //Choose how to compute the convergence order ========= //0: incremental 1: absolute (with analytical sol)  2: absolute (with projection of finest sol)...
   const unsigned norm_flag = 1;                    //Choose what norms to compute (//0 = only L2: //1 = L2 + H1) ==============

   
   // object ================  
    FE_convergence<>  fe_convergence;
    
    fe_convergence.convergence_study(files, ml_prob, unknowns, Solution_set_boundary_conditions, Solution_set_initial_conditions, ml_mesh, ml_mesh_all_levels, max_number_of_meshes, norm_flag, conv_order_flag, my_main);
  
    
  return 0;
}



template < class system_type, class real_num, class real_num_mov = double >
void AssembleSolidMech(MultiLevelProblem& ml_prob) {
    

  AssembleSolidMech< system_type, real_num, real_num_mov > (  ml_prob, 
                                                              ml_prob.parameters.get<Solid>("Solid"), 
                                                            & ml_prob.get_system< system_type >(0), 
                                                              ml_prob.get_system< system_type >(0).get_unknown_list_for_assembly());

}



template < class system_type, class real_num, class real_num_mov = double >
void AssembleSolidMech(MultiLevelProblem& ml_prob,
                       const Solid & solid_in,
                       system_type * mlPdeSys,
                       const std::vector< Unknown > &  unknowns) {
    
  //  ml_prob is the global object from/to where get/set all the data
  //  level is the level of the PDE system to be assembled
  //  levelMax is the Maximum level of the MultiLevelProblem
  //  assembleMatrix is a flag that tells if only the residual or also the matrix should be assembled


  const unsigned 		level 		    = mlPdeSys->GetLevelToAssemble();
  bool 			assembleMatrix 		    = mlPdeSys->GetAssembleMatrix(); 
  const char* 			pdename         = mlPdeSys->name().c_str();

  Mesh*          		msh    		= ml_prob._ml_msh->GetLevel(level);
  elem*          		el     		= msh->el;

  MultiLevelSolution*  		ml_sol  = ml_prob._ml_sol;
  Solution*    			sol      	= ml_prob._ml_sol->GetSolutionLevel(level); 


  LinearEquationSolver* 	pdeSys  = mlPdeSys->_LinSolver[level];
  SparseMatrix*    		JAC    		= pdeSys->_KK;
  NumericVector*   		RES         = pdeSys->_RES;


  const unsigned 	 dim = msh->GetDimension();
  const unsigned    dim2 = (3 * (dim - 1) + !(dim - 1));
  const unsigned   nel   = msh->GetNumberOfElements();
  const unsigned igrid   = msh->GetLevel();
  const unsigned  iproc  = msh->processor_id();
  // reserve memory for the local standard vectors
  const unsigned max_size_elem_dofs = static_cast< unsigned >(ceil(pow(3, dim)));          // conservative: based on line3, quad9, hex27

 
  
  //----------- unknowns ------------------------------
   const unsigned int n_unknowns = mlPdeSys->GetSolPdeIndex().size();
//      enum Sol_pos {pos_dx = 0, pos_dy, pos_p};  //these are known at compile-time
//      enum Sol_pos {pos_dx = 0, pos_dy, pos_dz, pos_p};
//   constexpr unsigned int pos_dx = 0;  
//   constexpr unsigned int pos_dy = 1;  
//   constexpr unsigned int pos_dp = 2;  
//   if (dim == 3) {std::cout << "Uncomment the 3d enum and recompile"; abort(); }
   
  constexpr int sol_index_displ = 0;     //known at compile time
  const     int sol_index_press = sol_index_displ + dim;      //known at run time
  constexpr int state_pos_begin = sol_index_displ;   //known at compile time
  const int adj_pos_begin = dim + 1;
  const int sol_index_adj = adj_pos_begin;      //known at run time
  const int sol_index_press_adj = sol_index_adj + dim;      //known at run time

  const int  n_state_vars = dim + 1;
  
  vector < std::string > Solname(n_unknowns);     
  vector < unsigned int > SolIndex(n_unknowns);  
  vector < unsigned int > SolPdeIndex(n_unknowns);
  vector < unsigned int > SolFEType(n_unknowns);  


  for(unsigned ivar=0; ivar < n_unknowns; ivar++) {
       Solname[ivar]    = unknowns[ivar]._name; 
      SolIndex[ivar]	= ml_sol->GetIndex        (Solname[ivar].c_str());
    SolPdeIndex[ivar]	= mlPdeSys->GetSolPdeIndex(Solname[ivar].c_str());
//       assert(ivar == SolPdeIndex[ivar]);  //I would like this ivar order to coincide either with SolIndex or with SolPdeIndex, otherwise too many orders... it has to be  with SolPdeIndex, because SolIndex is related to the Solution object which can have more fields... This has to match the order of the unknowns[] argument
    SolFEType[ivar]	= ml_sol->GetSolutionType(SolIndex[ivar]);
  }

   constexpr int solFEType_max = BIQUADR_FE;  //biquadratic, this is the highest-order FE 

   vector < unsigned int > Sol_n_el_dofs(n_unknowns);
  
  //----------- of dofs and at quadrature points ------------------------------
  vector < vector < double > > phi_dof_qp(n_unknowns);
  vector < vector < real_num_mov > > phi_x_dof_qp(n_unknowns);   //the derivatives depend on the Jacobian, which depends on the unknown, so we have to be adept here  //some of these should be real_num, some real_num_mov...
  vector < vector < real_num_mov > > phi_xx_dof_qp(n_unknowns);  //the derivatives depend on the Jacobian, which depends on the unknown, so we have to be adept here

  vector < vector < double > > phi_hat_dof_qp(n_unknowns);
  vector < vector < double > > phi_x_hat_dof_qp(n_unknowns);
  vector < vector < double > > phi_xx_hat_dof_qp(n_unknowns);

  for(int fe=0; fe < n_unknowns; fe++) {  
        phi_dof_qp[fe].reserve(max_size_elem_dofs);
    phi_hat_dof_qp[fe].reserve(max_size_elem_dofs);
      phi_x_dof_qp[fe].reserve(max_size_elem_dofs * dim);
  phi_x_hat_dof_qp[fe].reserve(max_size_elem_dofs * dim);
     phi_xx_dof_qp[fe].reserve(max_size_elem_dofs * dim2);
 phi_xx_hat_dof_qp[fe].reserve(max_size_elem_dofs * dim2);
   }
   
  //----------- at dofs ------------------------------
  vector < double >   Jac;   Jac.reserve( n_unknowns * max_size_elem_dofs * n_unknowns * max_size_elem_dofs);
  vector < real_num > Res;   Res.reserve( n_unknowns * max_size_elem_dofs);
           vector < int >       L2G_dofmap_AllVars;   L2G_dofmap_AllVars.reserve( n_unknowns *max_size_elem_dofs);
  vector < vector < int > >         L2G_dofmap(n_unknowns);  for(int i = 0; i < n_unknowns; i++) {    L2G_dofmap[i].reserve(max_size_elem_dofs); }
  vector < vector < real_num > > SolVAR_eldofs(n_unknowns);  for(int k = 0; k < n_unknowns; k++) { SolVAR_eldofs[k].reserve(max_size_elem_dofs); }
 //=================state_only - for Jac retrieval via AD
  vector < double >   Jac_state;   Jac_state.reserve( n_state_vars * max_size_elem_dofs * n_state_vars * max_size_elem_dofs);
  vector < real_num > Res_state;   Res_state.reserve( n_state_vars * max_size_elem_dofs);
 

  // geometry (at dofs) --------------------------------
  vector< vector < real_num_mov > >   coords(dim);
  vector  < vector  < double > >  coords_hat(dim);
  unsigned coordsType = BIQUADR_FE; // get the finite element type for "x", it is always 2 (LAGRANGE TENSOR-PRODUCT-QUADRATIC)
  for(int i = 0; i < dim; i++) {   
           coords[i].reserve(max_size_elem_dofs); 
       coords_hat[i].reserve(max_size_elem_dofs); 
 }
 
 //************** geometry phi **************************
  vector < vector < double > > phi_dof_qp_domain(dim);
  vector < vector < real_num_mov > > phi_x_dof_qp_domain(dim);
  vector < vector < real_num_mov > > phi_xx_dof_qp_domain(dim);
  vector < vector < double > > phi_dof_qp_domain_hat(dim);
  vector < vector < double > > phi_x_dof_qp_domain_hat(dim);
  vector < vector < double > > phi_xx_dof_qp_domain_hat(dim);
 
  for(int fe = 0; fe < dim; fe++) {
        phi_dof_qp_domain[fe].reserve(max_size_elem_dofs);
      phi_x_dof_qp_domain[fe].reserve(max_size_elem_dofs * dim);
     phi_xx_dof_qp_domain[fe].reserve(max_size_elem_dofs * dim2);
        phi_dof_qp_domain_hat[fe].reserve(max_size_elem_dofs);
      phi_x_dof_qp_domain_hat[fe].reserve(max_size_elem_dofs * dim);
     phi_xx_dof_qp_domain_hat[fe].reserve(max_size_elem_dofs * dim2);
   }
 
 
 
  // geometry ------------------------------------------
  

  //------------ at quadrature points ---------------------
  real_num_mov weight_qp = 0.;
    double weight_hat_qp = 0.;
    vector < real_num > SolVAR_qp(n_unknowns);
    vector < vector < real_num > > gradSolVAR_qp(n_unknowns);
    vector < vector < real_num > > gradSolVAR_hat_qp(n_unknowns);
    for(int k = 0; k < n_unknowns; k++) { 
          gradSolVAR_qp[k].resize(dim);
      gradSolVAR_hat_qp[k].resize(dim);
    }
      

   // ------------------------------------------------------------------------
    // Physical parameters
    const int    solid_model	= solid_in.get_physical_model();
    const double mu_lame 	    = solid_in.get_lame_shear_modulus();
    const double lambda_lame 	= solid_in.get_lame_lambda();

    const bool incompressible   = (0.5  ==  solid_in.get_poisson_coeff()) ? 1 : 0;
    const bool penalty = solid_in.get_if_penalty();

    // gravity
    double _gravity[3] = {1000., 0., 0.};
    // -----------------------------------------------------------------
 
    RES->zero();
  if (assembleMatrix) JAC->zero();
  
  adept::Stack & stack = FemusInit::_adeptStack;  // call the adept stack object for potential use of AD
  const assemble_jacobian< real_num, double/*real_num_mov*/ > assemble_jac;
 //=================state_only - for Jac retrieval via AD
  adept::Stack & stack_state = FemusInit::_adeptStack;  // call the adept stack object for potential use of AD
  const assemble_jacobian< real_num, double/*real_num_mov*/ > assemble_jac_state;


  for (int iel = msh->_elementOffset[iproc]; iel < msh->_elementOffset[iproc + 1]; iel++) {


   //all vars ###################################################################  
  for (unsigned  k = 0; k < n_unknowns; k++) {
    unsigned ndofs_unk = msh->GetElementDofNumber(iel, SolFEType[k]);
       Sol_n_el_dofs[k] = ndofs_unk;
       SolVAR_eldofs[k].resize(Sol_n_el_dofs[k]);
          L2G_dofmap[k].resize(Sol_n_el_dofs[k]); 
    for (unsigned i = 0; i < SolVAR_eldofs[k].size(); i++) {
       unsigned solDof = msh->GetSolutionDof(i, iel, SolFEType[k]);
       SolVAR_eldofs[k][i] = (*sol->_Sol[SolIndex[k]])(solDof);
          L2G_dofmap[k][i] = pdeSys->GetSystemDof(SolIndex[k], SolPdeIndex[k], i, iel);    // global to global mapping between solution node and pdeSys dof
      }
    }
    
    L2G_dofmap_AllVars.resize(0);
      for (unsigned  k = 0; k < n_unknowns; k++)     L2G_dofmap_AllVars.insert(L2G_dofmap_AllVars.end(),L2G_dofmap[k].begin(),L2G_dofmap[k].end());

    unsigned sum_Sol_n_el_dofs = 0;
    for (unsigned  k = 0; k < n_unknowns; k++) { sum_Sol_n_el_dofs += Sol_n_el_dofs[k]; }
    Jac.resize(sum_Sol_n_el_dofs * sum_Sol_n_el_dofs);  std::fill(Jac.begin(), Jac.end(), 0.);
    Res.resize(sum_Sol_n_el_dofs);                      std::fill(Res.begin(), Res.end(), 0.);
 
 //=================state_only - for Jac retrieval via AD
    unsigned state_sum_Sol_n_el_dofs = 0;
    for (unsigned  k = 0; k < n_state_vars; k++) { state_sum_Sol_n_el_dofs += Sol_n_el_dofs[k]; }
    Jac_state.resize(state_sum_Sol_n_el_dofs * state_sum_Sol_n_el_dofs);    std::fill(Jac_state.begin(), Jac_state.end(), 0.);
    Res_state.resize(state_sum_Sol_n_el_dofs);                              std::fill(Res_state.begin(), Res_state.end(), 0.);
  //all vars ###################################################################
  
    
  // geometry *****************************
    short unsigned ielGeom = msh->GetElementType(iel);

      unsigned nDofsX = msh->GetElementDofNumber(iel, coordsType);    // number of coordinate element dofs
    
    for(int ivar=0; ivar<dim; ivar++) {
      coords[ivar].resize(nDofsX);
      coords_hat[ivar].resize(nDofsX);
    }
    
   for( unsigned i = 0; i < nDofsX; i++) {
      unsigned coordsDof  = msh->GetSolutionDof(i, iel, coordsType);    // global to global mapping between coordinates node and coordinate dof // via local to global solution node
      for(unsigned ivar = 0; ivar < dim; ivar++) {
          //Fixed coordinates (Reference frame)
	coords_hat[ivar][i] = (*msh->_topology->_Sol[ivar])(coordsDof);
      }
    }

  //***************************************  
    assert(nDofsX == Sol_n_el_dofs[sol_index_displ]);
    //Moving coordinates (Moving frame)
      for (unsigned idim = 0; idim < dim; idim++) {
        for (int j = 0; j < nDofsX; j++) {
          coords[idim][j] = coords_hat[idim][j] + SolVAR_eldofs[SolIndex[idim]][j];
        }
      }
  
  // geometry end *****************************

      
     assemble_jac.prepare_before_integration_loop(stack);
     assemble_jac_state.prepare_before_integration_loop(stack_state); //=================state_only - for Jac retrieval via AD
     
  
    // *** Gauss point loop ***
    for (unsigned ig = 0; ig < ml_prob.GetQuadratureRule(ielGeom).GetGaussPointsNumber(); ig++) {

	// *** get Jacobian and test function and test function derivatives ***
      for(int fe=0; fe < n_unknowns; fe++) {
	msh->_finiteElement[ielGeom][SolFEType[fe]]->Jacobian(coords,    ig,weight_qp,    phi_dof_qp[fe],    phi_x_dof_qp[fe],    phi_xx_dof_qp[fe]);
	msh->_finiteElement[ielGeom][SolFEType[fe]]->Jacobian(coords_hat,ig,weight_hat_qp,phi_hat_dof_qp[fe],phi_x_hat_dof_qp[fe],phi_xx_hat_dof_qp[fe]);
      }
      
   //=============== Integration at qp ========================================
         //HAVE TO RECALL IT TO HAVE BIQUADRATIC JACOBIAN...
    for (unsigned int d = 0; d < dim; d++) {
         msh->_finiteElement[ielGeom][coordsType]->Jacobian(coords,    ig, weight_qp,    phi_dof_qp_domain[d],     phi_x_dof_qp_domain[d],     phi_xx_dof_qp_domain[d]);
         msh->_finiteElement[ielGeom][coordsType]->Jacobian(coords_hat,ig, weight_hat_qp,phi_dof_qp_domain_hat[d], phi_x_dof_qp_domain_hat[d], phi_xx_dof_qp_domain_hat[d]);
      }
     
  //begin unknowns eval at gauss points ********************************
	for(unsigned unk = 0; unk <  n_unknowns; unk++) {
	  SolVAR_qp[unk] = 0.;

	  for(unsigned ivar2=0; ivar2<dim; ivar2++){ 
	    gradSolVAR_qp[unk][ivar2] = 0.; 
	    gradSolVAR_hat_qp[unk][ivar2] = 0.; 
	  }
	  
	  for(unsigned i = 0; i < Sol_n_el_dofs[unk]; i++) {
	    SolVAR_qp[unk]    += phi_dof_qp[ unk ][i] * SolVAR_eldofs[unk][i];
	    for(unsigned ivar2 = 0; ivar2 < dim; ivar2++) {
	      gradSolVAR_qp[unk][ivar2]     += phi_x_dof_qp[ unk ][i*dim+ivar2] * SolVAR_eldofs[unk][i]; 
	      gradSolVAR_hat_qp[unk][ivar2] += phi_x_hat_dof_qp[ unk ][i*dim+ivar2] * SolVAR_eldofs[unk][i]; 
	    }
	  }
	  
	}  
 //end unknowns eval at gauss points ********************************


//  assemble_jacobian< real_num, real_num_mov >::mass_residual (Res, Sol_n_el_dofs, sum_Sol_n_el_dofs, SolPdeIndex, phi_dof_qp, SolVAR_qp, weight_hat_qp); //identity


 //*******************************************************************************************************
   vector < vector < real_num_mov > > Cauchy(3); for (int i = 0; i < Cauchy.size(); i++) Cauchy[i].resize(3);
   real_num_mov J_hat;
   real_num_mov trace_e_hat;

    Cauchy = Solid::get_Cauchy_stress_tensor< real_num_mov >(solid_model, mu_lame, lambda_lame, dim, sol_index_displ, sol_index_press, gradSolVAR_hat_qp, SolVAR_qp, SolPdeIndex, J_hat, trace_e_hat);

    

              //BEGIN residual Solid Momentum in moving domain
          for (unsigned i = 0; i < Sol_n_el_dofs[sol_index_displ]; i++) {

              real_num_mov Cauchy_direction[3] = {0., 0., 0.};

              for (int idim = 0.; idim < dim; idim++) {
                for (int jdim = 0.; jdim < dim; jdim++) {
                  Cauchy_direction[idim] += phi_x_dof_qp[ idim ][i * dim + jdim] * Cauchy[idim][jdim];
                }
              }

              for (int idim = 0; idim < dim; idim++) {
                Res[ assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, SolPdeIndex[idim], i) ] += ( Cauchy_direction[idim] -  phi_dof_qp[ idim ][i] * _gravity[idim] ) * weight_qp;
              }

            }
              //END residual Solid Momentum in moving domain
              

              //BEGIN residual solid mass balance in reference domain
            for (unsigned i = 0; i < Sol_n_el_dofs[sol_index_press]; i++) {
                
              Res[ assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, SolPdeIndex[sol_index_press], i) ] += 
             weight_hat_qp * phi_dof_qp[ sol_index_press ][i] * Solid::get_mass_balance_reference_domain< real_num_mov >(solid_model, penalty, incompressible, lambda_lame, trace_e_hat, J_hat, SolVAR_qp, SolPdeIndex, sol_index_press);
//               weight_qp * phi_dof_qp[ sol_index_press ][i] * Solid::get_mass_balance_moving_domain< real_num_mov >(gradSolVAR_qp, SolPdeIndex);
              
            }
              //END residual solid mass balance in reference domain
// 
//  // I x = 5 for adjoint block---------------------------
//   for(unsigned adj_unk = 0 + adj_pos_begin; adj_unk < n_unknowns; adj_unk++) { 
//          for (unsigned i = 0; i < Sol_n_el_dofs[adj_unk]; i++) {
//                 Res[ assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, SolPdeIndex[adj_unk], i) ] += (5. - SolVAR_qp[SolPdeIndex[adj_unk]]) *  phi_dof_qp[ adj_unk ][i] * weight_qp;
//               }
//           }
//  // I x = 5 for adjoint block---------------------------
 
 //ADJOINT=====================================
         for (unsigned i = 0; i < Sol_n_el_dofs[sol_index_adj]; i++) {
               for (int idim = 0; idim < dim; idim++) {
   Res[assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, SolPdeIndex[idim + adj_pos_begin], i)] += 
   SolVAR_qp[SolPdeIndex[idim]] * phi_dof_qp[ idim + adj_pos_begin ][i] * weight_qp;
               }
          }
 //ADJOINT=====================================
    } // end gauss point loop

    //--------------------------------------------------------------------------------------------------------

    
  //ADJOINT=====================================
 for (int i = 0; i < Res_state.size(); i++) {
        Res_state[i] = Res[i];
  }
  
    std::vector < double > Res_state_double(Res_state.size());

    for (int i = 0; i < Res_state_double.size(); i++) {
      Res_state_double[i] = - Res_state[i].value();
    }


    stack_state.dependent(  & Res_state[0], Res_state_double.size());      // define the dependent variables
    for (unsigned  k = 0; k < n_state_vars; k++) {   
    stack_state.independent(&SolVAR_eldofs[k][0],       SolVAR_eldofs[k].size());    // define the independent variables
    }
    
    stack_state.jacobian(&Jac_state[0], false);    // get the jacobian matrix (ordered by column major for Adjoint) // Adj = transpose of state block

    stack_state.clear_independents();
    stack_state.clear_dependents();    


      for(unsigned i_block = adj_pos_begin; i_block < n_unknowns; i_block++) {
        for(unsigned i_dof = 0; i_dof < Sol_n_el_dofs[i_block]; i_dof++) {
            for(unsigned j_block = 0; j_block < n_state_vars; j_block++) {
               for(unsigned j_dof = 0; j_dof < Sol_n_el_dofs[j_block]; j_dof++) {
    Res[assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, i_block, i_dof)] += 
    Jac_state[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, state_sum_Sol_n_el_dofs,i_block - n_state_vars , j_block, i_dof, j_dof )] * SolVAR_eldofs[j_block + adj_pos_begin][j_dof];
                }
           }
        }
   }
  //ADJOINT=====================================

       
     if (assembleMatrix) assemble_jac.compute_jacobian_outside_integration_loop(stack, SolVAR_eldofs, Res, Jac, L2G_dofmap_AllVars, RES, JAC);


// // //      for (int idim = 0; idim < dim; idim++) {
// // //           for (unsigned i = 0; i < Sol_n_el_dofs[sol_index_adj]; i++) {
// // //     Res[assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, adj_pos_begin + idim, i) ] =    
// // //     Res[assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, idim, i) ];    
// // // //              for (unsigned j = 0; j < Sol_n_el_dofs[sol_index_adj]; j++) {
// // // //     Jac[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, sum_Sol_n_el_dofs, adj_pos_begin + idim , adj_pos_begin + idim , i, j) ] =   
// // // //     Jac[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, sum_Sol_n_el_dofs, /*adj_pos_begin +*/ idim , /*adj_pos_begin +*/ idim , i, j) ];
// // // //           }
// // //        }      
// // //     }    
// // // 
// // // //          for (int idim = 0; idim < dim; idim++) {
// // // //              for (unsigned i = 0; i < Sol_n_el_dofs[sol_index_displ]; i++) {
// // // //           for (unsigned j = 0; j < Sol_n_el_dofs[sol_index_press_adj]; j++) {
// // // //     Jac[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, sum_Sol_n_el_dofs, sol_index_adj + idim, sol_index_press_adj, i, j) ] =   
// // // //     Jac[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, sum_Sol_n_el_dofs, sol_index_displ + idim, sol_index_press,   i, j) ];
// // // //             }
// // // //                        
// // // //          }
// // // //        }      
// // // 
// // //     for (unsigned i = 0; i < Sol_n_el_dofs[sol_index_press_adj]; i++) {
// // //     Res[assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, sol_index_press_adj, i) ] =    
// // //     Res[assemble_jacobian<double,double>::res_row_index(Sol_n_el_dofs, sol_index_press, i) ];
// // // //                    for (int idim = 0; idim < dim; idim++) {
// // // //              for (unsigned j = 0; j < Sol_n_el_dofs[sol_index_displ]; j++) {
// // // //     Jac[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, sum_Sol_n_el_dofs, sol_index_press_adj, sol_index_adj + idim, i, j) ] =   
// // // //     Jac[assemble_jacobian<double,double>::jac_row_col_index(Sol_n_el_dofs, sum_Sol_n_el_dofs, sol_index_press,     sol_index_displ + idim, i, j) ];
// // // //             }
// // // //                        
// // // //          }
// // //        }

       
    assemble_jacobian<real_num,real_num_mov>::print_element_residual(iel, Res, Sol_n_el_dofs, 9, 5);
    assemble_jacobian<real_num,real_num_mov>::print_element_jacobian(iel, Jac, Sol_n_el_dofs, 9, 5);
    
  } //end element loop for each process


 if (assembleMatrix) JAC->close();
                     RES->close();
  
  
  
    //print JAC and RES to files
//     const unsigned nonlin_iter = mlPdeSys->GetNonlinearIt();
//     assemble_jacobian< real_num_mov,double >::print_global_jacobian(assembleMatrix, ml_prob, JAC, nonlin_iter);
//     assemble_jacobian< real_num_mov,double >::print_global_residual(ml_prob, RES, nonlin_iter);

  
}




template < class real_num > 
const MultiLevelSolution  My_main_single_level< real_num >::run_on_single_level(const Files & files,
                                                                                MultiLevelProblem & ml_prob,
                                                                                const std::vector< Unknown > &  unknowns,
                                                                                const MultiLevelSolution::BoundaryFuncMLProb SetBoundaryCondition_in,
                                                                                const MultiLevelSolution::InitFuncMLProb SetInitialCondition_in,
                                                                                MultiLevelMesh & ml_mesh,
                                                                                const unsigned lev) const {
                                                                                    
                                                                                    
  // ======= Mesh  ==================
            unsigned numberOfUniformLevels = lev + 1;  //this has to have a + 1 for the convergence study
            unsigned numberOfSelectiveLevels = 0;
            ml_mesh.RefineMesh(numberOfUniformLevels , numberOfUniformLevels + numberOfSelectiveLevels, NULL);
            ml_mesh.EraseCoarseLevels(numberOfUniformLevels - 1);

            ml_mesh.PrintInfo();
                                                                                    
  // ======= Solution  ==================
  MultiLevelSolution ml_sol(&ml_mesh);

  ml_sol.SetWriter(VTK);
  ml_sol.GetWriter()->SetDebugOutput(true);
  
  // ======= Problem ========================
  ml_prob.SetMultiLevelMeshAndSolution(& ml_mesh,& ml_sol);
  
  ml_prob.get_systems_map().clear();

  //material  ==================
              //Adimensional quantity (Lref,Uref)
            double Lref = 1.;
            double Uref = 1.;
           // *** apparently needed by non-AD assemble only **********************
            // add fluid material
            Parameter par(Lref,Uref);
            
            // Generate Solid Object
            double E = 1500000;
            double ni = 0.5;
            double rhos = 1000;
            Solid solid;
            solid = Solid(par,E,ni,rhos,MODEL);

            std::cout << "Solid properties: " << std::endl;
            std::cout << solid << std::endl;
            
            
            ml_prob.parameters.set<Solid>("Solid") = solid;
  //end material  ==================

  // ======= Solution, II ==================

  ml_sol.AttachSetBoundaryConditionFunction(SetBoundaryCondition_in);
  for (unsigned int u = 0; u < unknowns.size(); u++)  { 
      ml_sol.AddSolution(unknowns[u]._name.c_str(), unknowns[u]._fe_family, unknowns[u]._fe_order, unknowns[u]._time_order, unknowns[u]._is_pde_unknown);
      ml_sol.Initialize(unknowns[u]._name.c_str(), SetInitialCondition_in, &ml_prob);
      ml_sol.GenerateBdc(unknowns[u]._name.c_str(), "Steady", & ml_prob);
  }
  
  // ======= System ========================
  NonLinearImplicitSystem& system = ml_prob.add_system < NonLinearImplicitSystem > ("SolidMech");

  for (unsigned int u = 0; u < unknowns.size(); u++)   system.AddSolutionToSystemPDE(unknowns[u]._name.c_str());
 
  system.set_unknown_list_for_assembly(unknowns); 
            
  system.SetAssembleFunction( AssembleSolidMech< NonLinearImplicitSystem, adept::adouble, adept::adouble >);

  // initialize and solve the system
  system.init();
  
  // Solver and preconditioner
  system.SetOuterSolver(PREONLY);
  system.SetSolverFineGrids(PREONLY);
  system.SetPreconditionerFineGrids(LU_PRECOND);

  //for Vanka   
  system.ClearVariablesToBeSolved();
  system.AddVariableToBeSolved("All");

  system.SetDebugNonlinear(true);

//   system.SetMaxNumberOfNonLinearIterations(2);
//   system.SetNonLinearConvergenceTolerance(1.e-30);
//   system.SetDebugLinear(true);
//   system.SetMaxNumberOfLinearIterations(4);
//   system.SetAbsoluteLinearConvergenceTolerance(1.e-10);
 
  system.MGsolve();

  // ======= Print ========================
  // set moving variables
  std::vector < std::string > mov_vars;
  mov_vars.push_back("DX");
  mov_vars.push_back("DY");
  if ( ml_mesh.GetDimension() == 3 ) mov_vars.push_back("DZ");
  ml_sol.GetWriter()->SetMovingMesh(mov_vars);
  
  // print solutions
  std::vector < std::string > variablesToBePrinted;  variablesToBePrinted.push_back("All");
  ml_sol.GetWriter()->Write(files.GetOutputPath(),"biquadratic", variablesToBePrinted, lev);
 
 return ml_sol;

}