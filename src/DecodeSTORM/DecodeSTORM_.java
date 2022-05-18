/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
//package DecodeSTORM;
import ij.ImagePlus;
import ij.gui.ImageWindow;
import ij.gui.Plot;
import ij.gui.Roi;
import ij.io.FileSaver;
import ij.plugin.filter.PlugInFilter;
import ij.plugin.frame.RoiManager;
import static ij.plugin.filter.PlugInFilter.DOES_16;
import static ij.plugin.filter.PlugInFilter.NO_CHANGES;
import static ij.plugin.filter.PlugInFilter.NO_IMAGE_REQUIRED;
import static ij.plugin.filter.PlugInFilter.SUPPORTS_MASKING;
import ij.process.*;
import java.awt.Color;
import java.awt.image.ColorModel;
import java.awt.image.IndexColorModel;
import java.io.*;
import javax.swing.*;

public class DecodeSTORM_ implements PlugInFilter {

    // public ImagePlus CurImagePlus;
    // public ImageProcessor CurImageProcessor;
    // display of super resolution image
    // for 2d image, it's a float image, 3d is a color(RGB) image
    public volatile ImageWindow CurSRImageWindow;
    public ColorModel hotCM;
    public Roi roi;
    public Plot ScatterPlot;
    public double WindowW;
    public double WindowH;
    public DecodeSTORM_Configurator MyConfigurator;
    public RoiInfo roiInfo;
    public RoiManager roiManager;

    public native static void lm_Convert(String ConvertFileName, int FileFormatSelect);
    //get localizaiton table of Armadillo format

    public native static void lm_GetInLoc1(String FilenameChannel);

    public native static void lm_GetInLoc2(String FilenameChanne2);

    public native static void lm_SetRoiLocPara(double UpLeftX, double UpLeftY, double LowRightX, double LowRightY);

    public native static void lm_GetRoiLoc1();

    public native static void lm_GetRoiLoc2();

    //set parameters
    public native static void lm_SetSaveDir(String ResultSaveDir);

    public native static void lm_SetFilteringPara(float MinPts, float distThre, float RadThre, float UnprecThres, float MaxOffFrame, float MaxDis);

    public native static void lm_SetDriftCorrPara(int IsDriftCorrectionI, int DriftCorrGroupFrameNum, float RawImPixSize);

    public native static void lm_SetDBSCANPara(int MinPts, float Eps);

    public native static void lm_SetSpatialStatisticsPara(float MaxAnaDis, float RingWidth);

    public native static void lm_SetColocalizationPara( float Rmax, float Step, float ColoThres, double width, double height);

    //Scatter
    public native static double[][] lm_GetChan1Loc();

    public native static double[][] lm_GetChan2Loc();

    public native static double[][] lm_GetChan1RoiLoc();

    public native static double[][] lm_GetChan2RoiLoc();

    public native static float[] lm_GetScatImInfo(boolean Ch1Exist, boolean Ch2Exist);

    public native static float[] lm_GetScatRoiImInfo(boolean Ch1Exist, boolean Ch2Exist);
    //Gasussian Render
    public native static float[] lm_GetCh1RoiSMLMImage(float RawImgPixelSize, int PixelZoom);
    
    public native static float[] lm_GetCh2RoiSMLMImage(float RawImgPixelSize, int PixelZoom);
    
    public native static float[] lm_GetCh1ArtifactCorrRoiSMLMImage(float RawImgPixelSize, int PixelZoom);
    
    public native static float[] lm_GetCh2ArtifactCorrRoiSMLMImage(float RawImgPixelSize, int PixelZoom);

    public native static float[] lm_GetCh1SMLMImage(float RawImgPixelSize, int PixelZoom);

    public native static float[] lm_GetCh2SMLMImage(float RawImgPixelSize, int PixelZoom);

    public native static double lm_GetMaxDispVal();

    public native static int[] lm_GetRenderImageInf();

    //Drift Correction 
    public native static void lm_StartDriftCorr(int isDriftCorrCh1, int isDriftCorrCh2, int isGPU, int isSave);

    //Filtering
    public native static void lm_Filtering(int isUnpFilter, int isDenFilt, int isDupFilt, int isGrouping, int isFilterChan1, int isFilterChan2, int isSave);

    //Spatial Statistics
    public native static double[][] lm_GetRdfIm(int ChanSele_1, int ChanSele_2, int isLinkPostStatistic);

    public native static float[] lm_GetRdfImInfo();
    
    public native static void lm_SaveRdfResult(int ChanSele_1, int ChanSele_2);

    public native static double[][] lm_GetRipleyHIm(int ChanSele_1, int ChanSele_2, int isLinkPostStatistic);

    public native static float[] lm_GetRipleyImInfo();

    public native static void lm_SaveRipleyResult(int ChanSele_1, int ChanSele_2);

    //Segmentation and Cluster
    public native static int[] lm_GetDbscanImCh1(int isEps, int isLinkPostCluster);

    public native static int[] lm_GetDbscanImCh2(int isEps, int isLinkPostCluster);

    public native static int[] lm_GetDbscanImInfo();

    public native static void lm_SaveDbscanResult(String DbscanSavePath);

    public native static void lm_VoronoiDiagramCh1(String VoronoiSavePath, int isLinkPostCluster);

    public native static void lm_VoronoiDiagramCh2(String VoronoiSavePath, int isLinkPostCluster);

