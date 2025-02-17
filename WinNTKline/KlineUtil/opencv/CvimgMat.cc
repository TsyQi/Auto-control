#include "CvimgMat.h"
#pragma warning (disable:4474)

using namespace cv;

Mat g_imageMat;

String imgFiles = "../KlineUtil/image/taoxi.";
const String g_jpegFile = imgFiles + "jpeg";
static String g_jpgFile = imgFiles + "jpg";

struct MatImages {
    Mat rawImage;
    Mat mskImage;
};
struct ROIImages {
    MatImages roiImages;
    int flag = 0;
};
struct GaussImage {
    MatImgSet set;
    Size ksize;
};

void createAlphaMat(Mat& mat)
{
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            Vec4b& rgba = mat.at<Vec4b>(i, j);
            rgba[0] = UCHAR_MAX;
            rgba[1] = saturate_cast<uchar>((float(mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX);
            rgba[2] = saturate_cast<uchar>((float(mat.rows - i)) / ((float)mat.rows) * UCHAR_MAX);
            rgba[3] = saturate_cast<uchar>(0.5 * (rgba[1] + rgba[2]));
        }
    }
}

int saveMat2PNG(int w, int h, const String& name)
{
    Mat mat(w, h, CV_8UC4);
    createAlphaMat(mat);
    std::vector<int>compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);
    try {
        imwrite(name, mat, compression_params);
    } catch (std::runtime_error& ex) {
        fprintf(stderr, "error trans jpeg to PNG format: %s\n", ex.what());
        return -1;
    }
    fprintf(stdout, "save png file = '%s' (%d, %d).\n", name.c_str(), w, h);
    return 0;
}

static void onROIxyTrackBar(int fix, void* usrdata)
{
    ROIImages* images = reinterpret_cast<ROIImages*>(usrdata);
    // images->rawImage.flags < 0 ? (Mat*)memcpy(images, usrdata, sizeof(RoiImages)) : &imread(dstPng);
    if (images->roiImages.rawImage.flags <= 0) {
        Mat img = imread(g_jpegFile);
        if (img.flags <= 0) {
            fprintf(stdout, "raw image is null!\n");
            return;
        }
        memcpy(&images->roiImages.rawImage, &img, sizeof(Mat));
    } else if (images->roiImages.mskImage.flags <= 0) {
        fprintf(stdout, "mask image is null!\n");
    }
    int x = 28;
    int y = 41;
    images->flag == 1 ? y = fix : x = fix;
    int w = images->roiImages.mskImage.cols;
    int h = images->roiImages.mskImage.rows;
    Mat imageROI = images->roiImages.rawImage(Rect(x, y, w, h));
    rectangle(images->roiImages.rawImage, Rect(x - 1, y - 1, w + 2, h + 2), Scalar(63, 63, 255), 2);
    addWeighted(imageROI, 0.2, images->roiImages.mskImage, 0.8, 0.0, imageROI);
    if (imageROI.data != NULL) {
        namedWindow("ROI", WINDOW_NORMAL);
        imshow("ROI", images->roiImages.rawImage);
    } else {
        fprintf(stdout, "addWeighted error, imageROI is null!\n");
    }
}

static void onBilateralTrackBar(int d, void* usrdata)
{
    MatImgSet* src = reinterpret_cast<MatImgSet*>(usrdata);
    if (src->image.flags < 0) {
        src->image = g_imageMat;
    }
    Mat out;
    bilateralFilter(src->image, out, d, d * 2, d / 2);
    imshow(src->name, out);
}

static void onMedianFilterTrackBar(int ksize, void* usrdata)
{
    MatImgSet* src = reinterpret_cast<MatImgSet*>(usrdata);
    if (ksize == 3 || ksize == 5) {
        src->image = imread(g_jpgFile, CV_8U | CV_16U | CV_32F);
    }
    if (ksize > 40) {
        src->image = imread(g_jpgFile, CV_8U);
    }
    if (ksize % 2 == 0) {
        ksize += 1;
    }
    Mat out;
    medianBlur(src->image, out, ksize);
    imshow(src->name, out);
}

static void onNeighbourAverageTrackBar(int ksize, void* usrdata)
{
    GaussImage* src = reinterpret_cast<GaussImage*>(usrdata);
    imshow(src->set.name, src->set.image);
    if (src->ksize == Size(3, 3)) {
        int size = 0;
        if (ksize != 0 && (ksize % 2 == 0)) {
            size = ksize + 1;
        } else {
            size = ksize;
        }
        src->ksize = Size(size, size);
    }
    Mat out;
    GaussianBlur(src->set.image, out, src->ksize, ksize, 0);
    imshow(src->set.name, out);
}

