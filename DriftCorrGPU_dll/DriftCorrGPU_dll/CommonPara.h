/*
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU LESSER GENERAL PUBLIC LICENSE as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU LESSER GENERAL PUBLIC LICENSE for more details.

You should have received a copy of the GNU LESSER GENERAL PUBLIC LICENSE
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/




// stored parameter number for each types of fitting for each fitted molecule

#define OutParaNumGS2D			12 // gaussian 2d
#define OutParaNumAS3D			12 // astigmatism 3d


// loc results information order for OutParaNumGS2D and OutParaNumAS3D and OutParaNumDH3D

#define Pos_PPho			0 // peak photon
#define Pos_XPos			1 // may have 0.5 or 1 pixel offset compared with other software
#define Pos_YPos			2 // may have 0.5 or 1 pixel offset compared with other software
#define Pos_ZPos			3 // may have 0.5 or 1 pixel offset compared with other software
#define Pos_SigX			4 // sigma x
#define Pos_SigY			5 // sigma y
#define Pos_TPho			6 // total photon
#define Pos_Bakg			7 // background
#define Pos_PSNR			8 // peak photon to background noise snr
#define Pos_CrbX			9 // crlb of x  
#define Pos_CrbY			10 // crlb of Y
#define Pos_Frme			11 // frame




// maximum molecules in allocated memory for each class
#define PointNumTh					20480
#define MaxPointNum					(20480*5)


