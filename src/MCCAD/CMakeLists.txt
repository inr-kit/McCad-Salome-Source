# Copyright (C) 2014-2015  KIT-INR/NK
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#####################################################################
#								    #
#			 Build McCad libraries			    #
#								    #
#####################################################################

############################################################################

#
# build McCad shared libraries
#---------------------------------------------------------------------------



#qiu make the debug version
#SET(CMAKE_BUILD_TYPE debug)

# additional include directories
INCLUDE_DIRECTORIES(
  McCadHeaders
  ${CAS_INCLUDE_DIRS}
  ${QT_INCLUDES}
  )

# additional preprocessor / compiler flags
# -DSALOME indicates the McCad-SALOME version
ADD_DEFINITIONS(  ${CAS_DEFINITIONS} ${QT_DEFINITIONS} -DSALOME )

IF(NOT DEFINED WIN32)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -funsigned-char -Wall -fmessage-length=0 -DNO_CXX_EXCEPTION -MD -DHAVE_LIMITS_H -DHAVE_IOSTREAM -DHAVE_CONFIG_H -fpermissive -pthread -fopenmp " )
ENDIF(NOT DEFINED WIN32)

MESSAGE ("CMAKE_CXX_FLAGS : ${CMAKE_CXX_FLAGS}")
#MESSAGE ("CAS_DEFINITIONS: ${CAS_DEFINITIONS}")
get_directory_property( DirDefs COMPILE_DEFINITIONS )
message( "COMPILE_DEFINITIONS = ${DirDefs}" )

SET(McSrc .)#qiu
SET(McInc ./McCadHeaders)#qiu

#qiu
QT4_WRAP_CPP(MOCS 	# ${McInc}/QMcCad_Application.h
#                        ${McInc}/QMcCad_Editor.h
#                        ${McInc}/QMcCad_ActionFactory.h
#                        ${McInc}/QMcCad_ActionMaster.h
#                        ${McInc}/QMcCad_View.h
#                        ${McInc}/QMcCad_LogWindow.h
                        ${McInc}/QMcCad_VGParamDialog.h
#                        ${McInc}/QMcCadGeomeTree_TreeWidget.hxx
#                        ${McInc}/QMcCadGeomeTree_AddToMGDialog.h
#                        ${McInc}/McCadDesign_MakeBox.hxx
#                        ${McInc}/McCadDesign_MakeCylinder.hxx
#                        ${McInc}/McCadDesign_MakeCone.hxx
#                        ${McInc}/McCadDesign_MakeSphere.hxx
#                        ${McInc}/McCadDesign_CopyTo.hxx
#                        ${McInc}/McCadDesign_Rotate.hxx
#                        ${McInc}/McCadDesign_MoveTo.hxx
#                        ${McInc}/McCadDesign_Scale.hxx
#                        ${McSrc}/McCadDesign/McCadMatManageDlg.hxx
#                        ${McInc}/QMcCad_TransparencyDialog.h
#                        ${McInc}/QMcCad_ClippingPlaneDialog.h
#                        ${McInc}/QMcCad_ReadListDialog.hxx
)


