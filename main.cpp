#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>

using namespace std;

class BMPImage {
private:
    BITMAPFILEHEADER fileHeader;
    BITMAPINFOHEADER infoHeader;
    unsigned char* imageData;
    ifstream fileStream;

public:
    bool openBMP(const string& fileName) {
        fileStream.open(fileName, ios::binary);
        if (!fileStream.is_open()) {
            return false;
        }

        fileStream.read((char*)&fileHeader, sizeof(fileHeader));
        fileStream.read((char*)&infoHeader, sizeof(infoHeader));

        if (fileHeader.bfType != 0x4D42 || (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32)) {
            return false;
        }

        int imageSize = infoHeader.biWidth * infoHeader.biHeight * (infoHeader.biBitCount / 8);
        imageData = new unsigned char[imageSize];
        fileStream.seekg(fileHeader.bfOffBits, ios::beg);
        fileStream.read((char*)imageData, imageSize);

        return true;
    }

    void displayBMP() {
        HWND hwnd = CreateWindowExW(
            0,
            L"STATIC",
            L"",
            WS_OVERLAPPEDWINDOW | SS_BITMAP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,
            NULL,
            GetModuleHandle(NULL),
            NULL
        );

        if (hwnd == NULL) {
            cout << "Не удалось создать окно." << endl;
            return;
        }

        HDC hdc = GetDC(hwnd);
        HBITMAP hBitmap = CreateBitmap(
            infoHeader.biWidth,
            infoHeader.biHeight,
            1,
            infoHeader.biBitCount,
            imageData
        );

        HDC hdcMem = CreateCompatibleDC(hdc);
        SelectObject(hdcMem, hBitmap);

        BITMAP bitmap;
        GetObject(hBitmap, sizeof(bitmap), &bitmap);
        BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        DeleteDC(hdcMem);
        DeleteObject(hBitmap);
        ReleaseDC(hwnd, hdc);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void closeBMP() {
        if (imageData != nullptr) {
            delete[] imageData;
            imageData = nullptr;
        }
        if (fileStream.is_open()) {
            fileStream.close();
        }
    }
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");
    if (argc != 2) {
        cout << "Неправильное количество аргументов. Используйте: drawBmp.exe <путь к файлу>" << endl;
        return 1;
    }

    BMPImage image;
    if (!image.openBMP(argv[1])) {
        cout << "Не удалось открыть файл: " << argv[1] << endl;
        return 1;
    }

    image.displayBMP();
    image.closeBMP();

    return 0;
}
