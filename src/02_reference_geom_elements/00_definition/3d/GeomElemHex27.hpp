#ifndef __femus_meshGencase_GeomElemHex27_hpp__
#define __femus_meshGencase_GeomElemHex27_hpp__



#include "GeomElemHex.hpp"

#include <vector>
#include <iostream>




namespace femus {



class GeomElemHex27 : public GeomElemHex {

public:
  
       GeomElemHex27() : GeomElemHex() { };

    unsigned int n_nodes()        const { return 27; };
    
    

    std::vector<unsigned> get_nodes_of_face(const unsigned f) const { std::vector<unsigned> my_faces(_faces[f],_faces[f] + 9);  return my_faces; }; 

private:
    
    static const unsigned _faces[6][9];

// Refinement - BEGIN ===
    float get_embedding_matrix(const uint,const uint,const uint);


       double get_prol(const uint /*j*/) {std::cout << "Hex27: no prolongation needed\n"; abort();};

private:
    
    static const float _embedding_matrix[8/*NCHILDS*/][27/*NNDS*/][27/*NNDS*/];   // (volume)
// Refinement - END ===


// File names - BEGIN ===
    std::string   get_name_med()  const { return "H27"; };
    std::string   get_name_xdmf() const { return "Hexahedron_27"; };
// File names - END ===

};


} //end namespace femus



#endif


//We could do the template over the number of dofs, as a non-type template parameter...
//but then if you want to templatize you still need the NUMBER of CHILDREN,
//so you should do two nontype templates


//clearly, since this is a SPECIFIC Finite element, some allocations must be EXPLICIT
//the fact of being STATIC is for other reasons, not for the specific fact i think...

//The number of CHILDS is more related to the  'dimension' and the GEOMETRY, it is 8 both for Hex8 and Hex27

//Prol is the prolongation of a FE over a GEOM, so Quad4 on Quad9, Quad8 on Quad9.
//so, is it to be associated to 