ADD_LIBRARY (TKMcCad	
#qiu ADD_LIBRARY (TKMcCadGUI			
#								${McSrc}/McCadGUI/McCadGUI_GuiFactory.cxx
#                                ${McSrc}/McCadGUI/McCadGUI_BaseDialog.cxx
#                                ${McSrc}/McCadGUI/McCadGUI_ColorDialog.cxx
#                                ${McSrc}/McCadGUI/McCadGUI_FileDialog.cxx
#                                ${McSrc}/McCadGUI/McCadGUI_InputDialog.cxx
#                                ${McSrc}/McCadGUI/McCadGUI_ProgressDialog.cxx #)


#ADD_LIBRARY (TKMcCadDiscDs		
#								${McSrc}/McCadDiscDs/McCadDiscDs_SequenceOfDiscFace_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_HSequenceOfDiscFace_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_ListOfDiscFace_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_SequenceOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_HSequenceOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_ListOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_SequenceNodeOfSequenceOfDiscFace_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_ListNodeOfListOfDiscFace_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_ListIteratorOfListOfDiscFace_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_SequenceNodeOfSequenceOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_ListNodeOfListOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_ListIteratorOfListOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_DiscFace.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_SequenceOfSeqOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_HSequenceOfHSeqOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_SequenceNodeOfSequenceOfSeqOfDiscSolid_0.cxx
#                                ${McSrc}/McCadDiscDs/McCadDiscDs_DiscSolid.cxx #)


#ADD_LIBRARY (TKMcCadTDS		
#								${McSrc}/McCadTDS/McCadTDS_ExtFace.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_ExtSolid.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_SequenceOfExtFace_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_HSequenceOfExtFace_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_SequenceOfExtSolid_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_HSequenceOfExtSolid_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_DataMapOfIntegerExtFace_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_DataMapOfIntegerSolid_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_SequenceNodeOfSequenceOfExtFace_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_SequenceNodeOfSequenceOfExtSolid_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_DataMapNodeOfDataMapOfIntegerExtFace_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_DataMapIteratorOfDataMapOfIntegerExtFace_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_DataMapNodeOfDataMapOfIntegerSolid_0.cxx
#                                ${McSrc}/McCadTDS/McCadTDS_DataMapIteratorOfDataMapOfIntegerSolid_0.cxx #)

#ADD_LIBRARY (TKMcCadGTOOL 		
#								${McSrc}/McCadGTOOL/McCadGTOOL.cxx #)

#ADD_LIBRARY (TKMcCadBoolExp		
#								${McSrc}/McCadBoolExp/McCadBoolExp_PostFixer.cxx
#                                ${McSrc}/McCadBoolExp/McCadBoolExp_ExprParser.cxx #)

#ADD_LIBRARY (TKMcCadEXPlug	
#								${McSrc}/McCadEXPlug/McCadEXPlug_ExchangePlugin.cxx
#                                ${McSrc}/McCadEXPlug/McCadEXPlug_PluginManager.cxx
#                                ${McSrc}/McCadEXPlug/McCadEXPlug.cxx
#                                ${McSrc}/McCadEXPlug/McCadEXPlug_MCBase.cxx
#                                ${McSrc}/McCadEXPlug/McCadEXPlug_SequenceOfSharedLibrary_0.cxx
#                                ${McSrc}/McCadEXPlug/McCadEXPlug_SequenceNodeOfSequenceOfSharedLibrary_0.cxx #)

#ADD_LIBRARY (TKMcCadExStep	
#								${McSrc}/McCadExDllStep/McCadExDllStep_Step.cxx #)

#ADD_LIBRARY (TKMcCadExIges		
#								${McSrc}/McCadExDllIges/McCadExDllIges_Iges.cxx #)

#ADD_LIBRARY (TKMcCadExBRep  	
#								${McSrc}/McCadExDllBrep/McCadExDllBrep_Brep.cxx #)

#ADD_LIBRARY (TKMcCadCSGGeom 	
#								${McSrc}/McCadCSGGeom/McCadCSGGeom_SequenceOfSurface_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_SequenceOfCell_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapOfIntegerSurface_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapOfIntegerCell_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_SequenceOfTransformation_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapOfIntegerTransformation_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_SequenceNodeOfSequenceOfSurface_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_SequenceNodeOfSequenceOfCell_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapNodeOfDataMapOfIntegerSurface_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerSurface_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapNodeOfDataMapOfIntegerCell_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerCell_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_SequenceNodeOfSequenceOfTransformation_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapNodeOfDataMapOfIntegerTransformation_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_DataMapIteratorOfDataMapOfIntegerTransformation_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_HSequenceOfSurface_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_HSequenceOfCell_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_HSequenceOfTransformation_0.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_GQ.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Sphere.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Plane.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Cylinder.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Torus.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Cell.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Cone.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Transformation.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_SurfaceOfRevolution.cxx
#                                ${McSrc}/McCadCSGGeom/McCadCSGGeom_Surface.cxx #)

#ADD_LIBRARY (TKMcCadCSGBuild 	
#								${McSrc}/McCadCSGBuild/McCadCSGBuild_SolidFromCSG.cxx #)

#ADD_LIBRARY (TKMcCadMcWrite 	
#								${McSrc}/McCadMcWrite/McCadMcWrite_McInputGenerator.cxx
#                                ${McSrc}/McCadMcWrite/McCadMcWrite_McnpInputGenerator.cxx
#                                ${McSrc}/McCadMcWrite/McCadMcWrite_TripoliInputGenerator.cxx #)

#ADD_LIBRARY (TKMcCadMcRead	
#								${McSrc}/McCadMcRead/McCadMcRead_SequenceOfFill_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_HSequenceOfFill_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_DataMapOfIntegerFill_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_SequenceNodeOfSequenceOfFill_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_DataMapNodeOfDataMapOfIntegerFill_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_DataMapIteratorOfDataMapOfIntegerFill_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_SequenceOfCell_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_HSequenceOfCell_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_DataMapOfIntegerCell_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_SequenceNodeOfSequenceOfCell_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_DataMapNodeOfDataMapOfIntegerCell_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_DataMapIteratorOfDataMapOfIntegerCell_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_SequenceOfCellPtr_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_HSequenceOfCellPtr_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_SequenceNodeOfSequenceOfCellPtr_0.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_Cell.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_Fill.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_FillBuilder.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_TrsfBuilder.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_CellBuilder.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_SurfaceFactory.cxx
#                                ${McSrc}/McCadMcRead/McCadMcRead_Reader.cxx #)

#ADD_LIBRARY (TKMcCadCSG 	
#								${McSrc}/McCadCSGAdapt/McCadCSGAdapt_ShapeAnalyser.cxx
#                                ${McSrc}/McCadCSGAdapt/McCadCSGAdapt_SolidAnalyser.cxx
#                                ${McSrc}/McCadCSGAdapt/McCadCSGAdapt_ShellAnalyser.cxx
#                                ${McSrc}/McCadCSGAdapt/McCadCSGAdapt_FaceAnalyser.cxx
#                                ${McSrc}/McCadCSGAdapt/McCadCSGAdapt_ShapeGeomAnalyser.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_TrivialCheck.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_Extender.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_FaceFuser.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_CellBuilder.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_Cutter.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_Decomposer.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_SignCheck.cxx
#                                ${McSrc}/McCadCSGTool/McCadCSGTool_SurfaceChecker.cxx#)
#ADD_LIBRARY (TKMcCadCSGUtil 	
#								${McSrc}/McCadCSGUtil/McCadCSGUtil_SolidBuilder.cxx #)

#ADD_LIBRARY (TKMcCadExMcnp 	
#								${McSrc}/McCadExDllMcnp/McCadExDllMcnp_Mcnp.cxx #)

#ADD_LIBRARY (TKMcCadExTripoli 	
#								${McSrc}/McCadExDllTripoli/McCadExDllTripoli_Tripoli.cxx#)

#ADD_LIBRARY (TKMcCadDiscretization 
#								${McSrc}/McCadDiscretization/McCadDiscretization_Face.cxx
#                                ${McSrc}/McCadDiscretization/McCadDiscretization_Solid.cxx #)

#ADD_LIBRARY (TKMcCadCom		
#								${McSrc}/McCadCom/McCadCom_Document.cxx
#                                ${McSrc}/McCadCom/McCadCom_View.cxx
#                                ${McSrc}/McCadCom/McCadCom_CasDocument.cxx
#                                ${McSrc}/McCadCom/McCadCom_CasView.cxx
#                                ${McSrc}/McCadCom/McCadCom_SequenceOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_MapOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapofIntegerListOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_SequenceOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_MapOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapofIntegerListOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_SequenceNodeOfSequenceOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListNodeOfListOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListIteratorOfListOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_StdMapNodeOfMapOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_MapIteratorOfMapOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapNodeOfDataMapofIntegerListOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapIteratorOfDataMapofIntegerListOfDocument_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_SequenceNodeOfSequenceOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListNodeOfListOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListIteratorOfListOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_StdMapNodeOfMapOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_MapIteratorOfMapOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapNodeOfDataMapofIntegerListOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapIteratorOfDataMapofIntegerListOfView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_SequenceOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_MapOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapofIntegerListOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_SequenceNodeOfSequenceOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListNodeOfListOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_ListIteratorOfListOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_StdMapNodeOfMapOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_MapIteratorOfMapOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapNodeOfDataMapofIntegerListOfCasView_0.cxx
#                                ${McSrc}/McCadCom/McCadCom_DataMapIteratorOfDataMapofIntegerListOfCasView_0.cxx
								#)

#ADD_LIBRARY (TKMcCadAEV		
#								${McSrc}/McCadAEV/McCadAEV_Application.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_Editor.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_Viewer.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceOfViewer_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListOfViewer_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceOfEditor_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListOfEditor_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceNodeOfSequenceOfViewer_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListNodeOfListOfViewer_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListIteratorOfListOfViewer_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceNodeOfSequenceOfEditor_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListNodeOfListOfEditor_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListIteratorOfListOfEditor_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceOfEditorPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListOfEditorPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceOfViewerPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListOfViewerPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceNodeOfSequenceOfEditorPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListNodeOfListOfEditorPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListIteratorOfListOfEditorPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_SequenceNodeOfSequenceOfViewerPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListNodeOfListOfViewerPtr_0.cxx
#                                ${McSrc}/McCadAEV/McCadAEV_ListIteratorOfListOfViewerPtr_0.cxx #)

#ADD_LIBRARY (TKMcCadMcVoid 	
# coment out to use new code of McCad5.1
#				${McSrc}/McCadMcVoid/McCadMcVoid_Generator.cxx
#                                ${McSrc}/McCadMcVoid/McCadMcVoid.cxx
#                                ${McSrc}/McCadMcVoid/McCadMcVoid_ForwardCollision.cxx
#                                ${McSrc}/McCadMcVoid/McCadConvexSolid.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomData.cxx
#                                ${McSrc}/McCadMcVoid/McCadVoidCell.cxx
#                                ${McSrc}/McCadMcVoid/McCadVoidCellManager.cxx
#                                ${McSrc}/McCadMcVoid/McCadExtFace.cxx
#                                ${McSrc}/McCadMcVoid/McCadExtAuxFace.cxx
#                                ${McSrc}/McCadMcVoid/McCadExtBndFace.cxx
#                                ${McSrc}/McCadMcVoid/IGeomFace.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomPlane.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomCylinder.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomCone.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomSphere.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomTorus.cxx
#                                ${McSrc}/McCadMcVoid/McCadGeomRevolution.cxx
#                                ${McSrc}/McCadMcVoid/McCadTransfCard.cxx
#                                ${McSrc}/McCadMcVoid/McCadSolid.cxx
#                                ${McSrc}/McCadMcVoid/McCadTripoliCell.cxx
#                                ${McSrc}/McCadMcVoid/IMcCadWriter.cxx
#                                ${McSrc}/McCadMcVoid/McCadMcnpWriter.cxx
#                                ${McSrc}/McCadMcVoid/McCadGDMLWriter.cxx
#                                ${McSrc}/McCadMcVoid/McCadTripoliWriter.cxx
#                                ${McSrc}/McCadMcVoid/McCadVoidGenerator.cxx

                                ${McSrc}/McCadMcVoid/McCadConvexSolid.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomData.cxx
                                ${McSrc}/McCadMcVoid/McCadVoidCell.cxx
                                ${McSrc}/McCadMcVoid/McCadVoidCellManager.cxx
                                ${McSrc}/McCadMcVoid/McCadExtFace.cxx
                                ${McSrc}/McCadMcVoid/McCadExtAstFace.cxx
                                ${McSrc}/McCadMcVoid/McCadExtBndFace.cxx
                                ${McSrc}/McCadMcVoid/IGeomFace.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomPlane.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomCylinder.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomCone.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomSphere.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomTorus.cxx
                                ${McSrc}/McCadMcVoid/McCadGeomRevolution.cxx
                                ${McSrc}/McCadMcVoid/McCadTransfCard.cxx
                                ${McSrc}/McCadMcVoid/McCadSolid.cxx
                                ${McSrc}/McCadMcVoid/McCadTripoliCell.cxx
                                ${McSrc}/McCadMcVoid/IMcCadWriter.cxx
                                ${McSrc}/McCadMcVoid/McCadMcnpWriter.cxx
                                ${McSrc}/McCadMcVoid/McCadTripoliWriter.cxx
                                ${McSrc}/McCadMcVoid/McCadVoidGenerator.cxx
                                ${McSrc}/McCadMcVoid/McCadVoidGenerator.cxx
                                ${McSrc}/McCadMcVoid/McCadStitchSurfaces.cxx
                                ${McSrc}/McCadMcVoid/McCadVoidCollision.cxx
                                ${McSrc}/McCadMcVoid/McCadAddAstSurface.cxx
                                ${McSrc}/McCadMcVoid/McCadGDMLWriter.cxx#)
#								#qiu move source from McCadTool to here
#								${McSrc}/McCadTool/MaterialManager.cxx
#                                ${McSrc}/McCadTool/Material.cxx
#                                ${McSrc}/McCadTool/MaterialGroup.cxx
#                                ${McSrc}/McCadTool/McCadGeomTool.cxx
#                                ${McSrc}/McCadTool/McCadMathTool.cxx
#                                ${McSrc}/McCadTool/McCadGeneTool.cxx
#                                ${McSrc}/McCadTool/McCadConvertConfig.cxx
#                                ${McSrc}/McCadTool/McCadFuseSurface.cxx #)
# coment out to use new code of McCad5.1
#ADD_LIBRARY (TKMcCadTool	 
#								${McSrc}/McCadTool/McCadTool_Task.cxx
#                                ${McSrc}/McCadTool/McCadTool_TaskMaster.cxx
#                                ${McSrc}/McCadTool/McCadTool_TaskHistory.cxx
#                                ${McSrc}/McCadTool/McCadTool_ListOfTask_0.cxx
#                                ${McSrc}/McCadTool/McCadTool_ListNodeOfListOfTask_0.cxx
#                                ${McSrc}/McCadTool/McCadTool_ListIteratorOfListOfTask_0.cxx
#                                ${McSrc}/McCadTool/MaterialManager.cxx
#                                ${McSrc}/McCadTool/Material.cxx
#                                ${McSrc}/McCadTool/MaterialGroup.cxx
#                                ${McSrc}/McCadTool/McCadGeomTool.cxx
#                                ${McSrc}/McCadTool/McCadMathTool.cxx
#                                ${McSrc}/McCadTool/McCadGeneTool.cxx
#                                ${McSrc}/McCadTool/McCadConvertConfig.cxx
#                                ${McSrc}/McCadTool/McCadFuseSurface.cxx #)

#                                ${McSrc}/McCadTool/McCadTool_Task.cxx
#                                ${McSrc}/McCadTool/McCadTool_TaskMaster.cxx
#                                ${McSrc}/McCadTool/McCadTool_TaskHistory.cxx
#                                ${McSrc}/McCadTool/McCadTool_ListOfTask_0.cxx
#                                ${McSrc}/McCadTool/McCadTool_ListNodeOfListOfTask_0.cxx
#                                ${McSrc}/McCadTool/McCadTool_ListIteratorOfListOfTask_0.cxx
                                ${McSrc}/McCadTool/MaterialManager.cxx
                                ${McSrc}/McCadTool/Material.cxx
                                ${McSrc}/McCadTool/MaterialGroup.cxx
                                ${McSrc}/McCadTool/McCadGeomTool.cxx
                                ${McSrc}/McCadTool/McCadMathTool.cxx
                                ${McSrc}/McCadTool/McCadGeneTool.cxx
                                ${McSrc}/McCadTool/McCadConvertConfig.cxx
                                ${McSrc}/McCadTool/McCadFuseSurface.cxx
                                ${McSrc}/McCadTool/McCadBooleanOpt.cxx
                                ${McSrc}/McCadTool/McCadEvaluator.cxx
                                ${McSrc}/McCadTool/McCadCSGTool_SurfaceChecker.cxx
                                ${McSrc}/McCadTool/GEOMAlgo_Splitter.cxx#)
#ADD_LIBRARY (TKMcCadDecompose
                                ${McSrc}/McCadDecompose/McCadDecompose.cxx
                                ${McSrc}/McCadDecompose/McCadTriangle.cxx
                                ${McSrc}/McCadDecompose/McCadBndSurfCylinder.cxx
                                ${McSrc}/McCadDecompose/McCadBndSurfPlane.cxx
                                ${McSrc}/McCadDecompose/McCadAstSurfPlane.cxx
                                ${McSrc}/McCadDecompose/McCadDcompSolid.cxx
                                ${McSrc}/McCadDecompose/McCadEdge.cxx
                                ${McSrc}/McCadDecompose/McCadEdgeLine.cxx
                                ${McSrc}/McCadDecompose/McCadEdgeEllipse.cxx
                                ${McSrc}/McCadDecompose/McCadEdgeCircle.cxx
                                ${McSrc}/McCadDecompose/McCadEdgeSpline.cxx
                                ${McSrc}/McCadDecompose/McCadSplitCylinders.cxx
                                ${McSrc}/McCadDecompose/McCadSplitCylnPln.cxx
                                ${McSrc}/McCadDecompose/McCadSurface.cxx
                                ${McSrc}/McCadDecompose/McCadBndSurface.cxx
                                ${McSrc}/McCadDecompose/McCadAstSurface.cxx
                                ${McSrc}/McCadDecompose/McCadSelSplitSurf.cxx
                                ${McSrc}/McCadDecompose/McCadRepair.cxx
                                ${McSrc}/McCadDecompose/McCadDcompGeomData.cxx)

