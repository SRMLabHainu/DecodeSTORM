#include "stdafx.h"
#include "Voronoi.h"

//static void DrawVoronoiDiagram(cv::Mat& img, Subdiv2D& subdiv);
cv::Mat main_img;
string VoronoiResSave;
int VoronoiClusterNum;
void Voronoi::VoronoiDiagram(const mat& inLoc) {

    DrawVoronoiDiagram(inLoc);

    Rect rect(uni_inLoc.col(0).min(), uni_inLoc.col(1).min(),
        uni_inLoc.col(0).max() - uni_inLoc.col(0).min() + 1,
        uni_inLoc.col(1).max() - uni_inLoc.col(1).min() + 1);
    main_img = img(rect);
    
    if (main_img.rows > 1080 || main_img.cols > 1920) {
        Size dsize = Size(main_img.cols / 5, main_img.rows / 5);
        resize(main_img, main_img, dsize, 0, 0, INTER_AREA);
        if (main_img.rows > 1080 || main_img.cols > 1920) {
            Size dsize = Size(main_img.cols / 2, main_img.rows / 2);
            resize(main_img, main_img, dsize, 0, 0, INTER_AREA);
        }
    }

    int threshold = VoronoiMonteCarlo(inLoc);
 
    VoronoiClusterNum = CaculateClusNum(threshold);

    imwrite(VoronoiResSave, main_img);
}

void Voronoi::DrawVoronoiDiagram(const mat& inLoc) {
    //uniquerows
    mat inLoc_round = round(inLoc);
    mat XY = inLoc_round.cols(1, 2);
    Unique_Res uniqueres = uniquerows(XY);
    uni_inLoc = uniqueres.Unique;
    //VoronoiDiagram
    Rect rect(0, 0, uni_inLoc.col(0).max() + 1, uni_inLoc.col(1).max() + 1);
    Subdiv2D subdiv(rect);
    img = cv::Mat(rect.size(), CV_8UC1);//CV_32FC1
    for (int i = 0; i < uni_inLoc.n_rows; i++)
    {
        Point2f fp((float)(uni_inLoc(i, 0)),
            (float)(uni_inLoc(i, 1)));
        subdiv.insert(fp);
    }

    img = Scalar::all(0);

    vector<vector<Point2f> > pts;//Stores the vertex coordinates of each polygon
    vector<Point2f> cen_Points;//origin
    subdiv.getVoronoiFacetList(vector<int>(), pts, cen_Points);

    vector<Point> ipt;//Stores the fixed point coordinates of the current polygon
    vector<Point2f> ipt2f;
    vector<vector<Point> > ipts(1);
    vec area;
    area.zeros(pts.size());
    uvec n = histc(uniqueres.Sort_indC, unique(uniqueres.Sort_indC));
    colvec ptNum(pts.size());
    //Draw Voronoi Diagram
    for (size_t i = 0; i < pts.size(); i++)
    {
        ptNum(i) = pts[i].size();
        ipt.resize(pts[i].size());
        ipt2f.resize(pts[i].size());
        mat ptsmat(pts[i].size(), 4, fill::zeros);
        for (size_t j = 0; j < pts[i].size(); j++) {
            ipt2f[j] = pts[i][j];
            ipt[j] = pts[i][j];
            ptsmat(j, 0) = ptNum(i);
            ptsmat(j, 2) = ipt2f[j].x;
            ptsmat(j, 3) = ipt2f[j].y;
        }

        Scalar color{ 255,255,255 };
        fillConvexPoly(img, ipt, color, 8, 0);

        area(i) = contourArea(ipt2f);

        for (size_t j = 0; j < pts[i].size(); j++) {
            ptsmat(j, 1) = area(i) / n(i);

        }
        Csmat = join_cols(Csmat, ptsmat);
        ipts[0] = ipt;
        polylines(img, ipts, true, Scalar(), 1, CV_AA, 0);
    }
    area = area / n;

    uvec indices = sort_index(Csmat.col(1));
    mat Csmat_123 = Csmat.cols(1, 3);
    Csmat.cols(1, 3) = Csmat_123.rows(indices);
    ptNum = ptNum.rows(sort_index(area));
    mat CsmatSort;
    for (int i = 0; i < ptNum.n_rows; i++)
    {
        mat ID = i * ones(ptNum(i));
        CsmatSort = join_cols(CsmatSort, ID);
    }
    Csmat.col(0) = CsmatSort;

    //DrawVoronoiDiagram(img, subdiv);
}

