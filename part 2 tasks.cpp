#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
struct Image{
    char idLength = 'A';
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescriptor;
    vector<unsigned char> BGRvalues;
    // struct for the header information
};

// writes the new file
void write(const string& fileName, Image fileHeader, const vector<int>& newPixelValues) {
    fstream newFile;
    newFile.open(fileName, ios_base::out | ios_base::binary | ios_base::trunc);
    if (newFile.is_open()) {
        newFile.write(&fileHeader.idLength, 1);
        newFile.write(&fileHeader.colorMapType, 1);
        newFile.write(&fileHeader.dataTypeCode, 1);
        newFile.write((char *) (&fileHeader.colorMapOrigin), 2);
        newFile.write((char *) (&fileHeader.colorMapLength), 2);
        newFile.write(&fileHeader.colorMapDepth, 1);
        newFile.write((char *) (&fileHeader.xOrigin), 2);
        newFile.write((char *) (&fileHeader.yOrigin), 2);
        newFile.write((char *) (&fileHeader.width), 2);
        newFile.write((char *) (&fileHeader.height), 2);
        newFile.write(&fileHeader.bitsPerPixel, 1);
        newFile.write(&fileHeader.imageDescriptor, 1);
        // write the struct data
        int area = fileHeader.height * fileHeader.width;
        for (int i=0; i < area*3; i++){
            newFile.write((char*)&newPixelValues.at(i), 1);
        }
    }
    else {cout << "could not open file write function" << endl;}
}
// write function here

