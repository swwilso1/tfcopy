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

#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include "TFFoundation.hpp"

using namespace TF::Foundation;

namespace copy
{
    /**
     * @brief function to format bytes into exabytes, petabytes, terrabytes,
     * gigabytes, etc...
     * @param bytes the bytes to format.
     * @return a String containing the formatted bytes.
     */
    auto format_total_bytes(double bytes) -> String;

    /**
     * @brief function to format seconds into millenia, centuries, decades, years,
     * months, days, hours, minutes, seconds.
     * @param seconds the number of seconds to format.
     * @return a String containing the formatted time.
     */
    auto format_seconds(double seconds) -> String;

} // namespace copy

#endif // UTILITIES_HPP
