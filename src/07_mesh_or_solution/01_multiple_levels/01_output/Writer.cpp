/*=========================================================================

 Program: FEMUS
 Module: Writer
 Authors: Eugenio Aulisa, Simone Bnà, Giorgio Bornia

 Copyright (c) FEMTTU
 All rights reserved.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------

#include "Writer.hpp"
#include "VTKWriter.hpp"
#include "GMVWriter.hpp"
#include "XDMFWriter.hpp"
#include "MultiLevelSolution.hpp"
#include "ElemType.hpp"
#include "FElemTypeEnum_list.hpp"




namespace femus {


  const unsigned Writer::FemusToVTKorToXDMFConn[27] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 23, 21, 20, 22, 24, 25, 26};

  Writer::Writer (/*const*/ MultiLevelSolution* ml_sol) :
    _ml_sol (ml_sol),
    _ml_mesh (ml_sol->GetMLMesh()) {
      
    _gridn = _ml_mesh->GetNumberOfLevels();
    
    _moving_mesh = 0;
    _graph = false;
    _surface = false;
    
    _debugOutput = false;
    
  }

  Writer::Writer (MultiLevelMesh* ml_mesh) :
    _ml_sol (NULL), 
    _ml_mesh (ml_mesh) {
      
    _gridn = _ml_mesh->GetNumberOfLevels();
    
    _moving_mesh = 0;
    _graph = false;
    _surface = false;
    
    _debugOutput = false;
    
  }

  Writer::~Writer() { }


  std::unique_ptr<Writer> Writer::build (const WriterEnum format, MultiLevelSolution * ml_sol)  {

    switch (format) {
      case VTK: {
        std::unique_ptr<Writer>   ap (new VTKWriter (ml_sol));
        return ap;
      }
      case GMV: {
        std::unique_ptr<Writer>   ap (new GMVWriter (ml_sol));
        return ap;
      }
#ifdef HAVE_HDF5
      case XDMF: {
        std::unique_ptr<Writer>   ap (new XDMFWriter (ml_sol));
        return ap;
      }
#endif
      default: {
        std::cout << "Format not supported" << std::endl;
        abort();
      }
    } //end switch

  }

  std::unique_ptr<Writer> Writer::build (const WriterEnum format, MultiLevelMesh * ml_mesh)  {

    switch (format) {
      case VTK: {
        std::unique_ptr<Writer>   ap (new VTKWriter (ml_mesh));
        return ap;
      }
      case GMV: {
        std::unique_ptr<Writer>   ap (new GMVWriter (ml_mesh));
        return ap;
      }
#ifdef HAVE_HDF5
      case XDMF: {
        std::unique_ptr<Writer>   ap (new XDMFWriter (ml_mesh));
        return ap;
      }
#endif
      default: {
        std::cout << "Format not supported" << std::endl;
        abort();
      }
    } //end switch

  }

  void Writer::SetMovingMesh (std::vector<std::string>& movvars_in) {
    _moving_mesh = 1;
    _moving_vars = movvars_in;
  }

  void Writer::SetGraphVariable (const std::string &graphVaraible) {
    _graph = true;
    _surface = false;
    _graphVariable = graphVaraible;
  }

  void Writer::SetSurfaceVariables (std::vector < std::string > &surfaceVariable) {
    _surface = true;
    _graph = false;
    _surfaceVariables = surfaceVariable;
  }


  
   std::string Writer::print_sol_bdc_res_eps_name(const std::string solName, const unsigned name) const {
       
            std::string printName;

            if( name == 0 ) printName = solName;
            else if( name == 1 ) printName = "Bdc" + solName;
            else if( name == 2 ) printName = "Res" + solName;
            else if( name == 3 ) printName = "Eps" + solName;
            else { abort(); }

       return printName;     
   }
   
   
  
   unsigned Writer::compute_sol_bdc_res_eps_size(const Solution * solution, const unsigned i) const {
       
       const bool is_unknown = solution->is_unknown_of_system(i);

       const bool is_debug   = _debugOutput;       
       
       const unsigned print_sol_size = 1 + 3 * is_debug * is_unknown;
       return  print_sol_size;
       
   }
   
   
    unsigned Writer::fe_index(const std::string & order_str) const {
        
        unsigned index = 0;
        
    if( !strcmp( order_str.c_str(), fe_fams_for_files[ FILES_CONTINUOUS_LINEAR ].c_str() ) )           {  index = 0;  }
    else if( !strcmp( order_str.c_str(), fe_fams_for_files[ FILES_CONTINUOUS_QUADRATIC ].c_str() ) )   {  index = 1;  }
    else if( !strcmp( order_str.c_str(), fe_fams_for_files[ FILES_CONTINUOUS_BIQUADRATIC ].c_str() ) ) {  index = 2;  }
    
        return index;
    }


} //end namespace femus