#qiu comment-out this library because McCad GUI is not used anymore
#ADD_LIBRARY (TKMcCadViewTool	${McSrc}/McCadViewTool/McCadViewTool_FitAll.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Axis.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Axo.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_BackProject.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_FrontProject.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_TopProject.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_BottomProject.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_LeftProject.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_RightProject.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Pan.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Rotate.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Reset.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Zoom.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_SetBackgroundColor.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_SetHiddenLine.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_SetAntialiasing.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Transparency.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Dump.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Redraw.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_ZoomOut.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_ZoomIn.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_SetShadingMode.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_WireFrame.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_Delete.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_SetShapeColor.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_ShowAll.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_HideSelected.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_RedisplaySelected.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_DisplaySelectedOnly.cxx
#                                ${McSrc}/McCadViewTool/McCadViewTool_ReadValueList.cxx )

#ADD_LIBRARY (TKMcCadConvertTools 
#								${McSrc}/McCadConvertTools/McCadConvertTools_Convertor.cxx
#                                 ${McSrc}/McCadConvertTools/McCadConvertTools_VoidGenerator.cxx #)

#ADD_LIBRARY (TKMcCadIOHelper    
#								${McSrc}/McCadIOHelper/McCadIOHelper_InputFileParser.cxx
#                                ${McSrc}/McCadIOHelper/McCadIOHelper_Expander.cxx
#                                ${McSrc}/McCadIOHelper/McCadIOHelper_Merger.cxx#)

