Code	File	Line	Column
MainFrm.cpp

  #include "dde.h"						
  dde::Uninitialize();					

PegAEsys.cpp

  #include "Dde.h"						
  dde::Setup(m_hInstance);				
  dde::PostAdvise(dde::ExtNumInfo);		
  dde::PostAdvise(dde::ExtStrInfo);		

PegAEsysView.cpp

  #include "Dde.h"						
  dde::PostAdvise(dde::ScaleInfo);		
  dde::PostAdvise(dde::DimLenInfo);		
  dde::PostAdvise(dde::DimAngZInfo);	
  dde::PostAdvise(dde::RelPosXInfo);	
  dde::PostAdvise(dde::RelPosYInfo);	
  dde::PostAdvise(dde::RelPosZInfo);	

PrimDim.cpp

  #include "DdeGItms.h"						
  dde::PostAdvise(dde::EngLenInfo);		
  dde::PostAdvise(dde::EngAngZInfo);	
  
PrimLine.cpp  

  #include "DdeGItms.h"						
  dde::PostAdvise(dde::EngLenInfo);		
  dde::PostAdvise(dde::EngAngZInfo);	
   
PrimPolygon.cpp

  #include "DdeGItms.h"						
  dde::PostAdvise(dde::EngLenInfo);		
  dde::PostAdvise(dde::EngAngZInfo);	

PrimPolyline.cpp

  #include "DdeGItms.h"						
  dde::PostAdvise(dde::EngLenInfo);		
  dde::PostAdvise(dde::EngAngZInfo);	
