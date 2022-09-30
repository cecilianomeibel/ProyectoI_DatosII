#include <iostream>
#include <boost/asio.hpp>
#include <fstream>
#include <opencv2/opencv_modules.hpp>
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

using namespace std;
using namespace cv;

//Segmentacion de la imagen

int divideImage(const cv::Mat& img, const int blockWidth, std::vector<cv::Mat>& blocks) //Se elimina el parametro de altura porque este va a ser constante
{
    // Esto comprueba si la imagen carga bien para luego segmentarla
    if (!img.data || img.empty())
    {
        std::cout << "Error en la imagen: No se ha podido cargar la imagen para segmentarla" << std::endl;
        return EXIT_FAILURE;
    }

        // inicializar las dimensiones de la imagen
        int imgWidth = img.cols;
        int imgHeight = img.rows;
        std::cout << "IMAGE SIZE: " << "(" << imgWidth << "," << imgHeight << ")" << std::endl;

        // inicializar las dimensiones del segmento (block)
        int bwSize;
        int bhSize = img.rows;

        int y0 = 0;
        int x0 = 0;
        while (x0 < imgWidth)
        {
            // La altura del segmento no cambia
            // Calcula el ancho de cada segmento
            bwSize = ((x0 + blockWidth) > imgWidth) * (blockWidth - (x0 + blockWidth - imgWidth)) + ((x0 + blockWidth) <= imgWidth) * blockWidth;

            // Crea los segmentos con el ancho calculado
            blocks.push_back(img(cv::Rect(x0, y0, bwSize, bhSize)).clone());

            // Actualizar la coordenada  x
            x0 = x0 + blockWidth;
        }
        return EXIT_SUCCESS;
    }



//Serialization
//Convertir la imagen de mat a string y viceversa

BOOST_SERIALIZATION_SPLIT_FREE( cv::Mat );

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

std::string save( const cv::Mat & mat ) //  Permite hacer la conversion de mat a un string
{
    std::ostringstream oss;
    boost::archive::text_oarchive toa( oss );
    toa << mat;

    return oss.str();
}

void load( cv::Mat & mat, const char * data_str ) // Permite hacer la conversion de string a un string a un mat
{
    std::stringstream ss;
    ss << data_str;

    boost::archive::text_iarchive tia( ss );
    tia >> mat;
}

string ReadMessage(boost::asio::ip::tcp::socket & socket) {
    boost::asio::streambuf buf; // Buffer de entrada de mensajes
    boost::asio::read_until( socket, buf, "\n"); //  Indica que lea mensaje del socket desde el buffer hasta el delimitador \n
    string data = boost::asio::buffer_cast<const char*>(buf.data()); // Hace cast del buffer de entrada a un char pointer (caracteres legibles)
    return data; // Retorna el mensaje recibido
}

//Funcion que envia mensaje al cliente


void SendMessage(boost::asio::ip::tcp::socket & socket, string message) {
    string msg = message + "\n"; // Declara variable string con un delimitador linea siguiente
    boost::asio::write( socket, boost::asio::buffer(msg)); // Envia mensaje a cliente mediante buffer
}


int main() {

    cv::Mat image = imread("/home/meibel/Descargas/convertidorImg-main/untitled/stitch.jpg", IMREAD_COLOR);
    if (image.empty()) { //Verify if the image has been readed correctly
        cout << "Image File "
             << "Not Found" << endl;
        cin.get(); // wait for any key press
        return -1;
    }


//Segmentar la imagen

// init vars
    const int blockw = 128;
    std::vector<cv::Mat> blocks;
    int divideStatus = divideImage(image, blockw, blocks);



//Socket

    boost::asio::io_service io_service; // Servicio de input/output
    boost::asio::ip::tcp::socket socket(io_service); // Declaracion de socket para conexiones

    socket.connect(boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string("127.0.0.1"), 1234));
    cout << "Conectado al servidor" << endl;
    string size = to_string(blocks.size());

    SendMessage(socket, size);
    string receivedMessage = ReadMessage(socket);
    receivedMessage.pop_back();
    cout << "Server dice que: "<<receivedMessage<<endl;

    for (int i = 0; i < blocks.size(); i++){
        cv::Mat TEMP = blocks[i];
        std::string serialized = save(TEMP);
        SendMessage(socket, serialized); // Escribe mensaje al servidor
        string receivedStatus = ReadMessage(socket);
        receivedMessage.pop_back();
        cout << "Server dice: "<<receivedStatus<<endl;
    }
    return 0;
}






