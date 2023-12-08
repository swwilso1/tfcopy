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

#ifndef STARTUP_PANEL_HPP
#define STARTUP_PANEL_HPP

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "TFFoundation.hpp"
#include "data_model.hpp"
#include "base_panel.hpp"

using namespace TF::Foundation;
using namespace ftxui;

namespace copy
{

    class StartupPanel : public BasePanel
    {
    public:

        StartupPanel(screen_type & screen, model_type & model);

        [[nodiscard]] auto Render() -> Element override;

        void set_loading_panel(std::shared_ptr<BasePanel> panel);

    private:

        bool m_activated_copy_panel{false};
        std::shared_ptr<BasePanel> m_loading_panel{nullptr};
    };

} // namespace copy

#endif // STARTUP_PANEL_HPP
