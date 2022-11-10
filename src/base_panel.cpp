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

#include "base_panel.hpp"

namespace copy
{

    BasePanel::BasePanel(screen_type & screen, model_type & model) : m_screen{screen}, m_model{model} {}

    auto BasePanel::main_ui_element(element_list elements) -> Element
    {
        auto tool_version = m_model.tool_version.as_string();
        auto tool_title = String::initWithFormat("%S (%@)", &m_model.tool_name, &tool_version);
        element_list boxes = {hbox({filler(), text(tool_title.stlString()) | bold, filler()}), separator()};

        for (auto & element : elements)
        {
            boxes.emplace_back(element);
        }

        return vbox(boxes) | border | bgcolor(m_model.background_color);
    }

} // namespace copy
