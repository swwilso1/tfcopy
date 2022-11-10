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

#include "TFFoundation.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "data_model.hpp"
#include "copy_panel.hpp"
#include "startup_panel.hpp"

using namespace TF::Foundation;
using namespace ftxui;
using namespace copy;

int main(int argc, const char ** argv)
{
    auto data_model = DataModel{};

    LOG_TO_FILE_AT_PATH("/tmp/tfcopy.log");

    ArgumentParser parser{};
    parser.setName(data_model.tool_name);
    parser.setVersion(data_model.tool_version);
    parser.setExitOnHelp(true);
    parser.addStoreTrueArgument({"-v", "--version"}, "", data_model.tool_name + " Version", false);
    parser.addPositionalArgument("source", ArgumentType::String, "Source path", false);
    parser.addPositionalArgument("destination", ArgumentType::String, "Destination path", false);

    if (! parser.parseArgs(argc, argv))
    {
        return -1;
    }

    bool display_version{false};
    parser.getValueForArgument("version", display_version);

    String source_path{};
    if (parser.hasValueForArgument("source"))
    {
        parser.getValueForArgument("source", source_path);
    }

    String destination_path{};
    if (parser.hasValueForArgument("destination"))
    {
        parser.getValueForArgument("destination", destination_path);
    }

    if (display_version)
    {
        std::cout << data_model.tool_version << std::endl;
        return 0;
    }

    if (source_path.empty())
    {
        std::cout << "Source path not given" << std::endl;
        return -1;
    }

    if (destination_path.empty())
    {
        std::cout << "Destination path not given" << std::endl;
        return -1;
    }

    FileManager file_manager{};

    if (! file_manager.itemExistsAtPath(source_path))
    {
        std::cout << source_path << " does not exist!" << std::endl;
        return -1;
    }

    auto screen = ScreenInteractive::Fullscreen();
    auto startup_component = std::make_shared<StartupPanel>(screen, data_model);
    auto copy_component = std::make_shared<CopyPanel>(screen, data_model);

    startup_component->set_copy_panel(copy_component);
    copy_component->set_paths(source_path, destination_path);

    data_model.set_current_panel(DataModel::ActivePanel::STARTUP);

    auto tab_component =
        Container::Tab({startup_component, copy_component}, data_model.get_panel_selector());

    screen.Loop(tab_component);

    return 0;
}
