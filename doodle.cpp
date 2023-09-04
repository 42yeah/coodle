// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 42yeah <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "doodle.h"
#include <sstream>


Point2D::Point2D() : x(0), y(0)
{

}

Point2D::Point2D(int x, int y) : x(x), y(y)
{

}

Segment::Segment(const std::vector<Line> &lines) : lines(lines), born(std::chrono::high_resolution_clock::now())
{

}

const std::vector<Line> &Segment::get_lines() const
{
    return lines;
}

bool Segment::is_after(const std::chrono::time_point<std::chrono::system_clock> &tp) const
{
    return born > tp;
}

bool Segment::is_before(const std::chrono::time_point<std::chrono::system_clock> &tp) const
{
    return born <= tp;
}

Doodle::Doodle() : cached_segment_it(segments.begin())
{

}

int Doodle::clean_stale_minutes(int threshold)
{
    std::lock_guard<std::mutex> lk(mu);

    auto non_stale = segments.begin();

    for (auto it = segments.begin(); it != segments.end(); it++)
    {
        if (!it->stale<std::chrono::minutes>(threshold))
        {
            non_stale = it;
            break;
        }
    }

    int len = non_stale - segments.begin();
    if (non_stale != segments.begin())
    {
        segments.erase(segments.begin(), non_stale);
        cached_segment_it = segments.begin();
    }
    return len;
}

std::vector<Segment>::const_iterator Doodle::segments_after(const std::chrono::time_point<std::chrono::system_clock> &tp)
{
    if (cached_tp > tp)
    {
        cached_tp = tp;
        cached_segment_it = segments.begin();
    }

    cached_segment_it = segments.begin(); // Disable cache??
    for (auto it = cached_segment_it; it != segments.end(); it++)
    {
        if (it->is_after(tp))
        {
            return it;
        }
    }
    return segments.end();
}

std::vector<Segment>::const_iterator Doodle::all_segments() const
{
    return segments.begin();
}

void Doodle::add_segment(const Segment &seg)
{
    std::lock_guard<std::mutex> lk(mu);

    segments.push_back(seg);
}

std::vector<Segment>::const_iterator Doodle::segment_end() const
{
    return segments.end();
}

std::string Doodle::serialize(std::vector<Segment>::const_iterator it)
{
    std::stringstream ss;
    while (it != segments.end())
    {
        const std::vector<Line> &lines = it->get_lines();
        if (lines.size() == 0)
        {
            // Skip segment
            it++;
            continue;
        }

        ss << "@ "; // @ marks the beginning of a new segment
        ss << lines[0].begin.x << " " << lines[0].begin.y << " ";
        for (int i = 0; i < lines.size(); i++)
        {
            ss << lines[i].end.x << " " << lines[i].end.y << " ";
        }

        it++;
    }
    return ss.str();
}

std::shared_ptr<Doodle> Doodle::instance = nullptr;

std::shared_ptr<Doodle> Doodle::inst()
{
    if (!instance)
    {
        instance = std::shared_ptr<Doodle>(new Doodle());
    }
    return instance;
}