static void onThreshTrackBar(int old, void* usrdata)
{
    MatImgSet* src = reinterpret_cast<MatImgSet*>(usrdata);
    if (src->image.flags <= 0) {
        src->image = g_imageMat;
    }
    Mat out;
    // 二值化 
    cvtColor(src->image, out, CV_BGR2GRAY);
    threshold(src->image, out, old, 255, 0);
    imshow(src->name, out);
}

static void onMixedTrackBar(int value, void* usrdata)
{
    MatImages* images = reinterpret_cast<MatImages*>(usrdata);
    double alpha = value / 255.0;
    double beta = (1.0 - alpha);
    Mat mixImage;
    addWeighted(images->rawImage, beta, images->mskImage, alpha, 0.0, mixImage);
    imshow("Mixed Weights", mixImage);
}

Mat CvimgMat::getImageMat(const String& img, int flg)
{
    Mat out, src = imread(img, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR);
    if (flg == 0)
        src = imread(img, flg);
    if (!src.data)
        fprintf(stdout, "error imread raw image!\n");
    else {
        out = src.clone();
        if (flg <= -1) {
            namedWindow("Raw image", WINDOW_NORMAL);
            imshow("Raw image", src);
        }
    }
    return out;
}

MatImgSet* CvimgMat::getImageSet(const String& img, const String& name)
{
    MatImgSet* image = NULL;
    int len = sizeof(String) + sizeof(Mat);
    image = reinterpret_cast<MatImgSet*>(malloc(len));
    if (image == NULL)
        return NULL;
    memset(image, 0, len);
    image->image = getImageMat(img);
    image->name = name;
    if (image->image.flags <= 0) {
        fprintf(stdout, "%s: source image flag invalid.\n", name.c_str());
        free(image);
        return NULL;
    }
    return image;
}

int CvimgMat::interestRegionImage(const String& src, const String& mask)
{
    saveMat2PNG(90, 90, g_jpegFile);
    Mat logImage = imread(mask);

    if (!logImage.data) {
        fprintf(stdout, "error imread logo image!\n");
        return -1;
    }
    Mat imageROI{ Rect(0, 90, logImage.cols, logImage.rows) };
    Mat imageMask = imread(mask, CV_8U);
    //将掩膜拷贝到ROI   
    logImage.copyTo(imageROI, imageMask);
    imshow("imageROI[0,90]", imageROI);

    Mat srcImage = imread(src);
    if (!srcImage.data) {
        fprintf(stdout, "error imread roi image!\n");
        return -2;
    }
    int len = sizeof(ROIImages);
    ROIImages* images = reinterpret_cast<ROIImages*>(malloc(len));
    if (images == NULL)
        return -3;
    memset(images, 0, len);
    images->roiImages.rawImage = srcImage;
    images->roiImages.mskImage = logImage;
    images->flag = 0;
    int roival = 28;
    int xMax = srcImage.cols - logImage.cols;
    int yMax = srcImage.rows - logImage.rows;
    namedWindow("ROI", WINDOW_AUTOSIZE);
    createTrackbar("align X", "ROI", &roival, xMax, onROIxyTrackBar, images);
    ROIImages* imagesy = reinterpret_cast<ROIImages*>(malloc(len));
    if (imagesy == NULL)
        return -4;
    memset(imagesy, 0, len);
    memcpy(imagesy, images, len);
    imagesy->flag = 1;
    createTrackbar("align Y", "ROI", &roival, yMax, onROIxyTrackBar, imagesy);
    imshow("ROI", srcImage);

    fprintf(stdout, "ROI mask logImage to imageMask OK.\n");
    return 0;
}

int CvimgMat::mixedModelImage(const String& img1, const String& img2)
{
    Mat image1 = imread(img1);
    saveMat2PNG(image1.rows, image1.cols, g_jpegFile);
    Mat image2 = imread(img2);
    // size(dpi) of image must same as image2
    if (image1.cols != image2.cols || image2.rows != image1.rows) {
        fprintf(stdout, "mixedModelImage: mismatch image's size!(%d,%d)->(%d,%d)\n", \
            image1.cols, image1.rows, image2.cols, image2.rows);
        return -2;
    }
    if (!image1.data || !image2.data) {
        fprintf(stdout, "error imread alpha image!\n");
        return -1;
    }
    int iv = 0;
    int len = 2 * sizeof(Mat);
    MatImages* images = reinterpret_cast<MatImages*>(malloc(len));
    if (images == NULL)
        return -3;
    memset(images, 0, len);
    images->rawImage = image1;
    images->mskImage = image2;
    String name = "Mixed Weights";
    namedWindow(name, WINDOW_NORMAL);
    createTrackbar("weight", name, &iv, 255, onMixedTrackBar, images);
    imshow(name, images->rawImage);
    fprintf(stdout, "Mixed image2 to image1 OK.\n");
    return 0;
}

