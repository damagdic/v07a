#include "main.h"
#include "rc.h"
#include <filesystem>
#include <commdlg.h>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;
namespace fs = std::filesystem;

void main_window::on_paint(HDC hdc)
{
    Gdiplus::Graphics graphics(hdc);

    Gdiplus::SolidBrush whiteBrush(Color(255, 255, 255));
    int width = GetClientWidth();
    int height = GetClientHeight();
    graphics.FillRectangle(&whiteBrush, 0, 0, width, height);

    if (image) {
        graphics.DrawImage(image.get(), 0, 0, width, height);

        Gdiplus::Font font(L"Arial", 16);
        Gdiplus::SolidBrush shadowBrush(Color(255, 0, 0, 0));
        Gdiplus::SolidBrush textBrush(Color(255, 255, 255, 255));

        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);
        format.SetLineAlignment(Gdiplus::StringAlignmentFar);

        float x = width / 2.0f;
        float y = static_cast<float>(height - 20);

        Gdiplus::PointF shadowPoint(x + 1, y + 1);
        graphics.DrawString(file_name.c_str(), -1, &font, shadowPoint, &format, &shadowBrush);

        Gdiplus::PointF textPoint(x, y);
        graphics.DrawString(file_name.c_str(), -1, &font, textPoint, &format, &textBrush);
    }
}

void main_window::on_command(int id)
{
    switch (id)
    {
    case ID_OPEN: {
        TCHAR filename[MAX_PATH] = {};
        OPENFILENAME ofn = {};
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = *this;
        ofn.lpstrFile = filename;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrFilter = _T("Images\0*.jpg;*.jpeg;*.png;*.bmp;*.gif;*.tif;*.tiff;*.emf\0All files\0*.*\0");
        ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

        if (GetOpenFileName(&ofn)) {
            image = std::make_unique<Gdiplus::Image>(filename);

            if (image && image->GetLastStatus() == Ok) {
                file_name = fs::path(filename).filename().wstring();
                InvalidateRect(*this, nullptr, TRUE);
            }
            else {
                image.reset();
            }
        }
        break;
    }
    case ID_EXIT:
        DestroyWindow(*this);
        break;
    }
}

void main_window::on_destroy()
{
    image.reset();
    ::PostQuitMessage(0);
}

int main_window::GetClientHeight()
{
    RECT rc;
    GetClientRect(*this, &rc);
    return rc.bottom - rc.top;
}

int main_window::GetClientWidth() {
    RECT rc;
    GetClientRect(*this, &rc);
    return rc.right - rc.left;
}

bool main_window::on_erase_bkgnd(HDC)
{
    return true;
}

int WINAPI _tWinMain(HINSTANCE instance, HINSTANCE, LPTSTR, int)
{
    gdiplus_application app;
    main_window wnd;
    wnd.create(0, WS_OVERLAPPEDWINDOW | WS_VISIBLE, _T("NWP 7"), (int64_t)LoadMenu(instance, MAKEINTRESOURCE(IDM_MAIN)));
    return app.run();
}
