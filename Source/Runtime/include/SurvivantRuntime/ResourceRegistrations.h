#pragma once
#include <SurvivantAudio/AudioClip.h>
#include <SurvivantCore/ECS/Scene.h>

#include <SurvivantRendering/Resources/Material.h>
#include <SurvivantRendering/Resources/Model.h>
#include <SurvivantRendering/RHI/IShader.h>
#include <SurvivantRendering/RHI/ITexture.h>

#include <SurvivantScripting/LuaScript.h>

#include <SurvivantPhysics/Material.h>


REGISTER_RESOURCE_TYPE(Scene, SvCore::ECS::Scene)

REGISTER_RESOURCE_TYPE(Model, SvRendering::Resources::Model)
REGISTER_RESOURCE_TYPE(Material, SvRendering::Resources::Material)
REGISTER_RESOURCE_TYPE(Script, SvScripting::LuaScript)
REGISTER_RESOURCE_TYPE(Shader, SvRendering::RHI::IShader)
REGISTER_RESOURCE_TYPE(Texture, SvRendering::RHI::ITexture)

REGISTER_RESOURCE_TYPE(AudioClip, SvAudio::AudioClip)
REGISTER_RESOURCE_TYPE(PhysicsMaterial, SvPhysics::Material)
