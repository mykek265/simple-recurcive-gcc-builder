#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <iterator>
#include <fstream>

namespace fs = std::filesystem;

namespace u8
{
    char const* ref(char8_t const* ch)
    {
        return reinterpret_cast<char const*>(ch);
    }

    char const* ref(std::u8string& text)
    {
        return reinterpret_cast<char const*>(text.c_str());
    }

    size_t len(std::u8string& text)
    {
        char const* p = ref(text);
        size_t count = 0;

        while (*p)
        {
            if ((*p & 0b10000000) == 0)
            {
                ++count;
            }
            else if ((*p & 0b11000000) == 0b11000000)
            {
                ++count;
            }

            ++p;
        }

        return count;
    }

    std::u8string subs(std::u8string& text, size_t from, size_t to)
    {
        std::u8string result;
        char const* p = ref(text);
        size_t count = 0;
        to += from;

        while(*p)
        {
            if ((*p & 0b10000000) == 0)
            {
                ++count;
            }
            else if ((*p & 0b11000000) == 0b11000000)
            {
                ++count;
            }

            if (count > to)
            {
                break;
            }

            if (count >= from)
            {
                result += *p;
            }
            ++p;
        }

        return result;
    }

    std::u8string subs(std::u8string& text, size_t to)
    {
        return subs(text, 0, to);
    }

    std::u8string get(std::u8string& text, size_t idx)
    {
        return subs(text, idx, 1);
    }

    std::u8string read_all_text(fs::path path)
    {
        auto u8path = path.u8string();
        std::basic_ifstream<char> fstr(
                    u8::ref(u8path),
                    std::ios::in | std::ios::binary
                );

        if (!fstr.good()) return u8"";

        return std::u8string(
                    std::istreambuf_iterator<char>(fstr),
                    std::istreambuf_iterator<char>()
                );
    }
}

// g++ main.cpp -std=c++20 -o build
int main()
{
    std::u8string result = u8"g++";
    std::u8string libs;
    for (auto const &entry : fs::recursive_directory_iterator(fs::current_path()))
    {
        auto path = entry.path();
        if (!path.has_extension()) continue;
        auto ext = path.extension().string();
        if (ext == ".libs")
        {
            libs += u8" " + u8::read_all_text(path);
        }
        if (ext != ".cpp") continue;

        result += u8" \"" + entry.path().u8string() + u8'\"';
    }
    result += u8" -o result ";
    result += libs;
    system(u8::ref(result));
    return 0;
}

