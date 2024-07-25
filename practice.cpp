#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

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
vector<Image> imageList;
vector<string> nameList;
bool fileExist = true;

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

// writes the new file if the list is a char
void writeChar(const string& fileName, Image fileHeader, const vector<unsigned char>& newPixelValues) {
    fstream newFile;
    newFile.open(fileName, ios_base::out | ios_base::binary | ios_base::trunc);
    if (newFile.is_open()) {
        newFile.write(&fileHeader.idLength, 1);
        newFile.write(&fileHeader.colorMapType, 1);
        newFile.write(&fileHeader.dataTypeCode, 1);
        newFile.write((char*) (&fileHeader.colorMapOrigin), 2);
        newFile.write((char*) (&fileHeader.colorMapLength), 2);
        newFile.write(&fileHeader.colorMapDepth, 1);
        newFile.write((char*) (&fileHeader.xOrigin), 2);
        newFile.write((char*) (&fileHeader.yOrigin), 2);
        newFile.write((char*) (&fileHeader.width), 2);
        newFile.write((char*) (&fileHeader.height), 2);
        newFile.write(&fileHeader.bitsPerPixel, 1);
        newFile.write(&fileHeader.imageDescriptor, 1);
        // write the struct data
        int area = fileHeader.height * fileHeader.width;
        for (int i=0; i < area*3; i++){
            newFile.write((char*)&newPixelValues.at(i), 1);
        }
    }
    else {cout << "could not open file write function" << endl << "file that could not be opened: " << fileName << endl;}
}

// reads new file and pushes it back into Image vector
Image read(const string& fileName){
    Image newImage{};
    fstream file;
    file.open(fileName, ios_base::in | ios_base::binary);
    if (file.is_open()){
        file.read((char*)(&newImage.idLength), 1);
        file.read((char*)(&newImage.colorMapType), 1);
        file.read((char*)(&newImage.dataTypeCode), 1);
        file.read((char*)(&newImage.colorMapOrigin), 2);
        file.read((char*)(&newImage.colorMapLength), 2);
        file.read((char*)(&newImage.colorMapDepth), 1);
        file.read((char*)(&newImage.xOrigin), 2);
        file.read((char*)(&newImage.yOrigin), 2);
        file.read((char*)(&newImage.width), 2);
        file.read((char*)(&newImage.height), 2);
        file.read((char*)(&newImage.bitsPerPixel), 1);
        file.read((char*)(&newImage.imageDescriptor), 1);
        // read image header
        vector<unsigned char> BGRvalues;
        int area = (int)newImage.width * (int)newImage.height;
        char BGR;
        for (int i=0; i < area*3; i++){
            file.read((char*)(&BGR), 1);
            BGRvalues.push_back(BGR);
        }
        newImage.BGRvalues = BGRvalues;
        // read and push the pixel data into a vector
        imageList.push_back(newImage);
        nameList.push_back(fileName);
        file.close();
        return newImage;
    }
    else{cout << "File does not exist." << endl; fileExist = false;}
}

// find image index
Image findIndex(const string& name){
    for (int i=0; i<nameList.size(); i++){
        if (name == nameList.at(i)){return imageList.at(i);}
        if (i+1 == nameList.size()){cout << "could not find index" << endl;}
    }
}

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
        int bottomLayer = (unsigned char) file1.BGRvalues.at(i);
        int topLayer = (unsigned char) addValue;
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
vector<int> combine(Image redLayer, Image greenLayer, Image blueLayer){
    vector<int> newPixelValues;
    int area = blueLayer.height * blueLayer.width;
    for (int n=0; n<area*3; n++){newPixelValues.push_back(0);}
    for (int b=0; b<area*3; b+=3){newPixelValues.at(b) = blueLayer.BGRvalues.at(b);}
    for (int g=1; g<area*3; g+=3){newPixelValues.at(g) = greenLayer.BGRvalues.at(g);}
    for (int r=2; r<area*3; r+=3){newPixelValues.at(r) = redLayer.BGRvalues.at(r);}
    return newPixelValues;
}