int CvimgMat::bilateralImage(const String& src)
{
    String name = "Bilateral";
    int len = sizeof(String) + sizeof(Mat);
    MatImgSet* image = reinterpret_cast<MatImgSet*>(malloc(len));
    if (image == NULL)
        return -1;
    memset(image, 0, len);
    image->image = getImageMat(src);
    image->name = name;
    if (image->image.flags <= 0) {
        fprintf(stdout, "%s: source image flag invalid.\n", name.c_str());
        free(image);
        return -2;
    }
    int bilateralval = 8;
    namedWindow(name, WINDOW_NORMAL);
    createTrackbar("bilateral", name, &bilateralval, 400, onBilateralTrackBar, image);
    imshow(name, image->image);
    fprintf(stdout, "BilateralFilter create OK.\n");
    return 0;
}

int CvimgMat::medianFilterImage(const String& src, int value)
{
    String name = "MedianFilter";
    int len = sizeof(String) + sizeof(Mat);
    MatImgSet* image = reinterpret_cast<MatImgSet*>(malloc(len));
    if (image == NULL)
        return -1;
    memset(image, 0, len);
    image->image = getImageMat(src);
    image->name = name;
    if (image->image.flags <= 0) {
        fprintf(stdout, "%s: source image flag invalid.\n", name.c_str());
        free(image);
        return -2;
    }
    if (value % 2 == 0)
        value += 1;
    namedWindow(name, WINDOW_NORMAL);
    createTrackbar("median", name, &value, 100, onMedianFilterTrackBar, image);
    imshow(name, image->image);
    fprintf(stdout, "MedianFilter create OK.\n");
    return 0;
}

int CvimgMat::neighbourAverageImage(const String& src, Size ksize)
{
    String name = "NeighbourAverage";
    int len = sizeof(MatImgSet) + sizeof(Size);
    GaussImage* image = reinterpret_cast<GaussImage*>(malloc(len));
    if (image == NULL)
        return -1;
    memset(image, 0, len);
    image->set.image = getImageMat(src);
    image->set.name = name;
    image->ksize = ksize;
    if (image->set.image.flags <= 0) {
        fprintf(stdout, "GaussianFilter: source image flag invalid.\n");
        free(image);
        return -2;
    }
    int value = 0;
    namedWindow(name);
    createTrackbar("Average size", name, &value, 99, onNeighbourAverageTrackBar, image);
    imshow(name, image->set.image);
    fprintf(stdout, "GaussianFilter create OK.\n");
    return 0;
}

int CvimgMat::thresholdImage(const String& src)
{
    String name = "Threshold";
    int len = sizeof(String) + sizeof(Mat);
    MatImgSet* image = reinterpret_cast<MatImgSet*>(malloc(len));
    if (image == NULL)
        return -1;
    memset(image, 0, len);
    image->image = getImageMat(src);
    image->name = name;
    if (image->image.flags <= 0) {
        fprintf(stdout, "%s: source image flag invalid.\n", name.c_str());
        free(image);
        return -2;
    }
    int posTrackBar = 128;
    namedWindow(name);
    createTrackbar("thresh", name, &posTrackBar, 255, onThreshTrackBar, image);
    imshow(name, image->image);
    fprintf(stdout, "%s sets for source image OK.\n", name.c_str());
    return 0;
}

int CvimgMat::cvMatTest(const String& file)
{
    if (!file.empty() && file.length() > 0) {
        g_jpgFile = file;
    }
    const cv::String bkgImage = "../KlineUtil/image/qdu.bmp";
    g_imageMat = getImageMat(g_jpgFile);

    interestRegionImage(bkgImage, g_jpegFile);
    mixedModelImage(g_jpgFile, g_jpegFile);
    neighbourAverageImage(g_jpgFile);
    medianFilterImage(g_jpgFile);
    bilateralImage(g_jpgFile);
    thresholdImage(g_jpgFile);

    while (char(waitKey(1)) != 27) { }
    return 0;
}

int CvimgMat::cvMatTest()
{
    return cvMatTest(g_jpegFile);
}
