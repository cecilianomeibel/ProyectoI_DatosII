#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>

#include <opencv2/core.hpp>
#include <boost/serialization/split_free.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/stitching.hpp"

#include <string>
#include <utility>
#include <vector>
using namespace std;
using namespace cv;


class ImageProcessing{
private: //atributes
    int width;
    int height;
    int delta_bright;
    float gamma;
    Mat *source;
    Mat *destiny;
public:
    ImageProcessing(int, int, int, float, Mat*, Mat*); //constructor
    int gaussian_blur();
    int gray_scale();
    int bright_control();
    int gamma_correction();
};
//constructor
ImageProcessing::ImageProcessing(int _witdh, int _height, int _delta_bright, float _gamma, Mat *_source, Mat *_destiny) {
    width = _witdh;
    height = _height;
    delta_bright = _delta_bright;
    gamma = _gamma;
    source = _source;
    destiny = _destiny;
}
int ImageProcessing::gaussian_blur() {
    if (source->empty()){
        return -1;
    }else{
        GaussianBlur(*source, *destiny , Size(3, 3), 0);
        return 0;
    }
}
int ImageProcessing::gray_scale() {
    if (source->empty()){
        return -1;
    }else{
        cvtColor(*source, *destiny, COLOR_BGR2GRAY);
        return 0;
    }
}
int ImageProcessing::bright_control() {
    if (source->empty()){
        return -1;
    }else{
        source->convertTo(*destiny, -1, 1, delta_bright);
        return 0;
    }
}
int ImageProcessing::gamma_correction() {
    if (source->empty()){
        return -1;
    }else{
        float invGamma = 1 / gamma;
        Mat table(1, 256, CV_8U);
        uchar *p = table.ptr();
        for (int i = 0; i < 256; ++i) {
            p[i] = (uchar) (pow(i / 255.0, invGamma) * 255);
        }
        LUT(*source, table, *destiny);
        return 0;
    }
}
//Funcion que segmenta la imagen en varios mat
int Segment_image(const cv::Mat& img, const int blockWidth, std::vector<cv::Mat>& blocks){
    // Checking if the image was passed correctly
    if (!img.data || img.empty())
    {
        std::cout << "Error al cargar imagen, no es posible empezar la segmentacion" << std::endl;
        return EXIT_FAILURE;
    }

    // Inicializa las dimensiones de la imagen
    int imgWidth = img.cols;
    int imgHeight = img.rows;
    std::cout << "IMAGE SIZE: " << "(" << imgWidth << "," << imgHeight << ")" << std::endl;

    // Inicializa las dimensiones del segmento
    int bwSize;
    int bhSize = img.rows;

    int y0 = 0;
    int x0 = 0;
    while (x0 < imgWidth)
    {
        bwSize = ((x0 + blockWidth) > imgWidth) * (blockWidth - (x0 + blockWidth - imgWidth)) + ((x0 + blockWidth) <= imgWidth) * blockWidth;

        blocks.push_back(img(cv::Rect(x0, y0, bwSize, bhSize)).clone());

        x0 = x0 + blockWidth;

    }
    return EXIT_SUCCESS;
}

//Serializacion, permite convertir de mat a string y viceversa
BOOST_SERIALIZATION_SPLIT_FREE( cv::Mat )

namespace boost {
    namespace serialization {
        template <class Archive>
        void save( Archive & ar, const cv::Mat & m, const unsigned int version )
        {
            size_t elemSize = m.elemSize();
            size_t elemType = m.type();

            ar & m.cols;
            ar & m.rows;
            ar & elemSize;
            ar & elemType;

            const size_t dataSize = m.cols * m.rows * m.elemSize();
            for ( size_t i = 0; i < dataSize; ++i )
                ar & m.data[ i ];
        }
        template <class Archive>
        void load( Archive & ar, cv::Mat& m, const unsigned int version )
        {
            int cols, rows;
            size_t elemSize, elemType;

            ar & cols;
            ar & rows;
            ar & elemSize;
            ar & elemType;

            m.create( rows, cols, static_cast< int >( elemType ) );
            const size_t dataSize = m.cols * m.rows * elemSize;
            for (size_t i = 0; i < dataSize; ++i)
                ar & m.data[ i ];
        }
    }
}

//Función que convierte la imagen mat a string
std::string save( const cv::Mat & mat )
{
    std::ostringstream oss;
    boost::archive::text_oarchive toa( oss );
    toa << mat;

    return oss.str();
}

//Función que convierte de un string a mat
void load( cv::Mat & mat, const char * data_str )
{
    std::stringstream ss;
    ss << data_str;

    boost::archive::text_iarchive tia( ss );
    tia >> mat;
}