// checks if the method given is a valid method
bool checkValidMethod(const string& method){
    if (method == "multiply" || method == "subtract" || method == "overlay" || method == "screen"
        || method == "combine" || method == "addred" || method == "addgreen" || method == "addblue" || method == "scalered"
        || method == "scalegreen" || method == "scaleblue" || method == "onlyred" || method == "onlygreen"
        || method == "onlyblue" || method == "flip"){return true;}
    else{return false;}
}

// returns true if file exists
bool fileExistF(const string& fileName){
    fstream file;
    file.open(fileName, ios_base::in);
    if (file.is_open()){return true;}
    else{return false;}
}




int main(int argc, char* argv[]) {
    if (argc < 2 || (strcmp(argv[1], "--help") == 0)) {
        cout << "Project 2: Image Processing, Spring 2023" << endl << endl;
        cout << "Usage: ./project2.out [output] [firstImage] [method] [...]" << endl;
        return 0;
    }

    string arg1 = argv[1];
    if (arg1.length() >= 4 && arg1.substr(arg1.length() - 4) != ".tga") {
        cout << "Invalid file name." << endl;
        return 0;
    }
    else if (arg1.length() < 4) {
        cout << "Invalid file name.";
        return 0;
    }
    // if file does not end in .tga print "invalid file name"

    if (argc == 2) {
        cout << "Invalid file name." << endl;
        return 0;
    }
    string arg2 = argv[2];
    if (arg2.length() >= 4 && arg2.substr(arg2.length() - 4) != ".tga") {
        cout << "Invalid file name." << endl;
        return 0;
    }
    else if (arg2.length() < 4) {
        cout << "Invalid file name.";
        return 0;
    }
    read(arg2);
    if (!fileExist) { return 0; }
    fileExist = true;
    // if file does not end in .tga print "invalid file name"
    // if file does not exist print "file does not exist

    if (argc == 3) {
        cout << "Invalid method name." << endl;
        return 0;
    }
    string arg3 = argv[3];
    bool isValid = checkValidMethod(arg3);
    if (!isValid) {
        cout << "Invalid method name." << endl;
        return 0;
    }
    // if not a valid method name or not provided print "invalid method name"



    string method;
    string outputFileName;
    string inputFile1;
    int index;
    // define values that will be used for manipulations

    if (argc >= 4) {
        outputFileName = argv[1];
        inputFile1 = argv[2];
        method = argv[3];
        index = 4;
    } else {
        cout << "Invalid number of args" << endl;
        return 0;
    }
    // make sure that every argument exists



    // handles all exceptions for first operation
    if (method == "screen" || method == "subtract" || method == "multiply" || method == "overlay") {
        if (argc < 5) {
            cout << "Missing argument." << endl;
            return 0;
        }
        string arg4 = argv[4];
        if (arg4.length() >= 4 && arg4.substr(arg4.length() - 4) != ".tga") {
            cout << "Invalid argument, invalid file name." << endl;
            return 0;
        }
        if (arg4.length() < 4) {
            cout << "Invalid argument, invalid file name." << endl;
            return 0;
        }
        bool doesFileExist = fileExistF(arg4);
        if (!doesFileExist) {
            cout << "Invalid argument, file does not exist." << endl;
            return 0;
        }
    }
        // if method is one of the algorithms and is not a valid name, print that it's not valid

    else if (method == "addred" || method == "addgreen" || method == "addblue" || method == "scalered" ||
             method == "scalegreen" || method == "scaleblue") {
        if (argc < 5) {
            cout << "Missing argument." << endl;
            return 0;
        }
        string arg4 = argv[4];
        try { int arg4Int = stoi(arg4); }
        catch (...) {
            cout << "Invalid argument, expected number." << endl;
            return 0;
        }
    }
        // if method is one of the add or scale algorithms and is not a valid name, print that it's not valid

    else if (method == "combine") {
        if (argc < 6) {
            cout << "Missing argument." << endl;
            return 0;
        }
        string arg4 = argv[4];
        if (arg4.length() >= 4 && arg4.substr(arg4.length() - 4) != ".tga") {
            cout << "Invalid argument, invalid file name." << endl;
            return 0;
        }
        if (arg4.length() < 4) {
            cout << "Invalid argument, invalid file name." << endl;
            return 0;
        }
        bool doesFileExist = fileExistF(arg4);
        if (!doesFileExist) {
            cout << "Invalid argument, file does not exist." << endl;
            return 0;
        }
        string arg5 = argv[5];
        if (arg5.length() >= 4 && arg5.substr(arg5.length() - 4) != ".tga") {
            cout << "Invalid argument, invalid file name." << endl;
            return 0;
        }
        if (arg5.length() < 4) {
            cout << "Invalid argument, invalid file name." << endl;
            return 0;
        }
        bool doesFileExist2 = fileExistF(arg5);
        if (!doesFileExist2) {
            cout << "Invalid argument, file does not exist." << endl;
            return 0;
        }
    }
        // if method is one of the algorithms and is not a valid name, print that it's not valid

    else if (method == "onlyred" || method == "onlygreen" || method == "onlyblue" || method == "flip") {}
    else {cout << "Invalid method name" << endl; return 0;}

// begin performing first operation
    vector<int> newPixelValues;
    if (method == "multiply") {
        string inputFile2 = argv[4];
        cout << "Multiplying " << inputFile1 << " and " << inputFile2 << " ..." << endl;
        Image firstImage = read(inputFile1);
        Image secondImage = read(inputFile2);
        newPixelValues = multiplyAlgorithm(firstImage, secondImage);
        index++;
    } else if (method == "subtract") {
        string inputFile2 = argv[4];
        cout << "Subtracting " << inputFile1 << " from " << inputFile2 << " ..." << endl;
        Image firstImage = read(inputFile1);
        Image secondImage = read(inputFile2);
        newPixelValues = subtractAlgorithm(firstImage, secondImage);
        index++;
    } else if (method == "screen") {
        string inputFile2 = argv[4];
        cout << "Screening " << inputFile1 << " and " << inputFile2 << " ..." << endl;
        Image firstImage = read(inputFile1);
        Image secondImage = read(inputFile2);
        newPixelValues = screenAlgorithm(firstImage, secondImage);
        index++;
    } else if (method == "overlay") {
        string inputFile2 = argv[4];
        cout << "Overlaying " << inputFile1 << " and " << inputFile2 << " ..." << endl;
        Image firstImage = read(inputFile1);
        Image secondImage = read(inputFile2);
        newPixelValues = overlayAlgorithm(firstImage, secondImage);
        index++;
    } else if (method == "scaleblue") {
        int scaleFactor = stoi(argv[4]);
        cout << "Scaling the blue channel by " << scaleFactor << " ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = scaleBlue(firstImage, scaleFactor);
        index++;
    } else if (method == "scalegreen") {
        int scaleFactor = stoi(argv[4]);
        cout << "Scaling the green channel by " << scaleFactor << " ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = scaleGreen(firstImage, scaleFactor);
        index++;
    } else if (method == "scalered") {
        int scaleFactor = stoi(argv[4]);
        cout << "Scaling the red channel by " << scaleFactor << " ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = scaleRed(firstImage, scaleFactor);
        index++;
    } else if (method == "addblue") {
        int addFactor = stoi(argv[4]);
        cout << "Adding +" << addFactor << " to the blue channel ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = addBlue(firstImage, addFactor);
        index++;
    } else if (method == "addgreen") {
        int addFactor = stoi(argv[4]);
        cout << "Adding +" << addFactor << " to the green channel ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = addGreen(firstImage, addFactor);
        index++;
    } else if (method == "addred") {
        int addFactor = stoi(argv[4]);
        cout << "Adding +" << addFactor << " to the red channel ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = addRed(firstImage, addFactor);
        index++;
    } else if (method == "onlyblue") {
        cout << "only blue" << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = onlyBlue(firstImage);
        index++;
    } else if (method == "onlygreen") {
        cout << "only green" << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = onlyGreen(firstImage);
        index++;
    } else if (method == "onlyred") {
        cout << "only red" << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = onlyRed(firstImage);
        index++;
    } else if (method == "combine") {
        string secondFileName = argv[4];
        string thirdFileName = argv[5];
        cout << "Combining channels of " << inputFile1 << ", " << secondFileName << ", and " << thirdFileName << " ..." << endl;
        Image firstImage = read(inputFile1);
        Image secondImage = read(secondFileName);
        Image thirdImage = read(thirdFileName);
        newPixelValues = combine(firstImage, secondImage, thirdImage);
        index += 2;
    } else if (method == "flip") {
        cout << "Flipping" << inputFile1 << " ..." << endl;
        Image firstImage = read(inputFile1);
        newPixelValues = flip(firstImage);
    } else {
        cout << "Invalid function" << endl;
    }
    Image newImage = read(inputFile1);
    int area = newImage.width * newImage.height;
    for (int i = 0; i < area * 3; i++) {
        newImage.BGRvalues.at(i) = newPixelValues.at(i);
    }
    // saves the image into a new image
    //// finish first 3 arguments which are always the same..........................................

    while (index < argc) {
        string newMethod = argv[index];

        // handles all exceptions for first operation
        if (newMethod == "screen" || newMethod == "subtract" || newMethod == "multiply" || newMethod == "overlay") {
            string argAfterMethod;
            cout << argc << endl; cout << index << endl;
            try{argAfterMethod = argv[index + 1];}
            catch(...){cout << "Missing argument." << endl; return 0;}
            if (argAfterMethod.length() >= 4 && argAfterMethod.substr(argAfterMethod.length() - 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            if (argAfterMethod.length() < 4) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            bool doesFileExist = fileExistF(argAfterMethod);
            if (!doesFileExist) {
                cout << "Invalid argument, file does not exist." << endl;
                return 0;
            }
        }
            // if method is one of the algorithms and is not a valid name, print that it's not valid

        else if (newMethod == "addred" || newMethod == "addgreen" || newMethod == "addblue" || newMethod == "scalered" ||
                 newMethod == "scalegreen" || newMethod == "scaleblue") {
            string argAfterMethod;
            try{argAfterMethod = argv[index + 1];}
            catch(...){cout << "Missing argument." << endl; return 0;}
            try {int newArgScale = stoi(argAfterMethod); }
            catch (...) {
                cout << "Invalid argument, expected number." << endl;
                return 0;
            }
        }
            // if method is one of the add or scale algorithms and is not a valid name, print that it's not valid

        else if (newMethod == "combine") {
            string argAfterMethod;
            try{argAfterMethod = argv[index + 1];}
            catch(...){cout << "Missing argument." << endl; return 0;}
            string argAfterMethod2;
            try{argAfterMethod2 = argv[index + 2];}
            catch(...){cout << "Missing argument." << endl; return 0;}

            if (argAfterMethod.length() >= 4 && argAfterMethod.substr(argAfterMethod.length() - 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            if (argAfterMethod.length() < 4) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            bool doesFileExist = fileExistF(argAfterMethod);
            if (!doesFileExist) {
                cout << "Invalid argument, file does not exist." << endl;
                return 0;
            }
            if (argAfterMethod2.length() >= 4 && argAfterMethod2.substr(argAfterMethod2.length() - 4) != ".tga") {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            if (argAfterMethod2.length() < 4) {
                cout << "Invalid argument, invalid file name." << endl;
                return 0;
            }
            bool doesFileExist2 = fileExistF(argAfterMethod2);
            if (!doesFileExist2) {
                cout << "Invalid argument, file does not exist." << endl;
                return 0;
            }
        }
            // if method is one of the algorithms and is not a valid name, print that it's not valid

        else if (newMethod == "onlyred" || newMethod == "onlygreen" || newMethod == "onlyblue" || newMethod == "flip") {}
        else {cout << "Invalid method name" << endl; return 0;}

        //// end fixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

        if (newMethod == "flip") {
            cout << "flipping again" << endl;
            newPixelValues = flip(newImage);
            index++;
        } else if (newMethod == "onlyblue") {
            cout << "only blue" << endl;
            newPixelValues = onlyBlue(newImage);
            index++;
        } else if (newMethod == "onlygreen") {
            cout << "only green" << endl;
            vector<int> newPixelValues = onlyGreen(newImage);
            index++;
        } else if (newMethod == "onlyred") {
            cout << "only red" << endl;
            newPixelValues = onlyRed(newImage);
            index++;
        } else if (newMethod == "multiply") {
            cout << "multiplying another time" << endl;
            string newInputFile1 = argv[index + 1];
            Image newInputImage1 = read(newInputFile1);
            newPixelValues = multiplyAlgorithm(newImage, newInputImage1);
            index += 2;
        } else if (newMethod == "subtract") {
            cout << "subtracting again" << endl;
            string newInputFile1 = argv[index + 1];
            Image newInputImage1 = read(newInputFile1);
            newPixelValues = subtractAlgorithm(newImage, newInputImage1);
            index += 2;
        } else if (newMethod == "screen") {
            cout << "screening again" << endl;
            string newInputFile1 = argv[index + 1];
            Image newInputImage1 = read(newInputFile1);
            newPixelValues = screenAlgorithm(newImage, newInputImage1);
            index += 2;
        } else if (newMethod == "overlay") {
            cout << "overlaying again" << endl;
            string newInputFile1 = argv[index + 1];
            Image newInputImage1 = read(newInputFile1);
            newPixelValues = overlayAlgorithm(newImage, newInputImage1);
            index += 2;
        } else if (newMethod == "scaleblue") {
            cout << "scaling blue again" << endl;
            int scaleFactor = stoi(argv[index + 1]);
            newPixelValues = scaleBlue(newImage, scaleFactor);
            index += 3;
        } else if (newMethod == "scalegreen") {
            cout << "scaling green again" << endl;
            int scaleFactor = stoi(argv[index + 1]);
            newPixelValues = scaleGreen(newImage, scaleFactor);
            index += 3;
        } else if (newMethod == "scalered") {
            cout << "scaling red again" << endl;
            int scaleFactor = stoi(argv[index + 1]);
            newPixelValues = scaleRed(newImage, scaleFactor);
            index += 3;
        } else if (newMethod == "addblue") {
            cout << "adding blue again" << endl;
            int addFactor = stoi(argv[index + 1]);
            newPixelValues = addBlue(newImage, addFactor);
            index += 3;
        } else if (newMethod == "addgreen") {
            cout << "adding green again" << endl;
            int addFactor = stoi(argv[index + 1]);
            newPixelValues = addGreen(newImage, addFactor);
            index += 3;
        } else if (newMethod == "addred") {
            cout << "adding red again" << endl;
            int addFactor = stoi(argv[index + 1]);
            newPixelValues = addRed(newImage, addFactor);
            index += 3;
        } else if (newMethod == "combine") {
            cout << "combining again" << endl;
            string secondFileName = argv[index + 1];
            string thirdFileName = argv[index + 2];
            Image secondImage = read(secondFileName);
            Image thirdImage = read(thirdFileName);
            newPixelValues = combine(newImage, secondImage, thirdImage);
            index += 3;
        }
        for (int j = 0; j < area * 3; j++) { newImage.BGRvalues.at(j) = newPixelValues.at(j); }
    }
    write(outputFileName, newImage, newPixelValues);
    return 0;


}
