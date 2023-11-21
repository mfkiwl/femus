/*=========================================================================

 Program: FEMUS
 Module: ElemType
 Authors: Eugenio Aulisa, Giorgio Bornia

 Copyright (c) FEMTTU
 All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------

#include "ElemType.hpp"

#include "Wedge.hpp"
#include "Tetrahedron.hpp"
#include "Hexahedron.hpp"
#include "Quadrilateral.hpp"
#include "Triangle.hpp"
#include "Edge.hpp"


#include <cstring>


#define  PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES  1



namespace femus {

  //-----------elem_type - BEGIN ----------------------------------------------------------------------------------------------

//   Constructor with FE quadrature evaluations
  elem_type::elem_type(const char* geom_elem, const char* fe_order, const char* order_gauss) :
  _gauss(NULL),
  _gauss_bdry(NULL)
  {
      
     initialize_dim_and_geom_elem(geom_elem);
      
     initialize_fe_soltype(fe_order);
     
     // Quadrature (without FE Evaluations) ====    
     initialize_quadrature_all(geom_elem, order_gauss);
      
  }
  

//   Constructor without FE quadrature evaluations
  elem_type::elem_type(const char* geom_elem, const char* fe_order) :
  _gauss(NULL),
  _gauss_bdry(NULL)
  {
      
     initialize_dim_and_geom_elem(geom_elem);
      
     initialize_fe_soltype(fe_order);
      
   
  }
  
  
  elem_type::~elem_type()
  {

// FE ====      
   deallocate_fe_parts();
   
// Refinement ====      
   deallocate_refinement_parts();

   
// Quadrature (without FE Evaluations) ====    
    deallocate_quadrature_all();
    
  }
  
///@todo maybe initialize all these pointers to NULL in case they may not be allocated
  void elem_type::deallocate_fe_parts() {
      
    delete    _pt_basis       ;  //also refinement implicitly here actually
    
    delete [] _IND            ;
    
  }
  
///@todo maybe initialize all these pointers to NULL in case they may not be allocated
  void elem_type::deallocate_refinement_parts() {
    
    
    delete [] _prol_val       ;
    delete [] _prol_ind       ;
    delete [] _mem_prol_val   ;
    delete [] _mem_prol_ind   ;
    
  }

  
  
 
   void elem_type::initialize_dim_and_geom_elem(const char* geom_elem) {
       
// GEOM ELEM ============
           if ( !strcmp(geom_elem, "hex") )    { _dim = 3; _GeomElemType = HEX;   }
      else if ( !strcmp(geom_elem, "tet") )    { _dim = 3; _GeomElemType = TET;   }
      else if ( !strcmp(geom_elem, "wedge") )  { _dim = 3; _GeomElemType = WEDGE; }
      else if ( !strcmp(geom_elem, "quad") )   { _dim = 2; _GeomElemType = QUAD;  }
      else if ( !strcmp(geom_elem, "tri") )    { _dim = 2; _GeomElemType = TRI;   }
      else if ( !strcmp(geom_elem, "line") )   { _dim = 1; _GeomElemType = LINE;  }
      else {
        std::cout << " No " << geom_elem << " implemented" << std::endl;
        abort();
      }
 
   }  
  
  
   void elem_type::initialize_fe_soltype(const char* fe_order) {

// FE FAMILY & ORDER ============
    if(!strcmp(fe_order,      fe_fams[CONTINUOUS_LINEAR].c_str()      ) )           _SolType = CONTINUOUS_LINEAR;
    else if(!strcmp(fe_order, fe_fams[CONTINUOUS_SERENDIPITY].c_str() ) )   _SolType = CONTINUOUS_SERENDIPITY;
    else if(!strcmp(fe_order, fe_fams[CONTINUOUS_BIQUADRATIC].c_str() ) )  _SolType = CONTINUOUS_BIQUADRATIC;
    else if(!strcmp(fe_order, fe_fams[DISCONTINUOUS_CONSTANT].c_str() ) )    _SolType = DISCONTINUOUS_CONSTANT;
    else if(!strcmp(fe_order, fe_fams[DISCONTINUOUS_LINEAR].c_str()    )) _SolType = DISCONTINUOUS_LINEAR;
    else {
      std::cout << fe_order << " is not a valid option " << std::endl;
      abort();
    }  
       

   }
   
   
   void elem_type::initialize_quadrature_all(const char* geom_elem, const char* order_gauss) {
       
     initialize_quadrature(geom_elem, order_gauss);
      
     initialize_quadrature_boundary(geom_elem, order_gauss);
     
  }
  
   
   void elem_type::initialize_quadrature(const char* geom_elem, const char* order_gauss) {
       
          _gauss = new  Gauss(geom_elem, order_gauss);
       
   }
       
   void elem_type::initialize_quadrature_boundary(const char* geom_elem, const char* order_gauss) {
       

      if ( !strcmp(geom_elem, "quad") || !strcmp(geom_elem, "tri") ) { //QUAD or TRI
           _gauss_bdry = new  Gauss("line", order_gauss);
       }
      else if ( !strcmp(geom_elem, "hex") ) {
           _gauss_bdry = new  Gauss("quad", order_gauss);
       }
      else if ( !strcmp(geom_elem, "tet") ) {
           _gauss_bdry = new  Gauss("tri", order_gauss);
       }
      else if ( !strcmp(geom_elem, "line") ) {
           _gauss_bdry = new  Gauss("point", order_gauss);
       }
      else if ( !strcmp(geom_elem, "wedge") ) {
           _gauss_bdry = new  Gauss("quad", order_gauss); ///@todo this is wrong, we have to do a VECTOR of quadratures
       }
      else {
        std::cout << " Boundary gauss points for " << geom_elem << " is not implemented yet" << std::endl;
        abort();
      }
       

   }
   
   
  void elem_type::deallocate_quadrature_all() {
      
    deallocate_quadrature();
    deallocate_quadrature_boundary();
      
  }

  void elem_type::deallocate_quadrature() {
      
    if (_gauss != NULL) delete _gauss;
      
  }
   
   
  void elem_type::deallocate_quadrature_boundary() {
      
    if (_gauss_bdry != NULL) delete _gauss_bdry;
      
  }
  
  
  elem_type_1D::elem_type_1D(const char* geom_elem, const char* fe_order, const char* order_gauss) :
    elem_type(geom_elem, fe_order, order_gauss)
  {

// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions
      
    initialize_fe_parts(geom_elem);

    initialize_refinement_parts(geom_elem);


    
    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_fe_quadrature_evaluations(order_gauss);


  }
  
  
  elem_type_1D::elem_type_1D(const char* geom_elem, const char* fe_order) :
    elem_type(geom_elem, fe_order)
  {

// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions
    initialize_fe_parts(geom_elem);


    initialize_refinement_parts(geom_elem);
    
    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_to_null_fe_quadrature_evaluations_all();
  }
  

  
  
  elem_type_2D::elem_type_2D(const char* geom_elem, const char* fe_order, const char* order_gauss):
    elem_type(geom_elem, fe_order, order_gauss)
  {

// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions

    initialize_fe_parts(geom_elem);


    initialize_refinement_parts(geom_elem);


    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_fe_quadrature_evaluations(order_gauss);
    

  }
  
  
  elem_type_2D::elem_type_2D(const char* geom_elem, const char* fe_order):
    elem_type(geom_elem, fe_order)
  {

// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions

    initialize_fe_parts(geom_elem);

    initialize_refinement_parts(geom_elem);

    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_to_null_fe_quadrature_evaluations_all();
  }
  

  elem_type_3D::elem_type_3D(const char* geom_elem, const char* fe_order, const char* order_gauss) :
    elem_type(geom_elem, fe_order, order_gauss)
  {
    
// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions

    initialize_fe_parts(geom_elem);
    

    initialize_refinement_parts(geom_elem);

    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_fe_quadrature_evaluations(order_gauss);
 
  }
  
  
  elem_type_3D::elem_type_3D(const char* geom_elem, const char* fe_order) :
    elem_type(geom_elem, fe_order)
  {
    
// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions

    initialize_fe_parts(geom_elem);
    

    initialize_refinement_parts(geom_elem);

    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_to_null_fe_quadrature_evaluations_all();
  }
  
  
   void elem_type_1D::initialize_quadrature_with_fe_evals_from_child(const char* geom_elem, const char* order_gauss) {
       
    //************ QUADRATURE ******************
      initialize_quadrature_all(geom_elem, order_gauss);
  
    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_fe_quadrature_evaluations(order_gauss);
  
   }
   
   
   void elem_type_2D::initialize_quadrature_with_fe_evals_from_child(const char* geom_elem, const char* order_gauss) {
       
    //************ QUADRATURE ******************
      initialize_quadrature_all(geom_elem, order_gauss);
  
    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_fe_quadrature_evaluations(order_gauss);
  
   }
   
   
   void elem_type_3D::initialize_quadrature_with_fe_evals_from_child(const char* geom_elem, const char* order_gauss) {
       
    //************ QUADRATURE ******************
      initialize_quadrature_all(geom_elem, order_gauss);
  
    //************ FE and QUADRATURE EVALUATIONS ******************
    initialize_fe_quadrature_evaluations(order_gauss);
  
   }
   
   
   /** destructor */
    elem_type_1D::~elem_type_1D() {
          
// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions
      // Quadrature ====      
          deallocate_shape_at_quadrature_points();
            
          deallocate_volume_shape_at_reference_boundary_quadrature_points();
    }
    

    /** destructor */
    elem_type_2D::~elem_type_2D() {
          
// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions
      // Quadrature ====      
          deallocate_shape_at_quadrature_points();
            
          deallocate_volume_shape_at_reference_boundary_quadrature_points();
    }
    
  
    /** destructor */
    elem_type_3D::~elem_type_3D() {
          
// these cannot be called from the father constructor because they contain calls to PURE VIRTUAL functions
      // Quadrature ====      
          deallocate_shape_at_quadrature_points();
            
          deallocate_volume_shape_at_reference_boundary_quadrature_points();
    }
    
    


  void elem_type::allocate_and_set_coarse_node_indices_IND(const basis* pt_basis_in)  {
      
    const unsigned n_dofs = GetNDofs();
    
    _IND = new const int * [ n_dofs ];

    for(int i = 0; i <  n_dofs ; i++) {
      _IND[i] = pt_basis_in->GetIND(i);
    }
    
  }
  
  
  
  
  void elem_type::set_NDofs_coarse(const basis* pt_basis_in)  {
  
     _nc  	 = pt_basis_in->n_dofs();

    
 }

  void elem_type::set_NDofs_fine(const basis* pt_basis_in)  {
    
        _nf 	 = pt_basis_in->n_dofs_fine();

  } 


   
   void elem_type::set_fine_coordinates_in_Basis_object(basis* pt_basis_in, const basis* linearElement_in) const  {
       
     if(_SolType < NFE_FAMS_C_ZERO_LAGRANGE ) {
         
       const unsigned n_nodes_biq_fine = pt_basis_in->Get_NDofs_maximum_to_fill_fine_coordinates_of_DofCarriers();
       
       
      for(int i = 0; i < n_nodes_biq_fine; i++) {
          
        std::vector<double> xm(_dim, 0.);
        
        for(int k = 0; k <  linearElement_in->n_dofs(); k++) {
            
          const unsigned element = *(linearElement_in->GetKVERT_IND(i) + 0);
          std::vector< double > xv(_dim);
          for(int d = 0; d < _dim; d++)  xv[d] = * (linearElement_in->GetXcoarse(linearElement_in->GetFine2CoarseVertexMapping(element, k)) + d);
          
          const unsigned vertex = *(linearElement_in->GetKVERT_IND(i) + 1);
          for(int d = 0; d < _dim; d++)  xm[d] += linearElement_in->eval_phi(linearElement_in->GetIND(k), linearElement_in->GetXcoarse(vertex)) * xv[d];
            
        }
        
        for(int d = 0; d < _dim; d++)    pt_basis_in->SetX(i, d, xm[d]);
 
      }
      
    }

       
  }

  
   void elem_type::set_prolongation_OneElement_All_FE(const basis* linearElement)  {

       const unsigned n_dofs = GetNDofs();
       
       const double threshold_derivative_nonzero = 1.0e-14;
       
      int counter = 0;

      std::vector<unsigned int> n_geom_elems_after_refinement = {2, 4, 8}; //this is valid for all six shapes we have: Line (1d); Quad, Tri (2d); Hex, Tet, Wedge (3d)
      
    for(int i = 0; i < _nf; i++) {

      std::vector<double> jac(_dim + 1, 0.);
      
      if(_SolType == 4 && i / n_geom_elems_after_refinement[_dim-1] >= 1) {  //if piece_wise_linear derivatives
        for(int k = 0; k < linearElement->n_dofs(); k++) {
            
          //coordinates of the coarse vertices with respect to the fine elements
          std::vector<double> xv(_dim);
          for(int d = 0; d < _dim; d++)  xv[d] = * (linearElement->GetXcoarse(linearElement->GetFine2CoarseVertexMapping(i % n_geom_elems_after_refinement[_dim-1], k)) + d);

          for(unsigned int ideriv = 0; ideriv < _dim; ideriv++) {
          if( i / n_geom_elems_after_refinement[_dim-1] == (ideriv + 1) ) {
              for(int d = 0; d < _dim; d++)  jac[d+1] += linearElement->eval_dphidxyz(ideriv, linearElement->GetIND(k), _pt_basis->GetX(i) ) * xv[d];
            }
          } //ideriv

          
        } //k
      }
      
      for(int j = 0; j <  n_dofs ; j++) {
        double phi = _pt_basis->eval_phi(_IND[j], _pt_basis->GetX(i) );
        if(_SolType == 4 && i / n_geom_elems_after_refinement[_dim-1] >= 1) {  //if piece_wise_linear
          phi = jac[j];
        }
        if(fabs(phi) >= threshold_derivative_nonzero) {
          counter++;
        }
      }
      
    } //_nf

    double* pt_d;
    int* pt_i;

    _prol_val = new double * [_nf + 1];
    _prol_ind = new int * [_nf + 1];
    _mem_prol_val = new double [counter];
    _mem_prol_ind = new int [counter];

    pt_d = _mem_prol_val;
    pt_i = _mem_prol_ind;

    for(int i = 0; i < _nf; i++) {

      std::vector<double> jac(_dim + 1, 0.);

      if(_SolType == 4 && i / n_geom_elems_after_refinement[_dim-1] >= 1) {  //if piece_wise_linear derivatives
        for(int k = 0; k < linearElement->n_dofs(); k++) {
            
          //coordinates of the coarse vertices with respect to the fine elements
          std::vector<double> xv(_dim);
          for(int d = 0; d < _dim; d++)  xv[d] = * (linearElement->GetXcoarse(linearElement->GetFine2CoarseVertexMapping(i % n_geom_elems_after_refinement[_dim-1], k)) + d);

          for(unsigned int ideriv = 0; ideriv < _dim; ideriv++) {
          if( i / n_geom_elems_after_refinement[_dim-1] == (ideriv + 1) ) {
              for(int d = 0; d < _dim; d++)  jac[d+1] += linearElement->eval_dphidxyz(ideriv, linearElement->GetIND(k), _pt_basis->GetX(i) ) * xv[d];
            }
          } //ideriv
          
        } //k
        
      }

      _prol_val[i] = pt_d;
      _prol_ind[i] = pt_i;
      for(int j = 0; j <  n_dofs ; j++) {
        double phi = _pt_basis->eval_phi(_IND[j], _pt_basis->GetX(i) );
        if(_SolType == 4 && i / n_geom_elems_after_refinement[_dim-1] >= 1) {  //if piece_wise_linear derivatives
          phi = jac[j];
        }
        if(fabs(phi) >= threshold_derivative_nonzero) {
          *(pt_d++) = phi;
          *(pt_i++) = j;
        }
      }
    }

    _prol_val[_nf] = pt_d;
    _prol_ind[_nf] = pt_i;

  
   }
   
   
   void elem_type_1D::allocate_and_fill_shape_at_quadrature_points()  {
       
     // shape function and its derivatives evaluated at Gauss'points
    const int n_gauss = _gauss->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();

    _phi = new double*[n_gauss];
    _dphidxi  = new double*[n_gauss];
    _d2phidxi2  = new double*[n_gauss];

    _phi_memory = new double [n_gauss *  n_dofs ];
    _dphidxi_memory  = new double [n_gauss *  n_dofs ];
    _d2phidxi2_memory  = new double [n_gauss *  n_dofs ];

    for(unsigned i = 0; i < n_gauss; i++) {
      _phi[i] = &_phi_memory[i *  n_dofs ];
      _dphidxi[i]  = &_dphidxi_memory[i *  n_dofs ];
      _d2phidxi2[i]  = &_d2phidxi2_memory[i *  n_dofs ];
    }

    const double* ptx[1] = {_gauss->GetGaussWeightsPointer() + n_gauss};  // you sum an integer to a pointer, which offsets the pointer as a result

    for(unsigned i = 0; i < n_gauss; i++) {
      double x[1];

      for(unsigned j = 0; j < 1; j++) {
        x[j] = *ptx[j];
        ptx[j]++;
      }

      for(int j = 0; j <  n_dofs ; j++) {
        _phi[i][j] = _pt_basis->eval_phi(_IND[j], x);
        _dphidxi[i][j] = _pt_basis->eval_dphidx(_IND[j], x);
        _d2phidxi2[i][j] = _pt_basis->eval_d2phidx2(_IND[j], x);
      }
    }
    

   }
    
    
   void elem_type_2D::allocate_and_fill_shape_at_quadrature_points()  {
       
    // shape function and its derivatives evaluated at Gauss'points
    const int n_gauss = _gauss->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();

    _phi = new double*[n_gauss];
    _dphidxi  = new double*[n_gauss];
    _dphideta = new double*[n_gauss];

    _d2phidxi2  = new double*[n_gauss];
    _d2phideta2 = new double*[n_gauss];

    _d2phidxideta  = new double*[n_gauss];

    _phi_memory = new double [n_gauss *  n_dofs ];
    _dphidxi_memory  = new double [n_gauss *  n_dofs ];
    _dphideta_memory = new double [n_gauss *  n_dofs ];

    _d2phidxi2_memory  = new double [n_gauss *  n_dofs ];
    _d2phideta2_memory = new double [n_gauss *  n_dofs ];
    _d2phidxideta_memory  = new double [n_gauss *  n_dofs ];

    for(unsigned i = 0; i < n_gauss; i++) {
      _phi[i] = &_phi_memory[i *  n_dofs ];
      _dphidxi[i]  = &_dphidxi_memory[i *  n_dofs ];
      _dphideta[i] = &_dphideta_memory[i *  n_dofs ];

      _d2phidxi2[i]  = &_d2phidxi2_memory[i *  n_dofs ];
      _d2phideta2[i] = &_d2phideta2_memory[i *  n_dofs ];

      _d2phidxideta[i]  = &_d2phidxideta_memory[i *  n_dofs ];

    }
    
    const double* ptx[2] = {_gauss->GetGaussWeightsPointer() + n_gauss, _gauss->GetGaussWeightsPointer() + 2 * n_gauss};

    for(unsigned i = 0; i < n_gauss; i++) {
      double x[2];

      for(unsigned j = 0; j < 2; j++) {
        x[j] = *ptx[j];
        ptx[j]++;
      }

      for(int j = 0; j <  n_dofs ; j++) {
        _phi[i][j] = _pt_basis->eval_phi(_IND[j], x);
        _dphidxi[i][j] = _pt_basis->eval_dphidx(_IND[j], x);
        _dphideta[i][j] = _pt_basis->eval_dphidy(_IND[j], x);
        _d2phidxi2[i][j] = _pt_basis->eval_d2phidx2(_IND[j], x);
        _d2phideta2[i][j] = _pt_basis->eval_d2phidy2(_IND[j], x);
        _d2phidxideta[i][j] = _pt_basis->eval_d2phidxdy(_IND[j], x);
      }

    }
    
    
   }
   
   
   
   void elem_type_3D::allocate_and_fill_shape_at_quadrature_points()  {
       
    // shape function and its derivatives evaluated at Gauss'points
    const int n_gauss = _gauss->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();

    _phi = new double*[n_gauss];
    _dphidxi  = new double*[n_gauss];
    _dphideta = new double*[n_gauss];
    _dphidzeta = new double*[n_gauss];

    _d2phidxi2  = new double*[n_gauss];
    _d2phideta2 = new double*[n_gauss];
    _d2phidzeta2 = new double*[n_gauss];

    _d2phidxideta  = new double*[n_gauss];
    _d2phidetadzeta = new double*[n_gauss];
    _d2phidzetadxi = new double*[n_gauss];

    _phi_memory = new double [n_gauss *  n_dofs ];
    _dphidxi_memory  = new double [n_gauss *  n_dofs ];
    _dphideta_memory = new double [n_gauss *  n_dofs ];
    _dphidzeta_memory = new double [n_gauss *  n_dofs ];

    _d2phidxi2_memory  = new double [n_gauss *  n_dofs ];
    _d2phideta2_memory = new double [n_gauss *  n_dofs ];
    _d2phidzeta2_memory = new double [n_gauss *  n_dofs ];

    _d2phidxideta_memory  = new double [n_gauss *  n_dofs ];
    _d2phidetadzeta_memory = new double [n_gauss *  n_dofs ];
    _d2phidzetadxi_memory = new double [n_gauss *  n_dofs ];

    for(unsigned i = 0; i < n_gauss; i++) {
      _phi[i] = &_phi_memory[i *  n_dofs ];
      _dphidxi[i]  = &_dphidxi_memory[i *  n_dofs ];
      _dphideta[i] = &_dphideta_memory[i *  n_dofs ];
      _dphidzeta[i] = &_dphidzeta_memory[i *  n_dofs ];

      _d2phidxi2[i]  = &_d2phidxi2_memory[i *  n_dofs ];
      _d2phideta2[i] = &_d2phideta2_memory[i *  n_dofs ];
      _d2phidzeta2[i] = &_d2phidzeta2_memory[i *  n_dofs ];

      _d2phidxideta[i]  = &_d2phidxideta_memory[i *  n_dofs ];
      _d2phidetadzeta[i] = &_d2phidetadzeta_memory[i *  n_dofs ];
      _d2phidzetadxi[i] = &_d2phidzetadxi_memory[i *  n_dofs ];

    }

    const double* ptx[3] = {_gauss->GetGaussWeightsPointer() +   n_gauss,
                            _gauss->GetGaussWeightsPointer() + 2 * n_gauss,
                            _gauss->GetGaussWeightsPointer() + 3 * n_gauss
                           };

    for(unsigned i = 0; i < n_gauss; i++) {
      double x[3];

      for(unsigned j = 0; j < 3; j++) {
        x[j] = *ptx[j];
        ptx[j]++;
      }

      double phisum = 0.;
      double dphidxisum = 0.;
      double dphidetasum = 0.;
      double dphidzetasum = 0.;
      double d2phidxi2sum = 0.;
      double d2phideta2sum = 0.;
      double d2phidzeta2sum = 0.;
      double d2phidxidetasum = 0.;
      double d2phidetadzetasum = 0.;
      double d2phidzetadxisum = 0.;

      for(int j = 0; j < n_dofs; j++) {
        _phi[i][j] = _pt_basis->eval_phi(_IND[j], x);
        _dphidxi[i][j] = _pt_basis->eval_dphidx(_IND[j], x);
        _dphideta[i][j] = _pt_basis->eval_dphidy(_IND[j], x);
        _dphidzeta[i][j] = _pt_basis->eval_dphidz(_IND[j], x);

        _d2phidxi2[i][j] = _pt_basis->eval_d2phidx2(_IND[j], x);
        _d2phideta2[i][j] = _pt_basis->eval_d2phidy2(_IND[j], x);
        _d2phidzeta2[i][j] = _pt_basis->eval_d2phidz2(_IND[j], x);

        _d2phidxideta[i][j] = _pt_basis->eval_d2phidxdy(_IND[j], x);
        _d2phidetadzeta[i][j] = _pt_basis->eval_d2phidydz(_IND[j], x);
        _d2phidzetadxi[i][j] = _pt_basis->eval_d2phidzdx(_IND[j], x);

        phisum += _phi[i][j];

        dphidxisum += _dphidxi[i][j];
        dphidetasum += _dphideta[i][j];
        dphidzetasum += _dphidzeta[i][j];

        d2phidxi2sum += _d2phidxi2[i][j];
        d2phideta2sum += _d2phideta2[i][j];
        d2phidzeta2sum += _d2phidzeta2[i][j];

        d2phidxidetasum += _d2phidxideta[i][j];
        d2phidetadzetasum +=  _d2phidetadzeta[i][j];
        d2phidzetadxisum += _d2phidzetadxi[i][j];
      }
    }
       
       
   }
   
   
   void elem_type_1D::allocate_and_fill_volume_shape_at_reference_boundary_quadrature_points_on_faces(const char* order_gauss)  {
       
        constexpr unsigned int dim = 1;  
   
#if PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES == 1   
       if(_SolType < NFE_FAMS_C_ZERO_LAGRANGE) {
#endif

      constexpr unsigned int nFaces = 2;
      const unsigned n_dofs = GetNDofs();
      
      _phiFace.resize(nFaces);
      _gradPhiFace.resize(nFaces);
      _hessianPhiFace.resize(nFaces);

      const double xi[nFaces] = { -1, 1};
      
      constexpr unsigned int nGaussPts_on_each_face = 1;

      for(int iface = 0; iface < nFaces; iface++) {
          
        _phiFace[iface].resize(nGaussPts_on_each_face);
        _gradPhiFace[iface].resize(nGaussPts_on_each_face);
        _hessianPhiFace[iface].resize(nGaussPts_on_each_face);

        _phiFace[iface][0].resize( n_dofs );
        _gradPhiFace[iface][0].resize( n_dofs );
        _hessianPhiFace[iface][0].resize( n_dofs );

        for(int j = 0; j <  n_dofs ; j++) {

          _phiFace[iface][0][j] = _pt_basis->eval_phi(_IND[j], &xi[iface]);

          //std::cout <<  _phiFace[iface][0][j] << " ";

          _gradPhiFace[iface][0][j].resize(1);
          _gradPhiFace[iface][0][j][0] = _pt_basis->eval_dphidx(_IND[j], &xi[iface]);

          //std::cout <<  _gradPhiFace[iface][0][j][0] << " ";

          _hessianPhiFace[iface][0][j].resize(1);
          _hessianPhiFace[iface][0][j][0].resize(1);
          _hessianPhiFace[iface][0][j][0][0] = _pt_basis->eval_d2phidx2(_IND[j], &xi[iface]);

          //std::cout <<  _hessianPhiFace[iface][0][j][0][0] << " ";
        }
        //std::cout << std::endl;
      }

#if PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES == 1   
    }
#endif   
   
   }
   
   
   
   void elem_type_2D::allocate_and_fill_volume_shape_at_reference_boundary_quadrature_points_on_faces(const char* order_gauss)  {
         
    const unsigned n_dofs = GetNDofs();
          
        constexpr unsigned int dim = 2;  

      basis* underlying_volume_basis;
      
       if( _SolType < NFE_FAMS_C_ZERO_LAGRANGE ) {
           underlying_volume_basis = _pt_basis;
       }
       else if( _SolType < NFE_FAMS ) {
              if( geom_el_type() == QUAD) underlying_volume_basis = new QuadLinear;
         else if( geom_el_type() == TRI)  underlying_volume_basis = new TriLinear;
       }
      
      
          
// // // #if PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES == 1   
// // //        if(_SolType < NFE_FAMS_C_ZERO_LAGRANGE) {
// // // #endif

//----- auxiliary basis for the faces //(the reference element has straight edges)
      basis* linearLine = new LineLinear;

      basis* faceBasis = linearLine;

// --------- quadrature - BEGIN
      Gauss faceGaussPoint = Gauss("line", order_gauss);
      const double* xi_ptr = {faceGaussPoint.GetGaussWeightsPointer() + faceGaussPoint.GetGaussPointsNumber()};
// --------- quadrature - END

      unsigned nFaces = /*_pt_basis*/underlying_volume_basis->get_geom_elem()->n_faces_total();
      
      _phiFace.resize(nFaces);
      _gradPhiFace.resize(nFaces);
      _hessianPhiFace.resize(nFaces);
      
      const unsigned n_face_dofs = faceBasis -> n_dofs();

      for(int iface = 0; iface < nFaces; iface++) {
          
        std::vector< double > xv_face(n_face_dofs);
        std::vector< double > yv_face(n_face_dofs);
        
        for(int jnode = 0; jnode < n_face_dofs; jnode++) {
          unsigned iDof = /*_pt_basis*/underlying_volume_basis->GetFaceDof(iface, jnode);
          xv_face[jnode] = *(/*_pt_basis*/underlying_volume_basis->GetXcoarse(iDof) + 0);
          yv_face[jnode] = *(/*_pt_basis*/underlying_volume_basis->GetXcoarse(iDof) + 1);
        }
        
        unsigned nGaussPts = faceGaussPoint.GetGaussPointsNumber();
        
        _phiFace[iface].resize(nGaussPts);
        _gradPhiFace[iface].resize(nGaussPts);
        _hessianPhiFace[iface].resize(nGaussPts);
        
        for(unsigned i = 0; i < nGaussPts; i++) {
          double x_qp_face[2] = {0., 0.};
          for(int j = 0; j <  n_face_dofs; j++) {
            x_qp_face[0] += faceBasis->eval_phi(faceBasis->GetIND(j), &xi_ptr[i]) * xv_face[j] ;
            x_qp_face[1] += faceBasis->eval_phi(faceBasis->GetIND(j), &xi_ptr[i]) * yv_face[j] ;
          }
          
          _phiFace[iface][i].resize( n_dofs );
          _gradPhiFace[iface][i].resize( n_dofs );
          _hessianPhiFace[iface][i].resize( n_dofs );
          
          for(int j = 0; j <  n_dofs ; j++) {
            _phiFace[iface][i][j] = _pt_basis->eval_phi(_IND[j], x_qp_face);

            _gradPhiFace[iface][i][j].resize(2);
            _gradPhiFace[iface][i][j][0] = _pt_basis->eval_dphidx(_IND[j], x_qp_face);
            _gradPhiFace[iface][i][j][1] = _pt_basis->eval_dphidy(_IND[j], x_qp_face);

            _hessianPhiFace[iface][i][j].resize(2);
            _hessianPhiFace[iface][i][j][0].resize(2);
            _hessianPhiFace[iface][i][j][1].resize(2);
            _hessianPhiFace[iface][i][j][0][0] = _pt_basis->eval_d2phidx2(_IND[j], x_qp_face);
            _hessianPhiFace[iface][i][j][1][1] = _pt_basis->eval_d2phidy2(_IND[j], x_qp_face);
            _hessianPhiFace[iface][i][j][0][1] = _pt_basis->eval_d2phidxdy(_IND[j], x_qp_face);
            _hessianPhiFace[iface][i][j][1][0] = _hessianPhiFace[iface][i][j][0][1];
          }
        }
      }
      delete linearLine;
      
// // // #if PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES == 1   
// // //     }
// // //     else if(_SolType < NFE_FAMS) {
// // //        // if I am a Disc element, I don't have the nodes with me,
// // //        // but I need them in order to compute the face quadrature points. 
// // //        // These are needed to locate the boundary quadrature points correctly on each face
// // //        // Basically, I need the underlying element, or more precisely the underlying Geometric Element
// // //        // in 2d, it is either a Quad or a Tri, and it will only be one of them 
// // // 
// // //         
// // //     }
// // // #endif
    
        
if( _SolType >= NFE_FAMS_C_ZERO_LAGRANGE  &&  _SolType < NFE_FAMS ) {
    delete underlying_volume_basis;
}
      
   }
   
   
   
   
   void elem_type_3D::allocate_and_fill_volume_shape_at_reference_boundary_quadrature_points_on_faces(const char* order_gauss)  {
          
    const unsigned n_dofs = GetNDofs();
   
#if PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES == 1   
       if(_SolType < NFE_FAMS_C_ZERO_LAGRANGE) {
#endif
           
        
      basis* linearQuad = new QuadLinear;
      basis* linearTri = new TriLinear;

      basis* faceBasis[2];
      faceBasis[0] = linearQuad;
      faceBasis[1] = linearTri;

// --------- quadrature - BEGIN
      Gauss quadGaussPoint = Gauss("quad", order_gauss);
      Gauss triGaussPoint = Gauss("tri", order_gauss);

      Gauss* faceGauss[2]; //two types: either Quadrilateral or Triangle
      faceGauss[0] = &quadGaussPoint;
      faceGauss[1] = &triGaussPoint;

      const double* xi[2] = { faceGauss[0]->GetGaussWeightsPointer() + faceGauss[0]->GetGaussPointsNumber(),
                              faceGauss[1]->GetGaussWeightsPointer() + faceGauss[1]->GetGaussPointsNumber()
                            };

      const double* yi[2] = { faceGauss[0]->GetGaussWeightsPointer() + 2 * (faceGauss[0]->GetGaussPointsNumber()),
                              faceGauss[1]->GetGaussWeightsPointer() + 2 * (faceGauss[1]->GetGaussPointsNumber())
                            };
// --------- quadrature - END

      unsigned nFaces = _pt_basis->get_geom_elem()->n_faces_total();
      _phiFace.resize(nFaces);
      _gradPhiFace.resize(nFaces);
      _hessianPhiFace.resize(nFaces);

      //only the first two face types
      for(unsigned type = 0; type < GeomElemBase::get_n_face_types_max(); type++) {
        for(int iface = _pt_basis->get_geom_elem()->n_faces_offset(type);
                iface < _pt_basis->get_geom_elem()->n_faces_offset(type + 1); iface++) {
          std::vector< double > xv(faceBasis[type] -> n_dofs());
          std::vector< double > yv(faceBasis[type] ->  n_dofs());
          std::vector< double > zv(faceBasis[type] ->  n_dofs());
          for(int jnode = 0; jnode < faceBasis[type] ->  n_dofs(); jnode++) {
            unsigned iDof = _pt_basis->GetFaceDof(iface, jnode);
            xv[jnode] = *(_pt_basis->GetXcoarse(iDof) + 0);
            yv[jnode] = *(_pt_basis->GetXcoarse(iDof) + 1);
            zv[jnode] = *(_pt_basis->GetXcoarse(iDof) + 2);
          }

          unsigned nGaussPts = faceGauss[type]->GetGaussPointsNumber();
          _phiFace[iface].resize(nGaussPts);
          _gradPhiFace[iface].resize(nGaussPts);
          _hessianPhiFace[iface].resize(nGaussPts);

          for(unsigned i = 0; i < nGaussPts; i++) {
            double x[3] = {0., 0., 0.};
            const double vertex[2] = {xi[type][i], yi[type][i]};
            for(int j = 0; j <  faceBasis[type] ->  n_dofs(); j++) {
              x[0] += faceBasis[type]->eval_phi(faceBasis[type]->GetIND(j), vertex) * xv[j] ;
              x[1] += faceBasis[type]->eval_phi(faceBasis[type]->GetIND(j), vertex) * yv[j] ;
              x[2] += faceBasis[type]->eval_phi(faceBasis[type]->GetIND(j), vertex) * zv[j] ;
            }

            _phiFace[iface][i].resize( n_dofs );
            _gradPhiFace[iface][i].resize( n_dofs );
            _hessianPhiFace[iface][i].resize( n_dofs );

            for(int j = 0; j <  n_dofs ; j++) {

              _phiFace[iface][i][j] = _pt_basis->eval_phi(_IND[j], x);

              _gradPhiFace[iface][i][j].resize(3);
              _gradPhiFace[iface][i][j][0] = _pt_basis->eval_dphidx(_IND[j], x);
              _gradPhiFace[iface][i][j][1] = _pt_basis->eval_dphidy(_IND[j], x);
              _gradPhiFace[iface][i][j][2] = _pt_basis->eval_dphidz(_IND[j], x);

              _hessianPhiFace[iface][i][j].resize(3);
              _hessianPhiFace[iface][i][j][0].resize(3);
              _hessianPhiFace[iface][i][j][1].resize(3);
              _hessianPhiFace[iface][i][j][2].resize(3);

              _hessianPhiFace[iface][i][j][0][0] = _pt_basis->eval_d2phidx2(_IND[j], x);
              _hessianPhiFace[iface][i][j][0][1] = _pt_basis->eval_d2phidxdy(_IND[j], x);
              _hessianPhiFace[iface][i][j][0][2] = _pt_basis->eval_d2phidzdx(_IND[j], x);

              _hessianPhiFace[iface][i][j][1][0] =  _hessianPhiFace[iface][i][j][0][1];
              _hessianPhiFace[iface][i][j][1][1] = _pt_basis->eval_d2phidy2(_IND[j], x);
              _hessianPhiFace[iface][i][j][1][2] = _pt_basis->eval_d2phidydz(_IND[j], x);

              _hessianPhiFace[iface][i][j][2][0] = _hessianPhiFace[iface][i][j][0][2];
              _hessianPhiFace[iface][i][j][2][1] = _hessianPhiFace[iface][i][j][1][2];
              _hessianPhiFace[iface][i][j][2][2] = _pt_basis->eval_d2phidz2(_IND[j], x);
            }
          }
        }
      }
      delete linearQuad;
      delete linearTri;
      
#if PHIFACE_ONLY_FOR_LAGRANGIAN_FAMILIES == 1   
    }
#endif


   }
      
      
   const basis*  elem_type::set_underlying_Linear_FE_basis(const char* geom_elem) const {
     
       basis* linearElement;
     
         if(!strcmp(geom_elem, "line")) {  linearElement = new LineLinear;  }
    else if(!strcmp(geom_elem, "tri"))  {  linearElement = new TriLinear;  }
    else if(!strcmp(geom_elem, "quad")) {  linearElement = new QuadLinear;  }
    else if(!strcmp(geom_elem, "hex")) {     linearElement = new HexLinear;    }
    else if(!strcmp(geom_elem, "tet")) {     linearElement = new TetLinear;    }
    else if(!strcmp(geom_elem, "wedge")) {   linearElement = new WedgeLinear;    }
    else {
      std::cout << geom_elem << " is not a valid option" << std::endl;
      abort();
    }
    
    return linearElement;    
    
    
   }
        
  
  void elem_type::initialize_fe_parts(const char* geom_elem) {

    
    // FE:
    set_current_FE_basis(geom_elem, _SolType);
    
    
    // FE:
    //get FE data from basis object
    set_NDofs_coarse(_pt_basis);
    
    // FE:
    allocate_and_set_coarse_node_indices_IND(_pt_basis);


    
  }
  
  
  
  void elem_type::initialize_refinement_parts(const char* geom_elem) {
 
    
    // FE: MG
    set_NDofs_fine(_pt_basis);
    
    
    // FE: MG   with   linearElement - BEGIN --------
    const basis* linearElement = set_underlying_Linear_FE_basis(geom_elem);
    
    // construction of coordinates ***********************************************************    
    set_fine_coordinates_in_Basis_object(_pt_basis, linearElement);

    //local prolongation matrix evaluation ***********************************************************
    set_prolongation_OneElement_All_FE(linearElement);

    delete linearElement;
    // FE: MG   with   linearElement - END --------

  
  
  

    
  }
   
  
  
  
  void elem_type_1D::initialize_to_null_fe_quadrature_evaluations_vol_at_bdry() {
      
      _phi_vol_at_bdry             = NULL  ;
      _phi_vol_at_bdry_memory      = NULL  ; 
      
      _dphidxi_vol_at_bdry         = NULL  ; 
      _dphidxi_vol_at_bdry_memory  = NULL  ;
  
      
}
  
  
  void elem_type_2D::initialize_to_null_fe_quadrature_evaluations_vol_at_bdry() {
  
      
             _phi_vol_at_bdry              = NULL  ;
             _phi_vol_at_bdry_memory       = NULL  ;
             
             _dphidxi_vol_at_bdry          = NULL  ;
             _dphidxi_vol_at_bdry_memory   = NULL  ;
             _dphideta_vol_at_bdry         = NULL  ;
             _dphideta_vol_at_bdry_memory  = NULL  ; 
      
    
}


  void elem_type_3D::initialize_to_null_fe_quadrature_evaluations_vol_at_bdry() {
 
      
           _phi_vol_at_bdry               = NULL;
           _phi_vol_at_bdry_memory        = NULL;
           
           _dphidxi_vol_at_bdry           = NULL ; 
           _dphidxi_vol_at_bdry_memory    = NULL ;
           _dphideta_vol_at_bdry          = NULL ;
           _dphideta_vol_at_bdry_memory   = NULL ; 
           _dphidzeta_vol_at_bdry         = NULL ;
           _dphidzeta_vol_at_bdry_memory  = NULL ;
      
      
      
      
      
      
}
  
  
  
  
  void elem_type_1D::initialize_to_null_fe_quadrature_evaluations_vol_at_vol() {
      
     _phi              = NULL   ;
     _phi_memory       = NULL   ;
     _dphidxi          = NULL   ;
     _dphidxi_memory   = NULL   ;
      
     _d2phidxi2        = NULL   ;
     _d2phidxi2_memory = NULL   ;
      
  }  
  
  void elem_type_2D::initialize_to_null_fe_quadrature_evaluations_vol_at_vol() {
      
     
       _phi                 = NULL        ;
       _phi_memory          = NULL        ;
       _dphidxi             = NULL        ;
       _dphidxi_memory      = NULL        ;
       _dphideta            = NULL        ;
       _dphideta_memory     = NULL        ;
      
       _d2phidxi2           = NULL        ;
       _d2phidxi2_memory    = NULL        ;
       _d2phideta2          = NULL        ;
       _d2phideta2_memory   = NULL        ;
      
       _d2phidxideta        = NULL        ;
       _d2phidxideta_memory = NULL        ;
      
      
      
  }
  
  
  void elem_type_3D::initialize_to_null_fe_quadrature_evaluations_vol_at_vol() {
      
      
       _phi                      = NULL     ;
       _phi_memory               = NULL     ; 
       _dphidxi                  = NULL     ;
       _dphidxi_memory           = NULL     ;
       _dphideta                 = NULL     ;
       _dphideta_memory          = NULL     ;
       _dphidzeta                = NULL     ;
       _dphidzeta_memory         = NULL     ;
      
      
       _d2phidxi2                = NULL     ;
       _d2phidxi2_memory         = NULL     ;
       _d2phideta2               = NULL     ;
       _d2phideta2_memory        = NULL     ;
       _d2phidzeta2              = NULL     ;
       _d2phidzeta2_memory       = NULL     ;
      
       _d2phidxideta             = NULL     ;
       _d2phidxideta_memory      = NULL     ;
       _d2phidetadzeta           = NULL     ;
       _d2phidetadzeta_memory    = NULL     ;
       _d2phidzetadxi             = NULL     ;
       _d2phidzetadxi_memory     = NULL     ;
      
      
      
  }
  
  
  void elem_type_1D::deallocate_shape_at_quadrature_points() {
      
     if ( _phi              != NULL)  delete [] _phi               ;
     if ( _phi_memory       != NULL)  delete [] _phi_memory        ;
     if ( _dphidxi          != NULL)  delete [] _dphidxi           ;
     if ( _dphidxi_memory   != NULL)  delete [] _dphidxi_memory    ;

     if ( _d2phidxi2        != NULL)  delete [] _d2phidxi2         ;
     if ( _d2phidxi2_memory != NULL)  delete [] _d2phidxi2_memory  ; 
        
  }
  
  
  void elem_type_2D::deallocate_shape_at_quadrature_points() {

    if ( _phi                 != NULL)    delete [] _phi                    ;
    if ( _phi_memory          != NULL)    delete [] _phi_memory             ;
    if ( _dphidxi             != NULL)    delete [] _dphidxi                ;
    if ( _dphidxi_memory      != NULL)    delete [] _dphidxi_memory         ;
    if ( _dphideta            != NULL)    delete [] _dphideta               ;
    if ( _dphideta_memory     != NULL)    delete [] _dphideta_memory        ;

    if ( _d2phidxi2           != NULL)    delete [] _d2phidxi2              ;
    if ( _d2phidxi2_memory    != NULL)    delete [] _d2phidxi2_memory       ;
    if ( _d2phideta2          != NULL)    delete [] _d2phideta2             ;
    if ( _d2phideta2_memory   != NULL)    delete [] _d2phideta2_memory      ;
    
    if ( _d2phidxideta        != NULL)    delete [] _d2phidxideta           ;
    if ( _d2phidxideta_memory != NULL)    delete [] _d2phidxideta_memory    ;
      
  } 
  
  
  void elem_type_3D::deallocate_shape_at_quadrature_points() {
      
    if (  _phi                      != NULL)  delete [] _phi                        ;
    if (  _phi_memory               != NULL)  delete [] _phi_memory                 ; 
    if (  _dphidxi                  != NULL)  delete [] _dphidxi                    ;
    if (  _dphidxi_memory           != NULL)  delete [] _dphidxi_memory             ;
    if (  _dphideta                 != NULL)  delete [] _dphideta                   ;
    if (  _dphideta_memory          != NULL)  delete [] _dphideta_memory            ;
    if (  _dphidzeta                != NULL)  delete [] _dphidzeta                  ;
    if (  _dphidzeta_memory         != NULL)  delete [] _dphidzeta_memory           ;

        
    if (  _d2phidxi2                != NULL)  delete [] _d2phidxi2                  ;
    if (  _d2phidxi2_memory         != NULL)  delete [] _d2phidxi2_memory           ;
    if (  _d2phideta2               != NULL)  delete [] _d2phideta2                 ;
    if (  _d2phideta2_memory        != NULL)  delete [] _d2phideta2_memory          ;
    if (  _d2phidzeta2              != NULL)  delete [] _d2phidzeta2                ;
    if (  _d2phidzeta2_memory       != NULL)  delete [] _d2phidzeta2_memory         ;

    if (  _d2phidxideta             != NULL)  delete [] _d2phidxideta               ;
    if (  _d2phidxideta_memory      != NULL)  delete [] _d2phidxideta_memory        ;
    if (  _d2phidetadzeta           != NULL)  delete [] _d2phidetadzeta             ;
    if (  _d2phidetadzeta_memory    != NULL)  delete [] _d2phidetadzeta_memory      ;
    if ( _d2phidzetadxi             != NULL)  delete [] _d2phidzetadxi              ;
    if (  _d2phidzetadxi_memory     != NULL)  delete [] _d2phidzetadxi_memory       ;
     
  }  
     
  

  
  void elem_type_1D::allocate_volume_shape_at_reference_boundary_quadrature_points_per_current_face() {
       
    const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();
    
    _phi_vol_at_bdry = new double*[n_gauss_bdry];
    _dphidxi_vol_at_bdry  = new double*[n_gauss_bdry];
    _phi_vol_at_bdry_memory = new double [n_gauss_bdry *  n_dofs ];
    _dphidxi_vol_at_bdry_memory  = new double [n_gauss_bdry *  n_dofs ];
    
     for (unsigned i = 0; i < n_gauss_bdry; i++) {
      _phi_vol_at_bdry[i] = &_phi_vol_at_bdry_memory[i *  n_dofs ];
      _dphidxi_vol_at_bdry[i]  = &_dphidxi_vol_at_bdry_memory[i *  n_dofs ];
     }
     
}

  void elem_type_1D::deallocate_volume_shape_at_reference_boundary_quadrature_points() { 
      
     if ( _phi_vol_at_bdry             != NULL)     delete [] _phi_vol_at_bdry               ;
     if ( _phi_vol_at_bdry_memory      != NULL)     delete [] _phi_vol_at_bdry_memory        ;
        
     if ( _dphidxi_vol_at_bdry         != NULL)     delete [] _dphidxi_vol_at_bdry           ;
     if ( _dphidxi_vol_at_bdry_memory  != NULL)     delete [] _dphidxi_vol_at_bdry_memory    ;

}


   void elem_type_2D::allocate_volume_shape_at_reference_boundary_quadrature_points_per_current_face() {

    const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();
    
    _phi_vol_at_bdry = new double*[n_gauss_bdry];
    _dphidxi_vol_at_bdry  = new double*[n_gauss_bdry];
    _dphideta_vol_at_bdry = new double*[n_gauss_bdry];
    _phi_vol_at_bdry_memory = new double [n_gauss_bdry *  n_dofs ];
    _dphidxi_vol_at_bdry_memory  = new double [n_gauss_bdry *  n_dofs ];
    _dphideta_vol_at_bdry_memory = new double [n_gauss_bdry *  n_dofs ];
    
     for (unsigned i = 0; i < n_gauss_bdry; i++) {
      _phi_vol_at_bdry[i] = &_phi_vol_at_bdry_memory[i *  n_dofs ];
      _dphidxi_vol_at_bdry[i]  = &_dphidxi_vol_at_bdry_memory[i *  n_dofs ];
      _dphideta_vol_at_bdry[i] = &_dphideta_vol_at_bdry_memory[i *  n_dofs ];
     }
     
    }


  void elem_type_2D::deallocate_volume_shape_at_reference_boundary_quadrature_points() {
      
      if ( _phi_vol_at_bdry              != NULL)   delete [] _phi_vol_at_bdry               ;
      if ( _phi_vol_at_bdry_memory       != NULL)   delete [] _phi_vol_at_bdry_memory        ;
        
      if ( _dphidxi_vol_at_bdry          != NULL)   delete [] _dphidxi_vol_at_bdry           ;
      if ( _dphidxi_vol_at_bdry_memory   != NULL)   delete [] _dphidxi_vol_at_bdry_memory    ;
      if ( _dphideta_vol_at_bdry         != NULL)   delete [] _dphideta_vol_at_bdry          ;
      if ( _dphideta_vol_at_bdry_memory  != NULL)   delete [] _dphideta_vol_at_bdry_memory   ; 
      
  }
  
    
  void elem_type_3D::allocate_volume_shape_at_reference_boundary_quadrature_points_per_current_face() {
      
    const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();
    
    _phi_vol_at_bdry = new double*[n_gauss_bdry];
    _dphidxi_vol_at_bdry  = new double*[n_gauss_bdry];
    _dphideta_vol_at_bdry = new double*[n_gauss_bdry];
    _dphidzeta_vol_at_bdry = new double*[n_gauss_bdry];
    _phi_vol_at_bdry_memory = new double [n_gauss_bdry *  n_dofs ];
    _dphidxi_vol_at_bdry_memory  = new double [n_gauss_bdry *  n_dofs ];
    _dphideta_vol_at_bdry_memory = new double [n_gauss_bdry *  n_dofs ];
    _dphidzeta_vol_at_bdry_memory = new double [n_gauss_bdry *  n_dofs ];
    
     for (unsigned i = 0; i < n_gauss_bdry; i++) {
      _phi_vol_at_bdry[i] = &_phi_vol_at_bdry_memory[i *  n_dofs ];
      _dphidxi_vol_at_bdry[i]   = & _dphidxi_vol_at_bdry_memory[i *  n_dofs ];
      _dphideta_vol_at_bdry[i]  = & _dphideta_vol_at_bdry_memory[i *  n_dofs ];
      _dphidzeta_vol_at_bdry[i] = & _dphidzeta_vol_at_bdry_memory[i *  n_dofs ];
     }
      
}


  void elem_type_3D::deallocate_volume_shape_at_reference_boundary_quadrature_points() { 
      
     if ( _phi_vol_at_bdry               != NULL)   delete [] _phi_vol_at_bdry               ;
     if ( _phi_vol_at_bdry_memory        != NULL)   delete [] _phi_vol_at_bdry_memory        ;
        
     if ( _dphidxi_vol_at_bdry           != NULL)    delete [] _dphidxi_vol_at_bdry           ; 
     if ( _dphidxi_vol_at_bdry_memory    != NULL)    delete [] _dphidxi_vol_at_bdry_memory    ;
     if ( _dphideta_vol_at_bdry          != NULL)    delete [] _dphideta_vol_at_bdry          ;
     if ( _dphideta_vol_at_bdry_memory   != NULL)    delete [] _dphideta_vol_at_bdry_memory   ; 
     if ( _dphidzeta_vol_at_bdry         != NULL)    delete [] _dphidzeta_vol_at_bdry         ;
     if ( _dphidzeta_vol_at_bdry_memory  != NULL)    delete [] _dphidzeta_vol_at_bdry_memory  ;
      
}
  
   

  

 void elem_type::initialize_to_null_fe_quadrature_evaluations_all() {
     
     initialize_to_null_fe_quadrature_evaluations_vol_at_vol();
     
     initialize_to_null_fe_quadrature_evaluations_vol_at_bdry();
     
 }
 
 
 
 void elem_type::initialize_fe_quadrature_evaluations(const char* order_gauss) {
     
    allocate_and_fill_shape_at_quadrature_points();

    allocate_and_fill_volume_shape_at_reference_boundary_quadrature_points_on_faces(order_gauss);

    // boundary
    allocate_volume_shape_at_reference_boundary_quadrature_points_per_current_face();

 }
 
 
  
  

  
   void elem_type_1D::set_current_FE_basis(const char* geom_elem, unsigned int FEType_in) { 
        

    if(!strcmp(geom_elem, "line")) {  //line
        
      if(_SolType == 0) _pt_basis = new LineLinear;
      else if(_SolType == 1) _pt_basis = new LineBiquadratic;
      else if(_SolType == 2) _pt_basis = new LineBiquadratic;
      else if(_SolType == 3) _pt_basis = new line0;
      else if(_SolType == 4) _pt_basis = new linepwLinear;
      else {
        std::cout << FEType_in/*fe_order*/ << " is not a valid option for " << geom_elem << std::endl;
        abort();
      }
    }
    else {
      std::cout << geom_elem << " is not a valid option" << std::endl;
      abort();
    }
    
    
    }


  
    
    

  void elem_type_2D::set_current_FE_basis(const char* geom_elem, unsigned int FEType_in) {
        

    if(!strcmp(geom_elem, "quad")) {  //QUAD
        
      if(_SolType == 0) _pt_basis = new QuadLinear;
      else if(_SolType == 1) _pt_basis = new QuadQuadratic;
      else if(_SolType == 2) _pt_basis = new QuadBiquadratic;
      else if(_SolType == 3) _pt_basis = new quad0;
      else if(_SolType == 4) _pt_basis = new quadpwLinear;
      else {
        std::cout << FEType_in/*fe_order*/ << " is not a valid option for " << geom_elem << std::endl;
        abort();
      }
    }
    else if(!strcmp(geom_elem, "tri")) {  //TRIANGLE
        
      if(_SolType == 0) _pt_basis = new TriLinear;
      else if(_SolType == 1) _pt_basis = new TriQuadratic;
      else if(_SolType == 2) _pt_basis = new TriBiquadratic;
      else if(_SolType == 3) _pt_basis = new tri0;
      else if(_SolType == 4) _pt_basis = new tripwLinear;
      else {
        std::cout << FEType_in/*fe_order*/ << " is not a valid option for " << geom_elem << std::endl;
        abort();
      }
    }
    else {
      std::cout << geom_elem << " is not a valid option" << std::endl;
      abort();
    }
        
    }
    
    

    
    void elem_type_3D::set_current_FE_basis(const char* geom_elem, unsigned int FEType_in) {
  
    
    if(!strcmp(geom_elem, "hex")) {  //HEX

      if(_SolType == 0) _pt_basis = new HexLinear;
      else if(_SolType == 1) _pt_basis = new HexQuadratic;
      else if(_SolType == 2) _pt_basis = new HexBiquadratic;
      else if(_SolType == 3) _pt_basis = new hex0;
      else if(_SolType == 4) _pt_basis = new hexpwLinear;
      else {
        std::cout << FEType_in/*fe_order*/ << " is not a valid option for " << geom_elem << std::endl;
        abort();
      }
    }
    else if(!strcmp(geom_elem, "wedge")) {  //WEDGE
        
      if(_SolType == 0) _pt_basis = new WedgeLinear;
      else if(_SolType == 1) _pt_basis = new WedgeQuadratic;
      else if(_SolType == 2) _pt_basis = new WedgeBiquadratic;
      else if(_SolType == 3) _pt_basis = new wedge0;
      else if(_SolType == 4) _pt_basis = new wedgepwLinear;
      else {
        std::cout << FEType_in/*fe_order*/ << " is not a valid option for " << geom_elem << std::endl;
        abort();
      }
    }
    else if(!strcmp(geom_elem, "tet")) {  //TETRAHEDRA
        
      if(_SolType == 0) _pt_basis = new TetLinear;
      else if(_SolType == 1) _pt_basis = new TetQuadratic;
      else if(_SolType == 2) _pt_basis = new TetBiquadratic;
      else if(_SolType == 3) _pt_basis = new tet0;
      else if(_SolType == 4) _pt_basis = new tetpwLinear;
      else {
        std::cout << FEType_in/*fe_order*/ << " is not a valid option for " << geom_elem << std::endl;
        abort();
      }
    }
    else {
      std::cout << geom_elem << " is not a valid option" << std::endl;
      abort();
    }
    
    
 }

 
 
 
 
 
  void elem_type_1D::fill_volume_shape_at_reference_boundary_quadrature_points_per_face(const unsigned  jface) const {
      
    const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();

 //evaluate volume shape functions and derivatives at reference boundary gauss points             
for (unsigned qp = 0; qp < n_gauss_bdry; qp++) {
             
      for (int dof = 0; dof <  n_dofs ; dof++) {
            _phi_vol_at_bdry[qp][dof] = _phiFace[jface][qp][dof]        ;
        _dphidxi_vol_at_bdry[qp][dof] = _gradPhiFace[jface][qp][dof][0] ;
        }
      
     }
    

  }
  
  
  
  void elem_type_3D::fill_volume_shape_at_reference_boundary_quadrature_points_per_face(const unsigned  jface) const {
      
    const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();

for (unsigned qp = 0; qp < n_gauss_bdry; qp++) {
             
      for (int dof = 0; dof <  n_dofs ; dof++) {
            _phi_vol_at_bdry[qp][dof] = _phiFace[jface][qp][dof]        ;
        _dphidxi_vol_at_bdry[qp][dof] = _gradPhiFace[jface][qp][dof][0] ;
       _dphideta_vol_at_bdry[qp][dof] = _gradPhiFace[jface][qp][dof][1] ;
      _dphidzeta_vol_at_bdry[qp][dof] = _gradPhiFace[jface][qp][dof][2] ;
      }
      
    }
    
      
 }
 

