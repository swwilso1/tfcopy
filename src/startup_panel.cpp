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

#include "startup_panel.hpp"

namespace copy
{

    StartupPanel::StartupPanel(screen_type & screen, model_type & model) : BasePanel(screen, model)
    {
    }

    auto StartupPanel::Render() -> Element
    {
        if (! m_activated_copy_panel)
        {
            auto activate = [this] {
                Sleep(std::chrono::milliseconds(250));
                m_copy_panel->Refresh();
                m_model.set_current_panel(DataModel::ActivePanel::COPY);
                m_screen.PostEvent(Event::Custom);
            };

            std::thread activate_thread{activate};
            activate_thread.detach();

            m_activated_copy_panel = true;
        }
        return main_ui_element({filler(), hbox({filler(), text(" "), filler()}), filler()});
    }

    void StartupPanel::set_copy_panel(std::shared_ptr<BasePanel> panel)
    {
        m_copy_panel = panel;
    }

} // namespace copy
