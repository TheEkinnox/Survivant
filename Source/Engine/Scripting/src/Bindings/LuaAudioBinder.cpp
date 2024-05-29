#include "SurvivantScripting/Bindings/LuaAudioBinder.h"

#include "SurvivantScripting/LuaTypeRegistry.h"

#include <SurvivantAudio/AudioContext.h>

using namespace SvAudio;
using namespace SvCore::Resources;

namespace SvScripting::Bindings
{
    void LuaAudioBinder::Bind(sol::state& p_luaState)
    {
        BindContext(p_luaState);
        BindAudioHandle(p_luaState);
    }

    void LuaAudioBinder::BindContext(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "Audio";

        sol::usertype userType = p_luaState.new_usertype<AudioContext>(
            typeName,
            sol::meta_function::construct, sol::no_constructor,
            "Play", sol::overload(
                [](const AudioContext& p_self, const GenericResourceRef& p_clip)
                -> AudioHandle
                {
                    const auto&        clipType       = p_clip.GetType();
                    const std::string& registeredType = ResourceRegistry::GetInstance().GetRegisteredTypeName<AudioClip>();

                    if (!CHECK(clipType.empty() || clipType == registeredType, "Attempted to play a non-audio resource"))
                        return {};

                    return p_self.Play(p_clip);
                },
                [](const AudioContext& p_self, const GenericResourceRef& p_clip, const float p_volume)
                -> AudioHandle
                {
                    const auto&        clipType       = p_clip.GetType();
                    const std::string& registeredType = ResourceRegistry::GetInstance().GetRegisteredTypeName<AudioClip>();

                    if (!CHECK(clipType.empty() || clipType == registeredType, "Attempted to play a non-audio resource"))
                        return {};

                    return p_self.Play(p_clip, p_volume);
                },
                [](const AudioContext& p_self, const GenericResourceRef& p_clip, const float p_volume, const float p_pan)
                -> AudioHandle
                {
                    const auto&        clipType       = p_clip.GetType();
                    const std::string& registeredType = ResourceRegistry::GetInstance().GetRegisteredTypeName<AudioClip>();

                    if (!CHECK(clipType.empty() || clipType == registeredType, "Attempted to play a non-audio resource"))
                        return {};

                    return p_self.Play(p_clip, p_volume, p_pan);
                },
                [](const AudioContext& p_self, const GenericResourceRef& p_clip, const float p_volume, const float p_pan,
                   const bool          p_paused)
                -> AudioHandle
                {
                    const auto&        clipType       = p_clip.GetType();
                    const std::string& registeredType = ResourceRegistry::GetInstance().GetRegisteredTypeName<AudioClip>();

                    if (!CHECK(clipType.empty() || clipType == registeredType, "Attempted to play a non-audio resource"))
                        return {};

                    return p_self.Play(p_clip, p_volume, p_pan, p_paused);
                }
            ),
            "StopAll", &AudioContext::StopAll
        );

        p_luaState[typeName] = &AudioContext::GetInstance();
    }

    void LuaAudioBinder::BindAudioHandle(sol::state& p_luaState)
    {
        static constexpr const char* typeName = "AudioHandle";

        sol::constructors<
            AudioHandle(),
            AudioHandle(const AudioHandle&),
            AudioHandle(const GenericResourceRef&),
            AudioHandle(const ResourceRef<AudioClip>&)
        > ctor;

        sol::usertype userType = p_luaState.new_usertype<AudioHandle>(
            typeName, ctor,
            sol::meta_function::concatenation, sol::overload(
                [](const char* p_str, const AudioHandle& p_self)
                {
                    std::ostringstream oss;
                    oss << p_str << p_self.GetClip().GetPath();
                    return oss.str();
                },
                [](const AudioHandle& p_self, const char* p_str)
                {
                    std::ostringstream oss;
                    oss << p_self.GetClip().GetPath() << p_str;
                    return oss.str();
                }
            ),
            "isValid", sol::readonly_property(&AudioHandle::operator bool),
            "clip", sol::readonly_property([](const AudioHandle& p_self)-> GenericResourceRef
            {
                return p_self.GetClip();
            }),
            "isPlaying", sol::property(
                &AudioHandle::IsPlaying,
                [](AudioHandle& p_self, const bool p_state)
                {
                    if (p_state == p_self.IsPlaying())
                        return;

                    if (!p_state)
                        p_self.Stop();
                    else
                        p_self.Play();
                }
            ),
            "isPaused", sol::property(
                [](const AudioHandle& p_self)
                {
                    return !p_self.IsPlaying();
                },
                &AudioHandle::SetPaused
            ),
            "volume", sol::property(
                &AudioHandle::GetVolume,
                &AudioHandle::SetVolume
            ),
            "pan", sol::property(
                &AudioHandle::GetPan,
                &AudioHandle::SetPan
            ),
            "speed", sol::property(
                &AudioHandle::GetSpeed,
                &AudioHandle::SetSpeed
            ),
            "isLooping", sol::property(
                &AudioHandle::IsLooping,
                &AudioHandle::SetLooping
            ),
            "loopPoint", sol::property(
                &AudioHandle::GetLoopPoint,
                &AudioHandle::SetLoopPoint
            ),
            "loopCount", sol::property(
                &AudioHandle::GetLoopCount,
                &AudioHandle::SetLoopCount
            ),
            "Play", sol::overload(
                &AudioHandle::Play, [](AudioHandle& p_self)
                {
                    return p_self.Play();
                },
                [](AudioHandle& p_self, const float p_volume)
                {
                    return p_self.Play(p_volume);
                },
                [](AudioHandle& p_self, const float p_volume, const float p_pan)
                {
                    p_self.Play(p_volume, p_pan);
                }
            ),
            "Stop", &AudioHandle::Stop
        );

        userType["__type"]["name"] = typeName;

        static const LuaTypeInfo& typeInfo = LuaTypeRegistry::GetInstance().RegisterType<AudioHandle>(typeName);
        return (void)typeInfo;
    }
}