//---------------------------------------------------------------------------------------------------------
  void elem_type_2D::fill_volume_shape_at_reference_boundary_quadrature_points_per_face(const unsigned  jface) const {

// // // //********* EVALUATION OF REFERENCE POINTS **********************
// // // //first of all I have to place the boundary gauss points in one of the faces of the volume
// // //           
// // //         
// // //     ///@todo check that our volume element shape is a quadrilateral, doesn't work for triangles for now
// // //                                                
// // //     std::vector<int> normal_ref(_dim);         std::fill(normal_ref.begin(), normal_ref.end(), 0.);
// // //     std::vector<double> normal_real(_dim);     std::fill(normal_real.begin(), normal_real.end(), 0.);
// // //     std::vector<double> tangent_real(_dim);     std::fill(tangent_real.begin(), tangent_real.end(), 0.);
// // //     std::vector <double> translation(_dim);        std::fill(translation.begin(), translation.end(), 0.);
// // //         
// // // // normals of the reference faces
// // //     if      (jface == 0) {  normal_ref[0] =  0;  normal_ref[1] = -1; }
// // //     else if (jface == 1) {  normal_ref[0] =  1;  normal_ref[1] =  0; }
// // //     else if (jface == 2) {  normal_ref[0] =  0;  normal_ref[1] =  1; }
// // //     else if (jface == 3) {  normal_ref[0] = -1;  normal_ref[1] =  0; }
// // //     
// // //     
// // // // translation to the face
// // //     for (unsigned d = 0; d < _dim; d++) { translation[d] = normal_ref[d]; }
// // //     
// // // // tangent computation ******
// // //    tangent_real[0] = (vt_bdry[0][1] - vt_bdry[0][0]);
// // //    tangent_real[1] = (vt_bdry[1][1] - vt_bdry[1][0]);
// // // //****************************
// // // 
// // //    
// // // // normal computation from the tangent ******
// // //    //rotation by 90 degrees clockwise
// // //    normal_real[0] =   tangent_real[1];
// // //    normal_real[1] = - tangent_real[0];
// // //     
// // //     double magn = 0.;
// // //     for (unsigned d = 0; d < _dim; d++) magn += normal_real[d] * normal_real[d]; 
// // //         
// // //      magn = sqrt(magn);
// // //     
// // //     for (unsigned d = 0; d < _dim; d++) { normal_real[d] /= magn; }
// // // //****************************
// // //     
// // // // angle between reference normal and real normal ******
// // //     double cosine_theta = 0.; 
// // //     for (unsigned d = 0; d < _dim; d++) cosine_theta += normal_real[d] * normal_ref[d];
// // //      
// // //     //here the fact is that the abscissa of the gauss_bdry rule is one-dimensional, 
// // //     //but at this stage we don't know what the direction of the abscissa is (x, y, or general)
// // //     //we should access the bdry element and compute the abscissa using the coordinates of it
// // //     //here what we have to do is to locate the reference boundary element in the reference volume element
// // //     //Notice that the SIGN of the direction is also important
// // //     //we need to understand:
// // //     // 1) where my boundary element is located in the reference volume element
// // //     // 2) in what direction it is oriented
// // // //****************************
// // // 
// // //     
// // // //here we compute at ALL quadrature points... it should be only for the current quadrature point...
// // //     
// // //     const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
// // //     
// // //     const double* pt_one_dim[1] = {_gauss_bdry->GetGaussWeightsPointer() + 1*n_gauss_bdry};
// // //    
// // //     
// // // // one-dim vector ****************************
// // //       double xi_one_dim[2];
// // //       for (unsigned d = 0; d < 1; d++) {
// // //         xi_one_dim[d] = *pt_one_dim[d];
// // //         pt_one_dim[d]++;
// // //       }
// // //       
// // //       xi_one_dim[1] = 0.; //placing along xi direction (eta = 0) ///@todo fix this here wrt the normal_ref above
// // // //****************************
// // //       
// // // // rotation matrix ****************************
// // //     double rotation[2][2] = {{0, 0}, {0, 0}};
// // // 
// // //     const double theta = acos(cosine_theta);
// // //     
// // //     rotation[0][0] =  cosine_theta;
// // //     rotation[0][1] = - sin( theta );
// // //     rotation[1][0] =   sin( theta );
// // //     rotation[1][1] =  cosine_theta;
// // //     
// // //  
// // //   std::vector <double> rotation_vec(_dim); std::fill(rotation_vec.begin(), rotation_vec.end(), 0.);
// // //       
// // //    for (unsigned k = 0; k < _dim; k++)  
// // //      for (unsigned d = 0; d < _dim; d++)  rotation_vec[k] +=  rotation[k][d] * xi_one_dim[d]; 
// // // //****************************
// // // 
// // //      
// // //      
// // // // rotate and translate ****************************
// // // //here we want to compute the reference gauss point in the volume that corresponds to the real gauss point related to ig_bdry
// // //  //we have to use a transformation that locates the 1d edge in one of the sides of my 2d elem
// // //     std::vector <double> ref_bdry_qp_coords_in_vol(_dim);
// // //       
// // //     for (unsigned d = 0; d < _dim; d++) ref_bdry_qp_coords_in_vol[d] =  rotation_vec[d]  + translation[d];
// // // // ****************************
    
    
    
    const int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    const unsigned n_dofs = GetNDofs();

for (unsigned qp = 0; qp < n_gauss_bdry; qp++) {
             
      for (int dof = 0; dof <  n_dofs ; dof++) {
            _phi_vol_at_bdry[qp][dof] = _phiFace[jface][qp][dof]        ; //_pt_basis->eval_phi(_IND[dof],    &ref_bdry_qp_coords_in_vol[0]); /*if ( abs(_phiFace[jface][qp][dof]        - _phi_vol_at_bdry[qp][dof]) > 1.e-3 ) abort();*/
        _dphidxi_vol_at_bdry[qp][dof] = _gradPhiFace[jface][qp][dof][0] ; //_pt_basis->eval_dphidx(_IND[dof], &ref_bdry_qp_coords_in_vol[0]); /*if ( abs(_gradPhiFace[jface][qp][dof][0] - _dphidxi_vol_at_bdry[qp][dof]) > 1.e-3 ) abort();*/
       _dphideta_vol_at_bdry[qp][dof] = _gradPhiFace[jface][qp][dof][1] ; //_pt_basis->eval_dphidy(_IND[dof], &ref_bdry_qp_coords_in_vol[0]); /*if ( abs(_gradPhiFace[jface][qp][dof][1] - _dphideta_vol_at_bdry[qp][dof]) > 1.e-3 ) abort();*/
      }
      
 }
    
             

}


