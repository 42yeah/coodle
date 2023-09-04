// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 42yeah <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wslistener.h"
#include "../doodle.h"


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSListener

void WSListener::onPing(const WebSocket &socket, const oatpp::String& message) {
    OATPP_LOGD(TAG, "onPing");
    socket.sendPong(message);
}

void WSListener::onPong(const WebSocket &socket, const oatpp::String& message) {
    OATPP_LOGD(TAG, "onPong");
}

void WSListener::onClose(const WebSocket &socket, v_uint16 code, const oatpp::String& message) {
    OATPP_LOGD(TAG, "onClose code=%d", code);
}

void WSListener::readMessage(const WebSocket &socket, v_uint8 opcode, p_char8 data, oatpp::v_io_size size)
{
    if (size == 0)
    { // message transfer finished
        auto whole_msg = message_buffer.toString();
        message_buffer.setCurrentPosition(0);

        OATPP_LOGD(TAG, "onMessage message='%s'", whole_msg->c_str());

        // The frontend can only ever send us ONE segment.
        size_t pos = 0;
        std::vector<int> ints;
        while ((pos = whole_msg->find(" ")) != std::string::npos)
        {
            int x = std::atoi(whole_msg->substr(0, pos).c_str());
            ints.push_back(x);
            whole_msg->erase(0, pos + 1);
        }

        if (ints.size() < 4)
        {
            // Invalid coords
            return;
        }

        std::vector<Line> lines;
        Point2D start(ints[0], ints[1]);

        for (int i = 2; i < ints.size(); i += 2)
        {
            Point2D end(ints[i], ints[i + 1]);
            lines.push_back(Line{ start, end });
            start = end;
        }

        {
            Doodle::inst()->add_segment(Segment(lines));
        }
    }
    else if(size > 0)
    { // message frame received
        message_buffer.writeSimple(data, size);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WSInstanceListener

std::atomic<v_int32> WSInstanceListener::SOCKETS(0);
std::vector<const WSListener::WebSocket *> WSInstanceListener::connected;
std::mutex WSInstanceListener::mu;

void WSInstanceListener::onAfterCreate(const WebSocket &socket, const std::shared_ptr<const ParameterMap>& params)
{
    SOCKETS ++;
    OATPP_LOGD(TAG, "New Incoming Connection. Connection count=%d", SOCKETS.load());

    /* In this particular case we create one WSListener per each connection */
    /* Which may be redundant in many cases */
    socket.setListener(std::make_shared<WSListener>());

    {
        std::lock_guard<std::mutex> lk(mu);
        connected.push_back(&socket);
    }

    // Acquire lock and send sync messages
    {
        const auto &doodle = Doodle::inst();
        std::lock_guard<std::mutex> lk(doodle->mu);
        std::string serialized = doodle->serialize(doodle->all_segments());
        socket.sendOneFrameText(serialized);
    }
}

void WSInstanceListener::onBeforeDestroy(const WebSocket &socket)
{
    SOCKETS --;
    OATPP_LOGD(TAG, "Connection closed. Connection count=%d", SOCKETS.load());

    // Find the socket and destroy it
    {
        std::lock_guard<std::mutex> lk(mu);
        auto idx = std::find(connected.begin(), connected.end(), &socket);
        if (idx != connected.end())
        {
            connected.erase(idx, idx + 1);
        }
    }
}
