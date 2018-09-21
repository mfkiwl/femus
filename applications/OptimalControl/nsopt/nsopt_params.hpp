#ifndef NSOPT_PARAMETERS
#define NSOPT_PARAMETERS


//*********************** Sets Number of subdivisions in X and Y direction *****************************************

#define NSUB_X  32
#define NSUB_Y  32

//******************************************* Desired Target  and RHS function*******************************************************

 double force[3] = {0.,0.,0.};
 double Vel_desired[3] = {0.125,0.,0.};

//*********************** Sets the regularization parameters *******************************************************

 double alpha_val = 1.;
 double beta_val = 1.;
 double gamma_val = 1.;
 
 
//******************************** switch between stokes and navier stokes *********************************************
 
 int advection_flag = 0;
 
//*********************** Find volume elements that contain a  Target domain element ********************************

int ElementTargetFlag(const std::vector<double> & elem_center) {

 //***** set target domain flag ********************************** 
  int target_flag = 0;
  
//    if ( elem_center[0] > 0.   - 1.e-5  &&  elem_center[0] < 0.25  + 1.e-5  && 
//         elem_center[1] > 0.25 - 1.e-5  &&  elem_center[1] < 0.75  + 1.e-5
//   ) //target on left 
   
    if (  elem_center[0] > 0.25 - 1.e-5  &&  elem_center[0] < 0.75  + 1.e-5  && 
	  elem_center[1] > 0.75  - 1.e-5  &&  elem_center[1] < 1.0   + 1.e-5
  ) //target on top

//    if ( elem_center[0] > 0.75  - 1.e-5  &&  elem_center[0] < 1.0   + 1.e-5  && 
//         elem_center[1] > 0.25 - 1.e-5  &&  elem_center[1] < 0.75  + 1.e-5
//   ) //target on right 
   
//     if (  elem_center[0] > 0.25 - 1.e-5  &&  elem_center[0] < 0.75  + 1.e-5  && 
// 	  elem_center[1] > 0.   - 1.e-5  &&  elem_center[1] < 0.25  + 1.e-5
//   ) //target on bottom
    {
     
     target_flag = 1;
     
  }
  
     return target_flag;

}


//*********************** Find volume elements that contain a Control Face element *********************************

int ControlDomainFlag(const std::vector<double> & elem_center) {

 //***** set control domain flag ***** 
  double mesh_size = 1./NSUB_Y;
  int control_el_flag = 0;
   if ( elem_center[1] >  1. - mesh_size ) { control_el_flag = 1; }

     return control_el_flag;
}




#endif