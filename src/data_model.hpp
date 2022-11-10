/******************************************************************************
 *
 * Tectiform Open Source License (TOS)
 *
 * Copyright (c) 2022 to 2022 Tectiform Inc.
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

#ifndef DATA_MODEL_HPP
#define DATA_MODEL_HPP

#include <string>
#include <ftxui/component/component_options.hpp>
#include "TFFoundation.hpp"

using namespace TF::Foundation;
using namespace ftxui;

namespace copy
{

    struct DataModel
    {
        enum class ActivePanel
        {
            STARTUP,
            COPY
        };

        using string_type = std::string;
        using version_type = Version;

        string_type tool_name{"Tectiform Copy Tool"};
        version_type tool_version{1, 0, 0};

        ButtonOption button_style{ButtonOption::Simple()};
        Color background_color{Color::GrayDark};
        Color foreground_window_background_color{Color::NavyBlue};

        DataModel();

        void set_current_panel(ActivePanel panel)
        {
            ui_tab_selector = static_cast<decltype(ui_tab_selector)>(panel);
        }

        [[nodiscard]] auto get_panel_selector() -> int32_t *
        {
            return &ui_tab_selector;
        }

    private:

        int32_t ui_tab_selector{};
    };

} // namespace copy

#endif // DATA_MODEL_HPP
