// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 42yeah <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOODLECONTROLLER_H
#define DOODLECONTROLLER_H

#include "oatpp-websocket/Handshaker.hpp"

#include "oatpp/web/server/api/ApiController.hpp"

#include "oatpp/network/ConnectionHandler.hpp"

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"

#include "../doodle.h"
#include "../websocket/doodlesync.h"

#include OATPP_CODEGEN_BEGIN(ApiController) //<-- codegen begin


class DoodleController : public oatpp::web::server::api::ApiController
{
private:
    typedef DoodleController __ControllerType;

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ConnectionHandler>, websocket_connection_handler, "websocket");

    DoodleSync doodle_sync;

public:
    DoodleController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, obj_mapper))
        : oatpp::web::server::api::ApiController(obj_mapper)
    {

    }

    ENDPOINT_ASYNC("GET", "/", WS)
    {
        ENDPOINT_ASYNC_INIT(WS);

        Action act() override {
            auto response = oatpp::websocket::Handshaker::serversideHandshake(request->getHeaders(), controller->websocket_connection_handler);
            return _return(response);
        }
    };
};

#endif // DOODLECONTROLLER_H
