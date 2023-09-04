// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 42yeah <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "doodlesync.h"
#include "wslistener.h"
#include <sstream>


DoodleSync::DoodleSync() : sync_thread(&DoodleSync::sync, this), alive(true)
{
    sync_thread.detach();
}

DoodleSync::~DoodleSync()
{
    alive = false;
}

void DoodleSync::sync()
{
    OATPP_LOGI("DoodleSync", "DoodleSync is starting.");

    auto now = std::chrono::high_resolution_clock::now();
    // Data race? Pfft. What does that even mean?
    while (alive)
    {
        // Step 1. obtain all connected websockets.
        std::vector<const WSListener::WebSocket *> websockets;
        {
            std::lock_guard<std::mutex> lk(WSInstanceListener::mu);
            websockets = WSInstanceListener::connected;
        }

        // Step 3. obtain delta segments.
        auto doodle = Doodle::inst();
        doodle->clean_stale_minutes(43200);

        // Step 4. serialize data since last fetch.
        std::string serialized = "";
        {
            std::lock_guard<std::mutex> lk(doodle->mu);
            auto it = doodle->segments_after(now);
            serialized = doodle->serialize(it);
        }
        now = std::chrono::high_resolution_clock::now();

        if (serialized.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        // Step 5. send them out to websockets.
        for (const auto &ws : websockets)
        {
            OATPP_LOGI("DoodleSync", "Send");
            ws->sendOneFrameText(serialized);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }


}
