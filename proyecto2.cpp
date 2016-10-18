#include <iostream>
#include <png++/png.hpp>
#include <string.h>
using namespace png;
using namespace std;

/**
 * \brief Retrieves the least significant bits from rgba pixels
 * Checks the rgba components of the pixel passed as a parameter
 * using the "and" operation to get the lsb, and then saving it into
 * an unsigned char, shifting the bit position as necessary since we
 * are doing it 4 bits as a time.
 * @param pixel the pixel whose bits are going to be retrieved.
 * @param c a counter that can be either 0 or 4, the first or
 * last half of the byte.
 * \return a char containing the information retrieved from the pixel.
*/

unsigned char retrieveLSB(rgba_pixel pixel, int c) {
  unsigned char byte= 0;
  byte |= ((pixel.red & 1) << (7 - c));
  byte |= ((pixel.green & 1) << (6 - c));
  byte |= ((pixel.blue & 1) << (5 - c));
  byte |= ((pixel.alpha & 1) << (4 - c));
  return byte;
}

/**
 * \brief Changes the lsb of the rgba components of a pixel
 * to the bits of a byte that is passed as a parameter
 * first we change the lsb to 0, and the we use the "or" operation
 * to encode the byte into the pixel, shifting it as necessary
 * snce we are encoding the first 4 bits.
 * @param pixel the pixel i wich we are going to store the iformation.
 * @param byte the byte that we are going to store.
 * \return a pixel containing the encoded information.
*/

rgba_pixel changeLSB(rgba_pixel pixel, unsigned char byte) {
  int r, g, b, a;
  int x= byte;
  r = (pixel.red & 254) | ((x >> 7) & 1); // 254=11111110 changes the lsb to 0
  g = (pixel.green & 254) | ((x >> 6) & 1);
  b = (pixel.blue & 254) | ((x >> 5) & 1);
  a = (pixel.alpha & 254) | ((x >> 4) & 1);
  return rgba_pixel(r, g, b, a);
}

/**
 * \brief Encodes the information of a file into an image using the changeLSB
 * method.
 * @param fileName the name of the file we are going to encode.
 * @param imageName the name of the file in wich we are going to encode the information.
 * @param imageOutput the name of the output file.
*/

void encode(char *fileName, string imageName, string imageOutput) {
  ifstream file (fileName);
  image< rgba_pixel > image(imageName);
  string line;
  unsigned char buffer;
  int byteCount = 0;
  for (size_t y = 0; y < image.get_height(); ++y) { // the for loops iterate the image pixel by pixel.
    for (size_t x = 0; x < image.get_width(); ++x) {
      if ( !(byteCount % 2) ) { // check to see if we are doing the first or last half of the byte
        if (file.eof()) buffer = 0; // we have finished to read the file
        else file >> std::noskipws >> buffer;
        image[y][x] = changeLSB(image[y][x], buffer);
        byteCount++;
      } else {
        image[y][x] = changeLSB(image[y][x], (buffer << 4));
        if (buffer == 0) {
          y = image.get_height(); // ends the loop
          x = image.get_width();
        }
        byteCount++;
      }
    }
  }
  image.write(imageOutput);
}

/**
 * \brief Decodes the information of an image using the retrieveLSB method.
 * @param fileName the name of the file with the decoded information.
 * @param imageName the name of the file that we ae going to decode.
*/

void decode(string imageName, char *fileName) {
  image< rgba_pixel > image(imageName);
  ofstream file (fileName);
  int byteCount= 0;
  unsigned char byte = 0;;
  int c= 0;
  for (size_t y = 0; y < image.get_height(); ++y) {
    for (size_t x = 0; x < image.get_width(); ++x) {
      if ( !(byteCount % 2) ) {
        byte |= retrieveLSB(image[y][x], c);
        c+= 4;
        byteCount++;
      } else {
        byte |= retrieveLSB(image[y][x], c);
        if (byte == 0) {
          file.close();
          return;
        }
        file << byte;
        byte &= 0;
        c= 0;
        byteCount++;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc < 3) {
   std::cout << "Indicate the flag and the file names" << std::endl;
   return 0;
 }
  char *flag= argv[1];
  if (flag[0]!= 'u' && flag[0]!= 'h')
    std::cout << "The flag needs to be either h for encoding or u for decoding" << std::endl;
  if (flag[0]== 'h') {
    encode(argv[2], argv[3], argv[4]);
  } else {
    decode(argv[2], argv[3]);
  }
  return 0;
}
