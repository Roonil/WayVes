#include "Arguments.h"
#include "CLI/CLI.hpp"
#include "Errors.h"

#include <unistd.h>

class WayVesFormatter : public CLI::Formatter {
public:
    std::string make_option_opts(const CLI::Option*) const override { return ""; }

    std::string make_option_desc(const CLI::Option* opt) const override
    {
        std::string desc = CLI::Formatter::make_option_desc(opt);
        std::string tag = typeTag(opt);
        if (tag.empty())
            return desc;

        static const bool colour = isatty(STDOUT_FILENO);
        tag = "⟨" + tag + "⟩";
        if (colour)
            tag = "\033[2;36m" + tag + "\033[0m";
        return desc.empty() ? tag : desc + "  " + tag;
    }

private:
    static std::string typeTag(const CLI::Option* opt)
    {
        if (opt->get_type_size() == 0 || opt->get_expected_max() == 0 || opt->get_type_name().empty())
            return "";

        std::string typeName = opt->get_type_name();
        std::string range;
        if (auto colon = typeName.find(':'); colon != std::string::npos) {
            auto open = typeName.find('[', colon), close = typeName.find(']', colon);
            if (open != std::string::npos && close != std::string::npos) {
                range = typeName.substr(open + 1, close - open - 1);
                if (auto dash = range.find(" - "); dash != std::string::npos)
                    range.replace(dash, 3, "–");
            }
            typeName.erase(colon);
        }

        std::string tag = CLI::detail::to_lower(typeName == "BOOLEAN" ? "bool" : typeName);
        if (!range.empty())
            tag += " · " + range;
        if (opt->get_expected_min() == 0)
            tag += opt->get_default_str().empty() ? " · optional" : " · optional, alone " + opt->get_default_str();
        else if (!opt->get_default_str().empty())
            tag += " · default " + opt->get_default_str();
        if (opt->get_required())
            tag += " · required";
        return tag;
    }
};

Arguments::Arguments(int argc, char* args[])
{

    CLI::App app { "WayVes - OpenGL-based Audio Visualiser for PipeWire on Wayland" };
    app.formatter(std::make_shared<WayVesFormatter>());

    int p = 0;

    app.set_help_flag("--help", "Show help");
    app.set_version_flag("-V,--version", "1.3.0");

    auto shaderPropertiesGroup = app.add_option_group("Shader Properties");
    auto pipeWirePropertiesGroup = app.add_subcommand("audio", "Pipe Wire Properties");
    auto instanceGroup = app.add_option_group("Instance Control");

    // TODO: Add Instance grouping as well.
    pipeWirePropertiesGroup->add_option("-a,--name", audioArguments.audioName, "Name of the Audio Thread to change")->required();
    pipeWirePropertiesGroup->add_option("-t,--target-object", audioArguments.targetObject, "PipeWire node to capture from, by its object.serial or node.name");
    pipeWirePropertiesGroup->add_option("-f,--audio-format", audioArguments.audioFormat, "Audio Format");
    pipeWirePropertiesGroup->add_option("-c,--channels", audioArguments.channels, "Number of Channels");
    pipeWirePropertiesGroup->add_option("-s,--sample-size", audioArguments.sampleSize, "Sample Size in bytes");
    pipeWirePropertiesGroup->add_option("-g,--fragment-size", audioArguments.fragmentSize, "Fragment Size in bytes");
    pipeWirePropertiesGroup->add_option("-r,--sample-rate", audioArguments.sampleRate, "Sample Rate in Hz");
    pipeWirePropertiesGroup->add_option("-S,--fft-scale", audioArguments.fftScale, "FFT Scale");
    pipeWirePropertiesGroup->add_option("-C,--fft-cutoff", audioArguments.fftCutOff, "FFT Cut-off");
    pipeWirePropertiesGroup->add_option("-m,--capture-mic", audioArguments.captureMic, "Capture Microphone Audio");
    pipeWirePropertiesGroup->add_option("-F,--apply-fft", audioArguments.applyFFT, "Apply FFT to the captured audio");

    shaderPropertiesGroup->excludes(pipeWirePropertiesGroup);

    instanceGroup->add_option("-i,--instance", instance, "Specify the Name of the Instance");
    instanceGroup->add_option("-f,--file", configFileName, "Specify the Name of the Configuration File");

    shaderPropertiesGroup->add_option("-c,--class", className, "Class Name to target");
    shaderPropertiesGroup->add_option("-z,--layer", layer, "Specify the Layer of the Shader Window");
    shaderPropertiesGroup->add_option("-x,--exclusive-layer", exclusiveLayer, "Toggle Exclusive Layer")->check(CLI::Range(0, 1));
    shaderPropertiesGroup->add_option("-l,--left-margin", marginLeft, "Specify the Left Margin");
    shaderPropertiesGroup->add_option("-r,--right-margin", marginRight, "Specify the Right Margin");
    shaderPropertiesGroup->add_option("-t,--top-margin", marginTop, "Specify the Top Margin");
    shaderPropertiesGroup->add_option("-b,--bottom-margin", marginBottom, "Specify the Bottom Margin");
    shaderPropertiesGroup->add_option("-s,--left-anchor", anchorLeft, "Specify the Left Anchor");
    shaderPropertiesGroup->add_option("-p,--right-anchor", anchorRight, "Specify the Right Anchor");
    shaderPropertiesGroup->add_option("-u,--top-anchor", anchorTop, "Specify the Top Anchor");
    shaderPropertiesGroup->add_option("-d,--bottom-anchor", anchorBottom, "Specify the Bottom Anchor");
    shaderPropertiesGroup->add_option("-v,--visibility", visibility, "Toggle Visibility, or set Visibility to the specified value")->expected(0, 1)->default_str("-1");
    shaderPropertiesGroup->add_option("-w,--width", windowWidth, "Specify Window Width");
    shaderPropertiesGroup->add_option("-h,--height", windowHeight, "Specify Window Height");

    instanceGroup->excludes(shaderPropertiesGroup);

    this->argc = argc;
    this->args = args;
    try {
        app.parse(argc, args);
    } catch (const CLI::CallForHelp& e) {
        std::cout << app.help() << std::endl;
        std::exit(0);
    }

    catch (const CLI::CallForVersion& e) {
        std::cout << app.version() << std::endl;
        std::exit(0);
    }

    catch (const CLI::ParseError& e) {
        Errors::throwError(std::string(e.what()) + ". Run with --help for the available options.");
    }
}

bool Arguments::layoutArgsSpecified()
{
    return anchorBottom.has_value() || anchorTop.has_value() || anchorLeft.has_value() || anchorRight.has_value() || layer.has_value() || marginBottom.has_value() || marginLeft.has_value() || marginRight.has_value() || marginTop.has_value() || visibility.has_value() || windowHeight.has_value() || windowWidth.has_value();
}

bool Arguments::configFileNameWithOtherArgs()
{
    return configFileName.has_value() && (layoutArgsSpecified() || className.has_value() || version.has_value());
}

void Arguments::signalGApps()
{
    if (configFileNameWithOtherArgs())
        Errors::throwError("Cannot specify Config File along with other Parameters except for instance");

    // TODO: Handle targeting layout changes under specific instances, so -i instanceName -c ... should apply changes only for shaders under that instance

    if (version.has_value()) {
        std::cout << version.value() << "\n";
        std::exit(0);
    }

    if (configFileName.has_value() || instance.has_value() || argc == 1)
        return;

    dBusHandler = new DBusHandler;
    dBusHandler->emitCLIArgs(std::string(!className.has_value() ? "" : className.value()), argc, args);

    std::exit(0);
}