    public native static void lm_IsSaveVoronoiResult(int isSave);

    //Colocalization
    public native static void lm_Colocalization(int isLinkPostColoc);

    public native static double[] lm_GetCBC1Hist();

    public native static double[] lm_GetCBC2Hist();

    public native static float[] lm_GetCBCHistInfo();


    public native static void lm_SaveColoclizationResult(String CBCScoreResSave);

    @Override
    public int setup(String string, ImagePlus ip) {
        System.loadLibrary("DecodeSTORM_CPPDLL");

        ScatterPlot = new Plot("Scatter Image", "", "", 12);
        //ScatterRoiPlot = new Plot("Scatter ROI Image", "", "", 12);
        //ScatterFilteringRoiPlot = new Plot("Scatter ROI Image", "", "", 12);
        roiInfo = new RoiInfo();
        MyConfigurator = new DecodeSTORM_Configurator(this);
        MyConfigurator.setVisible(true);
        roiManager = new RoiManager(false);
        return NO_IMAGE_REQUIRED + DOES_16 + NO_CHANGES + SUPPORTS_MASKING;
    }

    @Override
    public void run(ImageProcessor ip) {
        //CurImageProcessor = ip;
    }

    public void StartConvert() throws InterruptedException {
        Convert thread_Convert = new Convert();
        thread_Convert.start();
    }

    public class Convert extends Thread {

        @Override
        public void run() {
            lm_SetSaveDir(MyConfigurator.ResultsFilePath);
            lm_Convert(MyConfigurator.ConvertFilePath, MyConfigurator.FileFormatSelect);
            MyConfigurator.EnableConvert();
        }
    }

    public void StartScatter() throws InterruptedException {
        Scatter thread_Scatter = new Scatter();
        thread_Scatter.start();
    }

    public class Scatter extends Thread {

        @Override
        public void run() {

            ScatterPlot.updateImage();
            ScatterPlot.setLineWidth(2);
            ScatterPlot.setWindowSize(600, 600);
            if (MyConfigurator.DataPathChan1 != null && MyConfigurator.DataPathChan2 == null) {
                if (MyConfigurator.DataPathChan1.length() < 2) {
                    MyConfigurator.EnableScatter();
                    return;
                }
                if (!MyConfigurator.DataPathChan1.endsWith(".csv")) {
                    MyConfigurator.EnableScatter();
                    return;
                }
                double Ch1XY[][] = lm_GetChan1Loc();
                float ScatImInfo[] = lm_GetScatImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                WindowW = ScatImInfo[0] - ScatImInfo[1];
                WindowH = ScatImInfo[2] - ScatImInfo[3];
                ScatterPlot.setColor(Color.green);
                ScatterPlot.add("dot", Ch1XY[0], Ch1XY[1]);
                ScatterPlot.setLimits(ScatImInfo[1], ScatImInfo[0], ScatImInfo[2], ScatImInfo[3]);
                ScatterPlot.show();
            } else if (MyConfigurator.DataPathChan2 != null && MyConfigurator.DataPathChan1 == null) {
                if (MyConfigurator.DataPathChan2.length() < 2) {
                    MyConfigurator.EnableScatter();
                    return;
                }
                if (!MyConfigurator.DataPathChan2.endsWith(".csv")) {
                    MyConfigurator.EnableScatter();
                    return;
                }
                double Ch2XY[][] = lm_GetChan2Loc();
                float ScatImInfo[] = lm_GetScatImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterPlot.setColor(Color.red);
                ScatterPlot.add("dot", Ch2XY[0], Ch2XY[1]);
                ScatterPlot.setLimits(ScatImInfo[1], ScatImInfo[0], ScatImInfo[2], ScatImInfo[3]);
                ScatterPlot.show();
            } else if (MyConfigurator.DataPathChan1 != null && MyConfigurator.DataPathChan2 != null) {
                if (MyConfigurator.DataPathChan1.length() < 2 || MyConfigurator.DataPathChan2.length() < 2) {
                    MyConfigurator.EnableScatter();
                    return;
                }
                if (!MyConfigurator.DataPathChan1.endsWith(".csv") || !MyConfigurator.DataPathChan2.endsWith(".csv")) {
                    MyConfigurator.EnableScatter();
                    return;
                }
                double Ch1XY[][] = lm_GetChan1Loc();
                double Ch2XY[][] = lm_GetChan2Loc();
                float ScatImInfo[] = lm_GetScatImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterPlot.setColor(Color.green);
                ScatterPlot.add("dot", Ch1XY[0], Ch1XY[1]);
                ScatterPlot.setColor(Color.red);
                ScatterPlot.add("dot", Ch2XY[0], Ch2XY[1]);
                ScatterPlot.setLimits(ScatImInfo[1], ScatImInfo[0], ScatImInfo[2], ScatImInfo[3]);
                ScatterPlot.show();
            } else {
                MyConfigurator.EnableScatter();
                return;
            }
            MyConfigurator.EnableScatter();
        }
    }

    public void StartSelectROI() throws InterruptedException {
        SelectROI thread_SelectROI = new SelectROI();
        thread_SelectROI.start();
    }

    public class SelectROI extends Thread {

