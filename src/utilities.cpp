/******************************************************************************
 *
 * Tectiform Open Source License (TOS)
 *
 * Copyright (c) 2022 to 2023 Tectiform Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 *
 * ******************************************************************************/

#include "utilities.hpp"

namespace copy
{

    auto format_total_bytes(double bytes) -> String
    {
        constexpr double KILO{1024.0};
        constexpr double MEGA{1024 * KILO};
        constexpr double GIGA{1024 * MEGA};
        constexpr double TERRA{1024 * GIGA};
        constexpr double PETA{1024 * TERRA};
        constexpr double EXA{1024 * PETA};

        String formatted_bytes{};

        auto format = [](double bytes, double divisor, String & label) -> String {
            const auto divided_value = bytes / divisor;
            return String::initWithFormat("%-0.2f %@", divided_value, &label);
        };

        double divisor{};
        String label{};

        if (bytes - EXA >= 1)
        {
            divisor = EXA;
            label = "EB";
        }
        else if (bytes - PETA >= 1)
        {
            divisor = PETA;
            label = "PB";
        }
        else if (bytes - TERRA >= 1)
        {
            divisor = TERRA;
            label = "TB";
        }
        else if (bytes - GIGA >= 0)
        {
            divisor = GIGA;
            label = "GB";
        }
        else if (bytes - MEGA >= 0)
        {
            divisor = MEGA;
            label = "MB";
        }
        else if (bytes - KILO >= 0)
        {
            divisor = KILO;
            label = "KB";
        }
        else
        {
            divisor = 1;
            label = "B";
        }

        return format(bytes, divisor, label);
    }

    auto format_seconds(double seconds) -> String
    {
        constexpr double MINUTE = 60.0;
        constexpr double HOUR = 60 * MINUTE;
        constexpr double DAY = 24 * HOUR;
        constexpr double WEEK = 7 * DAY;
        // MONTH is imprecise for now
        constexpr double MONTH = 30 * DAY;
        constexpr double YEAR = 365 * DAY;
        constexpr double DECADE = 10 * YEAR;
        constexpr double CENTURY = 100 * YEAR;
        constexpr double MILLENIUM = 1000 * YEAR;

        auto calculate_divisor_and_label = [](double seconds, double divisor, const char * muti_label,
                                              const char * single_label) -> std::pair<double, String> {
            const auto exact_value = static_cast<double>(seconds) / static_cast<double>(divisor);

            String label{single_label};
            if (exact_value > 1)
            {
                label = muti_label;
            }
            return std::make_pair(divisor, label);
        };

        auto format = [](double seconds, std::pair<double, String> & divisor_and_label) -> String {
            const auto divided_value = seconds / divisor_and_label.first;
            return String::initWithFormat("%-6.6g %@", divided_value, &divisor_and_label.second);
        };

        std::pair<double, String> divisor_and_label{};

        if (seconds - MILLENIUM >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, MILLENIUM, "millenia", "millenium");
        }
        else if (seconds - CENTURY >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, CENTURY, "centuries", "century");
        }
        else if (seconds - DECADE >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, DECADE, "decades", "decade");
        }
        else if (seconds - YEAR >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, YEAR, "years", "year");
        }
        else if (seconds - MONTH >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, MONTH, "months", "month");
        }
        else if (seconds - WEEK >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, WEEK, "weeks", "week");
        }
        else if (seconds - DAY >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, DAY, "days", "day");
        }
        else if (seconds - HOUR >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, HOUR, "hours", "hour");
        }
        else if (seconds - MINUTE >= 1)
        {
            divisor_and_label = calculate_divisor_and_label(seconds, MINUTE, "minutes", "minute");
        }
        else
        {
            divisor_and_label = calculate_divisor_and_label(seconds, 1, "seconds", "second");
        }

        return format(seconds, divisor_and_label);
    }

} // namespace copy