#ADD_LIBRARY (TKMcCadMDReader    
#								${McSrc}/McCadMDReader/McCadMDReader_DataMapIteratorOfDataMapOfAsciiStringReal_0.cxx
#                                ${McSrc}/McCadMDReader/McCadMDReader_DataMapNodeOfDataMapOfAsciiStringReal_0.cxx
#                                ${McSrc}/McCadMDReader/McCadMDReader_DataMapOfAsciiStringReal_0.cxx
#                                ${McSrc}/McCadMDReader/McCadMDReader_MapAsciiStringHasher_0.cxx
#                                ${McSrc}/McCadMDReader/McCadMDReader_Reader.cxx #)

#ADD_LIBRARY (TKMcCadXCAF	
#								${McSrc}/McCadXCAF/McCadXCAF_Application.cxx
#                                ${McSrc}/McCadXCAF/McCadXCAF_TDocShapeLabelIterator.cxx #)

#qiu comment-out this library because McCad GUI is not used anymore
#ADD_LIBRARY (TKQMcCadTools	${McSrc}/McCadDesign/McCadDesign_Tool.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_MakeBox.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_MakeCylinder.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_MakeCone.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_MakeSphere.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_CopyTo.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_Rotate.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_MoveTo.cxx
#                                ${McSrc}/McCadDesign/McCadDesign_Scale.cxx
#                                ${McSrc}/McCadDesign/McCadMatManageDlg.cxx )

