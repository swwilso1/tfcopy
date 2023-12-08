/******************************************************************************
*
* Tectiform Open Source License (TOS)
*
* Copyright (c) 2022 to 2023 Tectiform Inc.

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

#include <functional>
#include "loading_panel.hpp"

namespace copy
{

    LoadingPanel::LoadingPanel(screen_type & screen, model_type & model) : BasePanel(screen, model)
    {
        m_buttons = Container::Horizontal({Button(
            "Exit",
            [this] {
                m_interrupted = true;
                auto closure = m_screen.ExitLoopClosure();
                closure();
            },
            m_model.button_style)});

        this->Add(Container::Vertical({m_buttons}));
    }

    auto LoadingPanel::Render() -> Element
    {
        auto box_text = String("Analyzing contents of ") + m_model.source_path;
        auto stl_text = box_text.stlStringInUTF8();
        auto formatted_total_bytes = format_total_bytes();
        auto counter_text =
            String::initWithFormat("total files: %u   total bytes: %@", m_model.total_files, &formatted_total_bytes);

        auto top_box = hbox({filler(), text(stl_text) | color(m_model.text_color), separator(),
                             spinner(m_spinner_charset, m_spinner_index) | color(m_model.text_color)});
        return main_ui_element(
            {filler(),
             hbox({filler(),
                   vbox({filler(), top_box, separator(),
                         hbox({filler(), text(counter_text.stlString()) | color(m_model.text_color), filler()}),
                         separator(), hbox({filler(), m_buttons->Render(), filler()}) | color(m_model.text_color),
                         filler()}) |
                       border | bgcolor(m_model.foreground_window_background_color) |
                       color(m_model.foreground_window_foreground_color),
                   filler()}),
             filler()});
    }

    void LoadingPanel::Refresh()
    {
        auto spinner_tickler = [this]() {
            while (! m_load_thread_finished)
            {
                m_spinner_index++;
                m_screen.PostEvent(Event::Custom);
                Sleep(std::chrono::milliseconds(67));
            }
            m_spinner_index = 0;
            m_screen.PostEvent(Event::Custom);
        };

        std::thread spinner_tickler_thread{spinner_tickler};
        spinner_tickler_thread.detach();

        if (! m_load_thread_started)
        {
            std::function<void()> load_function{};

            if (m_model.file_manager.fileExistsAtPath(m_model.source_path))
            {
                load_function = [this]() {
                    m_model.total_files = 1;
                    const auto properties = m_model.file_manager.propertiesForItemAtPath(m_model.source_path);
                    m_model.total_bytes = properties.size;
                    m_copy_panel->Refresh();
                    m_model.set_current_panel(DataModel::ActivePanel::COPY);
                    m_load_thread_finished = true;
                };
            }
            else if (m_model.file_manager.directoryExistsAtPath(m_model.source_path))
            {
                load_function = [this]() {
                    m_model.file_manager.walkItemsAtPath(
                        true, m_model.source_path, [this](const String & path) -> bool {
                            // Ignore directories, only look for actual files.
                            if (m_model.file_manager.directoryExistsAtPath(path))
                            {
                                return true;
                            }

                            auto properties = m_model.file_manager.propertiesForItemAtPath(path);
                            m_model.total_bytes += properties.size;
                            m_model.total_files += 1;
                            return true;
                        });
                    m_copy_panel->Refresh();
                    m_model.set_current_panel(DataModel::ActivePanel::COPY);
                    m_load_thread_finished = true;
                };
            }

            std::thread load_thread{load_function};
            load_thread.detach();
            m_load_thread_started = true;
        }
    }

    auto LoadingPanel::format_total_bytes() -> String
    {
        constexpr uint64_t KILO = 1024;
        constexpr uint64_t MEGA = 1024 * KILO;
        constexpr uint64_t GIGA = 1024 * MEGA;
        constexpr uint64_t TERRA = 1024 * GIGA;
        constexpr uint64_t PETA = 1024 * TERRA;

        String formatted_bytes{};

        auto format = [](uint64_t bytes, uint64_t divisor, String & label) -> String {
            const auto divided_value = static_cast<double>(bytes) / static_cast<double>(divisor);
            return String::initWithFormat("%g %@", divided_value, &label);
        };

        uint64_t divisor{};
        String label{};

        if (m_model.total_bytes / PETA > 0)
        {
            divisor = PETA;
            label = "Pb";
        }
        else if (m_model.total_bytes / TERRA > 0)
        {
            divisor = TERRA;
            label = "Tb";
        }
        else if (m_model.total_bytes / GIGA > 0)
        {
            divisor = GIGA;
            label = "Gb";
        }
        else if (m_model.total_bytes / MEGA > 0)
        {
            divisor = MEGA;
            label = "Mb";
        }
        else if (m_model.total_bytes / KILO > 0)
        {
            divisor = KILO;
            label = "Kb";
        }
        else
        {
            divisor = 1;
            label = "B";
        }

        return format(m_model.total_bytes, divisor, label);
    }

} // namespace copy