//Función que lee la entrada de los mensajes
string ReadMessage(boost::asio::ip::tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::asio::read_until( socket, buf, "\n" );
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

//Función que envia el mensaje de salida
void SendMessage(boost::asio::ip::tcp::socket & socket, string message) {
    string msg = message + "\n";
    boost::asio::write( socket, boost::asio::buffer(msg));
}

int main() {
    boost::asio::io_service io_service; //Servicio de envio y salida
    boost::asio::ip::tcp::acceptor acceptor_(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),1234));
    boost::asio::ip::tcp::socket socket_(io_service); //Declaración del socket

    cout << "Servidor funciona :)" << endl;
    acceptor_.accept(socket_);
    cout << "Se ha conectado un cliente :)" << endl;

    string sizeMessage = ReadMessage(socket_);
    sizeMessage.pop_back(); // Popping last character "\n"
    SendMessage(socket_, "Tamaño recibido");
    int size = stoi(sizeMessage);

    vector<cv::Mat> blocks;

    //Recibe a la imagen

    for (int i = 0; i < size; i++) {
        string message = ReadMessage(socket_); // Lee y declara mensaje del cliente
        SendMessage(socket_, "Segmento " + to_string(i) + " recibido");
        message.pop_back();
        Mat result;
        load(result, message.c_str());
        blocks.push_back(result);
    }
    /*
     * Crea una carpeta donde se almacenan las imagenes segmentadas
     */
    cv::utils::fs::createDirectory("Segmentos_Imagen");
    for (int j = 0; j < blocks.size(); j++)
    {
        std::string blockId = std::to_string(j);
        std::string blockImgName = "Segmentos_Imagen/block#" + blockId + ".jpg";
        imwrite(blockImgName, blocks[j]);
    }
    Mat Result;
    hconcat(blocks, Result);

    //Aplicación de filtros

    Mat BlurApplyed;
    ImageProcessing ImageBlur = ImageProcessing(Result.cols, Result.rows,100,3.0, &Result, &BlurApplyed);
    ImageBlur.gaussian_blur();

    Mat GrayApplyed;
    ImageProcessing ImageGray= ImageProcessing(Result.cols, Result.rows,100,3.0, &Result, &GrayApplyed);
    ImageGray.gray_scale();

    Mat BrightApplyed;
    ImageProcessing ImageBright = ImageProcessing(Result.cols, Result.rows,100,3.0, &Result, &BrightApplyed);
    ImageBright.bright_control();

    Mat GammaApplyed;
    ImageProcessing ImageGamma = ImageProcessing(Result.cols, Result.rows,75,3.0, &Result, &GammaApplyed);
    ImageGamma.gamma_correction();



    /*ImageProcessing ImageGamma = ImageProcessing(Result.cols, Result.rows,75,3.0, &Result, &GammaApplyed);
    ImageGamma.gamma_correction();
    Mat BlurApplyed;
    ImageProcessing ImageBlur = ImageProcessing(Result.cols, Result.rows,100,3.0, &Result, &BlurApplyed);
    ImageBlur.gaussian_blur();
    Mat BrightApplyed;
    ImageProcessing ImageBright = ImageProcessing(Result.cols, Result.rows,100,3.0, &Result, &BrightApplyed);
    ImageBright.bright_control();
    Mat GrayApplyed;
    ImageProcessing ImageGray= ImageProcessing(Result.cols, Result.rows,100,3.0, &Result, &GrayApplyed);
    ImageGray.gray_scale(); */

    const int blockw = 50; //Se asigna el tamaño a los segmentos


    //Envia la imagen con el gaussian blur aplicado

    vector<Mat> blocksBlur;
    int divideBlurStatus = Segment_image(BlurApplyed, blockw, blocksBlur); //Segmenta la imagen

    for (int i = 0; i < size ; i++){
        cv::Mat TEMP = blocksBlur[i];
        std::string serialized = save(TEMP);
        SendMessage(socket_, serialized);
        string receivedStatus = ReadMessage(socket_);
        receivedStatus.pop_back();
        cout << "Cliente: "<<receivedStatus<<endl;
    }

    //Envia la imagen con la escala de grises aplicado

    vector<Mat> blocksGray;
    int divideGrayStatus = Segment_image(GrayApplyed, blockw, blocksGray); //Segmenta la imagen

    for (int i = 0; i < size ; i++){
        cv::Mat TEMP = blocksGray[i];
        std::string serialized = save(TEMP);
        SendMessage(socket_, serialized);
        string receivedStatus = ReadMessage(socket_);
        receivedStatus.pop_back();
        cout << "Cliente: "<<receivedStatus<<endl;
    }

    //Envia la imagen con el brillo aplicado

    vector<Mat> Segment_Bright;
    int divideBrightStatus = Segment_image(BrightApplyed, blockw, Segment_Bright); //Segmenta la imagen
    for (int i = 0; i < size ; i++){
        cv::Mat TEMP = Segment_Bright[i];
        std::string serialized = save(TEMP);
        SendMessage(socket_, serialized);
        string receivedStatus = ReadMessage(socket_);
        receivedStatus.pop_back();
        cout << "Cliente: "<<receivedStatus<<endl;
    }

    //Envia la imagen con el control de gama aplicado
    vector<Mat> blocksGamma;
    int divideGammaStatus = Segment_image(GammaApplyed, blockw, blocksGamma); //Segmenta la imagen

    for (int i = 0; i < size ; i++){
        cv::Mat TEMP = blocksGamma[i];
        std::string serialized = save(TEMP);
        SendMessage(socket_, serialized);
        string receivedStatus = ReadMessage(socket_);
        receivedStatus.pop_back();
        cout << "Cliente: "<<receivedStatus<<endl;//Envia la imagen con la correción gama aplicada
    }

    return 0;
}