#ADD_LIBRARY (TKMcCadMessenger   
#								${McSrc}/McCadMessenger/McCadMessenger_Singleton.cxx
#                                ${McSrc}/McCadMessenger/McCadMessenger_StdOut.cxx
#)

#ADD_LIBRARY (TKQMcCadMessenger  
#								${McSrc}/QMcCadMessenger/QMcCadMessenger_MainWindowMessage.cxx
#)

#add_library (TKMcCadIOTools     
#								${McSrc}/McCadIOTools/McCadIOTools_ExportBrep.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ExportMcnp.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ExportIges.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ExportStep.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ExportTripoli.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ImportBrep.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ImportIges.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ImportMcnp.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_ImportStep.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools_MCExportBase.cxx
#                                ${McSrc}/McCadIOTools/McCadIOTools.cxx
#                                ${McSrc}/McCadIOTools/McCadInputModelData.cxx
								
#								${McSrc}/QMcCad/QMcCad_VGParamDialog.cpp  #qiu this file is used in
#								${MOCS}
#								)


#
# Link libraries with each other
#-------------------------------------------------------



#####################################################################
#								    #
#			 Build McCad executable			    #
#								    #
#####################################################################
#qiu
#SET ( QMCCAD ${McSrc}/QMcCad )

#SET (QSRCS 	${QMCCAD}/main.cpp
#                ${QMCCAD}/QMcCad_Application.cpp
#                ${QMCCAD}/QMcCad_Editor.cpp
#                ${QMCCAD}/QMcCad_ActionFactory.cpp
#                ${QMCCAD}/QMcCad_ActionMaster.cpp
#                ${QMCCAD}/QMcCad_Viewer.cpp
#                ${QMCCAD}/QMcCad_View.cpp
#                ${QMCCAD}/QMcCad_LogWindow.cpp
#                ${QMCCAD}/QMcCad_VGParamDialog.cpp
#                ${McSrc}/QMcCadGeomeTree/QMcCadGeomeTree_TreeWidget.cxx
#                ${McSrc}/QMcCadGeomeTree/QMcCadGeomeTree_AddToMGDialog.cxx
#                ${QMCCAD}/QMcCad_TransparencyDialog.cpp
#                ${QMCCAD}/QMcCad_ClippingPlaneDialog.cpp
#                ${QMCCAD}/QMcCad_ReadListDialog.cxx)

