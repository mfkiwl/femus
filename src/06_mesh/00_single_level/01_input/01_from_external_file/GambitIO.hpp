/*=========================================================================

 Program: FEMUS
 Module: GambitIO
 Authors: Eugenio Aulisa, Simone Bnà
 
 Copyright (c) FEMTTU
 All rights reserved. 

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __femus_mesh_GambitIO_hpp__
#define __femus_mesh_GambitIO_hpp__


// Local includes
#include "MeshInput.hpp"
#include "GeomElTypeEnum.hpp"

namespace femus
{

// Forward declarations
class Mesh;

/**
 * This class implements writing meshes in the Gmsh format.
 */

// ------------------------------------------------------------
// GambitIO class definition
class GambitIO : public MeshInput<Mesh>
{
 public:

// === Constructors / Destructor - BEGIN =================
  /**
   * Constructor.  Takes a non-const Mesh reference which it
   * will fill up with elements via the read() command.
   */
  explicit
  GambitIO (Mesh& mesh);

// === Constructors / Destructor - END =================
  

  /**
   * Reads in a mesh in the neutral gambit *.neu format
   * from the ASCII file given by name.
   *
   */
  virtual void read (const std::string& name, 
                     std::vector < std::vector < double> > &coords,
                     const double Lref, 
                     std::vector<bool> &type_elem_flag,
                     const bool read_groups,
                     const bool read_boundary_groups);
  
 private:
   
// === Geometric element, Connectivities - BEGIN =================
   /** Map from Gambit vertex index to Femus vertex index */
   static const unsigned GambitToFemusVertexIndex[N_GEOM_ELS][MAX_EL_N_NODES]; 
 
   /** Map from Gambit face index to Femus face index */
   static const unsigned GambitToFemusFaceIndex[N_GEOM_ELS][MAX_EL_N_FACES];
// === Geometric element, Connectivities - END =================
     
};


inline
GambitIO::GambitIO (Mesh& mesh) :
   MeshInput<Mesh>  (mesh)
{
}


} // namespace femus

#endif 
