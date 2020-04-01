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
int upperBlue = 135;

string fileName = "";
string out = "";

void parseFlag(int, char* argv[]);
void handleImg();
string getHelpInfo();
int comparePoint(Point p1, Point p2);
bool validRect(Point p1, Point p2);
bool validCharRect(Point p1, Point p2);




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
    info += "   -u  HSV to binary, upper blue value [default 135]\n";
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
    inRange(hsv, Scalar(lowerBlue, 50, 50), Scalar(upperBlue, 255, 255), bin);
    imwrite("tmp_" + fileName, bin);
    cout << "save tmp binary img as tmp_" << fileName << '\n';


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

    vector<vector<Point> > plateContours;
    vector<Vec4i> plateHierarchy;
    Mat resizePlatebak;
    resizedPlate.copyTo(resizePlatebak);
    findContours(resizedPlate, plateContours, plateHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
    cout << plateContours.size() << " possible contours in plate\n";
    vector<vector<Point> > charRect;
    for (int i = 0; i < plateContours.size();i++) {
        Rect rect = boundingRect(plateContours[i]);
        Point l;
        Point r;
        l = rect.tl();
        r = rect.br();
        line(resizePlatebak, l, r, Scalar(255), 1);
        if (validCharRect(l ,r)) {
            vector<Point> tmp;
            tmp.push_back(l);
            tmp.push_back(r);
            charRect.push_back(tmp);
        }
    }
    cout << charRect.size() << " pass char rect validation\n";
    if (charRect.size() != 6 && charRect.size() != 7) {
        cout << "error:char rect number must be 6 or 7\n";
        return;
    }

    string plateFileName = "plate_" + fileName;
    imwrite(plateFileName, resizePlatebak);
    cout << "save license plate cut as " << plateFileName << '\n';
    return;
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
    return deltaX > 100 && deltaY > 50 && deltaX > deltaY;
}

bool validCharRect(Point p1, Point p2) {
    int deltaX = abs(p1.x - p2.x);
    int deltaY = abs(p1.y - p2.y);
    return deltaX > 20 && deltaY > 47 && deltaY > deltaX;
}
