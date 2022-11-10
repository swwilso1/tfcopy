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

#ifndef BASE_PANEL_HPP
#define BASE_PANEL_HPP

#include <vector>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include "TFFoundation.hpp"
#include "data_model.hpp"

using namespace TF::Foundation;
using namespace ftxui;

namespace copy
{

    class BasePanel : public ComponentBase
    {
    public:
        using model_type = DataModel;
        using screen_type = ScreenInteractive;

        BasePanel(screen_type & screen, model_type & model);

        virtual ~BasePanel() {}

        virtual void Refresh() {}

    protected:
        using element_list = std::vector<Element>;

        [[nodiscard]] auto main_ui_element(element_list elements) -> Element;

        screen_type & m_screen;
        model_type & m_model;
    };

} // namespace copy

#endif // BASE_PANEL_HPP