SET (OCC_LIBRARIES 	${CAS_TKernel}
                        ${CAS_TKMath}
                        ${CAS_TKService}
                        ${CAS_TKG2d}
                        ${CAS_TKBRep}
                        ${CAS_TKG3d}
                        ${CAS_TKGeomAlgo}
                        ${CAS_TKV3d}
                        ${CAS_TKBool}
                        ${CAS_TKTopAlgo}
                        ${CAS_TKGeomBase}
                        ${CAS_TKFillet}
#                        ${CAS_TKV2d}
                        ${CAS_TKPrim}
                        ${CAS_TKHLR}
                        ${CAS_TKMesh}
                        ${CAS_TKBO}
                        ${CAS_TKShHealing}
                        ${CAS_TKCDF}
                        ${CAS_TKLCAF}
                        ${CAS_TKXDESTEP}
                        ${CAS_TKXDEIGES}
                        ${CAS_TKXCAF}
                        ${CAS_TKCAF}  
						${CAS_TKMeshVS}
						${CAS_TKXSBase}
						${CAS_TKSTEP}
						${CAS_TKIGES}
						)

#qiu
#SET (MCCAD_LIBRARIES	TKMcCadAEV
#                        TKMcCadBoolExp
#                        TKMcCadCom
#                        TKMcCadConvertTools
#                        TKMcCadCSG
#                        TKMcCadCSGBuild
#                        TKMcCadCSGGeom
#                        TKMcCadCSGUtil
#                        TKMcCadDiscDs
#                        TKMcCadDiscretization
#                        TKMcCadEXPlug
#                        TKMcCadGTOOL
#                        TKMcCadGUI
#                        TKMcCadIOHelper
#                        TKMcCadIOTools
#                        TKMcCadMcRead
#                        TKMcCadMcVoid
#                        TKMcCadMcWrite
#                        TKMcCadMDReader
#                        TKMcCadMessenger
#                        TKMcCadTDS
#                        TKMcCadTool
#                        TKMcCadXCAF
#                        TKMcCadViewTool
#                        TKQMcCadMessenger
#                        TKQMcCadTools
#     )


