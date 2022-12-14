#include "Utils.h"

#include <algorithm>
#include <random>
#include <sstream>

namespace junebug
{
    std::string PrintTime()
    {
        using namespace std::chrono;

        // get current time
        auto now = system_clock::now();

        // get number of milliseconds for the current second
        // (remainder after division into seconds)
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        // convert to std::time_t in order to convert to std::tm (broken time)
        auto timer = system_clock::to_time_t(now);

        // convert to broken time
        std::tm bt = __LocaltimeSafe__(timer);

        std::ostringstream oss;

        oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
        oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

        return oss.str();
    }

    SDL_Texture *GetRenderTexture(Vec2<int> size, SDL_Renderer *renderer, SDL_Texture *texture, bool enableForRenderer)
    {
        int texW, texH;
        if (texture)
            SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

        if (!texture || (texW != (int)size.x || texH != (int)size.y))
        {
            if (texture)
                SDL_DestroyTexture(texture);
            texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)size.x, (int)size.y);
        }

        if (enableForRenderer)
        {
            SDL_SetRenderTarget(renderer, texture);
            SDL_RenderClear(renderer);
        }

        return texture;
    }

    bool StringEndsWith(const std::string &str, const std::string &suffix)
    {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }

    bool IsWhitespace(const std::string &str)
    {
        return std::all_of(str.begin(), str.end(), isspace);
    }

    std::vector<std::string> StringSplit(const std::string &str, const std::string &delim)
    {
        std::vector<std::string> res;
        size_t last = 0;
        size_t next = 0;
        while ((next = str.find(delim, last)) != std::string::npos)
        {
            res.push_back(str.substr(last, next - last));
            last = next + 1;
        }
        res.push_back(str.substr(last));
        return res;
    }

    std::string StringSplitEntry(const std::string &str, const std::string &delim, int ind)
    {
        if (str.empty())
            return "";

        auto vec = StringSplit(str, delim);
        if (ind >= 0)
            return vec[Min(ind, (int)vec.size() - 1)];
        else
            return vec[Max((int)vec.size() + ind, 0)];
    }

    std::string GetFileName(const std::string &path)
    {
        size_t lastSlash = path.find_last_of("\\/");
        if (lastSlash == std::string::npos)
            return path;
        return path.substr(lastSlash + 1);
    }

    std::string TrimFileExtension(const std::string &fileName)
    {
        size_t lastDot = fileName.find_last_of(".");
        if (lastDot == std::string::npos)
            return fileName;
        return fileName.substr(0, lastDot);
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    static std::uniform_int_distribution<> dis2(8, 11);
    std::string UUID()
    {

        std::stringstream ss;
        int i;
        ss << std::hex;
        for (i = 0; i < 8; i++)
        {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 4; i++)
        {
            ss << dis(gen);
        }
        ss << "-4";
        for (i = 0; i < 3; i++)
        {
            ss << dis(gen);
        }
        ss << "-";
        ss << dis2(gen);
        for (i = 0; i < 3; i++)
        {
            ss << dis(gen);
        }
        ss << "-";
        for (i = 0; i < 12; i++)
        {
            ss << dis(gen);
        };
        return ss.str();
    }
}
