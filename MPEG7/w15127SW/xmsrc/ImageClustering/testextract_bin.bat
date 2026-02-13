ClusterWinExe.exe -pDC newpar/DominantColor.bin.par -a DominantColorServer -l ..\dbtest.lst
ClusterWinExe.exe -pCS newpar/ColorStructure256.bin.par -a ColorStructureServer -l ..\dbtest.lst
ClusterWinExe.exe -pSC newpar/ScalColor.bin.par -a ScalableColorServer -l ..\dbtest.lst
ClusterWinExe.exe -pCL newpar/ColLayout.bin.par -a ColorLayoutServer -l ..\dbtest.lst
ClusterWinExe.exe -pHT newpar/HomogeneousTexture.bin.par -a HomogeneousTextureServer -l ..\dbtest.lst
ClusterWinExe.exe -pEH newpar/EdgeHistogram.bin.par -a EdgeHistogramServer -l ..\dbtest.lst
