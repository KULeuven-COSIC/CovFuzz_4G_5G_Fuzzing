#pragma once

#include <string>
#include <boost/algorithm/string/trim.hpp>

constexpr const char* OK = "OK";
struct AT_Response {

    AT_Response() : is_OK(false) {}

    AT_Response(const std::string& request, std::string response) {
        this->request = request;
        if (size_t pos = response.find(request); pos != std::string::npos) {
            response.erase(pos, request.length());
            boost::algorithm::trim(response);
        }
        is_OK = (response.find("OK") != std::string::npos);
        this->response = response;
        timeout = (response == "TIMEOUT");
        error = (response.find("ERROR") != std::string::npos);
    }

    AT_Response(const AT_Response& resp) : is_OK(resp.is_OK), request(resp.request), response(resp.response), timeout(resp.timeout), error(resp.error) {}

    AT_Response& operator=(const AT_Response& resp) {
        is_OK = resp.is_OK;
        request = resp.request;
        response = resp.response;
        timeout = resp.timeout;
        error = resp.error;
        return *this;
    }

    bool is_OK;
    std::string request;
    std::string response = "";
    bool timeout = false;
    bool error = false;
};