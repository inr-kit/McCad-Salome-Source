***ATTENTION:*** An important bug is fixed on windows version!!
**************************************************************************************
McCad-Salome: MC geometry conversion tool McCad under Salome platform

McCad is a geometry conversion tool developed at KIT to enable the automatic conversions of CAD models into the Monte Carlo (MC) geometries. This version of McCad is integrated into the open-source Salome platform. 

For any questions related to the use of this software/library you may contact Ulrich Fischer(ulrich.fischer@kit.edu) or, for technical assistance, contact Lei Lu (lei.lu@kit.edu) and Yuefeng Qiu (yuefeng.qiu@kit.edu).Please subscribe our mailing list for receiving latest updates and asking question in our [Google group](https://groups.google.com/forum/#!forum/mccad-users), by send a empty email to  McCad-users+subscribe@googlegroups.com and reply the comfirmation email.
**************************************************************************************


It is not necessary to compile McCad in order to use it, because we provided also precompiled McCad binaries. You can find them in  repository [McCad-Salome-Binaries](https://github.com/inr-kit/McCad-Salome-Binaries). Compile it only if you cannot find precompiled McCad binaries, or you are going to change the source code. For Using McCad, Please find documents on Github repository [McCad-Salome-Docs](https://github.com/inr-kit/McCad-Salome-Docs).  

This release is tested under Salome_7.4.0. Using other Salome version is not guarantee to work, and might cause error during this process. 

**************************************************************************************
********Compilation and installation on Linux system********


***Installing Salome platform***
* Download Salome_7.4.0 platform from http://www.salome-platform.org/downloads/previous-versions/salome-v7.4.0 . 

  * Under the list "Binaries for officially supported Linux platforms", Choose the version which is closest to your OS. 

  * You need to register for downloading Salome. 

* Extract the zip file into a folder, then go into this folder and run the script "runInstall". A install wizard window will come out. 

* Click always "Next", and keep the default value if you don't care.

  *  If you like, you can change the installation folder;

  * It is highly recommended to install all modules except DOCUMENTS, in order to avoid unncessary error. 

  * There might be warnings on "cppunit" libraries and so on, it won't affect the use of Salome. 

* We abbreviate the Salome install folder as $SALOME


***Compiling McCad***

* Download this package, unzip and rename it as "MCCAD_SRC_0.5.0", place it under $SALOME.

* We need to add MCCAD package into the Salome environment for compiling it. In the following changes, back-up those file before changing them. 
	
  * Open $SALOME/build.sh (or $SALOME/build.sh), find line 32, and add "MCCAD" into the list. At the end it looks like: def_modules="${def_modules} RANDOMIZER SIERPINSKY ATOMIC ATOMGEN ATOMSOLV DOCUMENTATION MCCAD" 
  * Add the following environment variables to the end of $SALOME/env_build.sh (if you are using csh, then  $SALOME/env_build.csh)
```	
	#------ MCCAD ------
	export MCCAD_ROOT_DIR=${INST_ROOT}/MCCAD_0.5.0
	if [ -n "${ENV_FOR_LAUNCH}" ] ; then
	  if [ "${ENV_FOR_LAUNCH}" = "1" ] ; then
		exportp PATH ${MCCAD_ROOT_DIR}/bin/salome
		exportp LD_LIBRARY_PATH ${MCCAD_ROOT_DIR}/lib/salome
		exportp PYTHONPATH ${MCCAD_ROOT_DIR}/bin/salome:${MCCAD_ROOT_DIR}/lib/python${PYTHON_VERSION}/site-packages/salome
	  fi
	fi
	##
	#------ MCCAD_src ------
	export MCCAD_SRC_DIR=${INST_ROOT}/MCCAD_SRC_0.5.0
```
* Go to $SALOME, run the following command to compile McCad:
```
	./build.sh MCCAD
```	
* After the compilation, You can find the binaries in $SALOME/INSTALL/MCCAD_0.5.0. Copy this folder "MCCAD_0.5.0" to $SALOME folder. 


***Installing and running McCad***

* Be sure that your $SALOME/MCCAD_0.5.0 folder have following folders:

  *  bin
  *  lib
  *  share
  *  adm_local
  *  idl
  *  include

* Open $SALOME/KERNEL_7.4.0/salome.sh, add the following environment variables into this file.
```
	#------ MCCAD ------
	export MCCAD_ROOT_DIR=${INST_ROOT}/MCCAD_0.5.0
	if [ -n "${ENV_FOR_LAUNCH}" ] ; then
	  if [ "${ENV_FOR_LAUNCH}" = "1" ] ; then
		exportp PATH ${MCCAD_ROOT_DIR}/bin/salome
		exportp LD_LIBRARY_PATH ${MCCAD_ROOT_DIR}/lib/salome
		exportp PYTHONPATH ${MCCAD_ROOT_DIR}/bin/salome:${MCCAD_ROOT_DIR}/lib/python${PYTHON_VERSION}/site-packages/salome
	  fi
	fi
	##
	#------ MCCAD_src ------
	export MCCAD_SRC_DIR=${INST_ROOT}/MCCAD_SRC_0.5.0
```
* In your desktop, create a new file "runSalome.sh" and put following text into this file(replacing $SALOME with actual path!!): 
```
	#!/bin/bash
	source $SALOME/KERNEL_7.4.0/salome.sh
	$SALOME/salome_appli_7.4.0/salome --module=GEOM,SMESH,PARAVIS,MCCAD
```
* Under Desktop, make this file as executable script using this command:

	chmod +x ./runSalome.sh

* You can run McCad-Salome with running this script now. 

**************************************************************************************
********Compilation and installation on Windows system********

ATTENTION: In this following step you need to have Visual Studio 2010. Without it or with other version of Visual Studio will failed in following steps. 

***Installing Salome platform***

*  Download Salome_7.4.0 platform from http://www.salome-platform.org/downloads/previous-versions/salome-v7.4.0 . 
  * The version you need is "SALOME SDK self-extracting archive for 64bits Windows". 
  * You need to register for downloading Salome. 

* Extract the Package into a folder, here brief as $SALOME (be sure to replace it with actual path during installation). 

* Open cmd.exe from Windows Start menu, change folder to $SALOME\WORK, and execute "compile.bat". Please take a coffee, because it takes hours. When the compilation finished, Salome platform will be ready. 

***Compiling McCad***

* Download the McCad-Salome package, unzip it. 

* In $SALOME\MODULES, Create a folder "MCCAD", put the source code inside, and rename the source code folder as "MCCAD_SRC"

	
* We need to add McCad into the Salome environment. First make a backup of file $SALOME\WORK\set_env.bat

  * in line 55, add "MCCAD" into the "list" (add it anywhere inside the parenthesis, separate with at least one whitespace)
	
  * in line 81, add "GUI GEOM MED SMESH PARAVIS MCCAD" into "env_m_list"(add them inside the parenthesis, separate with at least one whitespace).

* In file $SALOME\WORK\compile.py, add the following line to line 35 

	all_modules.append('MCCAD')

* Open cmd.exe from Windows Start menu, change folder to $SALOME\WORK, and execute command:
 
	compile.bat MCCAD
	
* The compilation will finished in a few minutes. 

***Installing and using McCad***

* To run McCad, start cmd.exe in the Windows Start menu, and run the following command:

  * $SALOME\WORK\run_salome.bat --module=MCCAD
	
  * If you want to start also geometry, meshing and visualization module, using command: $SALOME\WORK\run_salome.bat --module=GEOM,SMESH,PARAVIS,MCCAD
	
* One more easy way to run the program is:

  * right-click $SALOME\WORK\run_salome.bat and "Send to -> Desktop (short-cut)";
	
  * right-click the short-cut link in the Desktop, choose "properties";
	
  * Behind the value of "Target", add " --module=MCCAD" or "--module=GEOM,SMESH,PARAVIS,MCCAD"(with a whitespace in the front). Click "OK". Next time you can start McCad with this short-cut link. 



**************************************************************************************
For more information, you can find in our publications.

* Lei Lu, Yuefeng Qiu, Ulrich Fischer, Improved solid decomposition algorithms for the CAD-to-MC conversion tool McCad, Fusion Engineering and Design, Available online 3 March 2017, ISSN 0920-3796, https://doi.org/10.1016/j.fusengdes.2017.02.040.

* Yuefeng Qiu, Lei Lu, Ulrich Fischer, Integrated approach for fusion multi-physics coupled analyses based on hybrid CAD and mesh geometries, Fusion Engineering and Design, Available online 4 July 2015, ISSN 0920-3796, http://dx.doi.org/10.1016/j.fusengdes.2015.06.118.

* L. Lu, U. Fischer, P. Pereslavtsev, Improved algorithms and advanced features of the CAD to MC conversion tool McCad, Fusion Engineering and Design, Volume 89, Issues 9–10, October 2014, Pages 1885-1888, ISSN 0920-3796, http://dx.doi.org/10.1016/j.fusengdes.2014.05.015.


Have fun!