//---------------------------------------------------------------------------------------------------------
//Compute volume jacobian and evaluate volume shape functions and derivatives at REAL boundary quadrature points

  void elem_type_2D::fill_volume_shape_funcs_at_boundary_quadrature_points_on_current_elem(
                                           const std::vector < std::vector < double > >& vt_vol, 
                                           const std::vector < std::vector < double> > & vt_bdry,  
                                           const unsigned& jface, 
                                           const unsigned& ig_bdry, 
                                           std::vector < double >& phi, 
                                           std::vector < double >& gradphi) const {
                                       

    fill_volume_shape_at_reference_boundary_quadrature_points_per_face(/*vt_bdry,*/ jface);
    
    const unsigned n_dofs = GetNDofs();

//Compute volume jacobian and its inverse at boundary gauss points
    double Jac[2][2] = {{0, 0}, {0, 0}};
    double JacInv[2][2];
    const double* dxi = _dphidxi_vol_at_bdry[ig_bdry];
    const double* deta = _dphideta_vol_at_bdry[ig_bdry];
    
    for (int inode = 0; inode <  n_dofs ; inode++, dxi++, deta++) {
      Jac[0][0] += (*dxi) * vt_vol[0][inode];  // d x/d csi
      Jac[0][1] += (*dxi) * vt_vol[1][inode];  // d y/d csi
      Jac[1][0] += (*deta) * vt_vol[0][inode]; // d x/d eta
      Jac[1][1] += (*deta) * vt_vol[1][inode]; // d y/d eta
    }
    double det = (Jac[0][0] * Jac[1][1] - Jac[0][1] * Jac[1][0]);

    JacInv[0][0] = Jac[1][1] / det;
    JacInv[0][1] = -Jac[0][1] / det;
    JacInv[1][0] = -Jac[1][0] / det;
    JacInv[1][1] = Jac[0][0] / det;
//Compute volume jacobian and its inverse at boundary gauss points - end
    
    
    //Use Jacobian        to go from the REAL to the CANONICAL coordinates
    //Use JacobianInverse to go from the CANONICAL to the REAL coordinates
    

//Compute shape functions and derivatives in REAL coordinates
    
    phi.resize( n_dofs );
    gradphi.resize( n_dofs  * 2);
    

    dxi  = _dphidxi_vol_at_bdry[ig_bdry];
    deta = _dphideta_vol_at_bdry[ig_bdry];

    for (int inode = 0; inode <  n_dofs ; inode++, dxi++, deta++) {

      phi[inode] = _phi_vol_at_bdry[ig_bdry][inode];

      gradphi[2 * inode + 0] = (*dxi) * JacInv[0][0] + (*deta) * JacInv[0][1];
      gradphi[2 * inode + 1] = (*dxi) * JacInv[1][0] + (*deta) * JacInv[1][1];

    }
    
  }
  

