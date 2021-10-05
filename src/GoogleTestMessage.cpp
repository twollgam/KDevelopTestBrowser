#include "GoogleTestMessage.h"

std::string GoogleTestMessage::toHtml(const std::string& message) const
{
    //trace("toHtml: " + message);
    auto html = message;
    
    for(auto position = html.find('/'); position != std::string::npos; position = html.find('/', position + 1))
    {
        auto colonPosition = html.find(':', position);
        
        if(colonPosition != std::string::npos)
        {
            auto endPosition = html.find_first_not_of("0123456789", colonPosition + 1);
            
            if(endPosition != std::string::npos)
            {
                const auto url = html.substr(position, endPosition - position);
                
                //trace("url: " + url);
                
                html.insert(endPosition, "</a>");
                html.insert(position, "<a href=\"" + url + "\">");
                
                position = endPosition + 4 + 11 + url.size();

                //html.insert(endPosition, "</u>");
                //html.insert(position, "<u>");
                
                //position = endPosition + 4 + 3;
                
                continue;
            }
        }
    }
    
    for(auto position = html.find_first_of("\n"); position != std::string::npos; position = html.find_first_of("\n", position))
    {
        html.replace(position, 1, "<br>");
    }
    
    //trace("html: " + html);
    return html;
}

