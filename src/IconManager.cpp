#include "IconManager.h"

#include <QIcon>

namespace
{
    const std::map<std::string, std::string> icons {
        { "Passed", 
            R"#(<?xml version="1.0" encoding="utf-8"?><svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 122.88 122.88" style="enable-background:new 0 0 122.88 122.88" xml:space="preserve"><style type="text/css"><![CDATA[
	.st0{fill-rule:evenodd;clip-rule:evenodd;fill:#3AAF3C;}
]]></style><g><path class="st0" d="M61.44,0c33.93,0,61.44,27.51,61.44,61.44c0,33.93-27.51,61.44-61.44,61.44C27.51,122.88,0,95.37,0,61.44 C0,27.51,27.51,0,61.44,0L61.44,0L61.44,0z M39.48,56.79c4.6,2.65,7.59,4.85,11.16,8.78c9.24-14.88,19.28-23.12,32.32-34.83 l1.28-0.49h14.28C79.38,51.51,64.53,69.04,51.24,94.68c-6.92-14.79-13.09-25-26.88-34.47L39.48,56.79L39.48,56.79z"/></g></svg>)#"
        },
        { "Error",
            R"#(<svg id="Layer_1" data-name="Layer 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 122.88 122.88"><defs><style>.cls-1{fill:#eb0100;}.cls-1,.cls-2{fill-rule:evenodd;}.cls-2{fill:#fff;}</style></defs><title>cancel</title><path class="cls-1" d="M61.44,0A61.44,61.44,0,1,1,0,61.44,61.44,61.44,0,0,1,61.44,0Z"/><path class="cls-2" d="M35.38,49.72c-2.16-2.13-3.9-3.47-1.19-6.1l8.74-8.53c2.77-2.8,4.39-2.66,7,0L61.68,46.86,73.39,35.15c2.14-2.17,3.47-3.91,6.1-1.2L88,42.69c2.8,2.77,2.66,4.4,0,7L76.27,61.44,88,73.21c2.65,2.58,2.79,4.21,0,7l-8.54,8.74c-2.63,2.71-4,1-6.1-1.19L61.68,76,49.9,87.81c-2.58,2.64-4.2,2.78-7,0l-8.74-8.53c-2.71-2.63-1-4,1.19-6.1L47.1,61.44,35.38,49.72Z"/></svg>)#"
        },
        { "NotRun",
            R"#(<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<svg
   xmlns:dc="http://purl.org/dc/elements/1.1/"
   xmlns:cc="http://creativecommons.org/ns#"
   xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
   xmlns:svg="http://www.w3.org/2000/svg"
   xmlns="http://www.w3.org/2000/svg"
   xmlns:sodipodi="http://sodipodi.sourceforge.net/DTD/sodipodi-0.dtd"
   xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
   version="1.1"
   id="Layer_1"
   x="0px"
   y="0px"
   viewBox="0 0 122.88 122.88"
   style="enable-background:new 0 0 122.88 122.88"
   xml:space="preserve"
   sodipodi:docname="question-mark-round.svg"
   inkscape:version="0.92.5 (2060ec1f9f, 2020-04-08)"><metadata
     id="metadata13"><rdf:RDF><cc:Work
         rdf:about=""><dc:format>image/svg+xml</dc:format><dc:type
           rdf:resource="http://purl.org/dc/dcmitype/StillImage" /></cc:Work></rdf:RDF></metadata><defs
     id="defs11" /><sodipodi:namedview
     pagecolor="#ffffff"
     bordercolor="#666666"
     borderopacity="1"
     objecttolerance="10"
     gridtolerance="10"
     guidetolerance="10"
     inkscape:pageopacity="0"
     inkscape:pageshadow="2"
     inkscape:window-width="1786"
     inkscape:window-height="1175"
     id="namedview9"
     showgrid="false"
     inkscape:zoom="1.920573"
     inkscape:cx="-9.1118642"
     inkscape:cy="57.055342"
     inkscape:window-x="1602"
     inkscape:window-y="503"
     inkscape:window-maximized="0"
     inkscape:current-layer="Layer_1" /><style
     type="text/css"
     id="style2">.st0{fill-rule:evenodd;clip-rule:evenodd;}</style><g
     id="g6"><path
       class="st0"
       d="M122.88,61.44C122.88,27.51,95.37,0,61.44,0C27.51,0,0,27.51,0,61.44c0,33.93,27.51,61.44,61.44,61.44 C95.37,122.88,122.88,95.37,122.88,61.44L122.88,61.44z M68.79,74.58H51.3v-1.75c0-2.97,0.32-5.39,1-7.25 c0.68-1.87,1.68-3.55,3.01-5.1c1.34-1.54,4.35-4.23,9.01-8.11c2.48-2.03,3.73-3.88,3.73-5.56c0-1.71-0.51-3.01-1.5-3.95 c-1-0.93-2.51-1.4-4.54-1.4c-2.19,0-3.98,0.73-5.4,2.16c-1.43,1.44-2.34,3.97-2.74,7.56l-17.88-2.22c0.61-6.57,3-11.86,7.15-15.85 c4.17-4.02,10.55-6.01,19.14-6.01c6.7,0,12.1,1.4,16.21,4.19c5.6,3.78,8.38,8.82,8.38,15.1c0,2.62-0.73,5.14-2.16,7.56 c-1.44,2.44-4.39,5.39-8.85,8.88c-3.09,2.48-5.05,4.44-5.86,5.93C69.19,70.24,68.79,72.19,68.79,74.58L68.79,74.58z M50.68,79.25 h18.76v16.53H50.68V79.25L50.68,79.25z"
       id="path4"
       style="fill:#009cff;fill-opacity:1" /></g></svg>)#"
        },
        { "Skipped",
            R"#(<svg id="Layer_1" data-name="Layer 1" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 122.88 111.24"><defs><style>.cls-1{fill:#fec901;fill-rule:evenodd;}.cls-2{fill:#010101;}</style></defs><title>risk</title><path d="M2.5,85l43-74.41h0a22.59,22.59,0,0,1,8-8.35,15.72,15.72,0,0,1,16,0,22.52,22.52,0,0,1,7.93,8.38l.23.44,42.08,73.07a20.91,20.91,0,0,1,3,10.84A16.44,16.44,0,0,1,121,102.4a15.45,15.45,0,0,1-5.74,6,21,21,0,0,1-11.35,2.78v0H17.7c-.21,0-.43,0-.64,0a19,19,0,0,1-7.83-1.74,15.83,15.83,0,0,1-6.65-5.72A16.26,16.26,0,0,1,0,95.18a21.66,21.66,0,0,1,2.2-9.62c.1-.2.2-.4.31-.59Z"/><path class="cls-1" d="M9.09,88.78l43-74.38c5.22-8.94,13.49-9.2,18.81,0l42.32,73.49c4.12,6.79,2.41,15.9-9.31,15.72H17.7C9.78,103.79,5,97.44,9.09,88.78Z"/><path class="cls-2" d="M57.55,83.15a5.47,5.47,0,0,1,5.85-1.22,5.65,5.65,0,0,1,2,1.3A5.49,5.49,0,0,1,67,86.77a5.12,5.12,0,0,1-.08,1.4,5.22,5.22,0,0,1-.42,1.34,5.51,5.51,0,0,1-5.2,3.25,5.63,5.63,0,0,1-2.26-.53,5.51,5.51,0,0,1-2.81-2.92A6,6,0,0,1,55.9,88a5.28,5.28,0,0,1,0-1.31h0a6,6,0,0,1,.56-2,4.6,4.6,0,0,1,1.14-1.56Zm8.12-10.21c-.19,4.78-8.28,4.78-8.46,0-.82-8.19-2.92-27.63-2.85-35.32.07-2.37,2-3.78,4.55-4.31a11.65,11.65,0,0,1,2.48-.25,12.54,12.54,0,0,1,2.5.25c2.59.56,4.63,2,4.63,4.43V38l-2.84,35Z"/></svg>)#"
        },
        { "Running",
            R"#(<?xml version="1.0" encoding="utf-8"?><svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 122.88 122.77" style="enable-background:new 0 0 122.88 122.77" xml:space="preserve"><style type="text/css"><![CDATA[
	.st0{fill-rule:evenodd;clip-rule:evenodd;}
]]></style><g style="fill:#808080"><path class="st0" d="M92.45,8.98c-1.35-0.94-2.88-1.35-4.44-1.04c-1.56,0.31-2.88,1.15-3.82,2.46l-5.27,7.43 c-1.87-0.83-3.82-1.46-5.9-1.98s-4.03-0.94-6.11-1.25l-1.67-9.72C64.94,3.21,64.09,2,62.89,1.06c-1.35-0.94-2.78-1.25-4.44-0.94 L46.26,2.31c-1.56,0.31-2.78,1.04-3.82,2.39c-0.94,1.35-1.35,2.78-1.04,4.44l1.56,8.88c-1.98,0.83-3.82,1.77-5.59,2.88 c-1.77,1.04-3.5,2.29-5.07,3.5l-8.26-5.69c-1.35-0.94-2.78-1.35-4.34-1.04c-1.56,0.31-2.88,1.15-3.82,2.5L8.85,30.22 c-0.94,1.35-1.35,2.88-1.04,4.44c0.31,1.67,1.15,2.88,2.5,3.82l7.43,5.27c-0.83,1.87-1.46,3.82-1.98,5.9 c-0.52,2.08-0.94,4.03-1.25,6.11L4.8,57.42c-8.24,1.55-3.58,13.36-2.57,18.98c0.31,1.56,1.04,2.78,2.36,3.82 c1.35,0.94,2.78,1.35,4.44,1.04l8.88-1.56c0.83,1.98,1.77,3.82,2.88,5.59c1.04,1.77,2.29,3.5,3.5,5.17l-5.69,8.16 c-0.94,1.35-1.35,2.78-1.04,4.34c0.31,1.56,1.14,2.88,2.46,3.82l10.13,7.11c1.35,0.94,2.88,1.25,4.44,0.94 c1.56-0.31,2.88-1.04,3.92-2.36l5.28-7.53c1.87,0.83,3.82,1.46,5.9,1.98c2.08,0.52,4.02,0.94,6.11,1.25l1.67,9.72 c0.31,1.67,1.15,2.88,2.36,3.82c1.35,0.94,2.78,1.25,4.44,0.94l12.18-2.19c1.56-0.31,2.78-1.04,3.82-2.36 c0.94-1.35,1.35-2.78,1.04-4.44l-1.56-8.88c1.98-0.83,3.82-1.77,5.59-2.88c1.77-1.04,3.51-2.26,5.17-3.5l8.16,5.69 c1.35,0.94,2.78,1.35,4.44,1.04c1.67-0.31,2.88-1.15,3.82-2.46l7.11-10.14c0.94-1.35,1.25-2.88,0.94-4.44s-1.04-2.88-2.39-3.92 L105.05,79c0.83-1.87,1.46-3.82,1.98-5.9c0.52-2.08,0.94-4.03,1.25-6.11l9.72-1.67c1.67-0.31,2.88-1.15,3.82-2.39 c0.94-1.35,1.25-2.78,0.94-4.44l-2.19-12.18c-0.31-1.56-1.04-2.78-2.36-3.82c-1.35-0.94-2.78-1.35-4.44-1.04l-8.88,1.56 c-0.83-1.87-1.77-3.71-2.88-5.59c-1.04-1.87-2.29-3.5-3.5-5.07l5.69-8.26c0.94-1.35,1.35-2.78,1.04-4.34 c-0.31-1.56-1.15-2.88-2.46-3.82L92.73,8.87L92.45,8.98L92.45,8.98L92.45,8.98L92.45,8.98z M57.12,44.41h5.12 c0.93,0,1.7,0.77,1.7,1.7v17.12h15.03c0.94,0,1.71,0.77,1.71,1.7v5.12c0,0.94-0.77,1.7-1.71,1.7H55.4V46.12 C55.4,45.18,56.17,44.41,57.12,44.41L57.12,44.41L57.12,44.41z M55.41,28.79c4.39-0.72,8.84-0.72,13.23,0.29 c4.25,1,8.12,2.72,11.65,5.11c3.39,2.44,6.4,5.54,8.84,9.26c2.44,3.68,3.97,7.84,4.68,12.23c0.72,4.39,0.72,8.84-0.29,13.23 c-1,4.25-2.72,8.12-5.11,11.65c-2.44,3.39-5.54,6.4-9.27,8.84c-3.68,2.44-7.84,3.97-12.23,4.68c-4.39,0.72-8.84,0.72-13.23-0.29 c-4.25-1-8.12-2.72-11.65-5.11c-3.39-2.44-6.4-5.54-8.84-9.26c-2.44-3.68-3.97-7.84-4.68-12.23c-0.72-4.39-0.72-8.84,0.29-13.23 c1-4.25,2.72-8.12,5.11-11.65c2.44-3.39,5.54-6.4,9.27-8.84C46.91,31.08,51.01,29.5,55.41,28.79L55.41,28.79L55.41,28.79 L55.41,28.79z"/></g></svg>)#"
        },
        { "Clock",
            R"#(<?xml version="1.0" encoding="utf-8"?><svg version="1.1" id="Layer_1" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" x="0px" y="0px" viewBox="0 0 237.54 237.54" style="enable-background:new 0 0 237.54 237.54" xml:space="preserve"><g style="fill:#808080"><path d="M118.77,0c32.8,0,62.49,13.29,83.98,34.79c21.49,21.49,34.79,51.19,34.79,83.98s-13.29,62.49-34.79,83.98 c-21.49,21.49-51.19,34.79-83.98,34.79c-32.8,0-62.49-13.29-83.98-34.79C13.29,181.26,0,151.56,0,118.77s13.29-62.49,34.79-83.98 C56.28,13.29,85.97,0,118.77,0L118.77,0z M109.06,60.2c0-3.59,2.91-6.5,6.5-6.5s6.5,2.91,6.5,6.5v60l45.14,26.76 c3.08,1.82,4.11,5.8,2.29,8.89c-1.82,3.08-5.8,4.11-8.89,2.29l-47.99-28.45c-2.11-1.08-3.55-3.27-3.55-5.79V60.2L109.06,60.2z M193.56,43.98C174.42,24.84,147.98,13,118.77,13c-29.21,0-55.65,11.84-74.79,30.98C24.84,63.12,13,89.56,13,118.77 c0,29.21,11.84,55.65,30.98,74.79c19.14,19.14,45.58,30.98,74.79,30.98c29.21,0,55.65-11.84,74.79-30.98 c19.14-19.14,30.98-45.58,30.98-74.79C224.54,89.56,212.7,63.12,193.56,43.98L193.56,43.98z"/></g></svg>)#"
        }
    };
    
    QImage makeImage(const std::string& svgtext)
    {
        return QImage::fromData(svgtext.data());
    }
    
    QIcon makeIcon(const std::string& svgtext)
    {
        const auto image = makeImage(svgtext);
        const auto pixmap = QPixmap::fromImage(image);
        
        return pixmap;
    }
    
    QImage getImage(const std::string& name)
    {
        const auto it = icons.find(name);
        
        if(it == icons.end())
            return QImage();
        
        return makeImage(it->second);
    }
    
    QIcon getIcon(const std::string& name)
    {
        const auto it = icons.find(name);
        
        if(it == icons.end())
            return QIcon();
        
        return makeIcon(it->second);
    }
}

QIcon IconManager::getIcon(KDevelop::TestResult::TestCaseResult result)
{
    switch (result)
    {
        case KDevelop::TestResult::NotRun:
            return ::getIcon("NotRun"); 

        case KDevelop::TestResult::Skipped:
            return ::getIcon("Skipped");

        case KDevelop::TestResult::Passed:
            return ::getIcon("Passed");

        case KDevelop::TestResult::Error:
            return ::getIcon("Error");

        case KDevelop::TestResult::Failed:
            return ::getIcon("Skipped");

        case KDevelop::TestResult::UnexpectedPass:
            // This is a very rare occurrence, so the icon should stand out
            return QIcon::fromTheme(QStringLiteral("dialog-warning"));

        case KDevelop::TestResult::ExpectedFail:
            return QIcon::fromTheme(QStringLiteral("dialog-ok"));
    }
    Q_UNREACHABLE();
}

QIcon IconManager::getIcon(const TestState& state)
{
    if(state.state == TestState::State::Started)
        return ::getIcon("Running");
    if(state.state == TestState::State::Executing)
        return ::getIcon("Running");

    return getIcon(state.result);
}

QImage IconManager::getImage(const std::string& name)
{
    return ::getImage(name);
}