//---------------------------------------------------------------------------------------------------------

  void elem_type_3D::fill_volume_shape_funcs_at_boundary_quadrature_points_on_current_elem(const std::vector < std::vector < double > >& vt_vol, 
                                           const std::vector < std::vector < double> > & vt_bdry,  
                                           const unsigned& jface, 
                                           const unsigned& ig_bdry, 
                                           std::vector < double >& phi, 
                                           std::vector < double >& gradphi) const {
                                       

//********* EVALUATION STAGE **********************
    const unsigned n_dofs = GetNDofs();
                                       
    //check that our volume element shape is a quadrilateral, doesn't work for triangles for now
    std::vector<int> tang_vec_ref(_dim);     std::fill(tang_vec_ref.begin(), tang_vec_ref.end(), 0.);
    std::vector<double> tang_vec_real(_dim);    std::fill(tang_vec_real.begin(), tang_vec_real.end(), 0.);
    double xi_factor;
        
 // normals of the reference faces
    
 // translation to the face

// tangent computation ******
    
// normal computation from the tangent ******
    
// angle between reference normal and real normal ******
    
    
    
    if      (jface == 0) { tang_vec_ref[0]  = 1;  tang_vec_ref[1] =  0; xi_factor = -1; }
    else if (jface == 1) { tang_vec_ref[0]  = 0;  tang_vec_ref[1] =  1; xi_factor = +1; }
    else if (jface == 2) { tang_vec_ref[0] =  1;  tang_vec_ref[1] =  0; xi_factor = +1; }
    else if (jface == 3) { tang_vec_ref[0]  = 0;  tang_vec_ref[1] =  1; xi_factor = -1; }
    
    tang_vec_real[0] = vt_bdry[0][1] - vt_bdry[0][0]; 
    tang_vec_real[1] = vt_bdry[1][1] - vt_bdry[1][0];
    
    double magn = 0.;
    for (unsigned d = 0; d < _dim; d++) magn += tang_vec_real[d]*tang_vec_real[d]; 
        
     magn = sqrt(magn);
    
    for (unsigned d = 0; d < _dim; d++) { tang_vec_real[d] /= magn; }
    
    double cosine_theta = 0.; 
    for (unsigned d = 0; d < _dim; d++) cosine_theta += tang_vec_real[d]*tang_vec_ref[d];

    
    
    //here the fact is that the abscissa of the gauss_bdry rule is one-dimensional, 
    //but at this stage we don't know what the direction of the abscissa is (x, y, or general)
    //we should access the bdry element and compute the abscissa using the coordinates of it
    //here what we have to do is to locate the reference boundary element in the reference volume element
    //Notice that the SIGN of the direction is also important
    //we need to understand:
    // 1) where my boundary element is located in the reference volume element
    // 2) in what direction it is oriented
    
    //here we compute for ALL quadrature points and for ALL dofs the test functions
    int n_gauss_bdry = _gauss_bdry->GetGaussPointsNumber();
    
    const double* pt_one_dim[1] = {_gauss_bdry->GetGaussWeightsPointer() + 1*n_gauss_bdry};
    
    std::vector < std::vector<double> > xi_qps(n_gauss_bdry);
    for (unsigned qp = 0; qp < n_gauss_bdry; qp++) { xi_qps[qp].resize(_dim); }
    
// std::cout << "Inside  ig = " << ig_bdry << " ";
for (unsigned qp = 0; qp < n_gauss_bdry; qp++) {
        
      double xi_one_dim[1];
      for (unsigned d = 0; d < 1; d++) {
        xi_one_dim[d] = *pt_one_dim[d];
        pt_one_dim[d]++;
      }

//here we want to compute the reference gauss point in the volume that corresponds to the real gauss point related to ig_bdry
      std::vector <double> ref_bdry_qp_coords_in_vol(2);
             ref_bdry_qp_coords_in_vol[1-abs(tang_vec_ref[0])] = cosine_theta * xi_one_dim[0]; 
             ref_bdry_qp_coords_in_vol[abs(tang_vec_ref[0])]   = xi_factor * 1.;
      
             
      for (int dof = 0; dof <  n_dofs ; dof++) {
             _phi_vol_at_bdry[qp][dof] = _pt_basis->eval_phi(_IND[dof],    &ref_bdry_qp_coords_in_vol[0]);
         _dphidxi_vol_at_bdry[qp][dof] = _pt_basis->eval_dphidx(_IND[dof], &ref_bdry_qp_coords_in_vol[0]);
        _dphideta_vol_at_bdry[qp][dof] = _pt_basis->eval_dphidy(_IND[dof], &ref_bdry_qp_coords_in_vol[0]);
      }
      
             xi_qps[qp] = ref_bdry_qp_coords_in_vol;
             
    }
    
             
//       for (unsigned d = 0; d < _dim; d++) std::cout << xi_qps[ig_bdry][d] << " ";
// std::cout << std::endl;
    
//********* END EVALUATION STAGE **********************
    

    phi.resize( n_dofs );
    gradphi.resize( n_dofs  * 3);
//     if(nablaphi) nablaphi->resize( n_dofs  * 6);


     //Jac ===============
    double Jac[3][3] = {{0., 0., 0.}, {0., 0., 0.}, {0., 0., 0.}};

    const double * dxi = _dphidxi_vol_at_bdry[ig_bdry];
    const double * deta = _dphideta_vol_at_bdry[ig_bdry];
    const double * dzeta = _dphidzeta_vol_at_bdry[ig_bdry];

    for(int inode = 0; inode <  n_dofs ; inode++, dxi++, deta++, dzeta++) {
      Jac[0][0] += (*dxi) * vt_vol[0][inode];
      Jac[0][1] += (*dxi) * vt_vol[1][inode];
      Jac[0][2] += (*dxi) * vt_vol[2][inode];
      Jac[1][0] += (*deta) * vt_vol[0][inode];
      Jac[1][1] += (*deta) * vt_vol[1][inode];
      Jac[1][2] += (*deta) * vt_vol[2][inode];
      Jac[2][0] += (*dzeta) * vt_vol[0][inode];
      Jac[2][1] += (*dzeta) * vt_vol[1][inode];
      Jac[2][2] += (*dzeta) * vt_vol[2][inode];
    }

    double det = (Jac[0][0] * (Jac[1][1] * Jac[2][2] - Jac[1][2] * Jac[2][1]) +
                Jac[0][1] * (Jac[1][2] * Jac[2][0] - Jac[1][0] * Jac[2][2]) +
                Jac[0][2] * (Jac[1][0] * Jac[2][1] - Jac[1][1] * Jac[2][0]));

     //JacI ===============
    double JacInv[3][3];
    
    JacInv[0][0] = (-Jac[1][2] * Jac[2][1] + Jac[1][1] * Jac[2][2]) / det;
    JacInv[0][1] = (Jac[0][2] * Jac[2][1] - Jac[0][1] * Jac[2][2]) / det;
    JacInv[0][2] = (-Jac[0][2] * Jac[1][1] + Jac[0][1] * Jac[1][2]) / det;
    JacInv[1][0] = (Jac[1][2] * Jac[2][0] - Jac[1][0] * Jac[2][2]) / det;
    JacInv[1][1] = (-Jac[0][2] * Jac[2][0] + Jac[0][0] * Jac[2][2]) / det;
    JacInv[1][2] = (Jac[0][2] * Jac[1][0] - Jac[0][0] * Jac[1][2]) / det;
    JacInv[2][0] = (-Jac[1][1] * Jac[2][0] + Jac[1][0] * Jac[2][1]) / det;
    JacInv[2][1] = (Jac[0][1] * Jac[2][0] - Jac[0][0] * Jac[2][1]) / det;
    JacInv[2][2] = (-Jac[0][1] * Jac[1][0] + Jac[0][0] * Jac[1][1]) / det;

    
     //==============================
     //==============================
     //==============================
    //Use the Jacobian here to go from the REAL back to the CANONICAL coordinates
 
    dxi = _dphidxi_vol_at_bdry[ig_bdry];
    deta = _dphideta_vol_at_bdry[ig_bdry];
    dzeta = _dphidzeta_vol_at_bdry[ig_bdry];

//     const double* dxi2 = _d2phidxi2[ig];
//     const double* deta2 = _d2phideta2[ig];
//     const double* dzeta2 = _d2phidzeta2[ig];
//     const double* dxideta = _d2phidxideta[ig];
//     const double* detadzeta = _d2phidetadzeta[ig];
//     const double* dzetadxi = _d2phidzetadxi[ig];

    for(int inode = 0; inode <  n_dofs ; inode++, dxi++, deta++, dzeta++) {

      phi[inode] = _phi_vol_at_bdry[ig_bdry][inode];

      gradphi[3 * inode + 0] = (*dxi) * JacInv[0][0] + (*deta) * JacInv[0][1] + (*dzeta) * JacInv[0][2];
      gradphi[3 * inode + 1] = (*dxi) * JacInv[1][0] + (*deta) * JacInv[1][1] + (*dzeta) * JacInv[1][2];
      gradphi[3 * inode + 2] = (*dxi) * JacInv[2][0] + (*deta) * JacInv[2][1] + (*dzeta) * JacInv[2][2];


    }
    
  }
  

} //end namespace femus






