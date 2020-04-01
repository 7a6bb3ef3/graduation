#include "opencv2/opencv.hpp"
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>


using namespace cv;
using namespace std;

int resizeWidth = 500;
int resizeHeight = 500;
double thresholdA = 100;
double thresholdB = 200;

int lowerBlue = 110;
int upperBlue = 135;

string fileName = "";
string out = "";

void parseFlag(int, char* argv[]);
void handleImg();
string getHelpInfo();



int main(int argc, char* argv[]) {
    if (argc == 1) {
        cout << "no picture specified ,use -f to set .system exit\n";
        return 0;
    }
    parseFlag(argc, argv);
    if (fileName.compare("") == 0) {
        cout << "empty file name";
        return 0;
    }
    handleImg();
    return 0;
}


string getHelpInfo() {
    string info = "Usage of diyusi:\n";
    info += "   -h  show help info\n";
    info += "   -f  input file name[required]\n";
    info += "   -o  output file name\n";
    info += "   -w  resize width[default 500]\n";
    info += "   -H  resize height[default 500]\n";
    info += "   -l  HSV mode convert to gray, lower blue value [default 110]\n";
    info += "   -u  HSV mode convert to gray, upper blue value [default 135]\n";
    info += "   -a  canny threshold paramA[default 100]\n";
    info += "   -b  canny threshold paramB[default 200]\n";
}


void parseFlag(int argc, char* argv[]){
    char ch;
    while ((ch = getopt(argc, argv, "hw:H:a:b:f:o:l:u:")) != -1)
    {

        switch (ch)
        {
        case 'h':
            cout << "help info\n";
            exit(0);
        case 'w':
            cout << "set width for resize " << optarg << '\n';
            resizeWidth = atoi(optarg);
            break;
        case 'H':
            cout << "set height for resize " << optarg << '\n';
            resizeWidth = atoi(optarg);
            break;
        case 'a':
            cout << "set threshold 1 " << optarg << '\n';
            thresholdA = atoi(optarg);
            break;
        case 'b':
            cout << "set threshold 2 " << optarg << '\n';
            thresholdB = atoi(optarg);
            break;
        case 'f':
            cout << "set image file name " << optarg << '\n';
            fileName = optarg;
            break;
        case 'o':
            cout << "set output file name " << optarg << '\n';
            out = optarg;
            break;
        case 'l':
            cout << "set lower blue " << optarg << '\n';
            lowerBlue = atoi(optarg);
            break;
        case 'u':
            cout << "set upper blue " << optarg << '\n';
            upperBlue = atoi(optarg);
            break;
        }
    }
}

void handleImg() {
    Mat raw;
    raw = imread(fileName);
    if (!raw.data) {
        cout << "no such file " << fileName << '\n';
        return;
    }
    Mat resizeOut;
    resize(raw, resizeOut, Size(resizeWidth, resizeHeight));

    Mat hsv;
    Mat gray;
    cvtColor(resizeOut ,hsv ,CV_BGR2HSV);
    inRange(hsv , Scalar(lowerBlue, 50, 50) ,Scalar(upperBlue ,255 ,255) , gray);

    Mat dst;
    Canny(gray, dst, thresholdA, thresholdB);

    if (out.compare("") == 0) {
        imwrite("out_" + fileName, dst);
    }
    else {
        imwrite(out, dst);
    }
    return;
}