        @Override
        public void run() {

            roiManager = RoiManager.getRoiManager();

            if (roiManager.getSelectedIndex() == -1) {
                MyConfigurator.EnableSelectROI();
                return;
            }
            roi = roiManager.getRoi(roiManager.getSelectedIndex());

            GetRoiInfo();
            Plot ScatterRoiPlot = new Plot("Scatter ROI Image", "", "", 12);
            ScatterRoiPlot.updateImage();
            ScatterRoiPlot.setLineWidth(2);
            ScatterRoiPlot.setWindowSize((int) roiInfo.Width + 100, (int) roiInfo.Height + 100);
            lm_SetRoiLocPara(roiInfo.LeftX_nm, roiInfo.LeftY_nm, roiInfo.RightX_nm, roiInfo.RightY_nm);
            if (MyConfigurator.DataPathChan1 != null && MyConfigurator.DataPathChan2 == null) {
                if (MyConfigurator.DataPathChan1.length() < 2) {
                    MyConfigurator.EnableSelectROI();
                    return;
                }
                if (!MyConfigurator.DataPathChan1.endsWith(".csv")) {
                    MyConfigurator.EnableSelectROI();
                    return;
                }
                lm_GetRoiLoc1();
                double[][] RoiLoc1XY = lm_GetChan1RoiLoc();
                float ScatRoiImInfo[] = lm_GetScatRoiImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterRoiPlot.setColor(Color.green);
                ScatterRoiPlot.add("dot", RoiLoc1XY[0], RoiLoc1XY[1]);
                ScatterRoiPlot.setLimits(ScatRoiImInfo[1], ScatRoiImInfo[0], ScatRoiImInfo[2], ScatRoiImInfo[3]);
                ScatterRoiPlot.show();

            } else if (MyConfigurator.DataPathChan2 != null && MyConfigurator.DataPathChan1 == null) {
                if (MyConfigurator.DataPathChan2.length() < 2) {
                    MyConfigurator.EnableSelectROI();
                    return;
                }
                if (!MyConfigurator.DataPathChan2.endsWith(".csv")) {
                    MyConfigurator.EnableSelectROI();
                    return;
                }
                lm_GetRoiLoc2();
                double[][] RoiLoc2XY = lm_GetChan2RoiLoc();
                float ScatRoiImInfo[] = lm_GetScatRoiImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterRoiPlot.setColor(Color.green);
                ScatterRoiPlot.add("dot", RoiLoc2XY[0], RoiLoc2XY[1]);
                ScatterRoiPlot.setLimits(ScatRoiImInfo[1], ScatRoiImInfo[0], ScatRoiImInfo[2], ScatRoiImInfo[3]);
                ScatterRoiPlot.show();
            } else if (MyConfigurator.DataPathChan1 != null && MyConfigurator.DataPathChan2 != null) {
                if (MyConfigurator.DataPathChan1.length() < 2 || MyConfigurator.DataPathChan2.length() < 2) {
                    MyConfigurator.EnableSelectROI();
                    return;
                }
                if (!MyConfigurator.DataPathChan1.endsWith(".csv") || !MyConfigurator.DataPathChan2.endsWith(".csv")) {
                    MyConfigurator.EnableSelectROI();
                    return;
                }
                lm_GetRoiLoc1();
                lm_GetRoiLoc2();
                double[][] RoiLoc1XY = lm_GetChan1RoiLoc();
                double[][] RoiLoc2XY = lm_GetChan2RoiLoc();
                float ScatRoiImInfo[] = lm_GetScatRoiImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterRoiPlot.setColor(Color.green);
                ScatterRoiPlot.add("dot", RoiLoc1XY[0], RoiLoc1XY[1]);
                ScatterRoiPlot.setColor(Color.red);
                ScatterRoiPlot.add("dot", RoiLoc2XY[0], RoiLoc2XY[1]);
                ScatterRoiPlot.setLimits(ScatRoiImInfo[1], ScatRoiImInfo[0], ScatRoiImInfo[2], ScatRoiImInfo[3]);
                ScatterRoiPlot.show();
            }
            MyConfigurator.EnableSelectROI();
        }
    }

    public void StartRendering() throws InterruptedException {
        Rendering thread_Rendering = new Rendering();
        thread_Rendering.start();

    }

    public class Rendering extends Thread {

