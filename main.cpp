#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/opencv.hpp"
#include <cstdio>
#include <iostream>
#include <string>
#include <unistd.h>


using namespace cv;
using namespace std;

int resizeWidth = 300;
int resizeHeight = 90;

int lowerBlue = 100;
int upperBlue = 124;

string fileName = "";
string out = "";

void parseFlag(int, char* argv[]);
void handleImg();
string getHelpInfo();
int comparePoint(Point p1, Point p2);
bool validRect(Point p1, Point p2);
bool validCharRect(Point p1, Point p2);
void removeRivets(Mat *src ,int offset);




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
    string info = "Usage: diyusi [-? value]\n";
    info += "For graduation project only.Thanks ICG ANEKI for your support and love in development.\n";
    info += "\nOptions:\n";
    info += "   -h  Display help info and exit\n";
    info += "   -f  Input file name[required]\n";
    info += "   -o  Output file name\n";
    info += "   -w  License plate final resized width [default 300]\n";
    info += "   -H  License plate final resized height [default 90]\n";
    info += "   -l  HSV to binary, lower blue value [default 100]\n";
    info += "   -u  HSV to binary, upper blue value [default 124]\n";
    return info;
}


void parseFlag(int argc, char* argv[]) {
    char ch;
    while ((ch = getopt(argc, argv, "hw:H:a:b:f:o:l:u:")) != -1)
    {

        switch (ch)
        {
        case 'h':
            cout << getHelpInfo();
            exit(0);
        case 'w':
            cout << "width for resize " << optarg << '\n';
            resizeWidth = atoi(optarg);
            break;
        case 'H':
            cout << "height for resize " << optarg << '\n';
            resizeHeight = atoi(optarg);
            break;
        case 'f':
            cout << "image file name " << optarg << '\n';
            fileName = optarg;
            break;
        case 'o':
            cout << "output file name " << optarg << '\n';
            out = optarg;
            break;
        case 'l':
            cout << "lower blue " << optarg << '\n';
            lowerBlue = atoi(optarg);
            break;
        case 'u':
            cout << "upper blue " << optarg << '\n';
            upperBlue = atoi(optarg);
            break;
        }
    }
}

void handleImg() {
    list<string> toTar;
    Mat raw;
    raw = imread(fileName);
    if (!raw.data) {
        cout << "error:no such file " << fileName << '\n';
        return;
    }
    Mat afterGaus;
    GaussianBlur(raw, afterGaus, Size(3, 3), 0, 0);

    Mat hsv;
    Mat bin;
    cvtColor(afterGaus, hsv, CV_BGR2HSV);
    inRange(hsv, Scalar(lowerBlue, 55, 57), Scalar(upperBlue, 255, 255), bin);
    imwrite("tmp_" + fileName, bin);
    toTar.push_back("tmp_" + fileName);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat bak;
    bin.copyTo(bak);
    findContours(bin, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    Mat imgContours(bin.size(), CV_8U, Scalar(0));
    drawContours(imgContours, contours, -1, Scalar(255));
    cout << contours.size() << " possible contours\n";
    int minX = 0 ,maxX = 0 ,minY = 0 ,maxY = 0;
    bool found = false;
    for (int i = 0; i < contours.size();i++) {
        cout << "parse contour " << i+1 << '\n';
        Point min, max;
        min.x = contours[i][0].x;
        min.y = contours[i][0].y;
        max.x = contours[i][0].x;
        max.y = contours[i][0].y;
        for (int j = 0; j < contours[i].size();j++) {
            if (comparePoint(min, contours[i][j]) > 0) {
                min.x = contours[i][j].x;
                min.y = contours[i][j].y;
            }
            if (comparePoint(max, contours[i][j]) < 0) {
                max.x = contours[i][j].x;
                max.y = contours[i][j].y;
            }
        }
        cout << "max (x ,y) -> (" << max.x << ',' << max.y << ")\n";
        cout << "min (x ,y) -> (" << min.x << ',' << min.y << ")\n";    
        if (validRect(max ,min)) {
            cout << "pass rect validation\n";
            minX = min.x;
            minY = min.y;
            maxX = max.x;
            maxY = max.y;
            found = true;
            break;
        }
    }
    if (!found) {
        cout << "no rectangular contour found\n";
        return;
    }

    bak = 255 - bak;
    Mat plate = bak(Range(minY ,maxY) ,Range(minX ,maxX));
    Mat resizedPlate;
    resize(plate, resizedPlate, Size(resizeWidth, resizeHeight));
    removeRivets(&resizedPlate ,10);
    
    Mat output[7];
    output[0] = resizedPlate(Range(10, 82), Range(5, 45));
    output[1] = resizedPlate(Range(10, 82), Range(45, 90));
    output[2] = resizedPlate(Range(10, 82), Range(95, 140));
    output[3] = resizedPlate(Range(10, 82), Range(140, 185));
    output[4] = resizedPlate(Range(10, 82), Range(185, 220));
    output[5] = resizedPlate(Range(10, 82), Range(220, 260));
    output[6] = resizedPlate(Range(10, 82), Range(260, 299));

    string plateFileName = "plate_" + fileName;
    imwrite(plateFileName, resizedPlate);
    toTar.push_back(plateFileName);
    for (int i = 0; i < 7;i++) {
        string chname = "char_" + to_string(i) + "_" + fileName;
        imwrite(chname, output[i]);
        toTar.push_back(chname);
    }

    list<string>::iterator it;
    string cmd = "tar -cf output.tar ";
    for (it = toTar.begin(); it != toTar.end(); it++) {
        cmd += *it + " ";
    }
    system(cmd.c_str());
    for (it = toTar.begin(); it != toTar.end(); it++) {
        cmd = "rm -f " + *it;
        system(cmd.c_str());
    }
    return;
}










void removeRivets(Mat *src ,int offset) {
    int maxX = src->cols;
    int maxY = src->rows;
    for (int y = 0; y < offset;y++) {
        for (int x = 0; x < maxX;x++) {
            src->at<uchar>(y, x) = 0;
        }
    }

    for (int y = maxY-1; y > maxY - 1 - offset;y--) {
        for (int x = 0; x < maxX; x++) {
            src->at<uchar>(y, x) = 0;
        }
    }

    for (int x = 0; x < 5;x++) {
        for (int y = 0; y < maxY;y++) {
            src->at<uchar>(y, x) = 0;
        }
    }

    for (int x = maxX-1; x > maxX - 6; x--) {
        for (int y = 0; y < maxY; y++) {
            src->at<uchar>(y, x) = 0;
        }
    }
}


int comparePoint(Point p1, Point p2) {
    int delta = p1.x * p1.x + p1.y * p1.y - (p2.x * p2.x + p2.y * p2.y);
    if (delta > 0) {
        return 1;
    }
    else if (delta < 0) {
        return -1;
    }
    return 0;
}


bool validRect(Point p1, Point p2) {
    int deltaX = abs(p1.x - p2.x);
    int deltaY = abs(p1.y - p2.y);
    return deltaX > 100 && deltaY > 40 && deltaX > deltaY;
}

bool validCharRect(Point p1, Point p2) {
    int deltaX = abs(p1.x - p2.x);
    int deltaY = abs(p1.y - p2.y);
    return deltaX > 20 && deltaY > 47 && deltaY > deltaX;
}
