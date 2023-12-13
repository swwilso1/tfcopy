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
#include "utilities.hpp"

namespace copy
{

    CopyPanel::CopyPanel(screen_type & screen, model_type & model) : BasePanel(screen, model)
    {
        m_progress_meter.set_callback([this](auto percentage) {
            // percentage should be an integer between 0-100 inclusive.
            m_percent_files_copied = static_cast<decltype(m_percent_files_copied)>(percentage) / 100;
            m_screen.PostEvent(Event::Custom);
        });

        m_current_file_progress_meter.set_callback([this](auto percentage) {
            // percentage should be an integer between 0-100 inclusive.
            m_percent_current_file_copied = static_cast<decltype(m_percent_current_file_copied)>(percentage) / 100;
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
        std::lock_guard<std::mutex> lock(m_progress_message_mutex);
        const auto duration = duration_cast<std::chrono::milliseconds>(SystemDate{} - m_start_copy_time);

        m_bytes_per_second = m_bytes_copied / (static_cast<double>(duration.count()) / 1000);

        const auto duration_text = m_duration_formatter.string_from_duration(duration);
        const auto text_for_file_progress = String::initWithFormat("%u/%u files", m_current_files, m_model.total_files);
        const auto formatted_bytes_per_second = format_total_bytes(m_bytes_per_second);
        const auto text_for_copy_rate = String::initWithFormat("%@/sec", &formatted_bytes_per_second);

        const auto remaining_time = static_cast<double>(m_model.bytes_remaining) / m_bytes_per_second;
        const auto remaining_milliseconds = std::chrono::milliseconds(static_cast<uint64_t>(remaining_time * 1000));
        const auto formatted_remaining_time = m_duration_formatter.string_from_duration(remaining_milliseconds);
        const auto text_for_time_remaining = String::initWithFormat("remaining: %@", &formatted_remaining_time);

        const auto individual_file_progress_box =
            hbox({gauge(m_percent_current_file_copied) | color(m_model.text_color)});
        const auto overall_file_progress_box = hbox({gauge(m_percent_files_copied) | color(m_model.text_color)});
        const auto statistics_box =
            hbox({filler(), separator(), text(duration_text.stlString()) | color(m_model.text_color), separator(),
                  text(text_for_file_progress.stlString()) | color(m_model.text_color), separator(),
                  text(text_for_copy_rate.stlString()) | color(m_model.text_color), separator(),
                  text(text_for_time_remaining.stlString()) | color(m_model.text_color), separator(), filler()});

        return main_ui_element(
            {filler(),
             hbox(
                 {filler(),
                  vbox({filler(),
                        hbox({text(m_progress_message) | size(WIDTH, EQUAL, 70) | color(m_model.text_color), filler()}),
                        separator(), individual_file_progress_box, separator(), overall_file_progress_box, separator(),
                        statistics_box, separator(),
                        hbox({filler(), m_buttons->Render(), filler()}) | color(m_model.text_color), filler()}) |
                      border | bgcolor(m_model.foreground_window_background_color) |
                      color(m_model.foreground_window_foreground_color),
                  filler()}),
             filler()});
    }

    void CopyPanel::Refresh()
    {
        if (! m_copy_thread_started)
        {

            std::function<void()> copy_function{};

            LOG(LogPriority::Info, "source path %@ destination path %@", m_model.source_path, m_model.destination_path)

            if (m_file_manager.fileExistsAtPath(m_model.source_path))
            {
                if (m_file_manager.directoryExistsAtPath(m_model.destination_path))
                {
                    auto base_name = m_file_manager.baseNameOfItemAtPath(m_model.source_path);
                    m_model.destination_path += FileManager::pathSeparator + base_name;
                }

                auto properties = m_file_manager.propertiesForItemAtPath(m_model.source_path);
                m_progress_meter.set_total(m_model.total_bytes);
                m_current_file_progress_meter.set_total(m_model.total_bytes);

                copy_function = [this] {
                    Sleep(std::chrono::milliseconds(500));
                    m_start_copy_time = SystemDate{};

                    auto notifier = ItemCopier::notifier_type{[this](auto & size) {
                        m_progress_meter.increment_by(size);
                        m_progress_meter.notify();
                        m_model.bytes_remaining -= size;
                        m_bytes_copied += static_cast<decltype(m_bytes_copied)>(size);

                        m_current_file_progress_meter.increment_by(size);
                        m_current_file_progress_meter.notify();
                    }};

                    auto interrupter = [this]() -> bool {
                        return m_interrupted;
                    };

                    auto base_file_name = m_file_manager.baseNameOfItemAtPath(m_model.source_path);

                    update_progress_message("Copying " + base_file_name);

                    ItemCopier copier{m_model.source_path, m_model.destination_path};
                    copier.set_notifier(notifier);
                    copier.set_interrupter(interrupter);
                    copier.copy();

                    const auto permissions = m_file_manager.permissionsForItemAtPath(m_model.source_path);
                    m_file_manager.setPermissionsForItemAtPath(m_model.destination_path, permissions);

                    update_progress_message("Finished Copying!");
                    m_copy_thread_finished = true;

                    m_file_progress_notifier.notify(1);
                    m_screen.PostEvent(Event::Custom);
                };
            }
            else if (m_file_manager.directoryExistsAtPath(m_model.source_path))
            {
                if (m_file_manager.fileExistsAtPath(m_model.destination_path))
                {
                    auto message = String::initWithFormat("cannot overwrite non-directory '%@' with directory '%@'",
                                                          &m_model.destination_path, &m_model.source_path);
                    update_progress_message(message);
                    m_copy_thread_finished = true;
                    return;
                }

                m_progress_meter.set_total(m_model.total_bytes);
                copy_function = [this] {
                    m_start_copy_time = SystemDate{};
                    bool encounteredError{false};

                    auto notifier = ItemCopier::notifier_type{[this](auto & size) {
                        m_progress_meter.increment_by(size);
                        m_progress_meter.notify();
                        m_model.bytes_remaining -= size;
                        m_bytes_copied += static_cast<decltype(m_bytes_copied)>(size);

                        m_current_file_progress_meter.increment_by(size);
                        m_current_file_progress_meter.notify();
                    }};

                    auto interrupter = [this]() -> bool {
                        return m_interrupted;
                    };

                    m_file_manager.walkItemsAtPath(
                        true, m_model.source_path,
                        [this, &notifier, &interrupter, &encounteredError](const String & path) -> bool {
                            auto directory_path_part = m_file_manager.dirNameOfItemAtPath(path);
                            auto base_file_name = m_file_manager.baseNameOfItemAtPath(path);

                            String destination_path{};
                            if (directory_path_part == m_model.source_path)
                            {
                                destination_path = m_model.destination_path;
                            }
                            else
                            {
                                auto sub_directory_part =
                                    directory_path_part.substringFromIndex(m_model.source_path.length() + 1);
                                destination_path =
                                    m_model.destination_path + FileManager::pathSeparator + sub_directory_part;
                            }

                            String full_destination_path{destination_path + FileManager::pathSeparator +
                                                         base_file_name};

                            auto fix_problematic_path_component = [](const String & component) -> String {
                                auto tmp_component = component.stringByReplacingOccurrencesOfStringWithString(":", "_");
                                tmp_component =
                                    tmp_component.stringByReplacingOccurrencesOfStringWithString("\"", "\\\\\"");
                                tmp_component =
                                    tmp_component.stringByReplacingOccurrencesOfStringWithString(" ", "\\ ");
                                LOG(LogPriority::Info, tmp_component)
                                if (tmp_component.last() == ' ')
                                {
                                    tmp_component = tmp_component.substringToIndex(tmp_component.length() - 1);
                                }
                                return tmp_component;
                            };

                            auto fix_problematic_file_paths =
                                [fix_problematic_path_component](const String & path) -> String {
                                auto path_components = path.substringsThatDoNotMatchString(FileManager::pathSeparator);
                                String::string_array_type new_path_components{};
                                for (auto & component : path_components)
                                {
                                    if (component.empty())
                                    {
                                        continue;
                                    }
                                    new_path_components.push_back(fix_problematic_path_component(component));
                                }
                                String new_path{FileManager::pathSeparator};
                                for (String::string_array_type::size_type i = 0; i < new_path_components.size(); i++)
                                {
                                    new_path += new_path_components[i];
                                    if (i < new_path_components.size() - 1)
                                    {
                                        new_path += FileManager::pathSeparator;
                                    }
                                }
                                return new_path;
                            };

                            if (m_model.fix_problematic_file_paths)
                            {
                                destination_path = fix_problematic_file_paths(destination_path);
                                full_destination_path = fix_problematic_file_paths(full_destination_path);
                            }

                            if (m_file_manager.directoryExistsAtPath(path))
                            {
                                if (! m_file_manager.directoryExistsAtPath(full_destination_path))
                                {
                                    try
                                    {
                                        m_file_manager.createDirectoriesAtPath(full_destination_path);
                                    }
                                    catch (std::exception & e)
                                    {
                                        encounteredError = true;
                                        LOG(LogPriority::Critical,
                                            "Error creating directory: " + full_destination_path + ": " + e.what())
                                        update_progress_message("Error creating directory: " + full_destination_path +
                                                                ": " + e.what());
                                        return false;
                                    }
                                }
                                return true;
                            }

                            if (! m_file_manager.directoryExistsAtPath(destination_path))
                            {
                                try
                                {
                                    m_file_manager.createDirectoriesAtPath(destination_path);
                                }
                                catch (std::exception & e)
                                {
                                    encounteredError = true;
                                    LOG(LogPriority::Critical,
                                        "Error creating directory: " + destination_path + ": " + e.what())
                                    update_progress_message("Error creating directory: " + destination_path + ": " +
                                                            e.what());
                                    return false;
                                }
                            }

                            update_progress_message("Copying " + base_file_name);

                            auto path_properties = m_model.file_manager.propertiesForItemAtPath(path);

                            m_current_file_progress_meter.set_total(path_properties.size);
                            m_current_file_progress_meter.reset();

                            try
                            {
                                auto copier = ItemCopier{path, full_destination_path};
                                copier.set_notifier(notifier);
                                copier.set_interrupter(interrupter);
                                copier.copy();
                            }
                            catch (std::exception & e)
                            {
                                encounteredError = true;
                                LOG(LogPriority::Critical,
                                    "Error copying: " + path + " to " + full_destination_path + e.what())
                                update_progress_message("Error copying " + path + " to " + full_destination_path +
                                                        ": " + e.what());
                                return false;
                            }

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

                    if (! encounteredError)
                    {
                        update_progress_message("Finished Copying!");
                    }
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
        std::lock_guard<std::mutex> lock(m_progress_message_mutex);
        m_progress_message = message.stlStringInUTF8();
        m_screen.PostEvent(Event::Custom);
    }

} // namespace copy
