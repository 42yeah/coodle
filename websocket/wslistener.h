// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 42yeah <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WSLISTENER_H
#define WSLISTENER_H

#include "oatpp-websocket/ConnectionHandler.hpp"
#include "oatpp-websocket/WebSocket.hpp"
#include "../doodle.h"
#include <vector>
#include <mutex>


/**
 * WebSocket listener listens on incoming WebSocket events.
 */
class WSListener : public oatpp::websocket::WebSocket::Listener
{
private:
    static constexpr const char* TAG = "Server_WSListener";

private:
    /**
     * Buffer for messages. Needed for multi-frame messages.
     */
    oatpp::data::stream::BufferOutputStream message_buffer;

public:
    /**
     * Called on "ping" frame.
     */
    void onPing(const WebSocket &socket, const oatpp::String& message) override;

    /**
     * Called on "pong" frame
     */
    void onPong(const WebSocket &socket, const oatpp::String& message) override;

    /**
     * Called on "close" frame
     */
    void onClose(const WebSocket &socket, v_uint16 code, const oatpp::String& message) override;

    /**
     * Called on each message frame. After the last message will be called once-again with size == 0 to designate end of the message.
     */
    void readMessage(const WebSocket &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size) override;
};

/**
 * Listener on new WebSocket connections.
 */
class WSInstanceListener : public oatpp::websocket::ConnectionHandler::SocketInstanceListener
{
private:
    static constexpr const char* TAG = "Server_WSInstanceListener";

public:
    /**
     * Counter for connected clients.
     */
    static std::atomic<v_int32> SOCKETS;
    static std::vector<const WSListener::WebSocket *> connected;
    static std::mutex mu;

public:
    /**
     *  Called when socket is created
     */
    void onAfterCreate(const WebSocket &socket, const std::shared_ptr<const ParameterMap>& params) override;

    /**
     *  Called before socket instance is destroyed.
     */
    void onBeforeDestroy(const WebSocket& socket) override;
};

#endif // WSLISTENER_H
