ClusterWinExe.exe -pDC newpar/DominantColor.xml.par -a DominantColorServer -l ..\dbtest.lst
ClusterWinExe.exe -pCS newpar/ColorStructure256.xml.par -a ColorStructureServer -l ..\dbtest.lst
ClusterWinExe.exe -pSC newpar/ScalColor.xml.par -a ScalableColorServer -l ..\dbtest.lst
ClusterWinExe.exe -pCL newpar/ColLayout.xml.par -a ColorLayoutServer -l ..\dbtest.lst
ClusterWinExe.exe -pHT newpar/HomogeneousTexture.xml.par -a HomogeneousTextureServer -l ..\dbtest.lst
ClusterWinExe.exe -pEH newpar/EdgeHistogram.xml.par -a EdgeHistogramServer -l ..\dbtest.lst