int Voronoi::VoronoiMonteCarlo(const mat& inLoc) {
    int iter = 10;
    int signif = 99;
    int fov = Fov(inLoc);
    double sizeA = (LowRightX - UpLeftX) * (LowRightY - UpLeftY);

    vec Area = VorArea_ia(inLoc);
    vector<vec> AreaR(iter);
    for (int i = 0; i < iter; i++)
    {
        mat Ar = zeros(round(fov * fov) * size(inLoc, 0) / sizeA, 12);
        Ar.cols(1, 2) = randu(size(Ar, 0), 2) * fov;
        mat Anewr = filterROI(Ar);
        AreaR[i] = VorArea_ia(Anewr);
    }

    int Nbins = round(2 * pow((size(Area, 0)), 0.3333333333));
    double lim = 3 * median(AreaR[1]);

    vec centers = histcenters(Area(find(Area < lim)), Nbins);
    vec counts = conv_to< vec >::from(hist(Area(find(Area < lim)), Nbins));
    mat counts_r = zeros(iter, Nbins);
    for (int i = 0; i < iter; i++)
    {
        counts_r.row(i) = conv_to< vec >::from(hist(AreaR[i](find(AreaR[i] < lim)), centers)).st();
    }

    mat MeanCounts = mean(counts_r);
    uvec ind = find((counts.st() - MeanCounts) < 0, 1);
    vec inters = intersection(join_cols(centers(ind - 1), centers(ind)), join_cols(counts(ind - 1), counts(ind)),
        join_cols(centers(ind - 1), centers(ind)), join_cols(MeanCounts(ind - 1), MeanCounts(ind)));
    int intersection = round(inters(0));

    return intersection;
}

int Voronoi::CaculateClusNum(int threshold) {
    int maxi = sum(Csmat.col(1) <= threshold);
    vector<mat> cluster(Csmat(maxi, 0));
    mat Csmat1 = Csmat.rows(0, maxi - 1);
    for (int i = 0; i < maxi; i++)
    {
        cluster[Csmat(i, 0)] = join_rows(cluster[Csmat(i, 0)], Csmat1(find(Csmat1.col(2) == Csmat(i, 2) && Csmat1.col(3) == Csmat(i, 3))).st());
    }

    vector<mat> UniqueCluster(cluster.size());
    for (int i = 0; i < cluster.size(); i++)
    {
        UniqueCluster[i] = unique(cluster[i]).st();
    }
    colvec conver = zeros(UniqueCluster.size());
  
    vector<mat> UniqueCluster1 = VorCluster(UniqueCluster, conver);
    
    //remove redundant clusters
    vector<mat> UniqueCluster2(UniqueCluster1.size());
    for (int i = 0; i < UniqueCluster1.size(); i++)
    {
        UniqueCluster2[i] = unique(UniqueCluster1[i]).st();
    
    }
    
    for (int i = 0; i < UniqueCluster2.size(); i++)
    {
        if (~UniqueCluster2[i].is_empty())
        {
            for (int j = i + 1; j < UniqueCluster2.size(); j++)
            {

                if (approx_equal(UniqueCluster2[j], UniqueCluster2[i], "reldiff", 0.1))
                {
                    UniqueCluster2[j].clear();
                }
            }
        }
    }
    vec isCluster(UniqueCluster2.size());
    for (int i = 0; i < UniqueCluster2.size(); i++)
    {
        isCluster(i) = UniqueCluster2[i].is_empty();
    }
    int ClusterNum = isCluster.n_rows - sum(isCluster);
    return ClusterNum;
}

int Voronoi::Fov(const mat& inLoc) {

    double maxXY;
    int fov;
    if (~inLoc.is_empty())
    {
        maxXY = max(max(inLoc.cols(1, 2)));
    }
    else {
        maxXY = 0;
    }
    if (maxXY > 18000) {
        fov = ceil(maxXY / 1000) * 1000;
    }
    else
    {
        fov = 18000;
    }

    return fov;
}

