#ifndef _MODULE_NETWORK_HPP_
#define _MODULE_NETWORK_HPP_

#include <json/json.h>

namespace Network {

    class Request {

    protected:
        std::string URL;

    public:
        Request(std::string URL);
        Json::Value GET();
        void POST(Json::Value root);
    };
}

#endif