#
# Executable
#---------------------------------------------------------------------------


#qiu ADD_EXECUTABLE (McCadApp ${QSRCS}
#                         ${MOCS}
#                         ${QMCCAD}/qrc_qmccad.cpp )

#
# link libraries to the executable
#---------------------------------------------------------------------------

#qiu TARGET_LINK_LIBRARIES (McCadApp  ${OCC_LIBRARIES}
#                                 ${QT_LIBRARIES}
#                                 ${MCCAD_LIBRARIES}
#                                 pthread )

#qiu compile as one library to avoid million of linking issue.
TARGET_LINK_LIBRARIES (TKMcCad   ${OCC_LIBRARIES}
                                 ${QT_LIBRARIES}
                                 #${MCCAD_LIBRARIES}
                                 #pthread 
								 )
ADD_EXECUTABLE (McCadDecomposer ${McSrc}/McCadDecompose/McCadDecomposerMain.cxx)
TARGET_LINK_LIBRARIES (McCadDecomposer TKMcCad  ${OCC_LIBRARIES} ${QT_LIBRARIES})

#
# define installation
#
#qiu only install libraries, without McCad executable
install ( TARGETS #qiu McCadApp
                  #qiu ${MCCAD_LIBRARIES}
#                  TKMcCadAEV
#                  TKMcCadBoolExp
#                  TKMcCadCom
#                  TKMcCadConvertTools
#                  TKMcCadCSG
#                  TKMcCadCSGBuild
#                  TKMcCadCSGGeom
#                  TKMcCadCSGUtil
#                  TKMcCadDiscDs
#                  TKMcCadDiscretization
#                  TKMcCadEXPlug
#                  TKMcCadGTOOL
#                  TKMcCadGUI
#                  TKMcCadIOHelper
#                  TKMcCadIOTools
#                  TKMcCadMcRead
#                  TKMcCadMcVoid
#                  TKMcCadMcWrite
#                  TKMcCadMDReader
#                  TKMcCadMessenger
#                  TKMcCadTDS
#                  TKMcCadTool
#                  TKMcCadXCAF
#qiu comment-out this library because McCad GUI is not used anymore
#				  TKMcCadViewTool
#                  TKQMcCadMessenger
#qiu comment-out this library because McCad GUI is not used anymore
#                  TKQMcCadTools
#                  TKMcCadExBRep
#                  TKMcCadExIges
#                  TKMcCadExStep
#                  TKMcCadExMcnp
#                  TKMcCadExTripoli
					TKMcCad
                  EXPORT ${PROJECT_NAME}TargetGroup
                  DESTINATION ${SALOME_INSTALL_LIBS}
                  )
