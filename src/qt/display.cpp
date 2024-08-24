/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/display.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/display.h"
#include "wx/private/display.h"
#include <QScreen>
#include "wx/qt/private/converter.h"
#include <QRect>
#include <QApplication>
#include <qguiapplication.h>

class wxDisplayImplQt : public wxDisplayImpl
{
public:
    wxDisplayImplQt( unsigned n );

    virtual wxRect GetGeometry() const wxOVERRIDE;
    virtual wxRect GetClientArea() const wxOVERRIDE;
    virtual int GetDepth() const wxOVERRIDE;

#if wxUSE_DISPLAY
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const wxOVERRIDE;
    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE;
    virtual bool ChangeMode(const wxVideoMode& mode) wxOVERRIDE;
#endif // wxUSE_DISPLAY
};

wxDisplayImplQt::wxDisplayImplQt( unsigned n )
    : wxDisplayImpl( n )
{
}

wxRect wxDisplayImplQt::GetGeometry() const
{
    return wxQtConvertRect( QApplication::primaryScreen()->geometry());
}

wxRect wxDisplayImplQt::GetClientArea() const
{
    return wxQtConvertRect( QApplication::primaryScreen()->geometry());
}

int wxDisplayImplQt::GetDepth() const
{
    return IsPrimary() ? QApplication::primaryScreen()->depth() : 0;
}

#if wxUSE_DISPLAY
wxArrayVideoModes wxDisplayImplQt::GetModes(const wxVideoMode& WXUNUSED(mode)) const
{
    return wxArrayVideoModes();
}

wxVideoMode wxDisplayImplQt::GetCurrentMode() const
{
    int width = QGuiApplication::primaryScreen()->geometry().width();
    int height = QGuiApplication::primaryScreen()->geometry().height();
    int depth = QGuiApplication::primaryScreen()->depth();

    return wxVideoMode( width, height, depth );
}

bool wxDisplayImplQt::ChangeMode(const wxVideoMode& WXUNUSED(mode))
{
    return false;
}
#endif // wxUSE_DISPLAY


//##############################################################################

#if wxUSE_DISPLAY

class wxDisplayFactoryQt : public wxDisplayFactory
{
public:
    virtual wxDisplayImpl *CreateDisplay(unsigned n) wxOVERRIDE;
    virtual unsigned GetCount() wxOVERRIDE;
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;
};

wxDisplayImpl *wxDisplayFactoryQt::CreateDisplay(unsigned n)
{
    return new wxDisplayImplQt( n );
}

unsigned wxDisplayFactoryQt::GetCount()
{
    return QApplication::screens().size();
}

int wxDisplayFactoryQt::GetFromPoint(const wxPoint& pt)
{
    for(const auto screen : QApplication::screens()){
        if(screen->geometry().contains( wxQtConvertPoint( pt ))){
            return QApplication::screens().indexOf( screen );
        }
    }
    return 0;
}

//##############################################################################

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactoryQt;
}

#else // wxUSE_DISPLAY

class wxDisplayFactorySingleQt : public wxDisplayFactorySingleQt
{
protected:
    virtual wxDisplayImpl *CreateSingleDisplay() wxOVERRIDE
    {
        return new wxDisplayImplQt(0);
    }
};

/* static */ wxDisplayFactory *wxDisplay::CreateFactory()
{
    return new wxDisplayFactorySingleQt;
}

#endif // wxUSE_DISPLAY/!wxUSE_DISPLAY
