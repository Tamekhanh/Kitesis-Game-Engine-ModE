#include "engine/renderer/scene_serializer.h"
#include "engine/renderer/sprite_component.h"
#include "engine/renderer/texture2d.h"
#include "engine/core/logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <unordered_map>
#include "engine/renderer/animation_component.h"
namespace engine::renderer
{

    using json = nlohmann::json;
    using engine::core::GameObject;

    static json SerializeGameObject(GameObject &obj)
    {
        json j;
        j["name"] = obj.Name();
        j["active"] = obj.active;

        auto &t = obj.transform;
        j["transform"]["position"] = {t.position.x, t.position.y};
        j["transform"]["rotation"] = t.rotation;
        j["transform"]["scale"] = {t.scale.x, t.scale.y};

        json componentsJson = json::array();
        for (auto &comp : obj.Components())
        {
            if (auto *anim = dynamic_cast<AnimationComponent *>(comp.get()))
            {
                json c;
                c["type"] = "AnimationComponent";
                json framesJson = json::array();
                for (auto &f : anim->Frames())
                {
                    json fj;
                    fj["texturePath"] = f.texturePath;
                    fj["frameIndex"] = f.frameIndex;
                    framesJson.push_back(fj);
                }
                c["frames"] = framesJson;
                c["frameDuration"] = anim->FrameDuration();
                c["looping"] = anim->Looping();
                componentsJson.push_back(c);
            }
        }
        j["components"] = componentsJson;

        json childrenJson = json::array();
        for (auto &child : obj.Children())
        {
            childrenJson.push_back(SerializeGameObject(*child));
        }
        j["children"] = childrenJson;

        return j;
    }

    void SceneSerializer::Save(GameObject &root, const std::filesystem::path &path)
    {
        json sceneJson;
        sceneJson["children"] = json::array();
        for (auto &child : root.Children())
        {
            sceneJson["children"].push_back(SerializeGameObject(*child));
        }

        std::ofstream out(path);
        if (!out.is_open())
        {
            engine::core::Logger::Error("Khong the ghi scene ra file: " + path.string());
            return;
        }
        out << sceneJson.dump(4);
        engine::core::Logger::Info("Da luu scene: " + path.string());
    }

    static void DeserializeGameObject(const json &j, GameObject &parent,
                                      SpriteRenderer &renderer,
                                      std::vector<std::unique_ptr<Texture2D>> &textureLibrary,
                                      std::unordered_map<std::string, Texture2D *> &textureCache)
    {
        std::string name = j.value("name", "GameObject");
        auto owned = std::make_unique<GameObject>(name);
        GameObject *obj = parent.AddChild(std::move(owned));
        obj->active = j.value("active", true);

        if (j.contains("transform"))
        {
            auto &t = j["transform"];
            auto pos = t.value("position", std::vector<float>{0.0f, 0.0f});
            obj->transform.position = {pos[0], pos[1]};
            obj->transform.rotation = t.value("rotation", 0.0f);
            auto scale = t.value("scale", std::vector<float>{1.0f, 1.0f});
            obj->transform.scale = {scale[0], scale[1]};
        }

        if (j.contains("components"))
        {
            for (auto &c : j["components"])
            {
                std::string type = c.value("type", "");
                if (type == "SpriteComponent")
                {
                    float w = c.value("width", 50.0f);
                    float h = c.value("height", 50.0f);
                    std::string texturePath = c.value("texturePath", "");

                    auto *sprite = obj->AddComponent<SpriteComponent>(renderer, w, h);

                    if (!texturePath.empty())
                    {
                        Texture2D *tex = nullptr;
                        auto it = textureCache.find(texturePath);
                        if (it != textureCache.end())
                        {
                            tex = it->second;
                        }
                        else
                        {
                            textureLibrary.push_back(std::make_unique<Texture2D>(texturePath));
                            tex = textureLibrary.back().get();
                            textureCache[texturePath] = tex;
                        }
                        sprite->BindTexture(*tex, texturePath);
                    }
                }
                if (type == "AnimationComponent")
                {
                    auto *anim = obj->AddComponent<AnimationComponent>();
                    if (c.contains("frames"))
                    {
                        for (auto &fj : c["frames"])
                        {
                            std::string path = fj.value("texturePath", "");
                            int idx = fj.value("frameIndex", 0);
                            anim->AddFrame(path, idx);
                        }
                    }
                    anim->SetFrameDuration(c.value("frameDuration", 0.1f));
                    anim->SetLooping(c.value("looping", true));
                }
            }
        }

        if (j.contains("children"))
        {
            for (auto &childJson : j["children"])
            {
                DeserializeGameObject(childJson, *obj, renderer, textureLibrary, textureCache);
            }
        }
    }

    void SceneSerializer::Load(GameObject &root, const std::filesystem::path &path,
                               SpriteRenderer &renderer,
                               std::vector<std::unique_ptr<Texture2D>> &textureLibrary)
    {
        if (!std::filesystem::exists(path))
        {
            engine::core::Logger::Error("Khong tim thay file scene: " + path.string());
            return;
        }

        std::ifstream in(path);
        json sceneJson;
        try
        {
            in >> sceneJson;
        }
        catch (const std::exception &e)
        {
            engine::core::Logger::Error(std::string("Loi doc file scene: ") + e.what());
            return;
        }

        root.ClearChildren();

        std::unordered_map<std::string, Texture2D *> textureCache;
        if (sceneJson.contains("children"))
        {
            for (auto &childJson : sceneJson["children"])
            {
                DeserializeGameObject(childJson, root, renderer, textureLibrary, textureCache);
            }
        }

        engine::core::Logger::Info("Da mo scene: " + path.string());
    }

} // namespace engine::renderer