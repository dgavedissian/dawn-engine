/*
 * Dawn Engine
 * Written by David Avedissian (c) 2012-2019 (git@dga.me.uk)
 */
#include <iostream>
#include <fstream>
#include <string>
#include <nonstd/optional.hpp>
#include <args.hxx>
#include <nlohmann/json.hpp>

using std::cout;
using std::cerr;
using std::endl;
using json = nlohmann::json;

struct ResourceDirectory {
    std::string name;
    std::string path;
};

struct ResourcePackage {
    std::string name;
    std::string path;
};

void CreateManifest(const std::vector<ResourceDirectory>& dirs, const std::vector<ResourcePackage>& pkgs, std::ostream& ostream) {
    json output;
    for (const auto& package : dirs) {
        auto& object = output[package.name];
        object["type"] = 0;
        object["path"] = package.path;
    }
    for (const auto& package : pkgs) {
        auto& object = output[package.name];
        object["type"] = 1;
        object["path"] = package.path;
    }
    ostream << output;
}

int main(int argc, char** argv) {
    args::ArgumentParser parser("Dawn Engine packaging tool.", "This tool can be used to build a manifest file and 0 or more package files, given a list of input directories.");
    args::ValueFlag<std::string> output(parser, "output", "Output manifest file", {'o', "output"});
    args::ValueFlagList<std::string> dirs(parser, "name|dir", "A resource directory definition, in the form: name|dir", {"dir"});
    args::ValueFlagList<std::string> pkgs(parser, "name|pkg", "A resource package definition, in the form: name|pkg", {"pkg"});
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (const args::Help&)
    {
        cout << parser;
        return 0;
    }
    catch (const args::ParseError& e)
    {
        cerr << e.what() << endl;
        cerr << parser;
        return 1;
    }
    catch (const args::ValidationError& e)
    {
        cerr << e.what() << endl;
        cerr << parser;
        return 1;
    }

    if (!output) {
        cerr << "Missing output." << endl;
        cerr << parser;
        return 1;
    }

    std::vector<ResourceDirectory> resource_dirs;
    std::vector<ResourcePackage> resource_pkgs;
    for (const std::string& dir_option : dirs) {
        // In the format: package|path
        if (std::count(dir_option.begin(), dir_option.end(), '|') != 1) {
            cerr << "Invalid resource directory definition: " << dir_option << endl;
            cerr << parser;
            continue;
        }

        std::string package, dir;
        auto separator = dir_option.find_first_of('|');
        package = dir_option.substr(0, separator);
        dir = dir_option.substr(separator + 1);
        resource_dirs.emplace_back(ResourceDirectory{package, dir});
    }
    for (const std::string& pkg_option : pkgs) {
        // In the format: package|path
        if (std::count(pkg_option.begin(), pkg_option.end(), '|') != 1) {
            cerr << "Invalid resource package definition: " << pkg_option << endl;
            cerr << parser;
            continue;
        }

        std::string package, dir;
        auto separator = pkg_option.find_first_of('|');
        package = pkg_option.substr(0, separator);
        dir = pkg_option.substr(separator + 1);
        resource_pkgs.emplace_back(ResourcePackage{package, dir});
    }
    std::ofstream manifest_file(output.Get());
    if (!manifest_file.is_open()) {
        cerr << "Unable to open manifest file for writing: " << output.Get() << endl;
        return 1;
    }
    CreateManifest(resource_dirs, resource_pkgs, manifest_file);
    return 0;
}