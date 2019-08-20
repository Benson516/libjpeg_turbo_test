#include <ROS_ICLU3_v0.hpp>

// Debug
#include <iostream>
// test
#include <FILTER_LIB.h>

// libjpeg-turbo
#include <errno.h>
#include "turbojpeg.h"

#define THROW(action, message) { \
  printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
  retval = -1; \
  exit(1); \
}
#define THROW_TJ(action)  THROW(action, "err") // THROW(action, tjGetErrorStr2(tjInstance))
#define THROW_UNIX(action)  THROW(action, strerror(errno))
#define DEFAULT_SUBSAMP  TJSAMP_444
#define DEFAULT_QUALITY  95

const char *subsampName[TJ_NUMSAMP] = {
  "4:4:4", "4:2:2", "4:2:0", "Grayscale", "4:4:0", "4:1:1"
};
const char *colorspaceName[TJ_NUMCS] = {
  "RGB", "YCbCr", "GRAY", "CMYK", "YCCK"
};


//------------------------------//
void jpeg_decompress_from_file(){

    // std::string input_file_name("/home/itri/2018_banner.jpg");
    std::string input_file_name("/home/itri/112254.jpg");


    // evaluation
    TIME_STAMP::Period period_jpeg("JPEG");

    tjscalingfactor scalingFactor = { 1, 1 };
    int outSubsamp = -1, outQual = -1;
    int flags = 0;
    int width, height;
    FILE *jpegFile = NULL;
    unsigned char *imgBuf = NULL, *jpegBuf = NULL;
    int retval = 0, i, pixelFormat = -1; //  TJPF_UNKNOWN;
    tjhandle tjInstance = NULL;

    /* Input image is a JPEG image.  Decompress and/or transform it. */
    long size;
    int inSubsamp, inColorspace;
    unsigned long jpegSize;

    /* Read the JPEG file into memory. */
    if ((jpegFile = fopen(input_file_name.c_str(), "rb")) == NULL)
        THROW_UNIX("opening input file");
    // Get size
    if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) || fseek(jpegFile, 0, SEEK_SET) < 0)
        THROW_UNIX("determining input file size");
    if (size == 0)
        THROW("determining input file size", "Input file contains no data");
    jpegSize = (unsigned long)size;
    if ((jpegBuf = (unsigned char *)tjAlloc(jpegSize)) == NULL)
        THROW_UNIX("allocating JPEG buffer");
    if (fread(jpegBuf, jpegSize, 1, jpegFile) < 1)
        THROW_UNIX("reading input file");
    fclose(jpegFile);  jpegFile = NULL;

    // 1
    //-----------------------------//
    period_jpeg.stamp();  period_jpeg.show_msec();
    //-----------------------------//

    // Create decompressor handler
    if ((tjInstance = tjInitDecompress()) == NULL)
        THROW_TJ("initializing decompressor");




    // Read header
    if (tjDecompressHeader3(tjInstance, jpegBuf, jpegSize, &width, &height, &inSubsamp, &inColorspace) < 0)
        THROW_TJ("reading JPEG header");
    printf("%s Image:  %d x %d pixels, %s subsampling, %s colorspace\n", "Input", width, height, subsampName[inSubsamp], colorspaceName[inColorspace]);



    // Actually decompress image
    /* Scaling and/or a non-JPEG output image format and/or compression options
      have been selected, so we need to decompress the input/transformed
      image. */
    width = TJSCALED(width, scalingFactor);
    height = TJSCALED(height, scalingFactor);
    if (outSubsamp < 0)
        outSubsamp = inSubsamp;

    pixelFormat = TJPF_BGRX;
    if ((imgBuf = (unsigned char *)tjAlloc(width * height * tjPixelSize[pixelFormat])) == NULL)
        THROW_UNIX("allocating uncompressed image buffer");

    // 2
    //-----------------------------//
    period_jpeg.stamp();  period_jpeg.show_msec();
    //-----------------------------//


    if (tjDecompress2(tjInstance, jpegBuf, jpegSize, imgBuf, width, 0, height, pixelFormat, flags) < 0)
        THROW_TJ("decompressing JPEG image");
    tjFree(jpegBuf);  jpegBuf = NULL;
    tjDestroy(tjInstance);  tjInstance = NULL;


    // 3
    //-----------------------------//
    period_jpeg.stamp();  period_jpeg.show_msec();
    //-----------------------------//

    // output: imgBuf
    // Mat (int rows, int cols, int type, void *data, size_t step=AUTO_STEP)
    cv::Mat image_decoded_BGRA(height, width, CV_8UC4, imgBuf);
    cv::Mat image_decoded;
    cvtColor(image_decoded_BGRA, image_decoded, CV_BGRA2BGR);
    std::cout << "Decoded image size = (" << image_decoded.cols << ", " << image_decoded.rows << ")\n";

    // 4
    //-----------------------------//
    period_jpeg.stamp();  period_jpeg.show_msec();
    //-----------------------------//

    // Show image
    namedWindow("Decoded image", cv::WINDOW_AUTOSIZE);
    imshow("Decoded image", image_decoded);
    waitKey(0);

}
//------------------------------//


using TIME_STAMP::Time;
using TIME_STAMP::Period;

int main(int argc, char *argv[])
{
    Time t_start(TIME_PARAM::NOW);
    t_start.show();

    Period period_1;


    Time t_a(3.0);
    t_a.show();
    Time t_b(1.0);
    t_b.show();

    Time t_c = t_b - t_a;
    t_c.show_sec();


    std::shared_ptr<int> a_ptr;
    a_ptr.reset(new int(5));
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";

    boost::any any_a = a_ptr;
    a_ptr.reset(new int(0));
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";
    std::cout << "any_a.use_count = " << ( boost::any_cast< std::shared_ptr<int> >(any_a) ).use_count() << "\n";
    a_ptr = boost::any_cast< std::shared_ptr<int> >(any_a);
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";


    std::shared_ptr<int> *a_ptr_ptr = boost::any_cast< std::shared_ptr<int> >(&any_a);
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";

    //
    std::cout << "any_a = " << *( boost::any_cast< std::shared_ptr<int> >(any_a) ) << "\n";
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";
    std::cout << "*a_ptr_ptr = " << *( *a_ptr_ptr ) << "\n";
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";


    boost::any any_b = any_a;
    std::cout << "b.use_count() = " << ( boost::any_cast< std::shared_ptr<int> >(any_b) ).use_count() << "\n";
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";

    //
    a_ptr.reset(new int(10));
    // std::cout << "b.use_count() = " << ( boost::any_cast< std::shared_ptr<int> >(any_b) ).use_count() << "\n";
    boost::any any_a_ptr = &a_ptr;
    std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";
    // std::cout << "any_a = " << *( boost::any_cast< std::shared_ptr<int> >(any_a_ptr) ) << "\n";
    // std::cout << "a.use_count() = " << a_ptr.use_count() << "\n";


    Time t_end(TIME_PARAM::NOW);
    t_end.show();

    Time t_delta = t_end - t_start;
    t_delta.show_sec();

    period_1.stamp();
    period_1.show_sec();



    // test, jpeg decoder
    jpeg_decompress_from_file();

    return 0;
}