        @Override
        public void run() {

            int PixelZoom = (int) (MyConfigurator.RawImPixSizeForRender / MyConfigurator.RenderPixSize);
            ColorModel hotCM = GetHotColorModel();
            ImagePlus Ch1ImagePlus = new ImagePlus();
            ImagePlus Ch2ImagePlus = new ImagePlus();
            FileSaver RenderingTifSaver;
            if (MyConfigurator.RenderCh1Enable == 1) {
                float RecImgF[];
                String RenderTitle;
                if(MyConfigurator.RenderRoiEnable == 1){
                 RecImgF = lm_GetCh1RoiSMLMImage(MyConfigurator.RawImPixSizeForRender, PixelZoom);
                 RenderTitle = "Channel_1 Roi Render Image";
                }else if(MyConfigurator.RenderArtifactCorrRoiEnable == 1){
                 RecImgF = lm_GetCh1ArtifactCorrRoiSMLMImage(MyConfigurator.RawImPixSizeForRender, PixelZoom);
                 RenderTitle = "Channel_1 ArtifactCorrRoi Render Image";
                }else{
                 RecImgF = lm_GetCh1SMLMImage(MyConfigurator.RawImPixSizeForRender, PixelZoom);
                 RenderTitle = "Channel_1 Render Image";
                }
                if (RecImgF.length > 10) {
                    int[] RenderImInfo = lm_GetRenderImageInf();
                    FloatProcessor Ch1ImageProcessor2D = new FloatProcessor(RenderImInfo[0], RenderImInfo[1]);
                    Ch1ImageProcessor2D.setPixels(RecImgF);
                    Ch1ImageProcessor2D.setColorModel(hotCM);
                    Ch1ImagePlus.setTitle(RenderTitle);
                    Ch1ImagePlus.setProcessor(Ch1ImageProcessor2D);
                    Ch1ImagePlus.setDisplayRange(0, lm_GetMaxDispVal());
                    Ch1ImagePlus.show();

                    if (MyConfigurator.RenderSave == 1) {
                        String RenderingSaveDir = MyConfigurator.ResultsFilePath + "RenderingResults";
                        File file = new File(RenderingSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        RenderingTifSaver = new FileSaver(Ch1ImagePlus);
                        RenderingTifSaver.saveAsTiff(RenderingSaveDir + "\\Channel_1_RenderIm.tif");
                    }
                }
            }

            if (MyConfigurator.RenderCh2Enable == 1) {
                float RecImgF[];
                String RenderTitle;
                if(MyConfigurator.RenderRoiEnable == 1){
                 RecImgF = lm_GetCh2RoiSMLMImage(MyConfigurator.RawImPixSizeForRender, PixelZoom);
                 RenderTitle = "Channel_2 Roi Render Image";
                }else if(MyConfigurator.RenderArtifactCorrRoiEnable == 1){
                 RecImgF = lm_GetCh2ArtifactCorrRoiSMLMImage(MyConfigurator.RawImPixSizeForRender, PixelZoom);
                 RenderTitle = "Channel_2 ArtifactCorrRoi Render Image";
                }else{
                 RecImgF = lm_GetCh2SMLMImage(MyConfigurator.RawImPixSizeForRender, PixelZoom);
                 RenderTitle = "Channel_2 Render Image";
                }
                if (RecImgF.length > 10) {
                    int[] RenderImInfo = lm_GetRenderImageInf();
                    FloatProcessor Ch2ImageProcessor2D = new FloatProcessor(RenderImInfo[0], RenderImInfo[1]);
                    Ch2ImageProcessor2D.setPixels(RecImgF);
                    Ch2ImageProcessor2D.setColorModel(hotCM);
                    Ch2ImagePlus.setTitle(RenderTitle);
                    Ch2ImagePlus.setProcessor(Ch2ImageProcessor2D);
                    Ch2ImagePlus.setDisplayRange(0, lm_GetMaxDispVal());
                    Ch2ImagePlus.show();

                    if (MyConfigurator.RenderSave == 1) {
                        String RenderingSaveDir = MyConfigurator.ResultsFilePath + "RenderingResults";
                        File file = new File(RenderingSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        RenderingTifSaver = new FileSaver(Ch2ImagePlus);
                        RenderingTifSaver.saveAsTiff(RenderingSaveDir + "\\Channel_2_RenderIm.tif");
                    }
                }
            }
            MyConfigurator.EnableRendering();
        }
    }

    public void StartDriftCorr() throws InterruptedException {
        DriftCorr thread_DriftCorr = new DriftCorr();
        thread_DriftCorr.start();
    }

    public class DriftCorr extends Thread {

        @Override
        public void run() {
            if (roiManager.getSelectedIndex() == -1) {
                MyConfigurator.EnableDriftCorr();
                return;
            }
            lm_SetSaveDir(MyConfigurator.ResultsFilePath);

            lm_SetDriftCorrPara(MyConfigurator.DriftCorrEnable, MyConfigurator.DriftCorrGroupFrameNum, MyConfigurator.RawImPixSize);

            lm_StartDriftCorr(MyConfigurator.DriftCorrCh1Enable, MyConfigurator.DriftCorrCh2Enable, MyConfigurator.DriftCorrGPU, MyConfigurator.DriftCorrSave);

            MyConfigurator.EnableDriftCorr();
        }
    }

    public void StartFiltering() throws InterruptedException {
        Filtering thread_Filtering = new Filtering();
        thread_Filtering.start();
    }

    public class Filtering extends Thread {

        @Override
        public void run() {
            if (roiManager.getSelectedIndex() == -1) {
                MyConfigurator.EnableFiltering();
                return;
            }
            lm_SetSaveDir(MyConfigurator.ResultsFilePath);

            lm_SetFilteringPara(MyConfigurator.FiltPara.StDev, MyConfigurator.FiltPara.distThre, MyConfigurator.FiltPara.RadThre,
                    MyConfigurator.FiltPara.UnprecThres, MyConfigurator.FiltPara.MaxOffFrame, MyConfigurator.FiltPara.MaxDis);

            if (MyConfigurator.FilteringSave == 1) {
                String FilteringSaveDir = MyConfigurator.ResultsFilePath + "FilteringResults";
                File file = new File(FilteringSaveDir);
                if (!file.exists()) {
                    file.mkdir();
                }
            }
            lm_Filtering(MyConfigurator.UnpFilterEnable, MyConfigurator.DenFiltEnable, MyConfigurator.DupFiltEnable,
                    MyConfigurator.MergeEnable, MyConfigurator.FilterChan1Enable, MyConfigurator.FilterChan2Enable, MyConfigurator.FilteringSave);
            Plot ScatterFilteringRoiPlot = new Plot("Scatter Filtering ROI Image", "", "", 12);
            ScatterFilteringRoiPlot.updateImage();
            ScatterFilteringRoiPlot.setLineWidth(2);
            ScatterFilteringRoiPlot.setWindowSize((int) roiInfo.Width + 100, (int) roiInfo.Height + 100);
            if (MyConfigurator.FilterChan1Enable == 1 && MyConfigurator.FilterChan2Enable == 0) {
                double[][] RoiLoc1XY = lm_GetChan1RoiLoc();
                float ScatFilterRoiImInfo[] = lm_GetScatRoiImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterFilteringRoiPlot.setColor(Color.green);
                ScatterFilteringRoiPlot.add("dot", RoiLoc1XY[0], RoiLoc1XY[1]);
                ScatterFilteringRoiPlot.setLimits(ScatFilterRoiImInfo[1], ScatFilterRoiImInfo[0], ScatFilterRoiImInfo[2], ScatFilterRoiImInfo[3]);
                ScatterFilteringRoiPlot.show();
            } else if (MyConfigurator.FilterChan2Enable == 1 && MyConfigurator.FilterChan1Enable == 0) {
                double[][] RoiLoc2XY = lm_GetChan2RoiLoc();
                float ScatFilterRoiImInfo[] = lm_GetScatRoiImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterFilteringRoiPlot.setColor(Color.red);
                ScatterFilteringRoiPlot.add("dot", RoiLoc2XY[0], RoiLoc2XY[1]);
                ScatterFilteringRoiPlot.setLimits(ScatFilterRoiImInfo[1], ScatFilterRoiImInfo[0], ScatFilterRoiImInfo[2], ScatFilterRoiImInfo[3]);
                ScatterFilteringRoiPlot.show();
            } else if (MyConfigurator.FilterChan1Enable == 1 && MyConfigurator.FilterChan2Enable == 1) {
                double[][] RoiLoc1XY = lm_GetChan1RoiLoc();
                double[][] RoiLoc2XY = lm_GetChan2RoiLoc();
                float ScatFilterRoiImInfo[] = lm_GetScatRoiImInfo(MyConfigurator.DataPathChan1 != null, MyConfigurator.DataPathChan2 != null);
                ScatterFilteringRoiPlot.setColor(Color.green);
                ScatterFilteringRoiPlot.add("dot", RoiLoc1XY[0], RoiLoc1XY[1]);
                ScatterFilteringRoiPlot.setColor(Color.red);
                ScatterFilteringRoiPlot.add("dot", RoiLoc2XY[0], RoiLoc2XY[1]);
                ScatterFilteringRoiPlot.setLimits(ScatFilterRoiImInfo[1], ScatFilterRoiImInfo[0], ScatFilterRoiImInfo[2], ScatFilterRoiImInfo[3]);
                ScatterFilteringRoiPlot.show();
            }
            MyConfigurator.EnableFiltering();
        }
    }

    public void StartSpatialStatistics() throws InterruptedException {
        SpatialStatistics thread_SpatialStatistics = new SpatialStatistics();
        thread_SpatialStatistics.start();
    }

    public class SpatialStatistics extends Thread {

        @Override
        public void run() {
            if (roiManager.getSelectedIndex() == -1) {
                MyConfigurator.EnableSpatialStatistics();
                return;
            }

            Plot SpatialStatisticsPlot;
            ImagePlus SpatialStatisticsImPlus;
            FileSaver SpatialStatisticsTifSaver;

            lm_SetSaveDir(MyConfigurator.ResultsFilePath);
            lm_SetSpatialStatisticsPara(MyConfigurator.MaxAnaDis, MyConfigurator.RingWidth);
            if (MyConfigurator.RdfEnable == 1) {

                SpatialStatisticsPlot = new Plot("Radial Distribution Fuction", "Radius (nm)", "RDF/G(r)");
                SpatialStatisticsPlot.setWindowSize(500, 500);
                double[][] XG = lm_GetRdfIm(MyConfigurator.ChanSele_1, MyConfigurator.ChanSele_2, MyConfigurator.LinkPostStatisticEnable);
                float[] RdfImageInfo = lm_GetRdfImInfo();
                SpatialStatisticsPlot.add("lines", XG[0], XG[1]);
                String MaxGrID = Float.toString(RdfImageInfo[2]);
                String GrBorderMax = Float.toString(RdfImageInfo[0]);
                String Lable = "Max G(r): " + GrBorderMax + " at Max r: " + MaxGrID;
                SpatialStatisticsPlot.addLabel(0.5, 0, Lable);
                SpatialStatisticsPlot.setLimits(0, MyConfigurator.MaxAnaDis, RdfImageInfo[1], RdfImageInfo[0]);
                SpatialStatisticsPlot.show();

                if (MyConfigurator.SpatialStatiticSave == 1) {
                    String SaveImName;
                    if (MyConfigurator.ChanSele_1 == 0 & MyConfigurator.ChanSele_2 == 0) {
                        SaveImName = "Channel_1_RDF.tif";
                    } else if (MyConfigurator.ChanSele_1 == 0 & MyConfigurator.ChanSele_2 == 1) {
                        SaveImName = "Channel_1-Channel_2_RDF.tif";
                    } else if (MyConfigurator.ChanSele_1 == 1 & MyConfigurator.ChanSele_2 == 0) {
                        SaveImName = "Channel_2-Channel_1_RDF.tif";
                    } else {
                        SaveImName = "Channel_2_RDF.tif";
                    }
                    String SpatialStatisticsSaveDir = MyConfigurator.ResultsFilePath + "SpatialStatisticsResults";
                    File file = new File(SpatialStatisticsSaveDir);
                    if (!file.exists()) {
                        file.mkdir();
                    }
                    SpatialStatisticsImPlus = SpatialStatisticsPlot.getImagePlus();
                    SpatialStatisticsTifSaver = new FileSaver(SpatialStatisticsImPlus);
                    SpatialStatisticsTifSaver.saveAsTiff(SpatialStatisticsSaveDir + "\\" + SaveImName);
                    lm_SaveRdfResult(MyConfigurator.ChanSele_1, MyConfigurator.ChanSele_2);
                }
            }

            if (MyConfigurator.RipleyHEnable == 1) {
                SpatialStatisticsPlot = new Plot("Ripley's H-function", "Radius (nm)", "L(r)-r");
                SpatialStatisticsPlot.setWindowSize(500, 500);
                double[][] XH = lm_GetRipleyHIm(MyConfigurator.ChanSele_1, MyConfigurator.ChanSele_2, MyConfigurator.LinkPostStatisticEnable);
                float[] RipleyImageInfo = lm_GetRipleyImInfo();
                SpatialStatisticsPlot.add("lines", XH[0], XH[1]);
                String MaxHrID = Float.toString(RipleyImageInfo[2]);
                String HrBorderMax = Float.toString(RipleyImageInfo[0]);
                String Lable = "Max L(r)-r: " + HrBorderMax + " at Max r: " + MaxHrID;
                SpatialStatisticsPlot.addLabel(0.5, 0, Lable);
                SpatialStatisticsPlot.setLimits(0, MyConfigurator.MaxAnaDis, RipleyImageInfo[1], RipleyImageInfo[0]);
                SpatialStatisticsPlot.show();

                if (MyConfigurator.SpatialStatiticSave == 1) {
                    String SaveImName;
                    if (MyConfigurator.ChanSele_1 == 0 & MyConfigurator.ChanSele_2 == 0) {
                        SaveImName = "Channel_1-Ripley's H.tif";
                    } else if (MyConfigurator.ChanSele_1 == 0 & MyConfigurator.ChanSele_2 == 1) {
                        SaveImName = "Channel_1-Channel_2_Ripley's H.tif";
                    } else if (MyConfigurator.ChanSele_1 == 1 & MyConfigurator.ChanSele_2 == 0) {
                        SaveImName = "Channel_2-Channel_1_Ripley's H.tif";
                    } else {
                        SaveImName = "Channel_2_Ripley's H.tif";
                    }
                    String SpatialStatisticsSaveDir = MyConfigurator.ResultsFilePath + "SpatialStatisticsResults";
                    File file = new File(SpatialStatisticsSaveDir);
                    if (!file.exists()) {
                        file.mkdir();
                    }
                    SpatialStatisticsImPlus = SpatialStatisticsPlot.getImagePlus();
                    SpatialStatisticsTifSaver = new FileSaver(SpatialStatisticsImPlus);
                    SpatialStatisticsTifSaver.saveAsTiff(SpatialStatisticsSaveDir + "\\" + SaveImName);
                    lm_SaveRipleyResult(MyConfigurator.ChanSele_1, MyConfigurator.ChanSele_2);
                  
                }
            }

            MyConfigurator.EnableSpatialStatistics();
        }
    }

    public void StartCluster() throws InterruptedException {
        Cluster thread_Cluster = new Cluster();
        thread_Cluster.start();
    }

    public class Cluster extends Thread {

        @Override
        public void run() {
            if (roiManager.getSelectedIndex() == -1) {
                MyConfigurator.EnableCluster();
                return;
            }

            lm_SetSaveDir(MyConfigurator.ResultsFilePath);

            lm_SetDBSCANPara(MyConfigurator.MinPts, MyConfigurator.Eps);

            ImagePlus DbscanCh1ImPlus = new ImagePlus();
            ImagePlus DbscanCh2ImPlus = new ImagePlus();
            ColorProcessor DbscanImProcessor;
            FileSaver DbscanTifSaver;

            if (MyConfigurator.DBSCANEnable == 1) {
                if (MyConfigurator.ClusterChan1Enable == 1) {
                    int[] DbscanImCh1 = lm_GetDbscanImCh1(MyConfigurator.EpsEnable, MyConfigurator.LinkPostClusterEnable);
                    int[] DbscanImCh1Info = lm_GetDbscanImInfo();
                    DbscanImProcessor = new ColorProcessor(DbscanImCh1Info[0], DbscanImCh1Info[1]);
                    DbscanImProcessor.setPixels(DbscanImCh1);
                    DbscanCh1ImPlus.setProcessor(DbscanImProcessor);
                    DbscanCh1ImPlus.setTitle("Channel_1 DBSCAN");
                    DbscanCh1ImPlus.show();
                    String MinPtsStr = Integer.toString(MyConfigurator.MinPts);
                    String EpsStr;
                    if (MyConfigurator.EpsEnable == 1) {
                        EpsStr = Float.toString(MyConfigurator.Eps);
                    } else {
                        EpsStr = Integer.toString(DbscanImCh1Info[2]);
                    }
                    String ClusterNum = Integer.toString(DbscanImCh1Info[3]);
                    String DbscanCh1Info = "Channel_1 DBSCAN: " + ClusterNum + " clusters were identified. MinPts:" + MinPtsStr + " Eps: " + EpsStr;
                    UIManager.put("OptionPane.okButtonText", "Ok");
                    JOptionPane.showMessageDialog(null, DbscanCh1Info, "Channel_1 DbscanInfo", JOptionPane.PLAIN_MESSAGE);

                    if (MyConfigurator.SegmentationClusterSave == 1) {
                        String SegmentationAndClusterSaveDir = MyConfigurator.ResultsFilePath + "SegmentationAndClusterResults";
                        File file = new File(SegmentationAndClusterSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        String DbscanSaveDir = SegmentationAndClusterSaveDir + "\\DBSCAN_Results";
                        file = new File(DbscanSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        DbscanTifSaver = new FileSaver(DbscanCh1ImPlus);
                        DbscanTifSaver.saveAsTiff(DbscanSaveDir + "\\Channel_1_DBSCAN.tif");
                        lm_SaveDbscanResult(DbscanSaveDir + "\\Channel_1_DBSCAN.csv");
                    }
                }


                if (MyConfigurator.ClusterChan2Enable == 1) {
                    int[] DbscanImCh2 = lm_GetDbscanImCh2(MyConfigurator.EpsEnable, MyConfigurator.LinkPostClusterEnable);
                    int[] DbscanImCh2Info = lm_GetDbscanImInfo();
                    DbscanImProcessor = new ColorProcessor(DbscanImCh2Info[0], DbscanImCh2Info[1]);
                    DbscanImProcessor.setPixels(DbscanImCh2);
                    DbscanCh2ImPlus.setProcessor(DbscanImProcessor);
                    DbscanCh2ImPlus.setTitle("Channel_2 DBSCAN");
                    DbscanCh2ImPlus.show();
                    String MinPtsStr = Integer.toString(MyConfigurator.MinPts);
                    String EpsStr;
                    if (MyConfigurator.EpsEnable == 1) {
                        EpsStr = Float.toString(MyConfigurator.Eps);
                    } else {
                        EpsStr = Integer.toString(DbscanImCh2Info[2]);
                    }
                    String ClusterNum = Integer.toString(DbscanImCh2Info[3]);
                    String DbscanCh2Info = "Channel_2 DBSCAN: " + ClusterNum + " clusters were identified. MinPts:" + MinPtsStr + " Eps: " + EpsStr;
                    UIManager.put("OptionPane.okButtonText", "Ok");
                    JOptionPane.showMessageDialog(null, DbscanCh2Info, "Channel_2 DbscanInfo", JOptionPane.PLAIN_MESSAGE);

                    if (MyConfigurator.SegmentationClusterSave == 1) {
                        String SegmentationAndClusterSaveDir = MyConfigurator.ResultsFilePath + "SegmentationAndClusterResults";
                        File file = new File(SegmentationAndClusterSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        String DbscanSaveDir = SegmentationAndClusterSaveDir + "\\DBSCAN_Results";
                        file = new File(DbscanSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        DbscanTifSaver = new FileSaver(DbscanCh2ImPlus);
                        DbscanTifSaver.saveAsTiff(DbscanSaveDir + "\\Channel_2_DBSCAN.tif");
                        lm_SaveDbscanResult(DbscanSaveDir + "\\Channel_2_DBSCAN.csv");
                    }
                }
            }
            String VoronoiSaveDir;
            if (MyConfigurator.VoronoiEnable == 1) {
                if (MyConfigurator.ClusterChan1Enable == 1) {
                    if (MyConfigurator.SegmentationClusterSave == 1) {
                        String SegmentationAndClusterSaveDir = MyConfigurator.ResultsFilePath + "SegmentationAndClusterResults";
                        File file = new File(SegmentationAndClusterSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        VoronoiSaveDir = SegmentationAndClusterSaveDir + "\\VoronoiResults";
                        file = new File(VoronoiSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                    } else {
                        VoronoiSaveDir = MyConfigurator.ResultsFilePath;
                    }
                    lm_VoronoiDiagramCh1(VoronoiSaveDir + "\\Channel_1_Voronoi.tif", MyConfigurator.LinkPostClusterEnable);
                    ImagePlus VorDiaCh1ImPlus = new ImagePlus(VoronoiSaveDir + "\\Channel_1_Voronoi.tif");
                    VorDiaCh1ImPlus.show();
                    lm_IsSaveVoronoiResult(MyConfigurator.SegmentationClusterSave);
                }
                if (MyConfigurator.ClusterChan2Enable == 1) {
                    if (MyConfigurator.SegmentationClusterSave == 1) {
                        String SegmentationAndClusterSaveDir = MyConfigurator.ResultsFilePath + "SegmentationAndClusterResults";
                        File file = new File(SegmentationAndClusterSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                        VoronoiSaveDir = SegmentationAndClusterSaveDir + "\\VoronoiResults";
                        file = new File(VoronoiSaveDir);
                        if (!file.exists()) {
                            file.mkdir();
                        }
                    }else {
                        VoronoiSaveDir = MyConfigurator.ResultsFilePath;
                    }
                    lm_VoronoiDiagramCh2(VoronoiSaveDir + "\\Channel_2_Voronoi.tif", MyConfigurator.LinkPostClusterEnable);
                    ImagePlus VorDiaCh2ImPlus = new ImagePlus(VoronoiSaveDir + "\\Channel_2_Voronoi.tif");
                    VorDiaCh2ImPlus.show();
                    lm_IsSaveVoronoiResult(MyConfigurator.SegmentationClusterSave);

                }

            }

            MyConfigurator.EnableCluster();
        }
    }

    public void StartColocalization() throws InterruptedException {
        Colocalization thread_Colocalization = new Colocalization();
        thread_Colocalization.start();
    }

    public class Colocalization extends Thread {

        @Override
        public void run() {

            if (roiManager.getSelectedIndex() == -1) {
                MyConfigurator.EnableColocalization();
                return;
            }
            lm_SetSaveDir(MyConfigurator.ResultsFilePath);

            lm_SetColocalizationPara( MyConfigurator.Rmax, MyConfigurator.Step, MyConfigurator.ColoThres, roiInfo.Width_nm, roiInfo.Height_nm);

            lm_Colocalization(MyConfigurator.LinkPostColocEnable);

            float[] CBCHistInfo = lm_GetCBCHistInfo();
       
            Plot ColocPlot;
            FileSaver ColocalizationTifSaver;

            double[] CBC1 = lm_GetCBC1Hist();
            ColocPlot = new Plot("CBC Score Ch1 -> Ch2", "CBC Score Ch1 -> Ch2", "Frequency");
            ColocPlot.setColor(Color.green);
            ColocPlot.addHistogram(CBC1, CBCHistInfo[0] / 100);
            ColocPlot.show();
            ImagePlus CBC1ImPlus = ColocPlot.getImagePlus();
            double[] CBC2 = lm_GetCBC2Hist();
            ColocPlot = new Plot("CBC Score Ch2 -> Ch1", "CBC Score Ch2 -> Ch1", "Frequency");
            ColocPlot.setColor(Color.red);
            ColocPlot.addHistogram(CBC2, CBCHistInfo[1] / 100);
            ColocPlot.show();
            ImagePlus CBC2ImPlus = ColocPlot.getImagePlus();

            

            if (MyConfigurator.ColocalizationSave == 1) {
                String ColocalizationSaveDir = MyConfigurator.ResultsFilePath + "ColocalizationResults";
                File file = new File(ColocalizationSaveDir);
                if (!file.exists()) {
                    file.mkdir();
                }

                lm_SaveColoclizationResult(ColocalizationSaveDir + "\\CBCScoreResults.csv");
                ColocalizationTifSaver = new FileSaver(CBC1ImPlus);
                ColocalizationTifSaver.saveAsTiff(ColocalizationSaveDir + "\\Channel_1-Channel_2_CBCScore.tif");
                ColocalizationTifSaver = new FileSaver(CBC2ImPlus);
                ColocalizationTifSaver.saveAsTiff(ColocalizationSaveDir + "\\Channel_2-Channel_1_CBCScore.tif");
            }
            MyConfigurator.EnableColocalization();
        }
    }

    public class RoiInfo {

        double LeftX_pix;
        double LeftY_pix;
        double Width;
        double Height;
        double RightX_pix;
        double RightY_pix;
        double LeftX_nm;
        double LeftY_nm;
        double RightX_nm;
        double RightY_nm;
        double Width_nm;
        double Height_nm;
    }

    public RoiInfo GetRoiInfo() {
        roiInfo.LeftX_pix = roi.getXBase();
        roiInfo.LeftY_pix = roi.getYBase();
        roiInfo.Width = roi.getFloatWidth();
        roiInfo.Height = roi.getFloatHeight();
        roiInfo.RightX_pix = roiInfo.LeftX_pix + roiInfo.Width;
        roiInfo.RightY_pix = roiInfo.LeftY_pix + roiInfo.Height;
        roiInfo.LeftX_nm = ScatterPlot.descaleX((int) roiInfo.LeftX_pix);
        roiInfo.LeftY_nm = ScatterPlot.descaleY((int) roiInfo.LeftY_pix);
        roiInfo.RightX_nm = ScatterPlot.descaleX((int) roiInfo.RightX_pix);
        roiInfo.RightY_nm = ScatterPlot.descaleY((int) roiInfo.RightY_pix);
        roiInfo.Width_nm = roiInfo.RightX_nm - roiInfo.LeftX_nm;
        roiInfo.Height_nm = roiInfo.RightY_nm - roiInfo.LeftY_nm;
        return roiInfo;

    }

    public static ColorModel GetHotColorModel() {//决定显示图像的颜色
        // construct hot colormap
        byte[] r = new byte[256], g = new byte[256], b = new byte[256];
        for (int q = 0; q < 85; q++) {
            r[q] = (byte) (3 * q);
            g[q] = 0;
            b[q] = 0;
        }
        for (int q = 85; q < 170; q++) {
            r[q] = (byte) (255);
            g[q] = (byte) (3 * (q - 85));
            b[q] = 0;
        }
        for (int q = 170; q < 256; q++) {
            r[q] = (byte) (255);
            g[q] = (byte) 255;
            b[q] = (byte) (3 * (q - 170));
        }
        return new IndexColorModel(3, 256, r, g, b);
    }
}
