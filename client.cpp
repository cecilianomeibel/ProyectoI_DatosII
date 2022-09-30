#include <iostream>
#include <boost/asio.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/core.hpp>

#include <cassert>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

//Serializacion, convierte la imagen mat a string y viceversa
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
//Funcion que convierte el mat a string
std::string save( const cv::Mat & mat )
{
    std::ostringstream oss;
    boost::archive::text_oarchive toa( oss );
    toa << mat;

    return oss.str();
}
//Funcion que convierte el string a mat
void load( cv::Mat & mat, const char * data_str )
{
    std::stringstream ss;
    ss << data_str;

    boost::archive::text_iarchive tia( ss );
    tia >> mat;
}

//Funcion que divide la imagen en varios segmentos mat
int Segment_Image(const cv::Mat& img, const int blockWidth, std::vector<cv::Mat>& blocks)
{
    // Verifica si la imagen se paso correctamente
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
//Funcion que lee los mensajes de entrada
string ReadMessage(boost::asio::ip::tcp::socket & socket) {
    boost::asio::streambuf buf; // Buffer de entrada de mensajes
    boost::asio::read_until( socket, buf, "\n"); //  Indica que lea mensaje del socket desde el buffer hasta el delimitador \n
    string data = boost::asio::buffer_cast<const char*>(buf.data()); // Hace cast del buffer de entrada a un char pointer (caracteres legibles)
    return data; // Retorna el mensaje recibido
}
//Funcion que envia los mensajes
void SendMessage(boost::asio::ip::tcp::socket & socket, string message) {
    string msg = message + "\n"; // Declara variable string con un delimitador linea siguiente
    boost::asio::write( socket, boost::asio::buffer(msg)); // Envia mensaje a cliente mediante buffer
}

int main() {
    //Administracion de la imagen
    cv::Mat image = imread("/home/gabrielwolf/Documents/WorkSpace Final/ProyectoI_DatosII-master/stitch.jpg", IMREAD_COLOR);
    if (image.empty()) { //Comprueba si la imagen se pudo leer
        cout << "Imagen "
             << "Imagen no encontrada" << endl;
        cin.get(); // Espera a que se presione una tecla
        return -1;
    }

    //Segmentacion de la imagen
    const int blockw = 50; //se define el tamaño de los segmentos de la imagen
    std::vector<cv::Mat> blocks; //vector que contiene los segmentos de la imagen
    int divideStatus = Segment_Image(image, blockw, blocks); //segmenta la imagen

    //Socket
    boost::asio::io_service io_service; //servicio de entrada y salida
    boost::asio::ip::tcp::socket socket(io_service); //declara los sockects para la conexion

    socket.connect(boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    cout << "Conexion con el servidor exitosa" << endl;
    string size = to_string(blocks.size());

    //Envia el tamaño del vector con los segmentos
    SendMessage(socket, size);
    string receivedMessage = ReadMessage(socket);
    receivedMessage.pop_back();
    cout << "Server: "<<receivedMessage<<endl;

    //Envia la imagen compartida
    for (int i = 0; i < blocks.size() ; i++){
        cv::Mat TEMP = blocks[i];
        std::string serialized = save(TEMP);
        SendMessage(socket, serialized);
        string receivedStatus = ReadMessage(socket);
        receivedStatus.pop_back();
        cout << "Server: "<<receivedStatus<<endl;
    }

    //Recibe la imagen con Gaussian Blur aplicado
    vector<Mat>BlurBlocks;
    for (int i = 0; i < blocks.size(); i++) {
        string message = ReadMessage(socket); // Lee y declara mensaje del cliente
        SendMessage(socket, "Segmento " + to_string(i) + " con Gaussian Blur aplicado");
        message.pop_back();
        Mat result;
        load(result, message.c_str());
        BlurBlocks.push_back(result);
    }
    Mat ResultBlurApplyied;
    hconcat(BlurBlocks, ResultBlurApplyied);
    imshow("Imagen con Gaussian Blur aplicado", ResultBlurApplyied);
    waitKey(0);


    //Recibe la imagen con escala de grises aplicada
    vector<Mat>GrayBlocks;
    for (int i = 0; i < blocks.size(); i++) {
        string message = ReadMessage(socket); // Lee y declara mensaje del cliente
        SendMessage(socket, "Segmento " + to_string(i) + " con escala de grises aplicada");
        message.pop_back();
        Mat result;
        load(result, message.c_str());
        GrayBlocks.push_back(result);
    }
    Mat ResultGrayApplyied;
    hconcat(GrayBlocks, ResultGrayApplyied);
    imshow("Imagen con escala de grises aplicada", ResultGrayApplyied);
    waitKey(0);


    //Recibe la imagen con Control de Brillo aplicado
    vector<Mat>Bright_Segments;
    for (int i = 0; i < blocks.size(); i++) {
        string message = ReadMessage(socket); // Lee y declara mensaje del cliente
        SendMessage(socket, "Segmento " + to_string(i) + " con Control de Brillo");
        message.pop_back();
        Mat result;
        load(result, message.c_str());
        Bright_Segments.push_back(result);
    }
    Mat ResultBrightApplyied;
    hconcat(Bright_Segments, ResultBrightApplyied);
    imshow("Imagen con Control de Brillo aplicado", ResultBrightApplyied);
    waitKey(0);


    //Recibe la imagen con la correcion gamma aplicada
    vector<Mat>GammaBlocks;
    for (int i = 0; i < blocks.size(); i++) {
        string message = ReadMessage(socket); // Lee y declara mensaje del cliente
        SendMessage(socket, "Segmento " + to_string(i) + " con correccion gamma aplicado");
        message.pop_back();
        Mat result;
        load(result, message.c_str());
        GammaBlocks.push_back(result);
    }
    Mat ResultGammaApplied;
    hconcat(GammaBlocks, ResultGammaApplied);
    imshow("Imagen con Correccion Gamma aplicada", ResultGammaApplied);
    waitKey(0);

    return 0;
}