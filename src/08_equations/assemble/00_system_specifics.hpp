#ifndef __femus_system_specifics_hpp__
#define __femus_system_specifics_hpp__


#include "CurrentElem.hpp"
#include "ElemType_template.hpp"
#include "Function.hpp"

#include <vector>
#include <string>



namespace femus {
    
    
class MultiLevelProblem;
class Mesh;
class MultiLevelSolution;


   /// @todo this should be templated as a class, or some of its functions should be templated
class system_specifics {
 
  public:
      
      system_specifics() {
         
         _assemble_function = NULL;
         _boundary_conditions_types_and_values = NULL;
         
         _assemble_function_for_rhs = NULL;
         _true_solution_function = NULL;         
         
      }
  
   //mesh files - BEGIN
   std::vector< std::string >   _mesh_files;  //same domain, only potentially multiple mesh discretizations 

   std::vector< std::string >   _mesh_files_path_relative_to_executable;  //same domain, only potentially multiple mesh discretizations 
   //mesh files - END
 
   //System name - BEGIN
   std::string  _system_name;  //for now we only accept 1 System in a certain App. This name is needed to retrieve the equation from the Problem
   //System name - END
   
   //func pointer of EQUATION - BEGIN
   /// @todo this should be templated
   typedef void (* AssembleFunctionType) (MultiLevelProblem &ml_prob);

   AssembleFunctionType  _assemble_function;
   //func pointer of EQUATION - END


   //func pointer of Boundary Conditions - BEGIN
    typedef bool (*BoundaryFunction) (const MultiLevelProblem * ml_prob, 
                                      const std::vector < double >& x,
                                      const char name[], 
                                      double &value, 
                                      const int FaceName,
                                      const double time);
    
    
    BoundaryFunction   _boundary_conditions_types_and_values;
   //func pointer of Boundary Conditions - END

  
   //function to use for the right-hand side of the assemble function - BEGIN
   Math::Function< double > *  _assemble_function_for_rhs;
   //function to use for the right-hand side of the assemble function - END
   
   //function for the exact solution (optional) - BEGIN
   Math::Function< double > *  _true_solution_function;
   //function for the exact solution (optional) - END
    
};


}



#endif
