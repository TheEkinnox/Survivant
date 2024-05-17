//BuildConfig.cpp
#include "SurvivantEditor/RuntimeBuild/BuildConfig.h"

#include <SurvivantCore/Debug/Assertion.h>

using namespace SvCore::Resources;
using namespace SvCore::Serialization;

namespace SvEditor::RuntimeBuild
{
	BuildConfig::BuildConfig(const SvCore::Resources::ResourceRef<SvCore::ECS::Scene>& p_scenes)
	{
		m_scene = p_scenes;
	}

	bool BuildConfig::Load(const std::string& p_fileName)
	{
		return FromJson(LoadJsonFile(p_fileName));
	}

	bool BuildConfig::Init()
	{
		return true;
	}

	bool BuildConfig::Save(const std::string& p_path, const bool p_pretty)
	{
		JsonStringBuffer buffer;

		if (!p_pretty)
		{
			JsonWriter writer(buffer);

			if (!ToJson(writer) || !ASSUME(writer.IsComplete(), "Failed to save BuildConfig - Generated json is incomplete"))
				return false;
		}
		else
		{
			JsonPrettyWriter writer(buffer);

			if (!ToJson(writer) || !ASSUME(writer.IsComplete(), "Failed to save BuildConfig - Generated json is incomplete"))
				return false;
		}

		std::ofstream fs(p_path);

		if (!CHECK(fs.is_open(), "Unable to open BuildConfig file at path \"%s\"", p_path.c_str()))
			return false;

		fs << std::string_view(buffer.GetString(), buffer.GetLength());

		return CHECK(!fs.bad(), "Failed to write BuildConfig to \"%s\"", p_path.c_str());
	}

	bool BuildConfig::ToJson(SvCore::Serialization::JsonWriter& p_writer) const
	{
		p_writer.StartObject();

		p_writer.Key("scene");
		if (!CHECK(m_scene.ToJson(p_writer), "Unable to serialize BuildConfig - Failed to serialize Scene"))
			return false;

		return p_writer.EndObject();
	}

	bool BuildConfig::FromJson(const SvCore::Serialization::JsonValue& p_json)
	{
		auto it = p_json.FindMember("scene");
		return CHECK(it != p_json.MemberEnd(), "Unable to deserialize BuildConfig - Missing scene") || !m_scene.FromJson(it->value);
	}
}