// returns vector of subtracted pixel values to be written
vector<int> subtractAlgorithm(Image bottom_Layer, Image top_Layer) {
    vector<int> newPixelValues;
    int area = bottom_Layer.width * bottom_Layer.height;
    for (int pix = 0; pix < area * 3; pix++) {
        auto bottomLayer = (unsigned char) bottom_Layer.BGRvalues.at(pix);
        auto topLayer = (unsigned char) top_Layer.BGRvalues.at(pix);
        int newPixel = (bottomLayer - topLayer);
        if (newPixel < 0) { newPixel = 0; }
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// returns vector of added pixel values to be written
vector<int> addAlgorithm(Image file1, Image file2) {
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix = 0; pix < area * 3; pix++) {
        auto bottomLayer = (unsigned char) file1.BGRvalues.at(pix);
        auto topLayer = (unsigned char) file2.BGRvalues.at(pix);
        int newPixel = (topLayer + bottomLayer);
        if (newPixel > 255) { newPixel = 255;}
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// returns vector of multiplied pixel values to be written
vector<int> multiplyAlgorithm(Image file1, Image file2) {
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix=0; pix<area*3; pix++){
        float bottomLayer = (unsigned char) file1.BGRvalues.at(pix);
        float topLayer = (unsigned char) file2.BGRvalues.at(pix);
        float newPixel = ((bottomLayer/255 * topLayer/255) * 255) + 0.5f;
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// returns vector of screened pixel values to be written
vector<int> screenAlgorithm(Image file1, Image file2) {
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix=0; pix<area*3; pix++){
        float bottomLayer = (unsigned char) file1.BGRvalues.at(pix);
        float topLayer = (unsigned char) file2.BGRvalues.at(pix);
        float newPixel = 1 - ((1-bottomLayer/255) * (1-topLayer/255));
        newPixel *= 255;
        newPixel += 0.5f;
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// returns vector of the overlayed pixel values to be written
vector<int> overlayAlgorithm(Image bottom_Layer, Image top_Layer) {
    vector<int> newPixelValues;
    int area = bottom_Layer.width * bottom_Layer.height;
    for (int pix=0; pix<area*3; pix++){
        float bottomLayer = (unsigned char) bottom_Layer.BGRvalues.at(pix);
        float topLayer = (unsigned char) top_Layer.BGRvalues.at(pix);
        float newPixel;

        if (bottomLayer/255 <= 0.5f){
            newPixel = 2 * bottomLayer/255 * topLayer/255;
        }
        else{
            newPixel = 2 * (1-bottomLayer/255) * (1-topLayer/255);
            newPixel = 1 - newPixel;
        }
        newPixel *= 255;
        newPixel += 0.5f;
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// returns vector of multiplied pixel values to be written
vector<int> multiplyAlgorithmNoGreen(Image file1, Image file2) {
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix=0; pix<area*3; pix+=3){
        float bottomLayerRed = (unsigned char) file1.BGRvalues.at(pix+2);
        float topLayerRed = (unsigned char) file2.BGRvalues.at(pix+2);
        float bottomLayerGreen = (unsigned char) file1.BGRvalues.at(pix+1);
        float bottomLayerBlue = (unsigned char) file1.BGRvalues.at(pix);
        float topLayerBlue = (unsigned char) file2.BGRvalues.at(pix);

        float newPixel = ((bottomLayerBlue/255 * topLayerBlue) * 255);
        newPixelValues.push_back(newPixel);
        newPixel = bottomLayerGreen;
        newPixelValues.push_back(newPixel);
        newPixel = ((bottomLayerRed/255 * topLayerRed) * 255);
        if (newPixel > 255){newPixel = 255;}
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// scales the blue channel
vector<int> scaleBlue(Image file1, float scaleValue){
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix=0; pix<area*3; pix+=3){
        float redPixel = (unsigned char) file1.BGRvalues.at(pix+2);
        float greenPixel = (unsigned char) file1.BGRvalues.at(pix+1);
        float bluePixel = (unsigned char) file1.BGRvalues.at(pix);

        float newPixel = (bluePixel/255 * scaleValue) * 255;
        if (newPixel > 255){newPixel = 255;}
        newPixelValues.push_back(newPixel);
        newPixel = greenPixel;
        newPixelValues.push_back(newPixel);
        newPixel = redPixel;
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// scales the green channel
vector<int> scaleGreen(Image file1, float scaleValue){
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix=0; pix<area*3; pix+=3){
        float redPixel = (unsigned char) file1.BGRvalues.at(pix+2);
        float greenPixel = (unsigned char) file1.BGRvalues.at(pix+1);
        float bluePixel = (unsigned char) file1.BGRvalues.at(pix);

        float newPixel = bluePixel;
        newPixelValues.push_back(newPixel);
        newPixel = (greenPixel/255 * scaleValue) * 255;
        if (newPixel > 255){newPixel = 255;}
        newPixelValues.push_back(newPixel);
        newPixel = redPixel;
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// scales the red channel
vector<int> scaleRed(Image file1, float scaleValue){
    vector<int> newPixelValues;
    int area = file1.width * file1.height;
    for (int pix=0; pix<area*3; pix+=3){
        float redPixel = (unsigned char) file1.BGRvalues.at(pix+2);
        float greenPixel = (unsigned char) file1.BGRvalues.at(pix+1);
        float bluePixel = (unsigned char) file1.BGRvalues.at(pix);

        float newPixel = bluePixel;
        newPixelValues.push_back(newPixel);
        newPixel = greenPixel;
        newPixelValues.push_back(newPixel);
        newPixel = (redPixel/255 * scaleValue) * 255;
        if (newPixel > 255){newPixel = 255;}
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// adds desired value to the blue channel
vector<int> addBlue(Image file1, int addValue){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int i=0; i<area*3; i+=3){
        auto bottomLayer = (unsigned char) file1.BGRvalues.at(i);
        auto topLayer = (unsigned char) addValue;
        int newPixel = bottomLayer + topLayer;
        if (newPixel > 255){newPixel=255;}
        newPixelValues.push_back(newPixel);
        newPixelValues.push_back(file1.BGRvalues.at(i+1));
        newPixelValues.push_back(file1.BGRvalues.at(i+2));
    }
    return newPixelValues;
}

// adds desired value to the green channel
vector<int> addGreen(Image file1, int addValue){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int i=1; i<area*3; i+=3){
        auto bottomLayer = (unsigned char) file1.BGRvalues.at(i);
        auto topLayer = (unsigned char) addValue;
        int newPixel = bottomLayer + topLayer;
        if (newPixel > 255){newPixel=255;}
        newPixelValues.push_back(file1.BGRvalues.at(i-1));
        newPixelValues.push_back(newPixel);
        newPixelValues.push_back(file1.BGRvalues.at(i+1));
    }
    return newPixelValues;
}

// adds desired value to the red channel
vector<int> addRed(Image file1, int addValue){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int i=2; i<area*3; i+=3){
        auto bottomLayer = (unsigned char) file1.BGRvalues.at(i);
        auto topLayer = (unsigned char) addValue;
        int newPixel = bottomLayer + topLayer;
        if (newPixel > 255){newPixel=255;}
        newPixelValues.push_back(file1.BGRvalues.at(i-2));
        newPixelValues.push_back(file1.BGRvalues.at(i-1));
        newPixelValues.push_back(newPixel);
    }
    return newPixelValues;
}

// splits into only blue channel
vector<int> onlyBlue(Image file1){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int i=0; i<area*3; i+=3){
        newPixelValues.push_back(file1.BGRvalues.at(i));
        newPixelValues.push_back(file1.BGRvalues.at(i));
        newPixelValues.push_back(file1.BGRvalues.at(i));
    }
    return newPixelValues;
}

// splits into only green channel
vector<int> onlyGreen(Image file1){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int i=1; i<area*3; i+=3){
        newPixelValues.push_back(file1.BGRvalues.at(i));
        newPixelValues.push_back(file1.BGRvalues.at(i));
        newPixelValues.push_back(file1.BGRvalues.at(i));
    }
    return newPixelValues;
}

// splits into only red channel
vector<int> onlyRed(Image file1){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int i=2; i<area*3; i+=3){
        newPixelValues.push_back(file1.BGRvalues.at(i));
        newPixelValues.push_back(file1.BGRvalues.at(i));
        newPixelValues.push_back(file1.BGRvalues.at(i));
    }
    return newPixelValues;
}

// flips the image
vector<int> flip(Image file1){
    vector<int> newPixelValues;
    int area = file1.height * file1.width;
    for (int j=area*3-3; j >= 0; j-=3){
        newPixelValues.push_back(file1.BGRvalues.at(j));
        newPixelValues.push_back(file1.BGRvalues.at(j+1));
        newPixelValues.push_back(file1.BGRvalues.at(j+2));
    }
    return newPixelValues;
}

// combines red, green, and blue layers into an image
vector<int> combine(Image blueLayer, Image greenLayer, Image redLayer){
    vector<int> newPixelValues;
    int area = blueLayer.height * blueLayer.width;
    for (int n=0; n<area*3; n++){newPixelValues.push_back(0);}
    for (int b=0; b<area*3; b+=3){newPixelValues.at(b) = blueLayer.BGRvalues.at(b);}
    for (int g=1; g<area*3; g+=3){newPixelValues.at(g) = greenLayer.BGRvalues.at(g);}
    for (int r=2; r<area*3; r+=3){newPixelValues.at(r) = redLayer.BGRvalues.at(r);}
    return newPixelValues;
}



int main() {
    // layer 1 file
    Image layer1{};
    fstream file;
    file.open("layer1.tga", ios_base::in | ios_base::binary);
    if (file.is_open()){
        file.read((char*)(&layer1.idLength), 1);
        file.read((char*)(&layer1.colorMapType), 1);
        file.read((char*)(&layer1.dataTypeCode), 1);
        file.read((char*)(&layer1.colorMapOrigin), 2);
        file.read((char*)(&layer1.colorMapLength), 2);
        file.read((char*)(&layer1.colorMapDepth), 1);
        file.read((char*)(&layer1.xOrigin), 2);
        file.read((char*)(&layer1.yOrigin), 2);
        file.read((char*)(&layer1.width), 2);
        file.read((char*)(&layer1.height), 2);
        file.read((char*)(&layer1.bitsPerPixel), 1);
        file.read((char*)(&layer1.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)layer1.width * (int)layer1.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        layer1.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file.close();
    }

    // car file
    Image car{};
    fstream file2;
    file2.open("car.tga", ios_base::in | ios_base::binary);
    if (file2.is_open()){
        file2.read((char*)(&car.idLength), 1);
        file2.read((char*)(&car.colorMapType), 1);
        file2.read((char*)(&car.dataTypeCode), 1);
        file2.read((char*)(&car.colorMapOrigin), 2);
        file2.read((char*)(&car.colorMapLength), 2);
        file2.read((char*)(&car.colorMapDepth), 1);
        file2.read((char*)(&car.xOrigin), 2);
        file2.read((char*)(&car.yOrigin), 2);
        file2.read((char*)(&car.width), 2);
        file2.read((char*)(&car.height), 2);
        file2.read((char*)(&car.bitsPerPixel), 1);
        file2.read((char*)(&car.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)car.width * (int)car.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file2.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        car.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file2.close();
    }

    // circles file
    Image circles{};
    fstream file3;
    file3.open("circles.tga", ios_base::in | ios_base::binary);
    if (file3.is_open()){
        file3.read((char*)(&circles.idLength), 1);
        file3.read((char*)(&circles.colorMapType), 1);
        file3.read((char*)(&circles.dataTypeCode), 1);
        file3.read((char*)(&circles.colorMapOrigin), 2);
        file3.read((char*)(&circles.colorMapLength), 2);
        file3.read((char*)(&circles.colorMapDepth), 1);
        file3.read((char*)(&circles.xOrigin), 2);
        file3.read((char*)(&circles.yOrigin), 2);
        file3.read((char*)(&circles.width), 2);
        file3.read((char*)(&circles.height), 2);
        file3.read((char*)(&circles.bitsPerPixel), 1);
        file3.read((char*)(&circles.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)circles.width * (int)circles.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file3.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        circles.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file3.close();
    }

    // layer blue file
    Image layer_blue{};
    fstream file4;
    file4.open("layer_blue.tga", ios_base::in | ios_base::binary);
    if (file4.is_open()){
        file4.read((char*)(&layer_blue.idLength), 1);
        file4.read((char*)(&layer_blue.colorMapType), 1);
        file4.read((char*)(&layer_blue.dataTypeCode), 1);
        file4.read((char*)(&layer_blue.colorMapOrigin), 2);
        file4.read((char*)(&layer_blue.colorMapLength), 2);
        file4.read((char*)(&layer_blue.colorMapDepth), 1);
        file4.read((char*)(&layer_blue.xOrigin), 2);
        file4.read((char*)(&layer_blue.yOrigin), 2);
        file4.read((char*)(&layer_blue.width), 2);
        file4.read((char*)(&layer_blue.height), 2);
        file4.read((char*)(&layer_blue.bitsPerPixel), 1);
        file4.read((char*)(&layer_blue.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)layer_blue.width * (int)layer_blue.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file4.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        layer_blue.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file4.close();
    }

    // layer green file
    Image layer_green{};
    fstream file5;
    file5.open("layer_green.tga", ios_base::in | ios_base::binary);
    if (file5.is_open()){
        file5.read((char*)(&layer_green.idLength), 1);
        file5.read((char*)(&layer_green.colorMapType), 1);
        file5.read((char*)(&layer_green.dataTypeCode), 1);
        file5.read((char*)(&layer_green.colorMapOrigin), 2);
        file5.read((char*)(&layer_green.colorMapLength), 2);
        file5.read((char*)(&layer_green.colorMapDepth), 1);
        file5.read((char*)(&layer_green.xOrigin), 2);
        file5.read((char*)(&layer_green.yOrigin), 2);
        file5.read((char*)(&layer_green.width), 2);
        file5.read((char*)(&layer_green.height), 2);
        file5.read((char*)(&layer_green.bitsPerPixel), 1);
        file5.read((char*)(&layer_green.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)layer_green.width * (int)layer_green.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file5.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        layer_green.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file5.close();
    }

    // layer red file
    Image layer_red{};
    fstream file6;
    file6.open("layer_red.tga", ios_base::in | ios_base::binary);
    if (file6.is_open()){
        file6.read((char*)(&layer_red.idLength), 1);
        file6.read((char*)(&layer_red.colorMapType), 1);
        file6.read((char*)(&layer_red.dataTypeCode), 1);
        file6.read((char*)(&layer_red.colorMapOrigin), 2);
        file6.read((char*)(&layer_red.colorMapLength), 2);
        file6.read((char*)(&layer_red.colorMapDepth), 1);
        file6.read((char*)(&layer_red.xOrigin), 2);
        file6.read((char*)(&layer_red.yOrigin), 2);
        file6.read((char*)(&layer_red.width), 2);
        file6.read((char*)(&layer_red.height), 2);
        file6.read((char*)(&layer_red.bitsPerPixel), 1);
        file6.read((char*)(&layer_red.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)layer_red.width * (int)layer_red.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file6.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        layer_red.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file6.close();
    }

    // layer 2 file
    Image layer2{};
    fstream file7;
    file7.open("layer2.tga", ios_base::in | ios_base::binary);
    if (file7.is_open()){
        file7.read((char*)(&layer2.idLength), 1);
        file7.read((char*)(&layer2.colorMapType), 1);
        file7.read((char*)(&layer2.dataTypeCode), 1);
        file7.read((char*)(&layer2.colorMapOrigin), 2);
        file7.read((char*)(&layer2.colorMapLength), 2);
        file7.read((char*)(&layer2.colorMapDepth), 1);
        file7.read((char*)(&layer2.xOrigin), 2);
        file7.read((char*)(&layer2.yOrigin), 2);
        file7.read((char*)(&layer2.width), 2);
        file7.read((char*)(&layer2.height), 2);
        file7.read((char*)(&layer2.bitsPerPixel), 1);
        file7.read((char*)(&layer2.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)layer2.width * (int)layer2.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file7.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        layer2.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file7.close();
    }

    // pattern 1 file
    Image pattern1{};
    fstream file8;
    file8.open("pattern1.tga", ios_base::in | ios_base::binary);
    if (file8.is_open()){
        file8.read((char*)(&pattern1.idLength), 1);
        file8.read((char*)(&pattern1.colorMapType), 1);
        file8.read((char*)(&pattern1.dataTypeCode), 1);
        file8.read((char*)(&pattern1.colorMapOrigin), 2);
        file8.read((char*)(&pattern1.colorMapLength), 2);
        file8.read((char*)(&pattern1.colorMapDepth), 1);
        file8.read((char*)(&pattern1.xOrigin), 2);
        file8.read((char*)(&pattern1.yOrigin), 2);
        file8.read((char*)(&pattern1.width), 2);
        file8.read((char*)(&pattern1.height), 2);
        file8.read((char*)(&pattern1.bitsPerPixel), 1);
        file8.read((char*)(&pattern1.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)pattern1.width * (int)pattern1.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file8.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        pattern1.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file8.close();
    }

    // pattern 2 file
    Image pattern2{};
    fstream file9;
    file9.open("pattern2.tga", ios_base::in | ios_base::binary);
    if (file9.is_open()){
        file9.read((char*)(&pattern2.idLength), 1);
        file9.read((char*)(&pattern2.colorMapType), 1);
        file9.read((char*)(&pattern2.dataTypeCode), 1);
        file9.read((char*)(&pattern2.colorMapOrigin), 2);
        file9.read((char*)(&pattern2.colorMapLength), 2);
        file9.read((char*)(&pattern2.colorMapDepth), 1);
        file9.read((char*)(&pattern2.xOrigin), 2);
        file9.read((char*)(&pattern2.yOrigin), 2);
        file9.read((char*)(&pattern2.width), 2);
        file9.read((char*)(&pattern2.height), 2);
        file9.read((char*)(&pattern2.bitsPerPixel), 1);
        file9.read((char*)(&pattern2.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)pattern2.width * (int)pattern2.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file9.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        pattern2.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file9.close();
    }

    // text file
    Image text{};
    fstream file10;
    file10.open("text.tga", ios_base::in | ios_base::binary);
    if (file10.is_open()){
        file10.read((char*)(&text.idLength), 1);
        file10.read((char*)(&text.colorMapType), 1);
        file10.read((char*)(&text.dataTypeCode), 1);
        file10.read((char*)(&text.colorMapOrigin), 2);
        file10.read((char*)(&text.colorMapLength), 2);
        file10.read((char*)(&text.colorMapDepth), 1);
        file10.read((char*)(&text.xOrigin), 2);
        file10.read((char*)(&text.yOrigin), 2);
        file10.read((char*)(&text.width), 2);
        file10.read((char*)(&text.height), 2);
        file10.read((char*)(&text.bitsPerPixel), 1);
        file10.read((char*)(&text.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)text.width * (int)text.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file10.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        text.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file10.close();
    }

    // text 2 file
    Image text2{};
    fstream file11;
    file11.open("text2.tga", ios_base::in | ios_base::binary);
    if (file11.is_open()){
        file11.read((char*)(&text2.idLength), 1);
        file11.read((char*)(&text2.colorMapType), 1);
        file11.read((char*)(&text2.dataTypeCode), 1);
        file11.read((char*)(&text2.colorMapOrigin), 2);
        file11.read((char*)(&text2.colorMapLength), 2);
        file11.read((char*)(&text2.colorMapDepth), 1);
        file11.read((char*)(&text2.xOrigin), 2);
        file11.read((char*)(&text2.yOrigin), 2);
        file11.read((char*)(&text2.width), 2);
        file11.read((char*)(&text2.height), 2);
        file11.read((char*)(&text2.bitsPerPixel), 1);
        file11.read((char*)(&text2.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)text2.width * (int)text2.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file11.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        text2.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        file11.close();
    }

//// finish reading all files....................................................................

// task 1
    vector<int> newPixelValues1 = multiplyAlgorithm(layer1, pattern1);
    write("part1.tga", layer1, newPixelValues1);

// task 2
    vector<int> newPixelValues2 = subtractAlgorithm(car, layer2);
    write("part2.tga", car, newPixelValues2);

// task 3
    int area = layer1.height * layer1.width;
    vector<int> tempPixelValues = multiplyAlgorithm(layer1, pattern2);
    Image newImagePart3;
    for (int j=0; j<area*3; j++){newImagePart3.BGRvalues.push_back(tempPixelValues.at(j));}
    // create new image using multiple alg and store
    vector<int> newPixelValues3 = screenAlgorithm(text, newImagePart3);
    // using previous results combine with screen alg
    write("part3.tga", layer1, newPixelValues3);

// task 4
    area = layer2.height * layer2.width;
    vector<int> tempPixelValuesPart4 = multiplyAlgorithm(layer2, circles);
    Image newImagePart4;
    for (int l=0; l<area*3; l++){newImagePart4.BGRvalues.push_back(tempPixelValuesPart4.at(l));}
    newImagePart4.height = layer2.height;
    newImagePart4.width = layer2.width;
    // create new image using multiply alg and store
    vector<int> newPixelValues4 = subtractAlgorithm(newImagePart4, pattern2);
    // using previous results combine with screen alg
    write("part4.tga", layer2, newPixelValues4);

// task 5
    area = layer1.height * layer1.width;
    vector<int> newPixelValues5 = overlayAlgorithm(pattern1, layer1);
    write("part5.tga", pattern1, newPixelValues5);

// task 6
    vector<int> newPixelValues6 = addGreen(car, 200);
    write("part6.tga", car, newPixelValues6);

// task 7
    area = car.height * car.width;
    Image newImagePart7;
    newImagePart7.height = car.height;
    newImagePart7.width = car.width;
    vector<int> tempPixelValuesPart7 = scaleRed(car, 4);
    for (int i=0; i<area*3; i++){newImagePart7.BGRvalues.push_back(tempPixelValuesPart7.at(i));}
    vector<int> newPixelValues7 = scaleBlue(newImagePart7, 0);
    write("part7.tga", car, newPixelValues7);

// task 8 blue
    vector<int> newPixelValues8_b = onlyBlue(car);
    write("part8_b.tga", car, newPixelValues8_b);

// task 8 green
    vector<int> newPixelValues8_g = onlyGreen(car);
    write("part8_g.tga", car, newPixelValues8_g);

// part 8 red
    vector<int> newPixelValues8_r = onlyRed(car);
    write("part8_r.tga", car, newPixelValues8_r);

// part 9
    area = layer_blue.height * layer_blue.width;
    vector<int> newPixelValues = combine(layer_blue, layer_green, layer_red);
    write("part9.tga", layer_blue, newPixelValues);

// part 10
    vector<int> newPixelValues10 = flip(text2);
    write("part10.tga", text2, newPixelValues10);

}