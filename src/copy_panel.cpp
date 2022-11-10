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

#include <functional>
#include "copy_panel.hpp"

namespace copy
{

    CopyPanel::CopyPanel(screen_type & screen, model_type & model) : BasePanel(screen, model)
    {
        m_progress_meter.set_callback([this](auto percentage) {
            // percentage should be an integer between 0-100 inclusive.
            m_percent_files_copied = static_cast<decltype(m_percent_files_copied)>(percentage) / 100;
            m_screen.PostEvent(Event::Custom);
        });

        m_file_progress_notifier.set_callback([this](auto & amount) {
            m_current_files += amount;
        });

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

    auto CopyPanel::Render() -> Element
    {
        auto duration = duration_cast<std::chrono::milliseconds>(SystemDate{} - m_start_copy_time);
        auto duration_text = m_duration_formatter.string_from_duration(duration);
        auto text_for_file_progress = String::initWithFormat("%u/%u files", m_current_files, m_total_files);
        auto top_box = hbox({gauge(m_percent_files_copied), separator(), text(text_for_file_progress.stlString()),
                             separator(), text(duration_text.stlString())});
        return main_ui_element({filler(),
                                hbox({filler(),
                                      vbox({filler(), top_box, separator(),
                                            hbox({text(m_progress_message) | size(WIDTH, EQUAL, 70), filler()}),
                                            separator(), hbox({filler(), m_buttons->Render(), filler()}), filler()}) |
                                          border | bgcolor(m_model.foreground_window_background_color),
                                      filler()}),
                                filler()});
    }

    void CopyPanel::Refresh()
    {
        if (! m_copy_thread_started)
        {

            std::function<void()> copy_function{};

            LOG(LogPriority::Info, "source path %@ destination path %@", m_source_path, m_destination_path)

            if (m_file_manager.fileExistsAtPath(m_source_path))
            {
                if (m_file_manager.directoryExistsAtPath(m_destination_path))
                {
                    auto base_name = m_file_manager.baseNameOfItemAtPath(m_source_path);
                    m_destination_path += FileManager::pathSeparator + base_name;
                }

                auto properties = m_file_manager.propertiesForItemAtPath(m_source_path);
                m_progress_meter.set_total(properties.size);
                m_total_files = 1;

                copy_function = [this] {
                    Sleep(std::chrono::milliseconds(500));
                    m_start_copy_time = SystemDate{};

                    auto notifier = ItemCopier::notifier_type{[this](auto & size) {
                        m_progress_meter.increment_by(size);
                        m_progress_meter.notify();
                    }};

                    auto interrupter = [this]() -> bool {
                        return m_interrupted;
                    };

                    auto base_file_name = m_file_manager.baseNameOfItemAtPath(m_source_path);

                    update_progress_message("Copying " + base_file_name);

                    ItemCopier copier{m_source_path, m_destination_path};
                    copier.set_notifier(notifier);
                    copier.set_interrupter(interrupter);
                    copier.copy();

                    auto permissions = m_file_manager.permissionsForItemAtPath(m_source_path);
                    m_file_manager.setPermissionsForItemAtPath(m_destination_path, permissions);

                    update_progress_message("Finished Copying!");
                    m_copy_thread_finished = true;

                    m_file_progress_notifier.notify(1);
                    m_screen.PostEvent(Event::Custom);
                };
            }
            else if (m_file_manager.directoryExistsAtPath(m_source_path))
            {
                if (m_file_manager.fileExistsAtPath(m_destination_path))
                {
                    update_progress_message("Unable to copy a directory to a file!");
                    m_copy_thread_finished = true;
                    return;
                }

                size_type bytes_to_copy{0};

                m_file_manager.walkItemsAtPath(true, m_source_path,
                                               [this, &bytes_to_copy](const String & path) -> bool {
                                                   // Ignore directories, only look for actual files.
                                                   if (m_file_manager.directoryExistsAtPath(path))
                                                   {
                                                       return true;
                                                   }

                                                   auto properties = m_file_manager.propertiesForItemAtPath(path);
                                                   bytes_to_copy += properties.size;
                                                   m_total_files += 1;
                                                   return true;
                                               });

                m_progress_meter.set_total(bytes_to_copy);
                copy_function = [this] {
                    m_start_copy_time = SystemDate{};

                    auto notifier = ItemCopier::notifier_type{[this](auto & size) {
                        m_progress_meter.increment_by(size);
                        m_progress_meter.notify();
                    }};

                    auto interrupter = [this]() -> bool {
                        return m_interrupted;
                    };

                    m_file_manager.walkItemsAtPath(
                        true, m_source_path, [this, &notifier, &interrupter](const String & path) -> bool {
                            auto directory_path_part = m_file_manager.dirNameOfItemAtPath(path);
                            auto base_file_name = m_file_manager.baseNameOfItemAtPath(path);

                            String destination_path{};
                            if (directory_path_part == m_source_path)
                            {
                                destination_path = m_destination_path;
                            }
                            else
                            {
                                auto sub_directory_part =
                                    directory_path_part.substringFromIndex(m_source_path.length() + 1);
                                destination_path = m_destination_path + FileManager::pathSeparator + sub_directory_part;
                            }

                            auto full_destination_path = destination_path + FileManager::pathSeparator + base_file_name;

                            if (m_file_manager.directoryExistsAtPath(path))
                            {
                                if (! m_file_manager.directoryExistsAtPath(full_destination_path))
                                {
                                    m_file_manager.createDirectoriesAtPath(full_destination_path);
                                }
                                return true;
                            }

                            if (! m_file_manager.directoryExistsAtPath(destination_path))
                            {
                                m_file_manager.createDirectoriesAtPath(destination_path);
                            }

                            update_progress_message("Copying " + base_file_name);

                            auto copier = ItemCopier{path, full_destination_path};
                            copier.set_notifier(notifier);
                            copier.set_interrupter(interrupter);
                            copier.copy();

                            if (m_interrupted)
                            {
                                return false;
                            }

                            auto permissions = m_file_manager.permissionsForItemAtPath(path);
                            m_file_manager.setPermissionsForItemAtPath(full_destination_path, permissions);

                            m_file_progress_notifier.notify(1);
                            m_screen.PostEvent(Event::Custom);
                            return true;
                        });

                    update_progress_message("Finished Copying!");
                    m_copy_thread_finished = true;
                };
            }
            else
            {
                LOG(LogPriority::Info, "Did not catch a valid case")
            }

            std::thread copy_function_thread{copy_function};
            copy_function_thread.detach();
            m_copy_thread_started = true;

            std::thread timer_tickle_thread{[this] {
                while (! m_copy_thread_finished)
                {
                    Sleep(std::chrono::seconds(1));
                    m_screen.PostEvent(Event::Custom);
                }
            }};
            timer_tickle_thread.detach();
        }
    }

    void CopyPanel::update_progress_message(const String & message)
    {
        m_progress_message = message.stlStringInUTF8();
        m_screen.PostEvent(Event::Custom);
    }

} // namespace copy
