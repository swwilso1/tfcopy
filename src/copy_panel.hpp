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

#ifndef COPY_PANEL_HPP
#define COPY_PANEL_HPP

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <mutex>

#include "TFFoundation.hpp"
#include "data_model.hpp"
#include "base_panel.hpp"

using namespace TF::Foundation;
using namespace ftxui;

namespace copy
{

    class CopyPanel : public BasePanel
    {
    public:
        CopyPanel(screen_type &, model_type &);

        [[nodiscard]] auto Render() -> Element override;

        void Refresh() override;

        void set_paths(const String & source, const String & destination)
        {
            m_source_path = source;
            m_destination_path = destination;
        }

    private:
        using size_type = uint64_t;

        Component m_buttons{};

        String m_source_path{};
        String m_destination_path{};

        FileManager m_file_manager{};
        ProgressMeter<size_type> m_progress_meter{};
        BasicProgressNotifier<size_type> m_file_progress_notifier{};
        bool m_copy_thread_started{false};
        bool m_copy_thread_finished{false};
        bool m_interrupted{false};

        size_type m_total_files{0};
        size_type m_current_files{0};
        float m_percent_files_copied{0.0};
        std::string m_progress_message{};
        std::mutex m_progress_message_mutex{};

        SystemDate m_start_copy_time{};
        DurationFormatter m_duration_formatter{"hh:mm:ss"};

        void update_progress_message(const String & message);
    };

} // namespace copy

#endif // COPY_PANEL_HPP
