/**
* @mainpage notitle
* <br><br><br>
* \image html McCad_KIT_Logo_1.png "version 0.1 (beta)" width=10cm
*
* @authors <A HREF="mailto:dennis.grosse@kit.edu"> Dennis Gro&szlig;e </A>
*
* <br>
* <B>Table of content:</B>
* \li <A HREF="#intro"> Introduction </A>
* \li <A HREF="#requirements"> Requirements </A>
* \li <A HREF="#install"> Installation </A>
* \li <A HREF="#use"> Use of McCad </A>
* \li <A HREF="#dev"> Development </A>
* \li <A HREF="#contact"> Contact </A>
*
* <br>
* @section intro Introduction
* McCad is a CAD interface programme for Monte Carlo (MC) particle transport codes. It has been developed at <A HREF="http://www.kit.edu">Karlsruhe Intitute of Technology (KIT)</A>. <br>
* In the current version the programme generates MC geometry description for the MC codes MCNP5 and Tripoli4. Additionally it reads MCNP5 input files and translates the geometry description into CAD geometry. <br><br>
* McCad uses only open source software and is conceived as open source project itself. Please take a look at the <A HREF="../LICENSE_draft3.pdf#page=1">License agreement</A>.<br><br>
* Supported CAD file formats are the CAD system independent formats STEP, IGES and BREP. The STEP file format is most suitable for the conversion process. It is supported by all major CAD systems. Due to the used CAD kernel and the capability of the MC codes to handle only algebraical surface types of order up to two (and tori) the choice for surfaces for a CAD model to be converted by McCad is restricted to the following types: <br>
* \li Planes
* \li Cylinders
* \li Cones
* \li Spheres
* \li Circular Tori
*
* All solids using unsuitable surface types will be neglected. One can test if a CAD file is suitable for conversion by running \c McCad \c -s \c InputFile.stp from command line. <br>
*
*<hr><br>
*
* @section requirements Requirements
* \li Operating System: Linux / Unix
* \li cmake >= 2.6.0 (for installation) 
* \li OpenCascade 6.3 
* \li Qt >= 4.2 
* \li doxygen (for documentation)
*
* For hardware requirements please take a look at the documentation of the OpenCascade 6.3 software package. <br>
*
*<hr><br>
*
* @section install Installation
* For the install of McCad the software listed above (<A HREF="#requirements">Requirements</A>) section must be installed and configured properly. <br>
*<br>
* This is a step by step installation guide:
*<pre>
 Assuming the following:
     QT4 exists on system already
     you have downloaded the OpenCASCADE 6.3.0 source tarball to $HOME/Downloads/OpenCASCADE_src.tgz
     you have copied the McCad sources to $MCCADROOT
     $HOME/opt/bin is in your path

 1. Before doing anything else, add this to your environment and startup scripts
     export CASROOT="$HOME/opt/OCC630"
     export MCCADROOT="$HOME/opt/McCad"

 2. Build OpenCASCADE 6.3.0 
     mkdir -p $CASROOT/install
     cd $CASROOT/install
     tar xzf $HOME/Downloads/OpenCASCADE_src.tgz
     cd OpenCASCADE6.3.0/ros
     configure --prefix=$CASROOT --with-tcl --with-tk --enable-wok
     make install

 3. Configure environment
     add OpenCascade variables to your environment
     the easiest way is to run the file $CASROOT/env_DRAW.sh after setting $CASROOT in your startup scripts

 4. Build McCad
     mkdir $MCCADROOT/build
     cd $MCCADROOT/build
     cmake ../
     make

 5. Configure environment for McCad
     export MCCAD_LIBDIR=$MCCADROOT/lib

 6. Put a copy of the executable in your path
     cp $MCCADROOT/bin/McCad $HOME/opt/bin
    or set a symbolic link
     ln -s $MCCADROOT/bin/McCad $HOME/opt/bin/McCad
    
                                                (Special thanks to Paul Wilson, who basically wrote this step-by-step guide)
</pre>
*
*<hr><br>
*
* @section use Use of McCad
* McCad integrates a graphical user interface (GUI) which allows the visualization of CAD geometry and the exchange of CAD data and MC geometry description (CAD<->MCNP, CAD->Tripoli). For the conversion of complex (large) models it should be considered to omit the GUI and perform the conversion step by step from command line. The reason herefore is, that the bigger a CAD model is the higher grows the probability for the occurence of solid parts that cannot be converted. The export functions implemented in the GUI are one-click exports. The algorithmic export from CAD to MC consists of three steps, from which two are critical. The first step is the sign constant decomposition of all input solids. The second step is the generation of potential void volumes. In both cases Boolean operations on CAD solids are performed. Unfortunately, the implementation of the Boolean operations in the used CAD kernel are not failsafe. Also, this is a beta release. There are still some undiscovered bugs in the McCad source code. (For more details on the underlying algorithm please take a look at: <em>D. Grosse, H. Tsige Tamirat, "Current Status of the CAD Interface Programme for MC Particle Transport Codes McCad", Internat. Conf. on Advances in Mathematics, Computional Methods and Reactor Physics, Saratoga Springs, N.Y., May 3-7, 2009, Proc. on CD-ROM, LaGrange Park, III.</em>)
*
*
* @subsection gui GUI
* <br>
* \image html gui1.png "McCad GUI displaying MCNP geometry"
* <br>
* The image shows the McCad GUI displaying the solid geometry of a MCNP input file. By default the void geometry will not be constructed. The frame on the left shows the material groups found in the MCNP input file in the upper area, and below the IDs of the reconstructed volumes. A mouse click on the material group selects all members of the group. <br>
* To modify the visualization the GUI offers different tools. <br>
*<br>
* Visualization tools:
* \li Zoom   (mouse wheel)
* \li Rotate (middle mouse button)
* \li Pan    (ctrl + left mouse button)
* \li Type of visualization -> Shaded, Wireframe, Hidden Lines
* \li Coloring (context menu, right click)
* \li Transparency (context menu, right click)
* \li Definition of clipping planes (window)
* \li Hiding of selected shapes 
* \li Displaying of selected shapes only
* \li Redisplaying of selected shapes
* \li different projections (display of two coordinates in the status bar)
*
* Import (ctr+i) | Export (ctrl+e) :<br>
*<br>
* The GUI imports STEP(.stp, .step), IGES(.igs, iges), BREP(.rle, brep) and MCNP(.mcn) files. Available surface types in MCNP are: <em>P,  PX,  PY,  PZ,  SO,  S,  SX,  SY,  SZ,  C/X,  C/Y,  C/Z,  CX,  CY,  CZ,  K/X,  K/Y,  K/Z,  KX,  KY,  KZ,  TX (circular),  TY (circular),  TZ (circular),  X,  Y, Z, P</em>; as well as the following macro bodies: <em> BOX,  RPP,  SPH,  RCC,  RHP</em>.
* The Export function supports the three mentioned CAD formats (caution BREP allows only one solid to be exported per file) as well as MCNP(.mcn) and Tripoli(.d, .tri) geometry description. When exporting to a MC file format a dialog with (optional) conversion parameters appears. The parameters affect the computation of the void space for the MC file.<br>
*
* \image html parameters.png "Optional MC Export Parameters"
* <br>
* Optional Parameters :
* \li <EM> Write Collision File [0|1] </EM>: If this box is checked, a test for collisions between input solids is performed. The result is written to the file \c .CollisionFile in the working directory. If it is unchecked, the programme trys to read \c .CollisionFile and extract the collision data. If you convert an identical geometry with a different parameter set, you can uncheck this box. It will reduce the conversion time. 
* \li <EM> Write Discrete Model [0|1] </EM>: All faces of all solids will be covered with sample points. These sample points are used for collision detection among the input solids, and between the input solids and the generated void space on CAD level. If this box is checked a \c _*.voxel file will be written for every input solid. The *_.voxel files contain the the location information of the sample points. If you rerun the same geometry with e.g. a different setting for the minimum cut face area, uncheck this box. It will reduce the conversion time.
* \li <EM>Input Units [MM|CM]</EM>: MC codes use \c cm as basis length unit. If the CAD model is not prepared in \c cm units (default in CAD systems is usually \c mm ) select \c mm as input units. Otherwise the output will be wrongly scaled.
* \li <em>Initial Surface Number [integer] </em>: If a CAD part for an already existing MC model will be converted, a range of surface IDs is already ocupied. This parameter sets the initial surface number for the converted CAD model.
* \li <em>Initial Cell Number [integer] </em>: same as <em>Initial Surface Number</em> but for solid volumes
* \li <em>Minimum Input Solid Volume [real]</em>: omits input solid volumes (input units) smaller than this value 
* \li <em>Minimum Void Volume [real]</em>: Determines the minimum volume (input units) of a cutted part of the bounding box (see next parameter). The smaller this value, the more void volumes will be generated
* \li <em>Minimum Decomposition Face Area [real] </em>: The void space is generated from a bounding box which surrounds the whole CAD model. The bounding box is decomposed by all sign changing planar faces of the decomposed CAD model. This value determines the minimum area (input units) a face may have to become a cut face. This parameter is used to regulate the number of void volumes that are generated by the code. The smaller this value, the more void volumes. 
* \li <em>Minimum Redecomposition Face Area [real]</em>: If a void volume collides with too many solids, perform a second decomposition cycle on the void volume. Use this value as minimum area (input units) for cut faces. This value must be smaller than <em>Minimum Decomposition Face Area</em>. This prevents an excessive use of the complement operator.
* \li <em>Maximum Number Of Predecomposed Cells</em>: If the number of cuts of the bounding box reaches this value, the cutting will stop. This prevents the production of too many void volumes.
* \li <em>Maximum Number Of Complement Cells</em>: Determines how many complement operators should be used at a max for one void volume.  This value shouldn't exceed \c 10
* \li <em>Minimum / Maximum Number Of Sample Points</em>: Determine the lower and upper limit for sample point generation. If too many sample points will be generated the code will become noticeable slower and the chance that the computer runs out of memory for larger models increases. If too little sample points are generated the collision test will not be reliable and the resulting MC file might become useless. The limits determine the number of sample points per edge. The desired resolution can be defined with the next parameters. 
* \li <em>X-/Y-Resolution</em>: Determines the desired mesh size for the sample point generation. (input units)
* \li <em>Tolerance</em>: Determines the tolerance for all geometrical operations (measurements) in the problem.
*
* A problem specific bounding box can be loaded into the GUI. One has to rename the corresponding CAD solid <em>"Bounding Box"</em> or <em>"BoundingBox"</em>. Remember these are reserved key words. Any solid carrying one of these names will be treated as a bounding box for the conversion problem. <br>
*These are optional parameters. For most models the predifined parameters should generate acceptable results. 
* <br>
* Design Tools: <br>
* <br>
* The design tools allow to create new macrobodies (Box, Cylinder, Sphere, Cone) and to move them in space (rotate, translate, copy to). Boolean operations are not yet implemented. <br> 
* <br>
*
* @subsection shell Command Line
* On the command line the conversion from CAD to MC is split into two steps. The first step is the sign constant decomposition of all CAD solids. The second is the completion of the model with void geometry and the printing into the MC input file. The following options are available:
* \li -d, --decomposition : perform a sign constant decomposition on a single CAD file. The decomposed geometry will be saved in a STEP file with the same name as the input file's name but with the prefix 'converted'. e.g McCad -d InputFile.stp -> convertedInputFile.stp
* \li -m, --mcnp : complete model by void and export the geometry description to an MCNP input file. This option requires a parameter file. The content of the parameter file is explained below.
* \li -t, --tripoli : complete model by void and export the geometry description to an Tripoli input file. This option requires a parameter file. The content of the parameter file is explained below.
* \li -e, --explode : explodes all solids from an input file into single STEP files named 'ExOut_<em>FILENAME</em>_#Nb.stp'.
* \li -f, --fuse : fuses all STEP files in a folder into a single STEP file. Requires a directory as input parameter. 
* \li -s, --surface-check : Requires a STEP file as input. Prints a list of all surface types in the file. Optionally insert a file name which the information will be written into.
*
*The first three options are the relevant to converting CAD data into MC input geometry. To perform the sign constant decomposition on all STEP files in the current directory (files beginning with 'converted' will be omitted) you can use the script <em> McCadConvertScript </em> which is located in <em>$MCCADROOT/bin</em>. To convert CAD data into MCNP geometry description use the <em>-m</em> option, for a convertion into Tripoli use <em>-t</em>. These options require a parameter file as input.
* In the following the content of the parameter is presented. All parameters in the file that are defined by a keyword are optional. At least one directory which contains <em>converted*.stp</em> files must be given.
*The parameter file could contain all keywords mentioned in the <A HREF="#gui">GUI subsection</A> and additionally the definition of a problem specific bounding box (keyword <em>BoundingBox</em>) and the definition of a material information file for the export into MCNP geometry description (keyword <em>MDFile</em>) . This is an example for a McCad parameter file: <br>
*<pre>
	#
	#      McCadInputFile.txt - EXAMPLE
	#
	# '#' introduces a comment line
	# The format of this file is "keyword" "Value" where value can be a
	# string, a real, or integer value. Order is irrelevant. 
	# Each line that isn't a comment or begins with a key word will be 
	# treated as input directory containing <em>converted*.stp</em> files.
	#
	# Do not put comments in keyword lines!!!
	#
	Units                    MM               ... CAD units MM or CM
	InitSurfNb               500              ... Initial Surface Number of output
	InitCellNb               100              ... Initial Volume Number of output
	WriteCollisionFile       1                ... Write .CollisionFile of input solids (1st run only)
	WriteDiscreteModel       1                ... Write *.voxel files for input solids
	MinimumInputSolidVolume  1.00             ... all volumes smaller than this will be neglected
	MinimumVoidVolume        1.25e2           ... make sure void volumes don't become to small
	MinimumSizeOfDecompositionFaceArea   50   ... first cut surface area
	MaximumNumberOfComplementedCells     10   ... how many complement operators may be used per void volume
	MaximumNumberOfPreDecompositionCells 500  ... sets upper bounds for number of void volumes after first cut
	MinimumSizeOfRedecompositionFaceArea 5    ... recut if maximum number of complement cells is exceeded
	MinimumNumberOfSamplePoints 10            ... lower bound for discretization along a face's edge
	MaximumNumberOfSamplePoints 50            ... upper bound for discretization along a face's edge
	XResolution  50                           ... desired resolution of discretization in x direction
	YResolution  50                           ... desired resolution of discretization in y direction
	Tolerance  1e-7                           ... tolerance for all methods
	#
	BoundingBox  /home/user/Testarea/Bounding_Box/BB.stp       ... location and name of bounding box
	# MDFile     /home/user/Testarea/MaterialList/MDList.txt   ... Material and Density information - MCNP only
	#
	# directories that contain the converted*.stp files
	/home/user/Testarea/Model_Part1/
	/home/user/Testarea/Model_Part2/
	/home/user/Testarea/Model_Part3/
 </pre>
* <br>
* For MCNP only, one can define a MDFile which contains material and density information for CAD solids. Unfortunately the material information is not transported with the STEP file. Therefore the need for a material information list arises. A MDFile is a plain ascii string text file containing three coloumns. In the first coloumn stands the name of a CAD file, e.g. <em>convertedTestSolids.stp</em>, without the file extension and without the conversion-prefix <em>converted</em>, i.e. <em>TestSolids</em>. The second coloumn holds the material number the solids should be given and the last coloumn contains the corresponding density. If for example the file <em>TestSolids.stp</em> contains three solid volumes which are supposed to consist of stainless steel, and another file <em>testSolids2.stp</em> containing six solids made of concrete, the corresponding MDfile could look like this:
*<pre>
	# Name		MaterialNumber	   Density
         TestSolids      1		    -8.3
         TestSolids2     2                  -2.4 
</pre>
* When using the MDFile parameter each step file can contain only solids with identical material composition and density. <br>
* <br>
* If you want to add universe labels to the cells in the output file (MCNP only) you can export the variable MCCAD_MAKEUNIVERSE=num, where num is the number of the universe. Importances for neutrons and photons will be set to 1 for all volumes except the outer voids for which the importances are set to 0. 
*<br>
*
* @subsection output Output
* <em>Output from the sign constant decomposition</em>
* \li \c .failedVolumeControl : The accumulated volume of all decomposition parts of a decomposed model will be compared to the volume of the original model. If a quotient differs more 1e-5 from 1 the name of the corresponding file and the volume information will be printed into this file.
*
* <em>Output from the generation of void</em>
* \li \c .Collision : Detected collisions among the input solids. Every volume is listed by the number of occurence. If a collision is detected the second number in a line will differ from 0. This value reflects the  number of collisions. The next values will be the IDs of the input volumes the current solid collides with.
* \li \c *.voxel : Contain the voxel points of the input solids.
* \li \c .cadVolumes : Contains a list with two coloumns. First coloumn: the number of a solid volume in the MC output file. Second coloumn: The corresponding volume calculated by the CAD kernel. 
* \li \c Mcnp_Model.mcn :  Standard name of the converted MCNP model after conversion from command line.
* \li \c Tripoli_Model.d : Standard name of the converted Tripoli model after conversion from command line.
* @subsection example Example
* In <em>$MCCADROOT/sample</em> is a simple CAD example file (brick.stp), a parameter file (McCadInputFile.txt) and a MDFile (MaterialList.txt) located. In the following we'll see how to translate the CAD file into MCNP geometry description including material information (only available for MCNP).
* Change directory to <em>$MCCADROOT/sample</em> and start the McCad GUI by entering <em>McCad</em>. You can tell the GUI that the current directory is the working directory by passing the argument '.' , i.e. "McCad ." <br>
* We import the CAD file <em>brick.stp</em> . Open the import dialog by clicking <em>File</em> and then <em>Import</em> or use the shortcut <em>ctrl+i</em> . Select the file <em>brick.stp</em> and click the <em>Import</em> button. The GUI should show this picture. <br>
*<br>
* \image html brick.png "Visualization of a simple example geometry"
*<br>
* The following paragraph is only relevant for conversion to MCNP! This paragraph describes how to assign material information to a shape. First add a material group by right clicking on the white space in the assembly viewer on the left side and select <em>Add new material group</em>. In the appearing dialog enter the name for the material group you prefere, the density and the material description as it will appear in the MCNP input file in the data section. Confirm the material group. It will now appear with its name in the assembly viewer under <em>Material Groups</em>. Select the solid object by clicking once on it in the visualization window or on its corresponding name in the assembly viewer. For selecting multiple objects, keep the <em>ctrl</em> button pushed down. Right click on the assembly viewer and choose <em> Add solids to material group</em>. Select the material group name from the appearing dialog and confirm your choice. The object is now in the chosen material group. Clicking once on the material group name in the assembly view selects and highlights all members of this group. <br> <br>
*To save a loaded geometry in an xml file (material information will be saved, too) choose <em>File -> Save As</em>. To export the geometry open the export dialog by choosing <em> File -> Export </em> or by its shortcut <em>ctrl+e</em>. Enter a name, choose a file type and click export.<br>
*If you've followed the material assignment steps above you can reimport the MCNP geometry into CAD. Remember that only material volumes will be restored from MC files.<br>
*<br> 
* \image html mc2cad.png "Visualization of the reconversion MCNP -> CAD"
* <br>
* <br>
* Conversion in command line
*<pre>
	$> McCad -h
	   =============================================
           	        McCad
	   =============================================

	   McCad is a CAD interface for Monte Carlo Transport Codes.
	
	   Usage: McCad [Options] [File | Path]  
	
	   Options:
		-d, --decompose :
				Decomposes the CAD input file, requires a STEP-FILE
				If no output file is defined, the output will be saved as
				converted'FILE'.
		-e, --explode:
				Calls the Exploder, requires a STEP-FILE
				Explodes all solids in a STEP file to distinct files.
				Does not accept an Output File.
		-f, --fuse :
				Calls the Fusioner, requires a DIRECTORY
				Fuses all STEP-FILES in the passed Directory into one single file.
		-m, --mcnp:
				Calls the VoidGenerator to generate void volumes,
				and prints the mcnp geometry into a file; requires an McCad_InputFile.
				See manual for more details on the InputFile.
		-s, --surface-check:
				Prints a list of all surface types of a given model into the output file.
		-t, --tripoli:
				Calls the VoidGenerator to generate void volumes,
				and prints the tripoli geometry into a file; requires an McCad_InputFile.
				See manual for more details on the InputFile.

 perform a surface check
	$> McCad -s brick.stp
	   ...
	   FILE: brick.stp

	   number of solids : 1

	   Surface Information for Solid 1
	   ========================================
	   Number Of Surfaces In Model : 38
	   ----------------------------------------
	   Planes                  : 38
	   Cylinder                : 0
	   Cones                   : 0
	   Spheres                 : 0
	   Toroidal Surfaces       : 0
  	   ========================================

 sign constant decomposition
	$> McCad -d brick.stp
	   ...
 compute void space and output to MCNP geometry description
	$> McCad -m McCadInputFile.txt
	   ...	
 output file is Mcnp_Model.mcn	
</pre>
* You can find an example for <em>McCadInputFile.txt</em> and the <em>MaterialList.txt</em> in the $MCCADROOT/sample.
* <hr><br>
*
* @section dev Development
* Implement support for the MC code <em>XYZ</em>: 
*<pre>
   inherit a class <em>McCadExDllXYZ_XYZ</em> from <em>McCadEXPlug_MCBase</em>. For an example take a look at <em>McCadExDllMcnp_Mcnp</em> 
   add the <em>McCadCSGGeom_XYZ</em> to the enumeration in <em>McCadCSGGeom_MCType.hxx</em>
   edit the corresponding print functions in the surface classes in <em>McCadCSGGeom_...</em>, e.g.: McCadCSGGeom_Plane::Print(...)
   implement a parameter switch into <em>main.cpp</em> to load the export function of the newly created class
</pre>
*<br>
* Extract the export function:<br>
* You might want to start at <em>McCadEXPlug_MCBase.cxx</em> which is the base class for all exports to MC data. It is inherited by <em>McCadExDllMcnp_Mcnp</em> and <em>McCadExDllTripoli_Tripoli</em>. The conversion is the same for all MC codes except for the final printing. The base class calls the convert tools <em>McCadConvertTools_Decomposer</em> which performs a sign constant decomposition of the input solids and <em>McCadConvertTools_VoidGenerator</em> which calculates the void space for the output file.  <br>
* <hr><br>
*
*
* @section contact Contact
* <A HREF="mailto:ulrich.fischer@kit.edu"> Ulrich Fischer </A> (official contact)<br> 
* <A HREF="mailto:dennis.grosse@kit.edu"> Dennis Gro&szlig;e </A> (technical questions) 
* <br>
*/