INSTALL(TARGETS McCadDecomposer EXPORT ${PROJECT_NAME}TargetGroup DESTINATION ${SALOME_INSTALL_BINS})

#install( FILES ${PROJECT_SOURCE_DIR}/bin/McCad
#               ${PROJECT_SOURCE_DIR}/bin/McCadConvertScript
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
#         PERMISSIONS OWNER_READ
#                     OWNER_WRITE
#                     OWNER_EXECUTE
#                     GROUP_READ
#                     GROUP_EXECUTE
#                     WORLD_READ
#                     WORLD_EXECUTE )

#install( DIRECTORY ${PROJECT_SOURCE_DIR}/doc
#                   ${PROJECT_SOURCE_DIR}/tutorial
#         DESTINATION ${CMAKE_INSTALL_PREFIX}/share/mccad
#         FILE_PERMISSIONS OWNER_READ
#                     OWNER_WRITE
#                     OWNER_EXECUTE
#                     GROUP_READ
#                     GROUP_EXECUTE
#                     WORLD_READ
#                     WORLD_EXECUTE
#         DIRECTORY_PERMISSIONS OWNER_READ
#                               OWNER_WRITE
#                               OWNER_EXECUTE
#                               GROUP_READ
#                               GROUP_EXECUTE
#                               WORLD_READ
#                               WORLD_EXECUTE )
