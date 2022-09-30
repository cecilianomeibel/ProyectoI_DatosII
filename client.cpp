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

int divideImage(const cv::Mat& img, const int blockWidth, const int blockHeight, std::vector<cv::Mat>& blocks)
{
    // Checking if the image was passed correctly
    if (!img.data || img.empty())
    {
        std::cout << "Image Error: Cannot load image to divide." << std::endl;
        return EXIT_FAILURE;
    }

    // init image dimensions
    int imgWidth = img.cols;
    int imgHeight = img.rows;
    std::cout << "IMAGE SIZE: " << "(" << imgWidth << "," << imgHeight << ")" << std::endl;

    // init block dimensions
    int bwSize;
    int bhSize;

    int y0 = 0;
    while (y0 < imgHeight)
    {
        // compute the block height
        bhSize = ((y0 + blockHeight) > imgHeight) * (blockHeight - (y0 + blockHeight - imgHeight)) + ((y0 + blockHeight) <= imgHeight) * blockHeight;

        int x0 = 0;
        while (x0 < imgWidth)
        {
            // compute the block witdh
            bwSize = ((x0 + blockWidth) > imgWidth) * (blockWidth - (x0 + blockWidth - imgWidth)) + ((x0 + blockWidth) <= imgWidth) * blockWidth;

            // crop block
            blocks.push_back(img(cv::Rect(x0, y0, bwSize, bhSize)).clone());

            // update x-coordinate
            x0 = x0 + blockWidth;
        }

        // update y-coordinate
        y0 = y0 + blockHeight;
    }
    return EXIT_SUCCESS;
}


//Serialization
//Convertir mat en string y viceversa

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

std::string save( const cv::Mat & mat ) // Pasar un mat a un string
{
    std::ostringstream oss;
    boost::archive::text_oarchive toa( oss );
    toa << mat;

    return oss.str();
}

void load( cv::Mat & mat, const char * data_str ) // Pasar un string a un mat
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
imshow("ImageWindow", image);
waitKey(0);

//Segmentar la imagen

// init vars
    const int blockw = 128;
    const int blockh = 128;
    std::vector<cv::Mat> blocks;
    int divideStatus = divideImage(image, blockw, blockh, blocks);
// debug: save blocks
/*
cv::utils::fs::createDirectory("blocksFolder");
for (int j = 0; j < blocks.size(); j++)
{
    std::string blockId = std::to_string(j);
    std::string blockImgName = "blocksFolder/block#" + blockId + ".jpeg";
    imwrite(blockImgName, blocks[j]);
}*/

//Socket

    boost::asio::io_service io_service; // Servicio de input/output
    boost::asio::ip::tcp::socket socket(io_service); // Declaracion de socket para conexiones
    boost::system::error_code error; // Variable para codigo de error especifico de Boost

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






