#include <iostream>
#include <boost/asio.hpp>
#include <opencv4/opencv2/opencv.hpp>
#include <opencv2/core/utils/filesystem.hpp>
#include <opencv2/core.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <cassert>
#include <string>
#include <vector>

using namespace cv;
using namespace std;

//Serializacion
//Proceso de conversion de la imagen de mat a string y viceversa

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

//Convierte la imagen mat a un string
std::string save( const cv::Mat & mat )
{
    std::ostringstream oss;
    boost::archive::text_oarchive toa( oss );
    toa << mat;

    return oss.str();
}
//Convierte la imagen string a un mat
void load( cv::Mat & mat, const char * data_str )
{
    std::stringstream ss;
    ss << data_str;

    boost::archive::text_iarchive tia( ss );
    tia >> mat;
}

//Funcion que carga la imagen para luego segmentarla

int Segment_Image(const cv::Mat& img, const int blockWidth, std::vector<cv::Mat>& blocks)
{
    // Verifica si la imagen se cargo de manera correcta
    if (!img.data || img.empty())
    {
        std::cout << "Error: No fue posible cargar la imagen para el proceso de segmentado" << std::endl;
        return EXIT_FAILURE;
    }

    // Inicializar las dimensiones de la imagen
    int imgWidth = img.cols;
    int imgHeight = img.rows;
    std::cout << "IMAGE SIZE: " << "(" << imgWidth << "," << imgHeight << ")" << std::endl;

    // Inicializar las dimensiones del segmento
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
//Funcion que lee los mensajes que ingresan

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

    // Administracion de la imagen
    cv::Mat image = imread("/home/gabrielwolf/Documents/WorkSpace Final/ProyectoI_DatosII-master/stitch.jpg", IMREAD_COLOR);
    if (image.empty()) { //Se asegura de que la imagen ha sido leida
        cout << "Documento imagen "
             << "No se encuentra" << endl;
        cin.get(); // Esperar que presione cualquier tecla
        return -1;
    }

    //Segmentacion de la imagen
    const int blockw = 50; //se define el tamaño de los segmentos que se generan de la imagen
    std::vector<cv::Mat> blocks; //vector que contiene los segmentos de la imagen
    int divideStatus = Segment_Image(image, blockw, blocks); //segmenta la imagen

    //Socket

    boost::asio::io_service io_service; //servicio de entrada y salida
    boost::asio::ip::tcp::socket socket(io_service); //declara los sockets para la conexion

    socket.connect(boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    cout << "Conexion excitosa al servidor" << endl;
    string size = to_string(blocks.size());

    //recibe el tamaño del vector de segmentos

    SendMessage(socket, size);
    string receivedMessage = ReadMessage(socket);
    receivedMessage.pop_back();
    cout << "Server: "<<receivedMessage<<endl;

    //Recibe la imagen y añade en ella el vector de mat
    for (int i = 0; i < blocks.size() ; i++){
        cv::Mat TEMP = blocks[i];
        std::string serialized = save(TEMP);
        SendMessage(socket, serialized);
        string receivedStatus = ReadMessage(socket);
        receivedMessage.pop_back();
        cout << "Server: "<<receivedStatus<<endl;
    }
    return 0;
}