vec Voronoi::VorArea_ia(const mat& inLoc) {
    //uniquerows
    mat inLoc_round = round(inLoc);
    mat XY = inLoc_round.cols(1, 2);
    Unique_Res Uniqueres = uniquerows(XY);
    mat Uni_inLoc = Uniqueres.Unique;
    //VoronoiDiagram
    Rect rect(0, 0, Uni_inLoc.col(0).max() + 1, Uni_inLoc.col(1).max() + 1);
    Subdiv2D subdiv(rect);
    cv::Mat Img(rect.size(), CV_8UC1);//CV_32FC1
    for (int i = 0; i < Uni_inLoc.n_rows; i++)
    {
        Point2f fp((float)(Uni_inLoc(i, 0)),
            (float)(Uni_inLoc(i, 1)));
        subdiv.insert(fp);
    }

    Img = Scalar::all(0);

    vector<vector<Point2f> > pts;//Stores the vertex coordinates of each polygon
    vector<Point2f> cen_Points;//origin
    subdiv.getVoronoiFacetList(vector<int>(), pts, cen_Points);

    vector<Point> ipt;//Stores the fixed point coordinates of the current polygon
    vector<Point2f> ipt2f;
    vector<vector<Point> > ipts(1);
    vec area;
    area.zeros(pts.size());
    uvec n = histc(Uniqueres.Sort_indC, unique(Uniqueres.Sort_indC));
    
    //Draw Voronoi Diagram
    for (size_t i = 0; i < pts.size(); i++)
    {
        ipt.resize(pts[i].size());
        ipt2f.resize(pts[i].size());
        for (size_t j = 0; j < pts[i].size(); j++) {
            ipt2f[j] = pts[i][j];
            ipt[j] = pts[i][j];
        }

        Scalar color{ 255,255,255 };
        fillConvexPoly(Img, ipt, color, 8, 0);

        area(i) = contourArea(ipt2f);
        ipts[0] = ipt;
        polylines(Img, ipts, true, Scalar(), 1, CV_AA, 0);
    }
    area = area / n;

    return area;
}

mat Voronoi::filterROI(const mat& inLoc) {
    uvec ind = inLoc.col(1) > UpLeftX && inLoc.col(1) < LowRightX &&
        inLoc.col(2) > UpLeftY && inLoc.col(2) < LowRightY;
    mat outLoc = inLoc.rows(find(ind == 1));
    return outLoc;
}

vec Voronoi::histcenters(const vec& Vec, int bins) {
    double maxV = max(Vec);
    double minV = min(Vec);
    double intervalHalf = (maxV - minV) / bins / 2;
    vec centers = linspace(minV + intervalHalf, maxV - intervalHalf, bins);
    return centers;
}

vec Voronoi::intersection(const vec& x1, const vec& y1, const vec& x2, const vec& y2) {
    double a1 = (y1(1) - y1(0)) / (x1(1) - x1(0));
    double a2 = (y2(1) - y2(0)) / (x2(1) - x2(0));
    double b1 = y1(0) - a1 * x1(0);
    double b2 = y2(0) - a2 * x2(0);
    double xi = (b2 - b1) / (a1 - a2);
    double yi = a1 * xi + b1;
    vec xyi = { xi ,yi };
    return xyi;
}

vector<mat> Voronoi::VorCluster(const vector<mat>& UniqueCluster, colvec conver) {
    uvec conver_id = find(conver == 0);
    vector<mat> UniqueClusterTemp(UniqueCluster);
    for (int i = 0; i < conver_id.n_rows; i++)
    {
        int m = conver_id(i);
        for (int j = 0; j < UniqueCluster[m].n_cols; j++)
        {
            int k = UniqueCluster[m](j);
            mat ind = BsxFunEq(UniqueClusterTemp[m], UniqueCluster[k]);
            uvec addId = any(ind, 2);
            UniqueClusterTemp[m] = join_rows(UniqueClusterTemp[m], UniqueCluster[k](find(addId == 0)).st());

        }
    }
    for (int i = 0; i < conver.n_rows; i++)
    {
        conver(i) = approx_equal(UniqueClusterTemp[i], UniqueCluster[i], "reldiff", 0.1); ;
    }
    vector<mat> UniqueCluster1;

    if (sum(conver) != conver.n_rows)
    {
        UniqueCluster1 = VorCluster(UniqueClusterTemp, conver);
    }
    else {
        UniqueCluster1 = UniqueClusterTemp;
    }

    return UniqueCluster1;
}

mat Voronoi::BsxFunEq(const mat& Mat1, const mat& Mat2) {
    mat ind = zeros(Mat2.n_cols, Mat1.n_cols);
    for (size_t i = 0; i < Mat2.n_cols; i++)
    {
        uvec SameValueIdUvec = find(Mat1 == Mat2(i));
        if (SameValueIdUvec.n_rows > 0) {
            int SameValueId = SameValueIdUvec(0);
            ind(i, SameValueId) = 1;
        }
    }

    return ind;
}

