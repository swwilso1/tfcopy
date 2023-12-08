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

#ifndef LOADING_PANEL_HPP
#define LOADING_PANEL_HPP

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
    class LoadingPanel : public BasePanel
    {
    public:
        LoadingPanel(screen_type &, model_type &);

        [[nodiscard]] auto Render() -> Element override;

        void Refresh() override;

        void set_copy_panel(std::shared_ptr<BasePanel> panel)
        {
            m_copy_panel = panel;
        }

    private:
        Component m_buttons{};

        bool m_interrupted{false};
        bool m_load_thread_finished{false};
        bool m_load_thread_started{false};

        int32_t m_spinner_charset{19};
        size_t m_spinner_index{0};

        std::shared_ptr<BasePanel> m_copy_panel{nullptr};

        auto format_total_bytes() -> String;
    };

} // namespace copy

#endif // LOADING_PANEL_HPP
