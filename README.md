# DecodeSTORM
DecodeSTORM is an user-friendly ImageJ plug-in for quantitative analysis data in single-molecule localization microscopy. DecodeSTORM classifies and modularizes various methods that are useful in SMLM data analysis, including format conversion, channel registration, artifact correction (containing drift correction and localizations filtering), quantitative analysis (containing segmentation and clustering, spatial distribution statistics and colocalization) and visualization. The modularized methods can be combined freely, and thus allow users to have an optimal combination of methods to improving the accuracy of quantitative data analysis.
# System requirements and installation
1, Windows 7 sp1 or newer, x64.

2, NVidia CUDA enabled GPU with compute capability no less than 3.5.

**Note: please upgrade your GPU driver to the newest (https://www.geforce.com/drivers , Compatible Driver Versions >= 456.71) or the plug-in can't work successfully.**

3, ImageJ/Fiji, Micro-Manager 2.0 (beta or gamma). 
**Note: please set at least 6 GB memory buffer for ImageJ, Micro-Manager and ImageJ of Micro-Manager.**
(The 64 bits ImageJ with Java 1.8 works well and can be downloaded at https://imagej.nih.gov/ij/download.html)

4, Download and install Microsoft Visual C++ 2015-2022 Redistributable (x64) (https://learn.microsoft.com/zh-cn/cpp/windows/latest-supported-vc-redist?view=msvc-160). 

5, Download the newest DecodeSTORM release from https://github.com/SRMLabHainu/DecodeSTORM/releases.

6, Copy the downloaded .dll files into installation directory of ImageJ, and .jar files into "plugins" or "mmplugins" folder for ImageJ.

7, Note: DecodeSTORM only supports English file name / path.
# Dependency
The development of DecodeSTORM depend on Armadillo and OpenCV. Armadillo download address: http://arma.sourceforge.net/download.html OpenCV download address: https://opencv.org/releases/
