// Doodle holds everything in the website canvas.
// SPDX-FileCopyrightText: 2023 42yeah hey@42yeah.is
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DOODLE_H
#define DOODLE_H

#include <vector>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>


struct Point2D
{
    Point2D();
    Point2D(int x, int y);

    int x, y;
};

struct Line
{
    Point2D begin;
    Point2D end;
};

class Segment
{
public:
    Segment(const std::vector<Line> &lines);

    ~Segment() = default;

    template<typename T>
    bool stale(int threshold) const
    {
        const auto now = std::chrono::high_resolution_clock::now();
        const auto elapsed = now - born;

        auto time_passed = std::chrono::duration_cast<T>(elapsed);
        auto c = time_passed.count();

        return c >= threshold;
    }

    bool is_after(const std::chrono::time_point<std::chrono::system_clock> &tp) const;
    bool is_before(const std::chrono::time_point<std::chrono::system_clock> &tp) const;

    const std::vector<Line> &get_lines() const;

private:
    std::vector<Line> lines;
    std::chrono::time_point<std::chrono::system_clock> born;
};

/**
 * Doodle holds everything within the website canvas.
 */
class Doodle
{
public:
    Doodle(const Doodle& other) = delete;

    /**
     * Clean stale segments when they are @threshold minutes old.
     * Aquires lock
     */
    int clean_stale_minutes(int threshold);

    /**
     * All four query functions won't lock; it is up to later code to lock them up
     * Not very graceful, I know, but this adds flexibility to code
     */
    std::vector<Segment>::const_iterator segments_after(const std::chrono::time_point<std::chrono::system_clock> &tp);

    std::vector<Segment>::const_iterator all_segments() const;

    std::vector<Segment>::const_iterator segment_end() const;

    std::string serialize(std::vector<Segment>::const_iterator it);

    /**
     * Add new segment to the fray. Acquires lock
     */
    void add_segment(const Segment &seg);

    /**
     * Destructor
     */
    ~Doodle() = default;

    static std::shared_ptr<Doodle> inst();

    std::mutex mu;

private:
    /**
     * Default constructor
     */
    Doodle();

    std::vector<Segment> segments;

    std::vector<Segment>::const_iterator cached_segment_it;
    std::chrono::time_point<std::chrono::system_clock> cached_tp;

    static std::shared_ptr<Doodle> instance;
};

#endif // DOODLE_H
