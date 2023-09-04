// DoodleSync takes care of synchronizing doodle across devices.
// SPDX-FileCopyrightText: 2023 42yeah <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOODLESYNC_H
#define DOODLESYNC_H

#include <thread>
#include "../doodle.h"

/**
 * DoodleSync takes care of synchronizing doodle across devices.
 */
class DoodleSync
{
public:
    /**
     * Default constructor
     */
    DoodleSync();

    DoodleSync(const DoodleSync& other) = delete;

    /**
     * Destructor
     */
    ~DoodleSync();

    void sync();

private:
    std::thread sync_thread;
    bool alive;
};

#endif // DOODLESYNC_H
