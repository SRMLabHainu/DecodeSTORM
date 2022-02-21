#include "stdafx.h"
#include "VoronoiDiagram.h"

static void DrawVoronoiDiagram(cv::Mat& img, Subdiv2D& subdiv);
cv::Mat main_img;
string VoronoiResSave;
void VoronoiDiagram(const mat& inLoc) {


    int pixel = 10;
    mat inLoc_round = round(inLoc);
    mat XY = inLoc_round.cols(1, 2);
    //uniquerows
    Unique_Res res;
    res = uniquerows(XY);
    mat uni_inLoc = res.Unique;
    if (mean(uni_inLoc.col(0)) > 1000 || mean(uni_inLoc.col(1)) > 1000)
    {
        uni_inLoc = uni_inLoc / pixel;
    }
    //VoronoiDiagram
    Rect rect(0, 0, uni_inLoc.col(0).max() + 1, uni_inLoc.col(1).max() + 1);
    Subdiv2D subdiv(rect);
    cv::Mat img(rect.size(), CV_8UC1);//CV_32FC1
    for (int i = 0; i < uni_inLoc.n_rows; i++)
    {
        Point2f fp((float)(uni_inLoc(i, 0)),
            (float)(uni_inLoc(i, 1)));
        subdiv.insert(fp);
    }

    img = Scalar::all(0);
    DrawVoronoiDiagram(img, subdiv);
    Rect area(uni_inLoc.col(0).min(), uni_inLoc.col(1).min(),
        uni_inLoc.col(0).max() - uni_inLoc.col(0).min() + 1,
        uni_inLoc.col(1).max() - uni_inLoc.col(1).min() + 1);
    main_img = img(area);
    imwrite(VoronoiResSave, main_img);
   // imshow(WinName, main_img);
   // waitKey(0);
    //destroyWindow(WinName);

    //for (size_t i = 0; i < main_img.rows; i++)
    //{
    //    for (size_t j = 0; j < main_img.cols; j++)
    //    {
    //        int Pixel = (int)((*(main_img.data + main_img.step[0] * i + main_img.step[1] * j)) * 1);
    //        string PixHex = rgb2hex(Pixel, Pixel, Pixel, true);
    //        stringstream ss2;
    //        int PixDec;
    //        ss2 << hex << PixHex; //
    //        ss2 >> PixDec;
    //        Voronoi_Image(i, j) = PixDec;

    //    }
    //}

    //std::string rgb2hex(int r, int g, int b, bool with_head)
    //{
    //    std::stringstream ss;
    //    if (with_head)

    //        ss << std::hex << (r << 16 | g << 8 | b);
    //    return ss.str();
    //}
}


static void DrawVoronoiDiagram(cv::Mat& img, Subdiv2D& subdiv)
{
    vector<vector<Point2f> > pts;//Stores the vertex coordinates of each polygon
    vector<Point2f> cen_Points;//origin
    subdiv.getVoronoiFacetList(vector<int>(), pts, cen_Points);

    vector<Point> ipt;//Stores the fixed point coordinates of the current polygon
    vector<vector<Point> > ipts(1);
    //Draw Voronoi Diagram
    for (size_t i = 0; i < pts.size(); i++)
    {
        ipt.resize(pts[i].size());
        for (size_t j = 0; j < pts[i].size(); j++)
            ipt[j] = pts[i][j];

        Scalar color{ 255,255,255 };
        fillConvexPoly(img, ipt, color, 8, 0);

        ipts[0] = ipt;
        polylines(img, ipts, true, Scalar(), 1, CV_AA, 0);

    }
}