#include <boost/network/protocol/http.hpp>
#include <Network/Request.hpp>

namespace http = boost::network::http;

Network::Request::Request(std::string URL) {

    this->URL = URL;
}

Json::Value Network::Request::GET() {

    http::client client;
    http::client::request request(URL);

    Json::Value root;
    Json::Reader reader;
    reader.parse( static_cast<std::string>(body(client.get(request))), root );

    return root;
}

void Network::Request::POST(Json::Value root) {

    http::client client;
    http::client::request request(URL);

    Json::StyledWriter writer;

    std::string data = writer.write(root);
    std::string enctype = "application/x-www-form-urlencoded";

    client.post(request,data,enctype);
}
