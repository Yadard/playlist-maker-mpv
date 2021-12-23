#include <concepts>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <array>
#include <string>
#include <string_view>
#include <utility>

#include "regex.hpp"

namespace fs = std::filesystem;

struct string : std::string {
    using std::string::string;

    string &replace_all(const char ch, std::string_view str) {
        for (auto it = this->begin(); it != this->end(); it++) {
            if (*it == ch) {
                this->replace(it, std::next(it), str);
            }
        }
        return *this;
    }
};

std::string get_series_title() {
    string title_season = "";
    fs::path cwd = fs::current_path();
    size_t diff = std::distance(cwd.begin(), cwd.end());
    auto show_title = std::next(cwd.begin(), diff - 2);
    title_season += show_title->string();
    auto season_name = std::next(cwd.begin(), diff - 1);
    title_season += R"([\W_]*?(?:)";
    title_season += season_name->string();
    title_season += R"()?[\W_]*?)";
    title_season.replace_all(' ', R"([\W_]*)");
    return title_season;
}

std::string get_regex() { return get_series_title() + R"((\d+))"; }

int main(int argc, char **argv) {
    std::array<const char *, 41> formats = {
        ".webm", ".mkv",  ".flv", ".vob",  ".ogv", ".ogg",  ".drc",
        ".gif",  ".gifv", ".mng", ".avi",  ".TS",  ".M2TS", ".MTS",
        ".qt",   ".mov",  ".wmv", ".yuv",  ".rm",  ".rmvb", ".viv",
        ".asf",  ".amv",  ".mp4", ".m4p",  ".m4v", ".mpg",  ".mpeg",
        ".m2v",  ".svi",  ".m4v", ".3gp",  ".3g2", ".mxf",  ".roq",
        ".nsv",  ".flv ", ".f4v", ".f4p ", ".f4a", ".f4b"};

    Regex::Regex re(get_regex().c_str(), PCRE2_CASELESS);
    std::vector<fs::directory_entry> episodes;
    for (auto file : fs::directory_iterator(fs::current_path())) {
        if (std::find(formats.begin(), formats.end(),
                      file.path().extension().string()) != formats.end() &&
            !file.is_directory())
            episodes.push_back(file);
    }
    std::sort(episodes.begin(), episodes.end(),
              [&](fs::directory_entry &a, fs::directory_entry &b) {
                  std::string str_a = a.path().filename().string();
                  std::string str_b = b.path().filename().string();
                  auto number_a =
                      std::stoi(std::string(re.search(str_a.c_str())[1]));
                  auto number_b =
                      std::stoi(std::string(re.search(str_b.c_str())[1]));
                  if (number_a < number_b) {
                      return true;
                  } else {
                      return false;
                  }
              });

    std::ofstream playlist("playlist.txt");
    for (auto ep : episodes) {
        playlist << ep.path().filename().string() << '\n';
    }
    playlist.close();
    system("mpv --playlist=playlist.txt");
    return 0;
}