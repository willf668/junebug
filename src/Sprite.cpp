#include "Sprite.h"
#include "Actors.h"
#include "Game.h"
#include "Camera.h"
#include "Files.h"

#include <filesystem>
#include <algorithm>
#include <iostream>
namespace fs = std::filesystem;

using namespace junebug;

Sprite::Sprite()
{
}

Sprite::~Sprite()
{
}

void Sprite::SetOrigin(SpriteOrigin origin, const Vec2<int> &offset)
{
    switch (origin)
    {
    case SpriteOrigin::TopLeft:
        mOrigin = Vec2<int>::Zero;
        break;
    case SpriteOrigin::TopRight:
        mOrigin.x = mTexSize.x;
        break;
    case SpriteOrigin::BottomLeft:
        mOrigin.y = mTexSize.y;
        break;
    case SpriteOrigin::BottomRight:
        mOrigin = mTexSize;
        break;
    case SpriteOrigin::Center:
        mOrigin = mTexSize / 2;
        break;
    }

    mOrigin += offset;
}

void Sprite::Draw(Camera *cam, SDL_Renderer *renderer, const Vec2<float> &pos, const Vec2<int> &partPos, const Vec2<int> &partSize, int frame, const SpriteProperties &properties)
{
    if (frame < 0 || frame >= mTextures.size())
        return;
    SDL_Texture *texture = mTextures[frame];
    if (texture && cam)
    {
        SDL_Rect src;
        src.w = static_cast<int>(partSize.x);
        src.h = static_cast<int>(partSize.y);
        src.x = static_cast<int>(partPos.x);
        src.y = static_cast<int>(partPos.y);

        SDL_Rect dest;
        dest.w = static_cast<int>(partSize.x * properties.scale.x * cam->GetZoom());
        dest.h = static_cast<int>(partSize.y * properties.scale.y * cam->GetZoom());

        Vec2<float> dPos = GetDrawPosition(pos - mOrigin * properties.scale);
        if (properties.roundToCamera)
        {
            dPos.x = round(dPos.x);
            dPos.y = round(dPos.y);
        }
        dest.x = static_cast<int>(dPos.x);
        dest.y = static_cast<int>(dPos.y);

        SDL_SetTextureColorMod(texture, properties.color.r, properties.color.g, properties.color.b);
        SDL_SetTextureAlphaMod(texture, properties.color.a);
        SDL_RenderCopyEx(renderer,
                         texture,
                         &src,
                         &dest,
                         -properties.rotation,
                         nullptr,
                         SDL_FLIP_NONE);
    }
}

void Sprite::AddTexture(SDL_Texture *texture)
{
    if (!texture)
        return;
    mTextures.push_back(texture);
    if (mTextures.size() == 1)
        SDL_QueryTexture(texture, nullptr, nullptr, &mTexSize.x, &mTexSize.y);
}

SDL_Texture *Sprite::LoadTextureFile(std::string &fileName)
{
    std::error_code ec;
    if (fs::is_regular_file(fileName, ec))
    {
        SDL_Texture *tex = Game::Get()->GetTexture(fileName);
        if (!tex)
            return nullptr;

        AddTexture(tex);
        if (ec)
        {
            PrintLog("Error for", fileName, "in is_regular_file:", ec.message());
            return nullptr;
        }
        return tex;
    }
    return nullptr;
}

bool Sprite::LoadMetadataFile(std::string &folder)
{
    // Load json
    Json *json = nullptr;
    try
    {
        json = new Json(folder + "/info.json");
    }
    catch (std::exception &e)
    {
        PrintLog("Sprite metadata", folder, "errored with", e.what());
        return false;
    }
    if (!json || !json->IsValid())
    {
        print("Sprite metadata", folder, "is invalid");
        if (json)
            delete json;
        return false;
    }

    // Don't unload textures from the sprite level; just clear the sprite's list
    mTextures.clear();
    std::vector<std::string> frames = Json::GetStringArray(json, "frames");
    for (auto &frame : frames)
    {
        std::string fileName = folder + "/" + frame;
        if (!LoadTextureFile(fileName))
        {
            PrintLog("Failed to load frame", fileName);
        }
    }

    delete json;
    return true;
}

void Sprite::AddAnimation(const std::string &name,
                          const std::vector<SDL_Texture *> &images)
{
    mAnims.emplace(name, images);
}