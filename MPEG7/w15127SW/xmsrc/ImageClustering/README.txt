/////////////////////////////////////////
// This software was developed by      //
// Robert O'Callaghan                  //
// Mitsubishi Electric ITE VI-Lab      //
// Export Control Status: NLR          //
// Version 1.0                         //
// Copyright (c) 2005                  //
/////////////////////////////////////////

This is the README file for the MPEG-7 Reference Software on "Situation"-Based Image Clustering.
To get information about

- how to compile this software see the Compilation.txt file coming with this
  archive in the same directory.
- the function of each Descriptor see the .txt files
  in the directory newsrc/Doc/Video of this archive

The software is based on the original MPEG-7 XM.  It is essentially a stripped down and
modified version to fit the purposes of the clustering algorithm. The software operates
in two modes:

1) Extraction mode, similar to the standard XM:

ClusterWinExe.exe -p[DC|CS|SC|CL|HT|EH] {ParameterFile} -a {XXXServer} -l {ImageListFile}

where 
DC -> DominantColor
CS -> ColorStructure
SC -> ScalableColor
CL -> ColorLayout
HT -> HomogeneousTexture
EH -> EdgeHistogram

e.g.,

ClusterWinExe.exe -pCL newpar/ColLayout.par -a ColorLayoutServer -l ..\dbtest.lst


2) Clustering mode

ClusterWinExe.exe -pEH {EHParFile} -pHT {HTParFile} -pDC {DCParFile} -pCS {CSParFile} -pSC {SCParFile} -pCL {CLParFile} -l {ImageListFile} -o {ClusterOutputFile}

e.g.,

ClusterWinExe.exe -pEH newpar/EdgeHistogram.par -pHT newpar/HomogeneousTexture.par -pDC newpar/DominantColor.par -pCS newpar/ColorStructure256.par -pSC newpar/ScalColor.par -pCL newpar/ColLayout.par -l ..\dbtest.lst -o testclustout.txt



///////////////////////////////////////////////////////////////////////////////
// This software module was originally developed by
//
// Stephan Herrmann, Munich University of Technology
// (contributing organizations names)
//
// in the course of development of the MPEG-7 Experimentation Model.
//
// This software module is an implementation of a part of one or more MPEG-7
// Experimentation Model tools as specified by the MPEG-7 Requirements.
//
// ISO/IEC gives users of MPEG-7 free license to this software module or
// modifications thereof for use in hardware or software products claiming
// conformance to MPEG-7.
//
// Those intending to use this software module in hardware or software products
// are advised that its use may infringe existing patents. The original
// developer of this software module and his/her company, the subsequent
// editors and their companies, and ISO/IEC have no liability for use of this
// software module or modifications thereof in an implementation.
//
// Copyright is not released for non MPEG-7 conforming products. The
// organizations named above retain full right to use the code for their own
// purpose, assign or donate the code to a third party and inhibit third parties
// from using the code for non MPEG-7 conforming products.
//
// Copyright (c) 1998-2002.
//
// This notice must be included in all copies or derivative works.