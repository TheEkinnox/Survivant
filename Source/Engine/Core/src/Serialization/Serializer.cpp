#include "SurvivantCore/Serialization/Serializer.h"

#include <rapidjson/istreamwrapper.h>

#if defined(_DEBUG) || defined(SV_VERBOSE_LOG)
#include <rapidjson/error/en.h>
#endif

namespace SvCore::Serialization
{
    JsonDocument LoadJsonFile(const std::string& p_path)
    {
        std::ifstream fs(p_path);

        if (!CHECK(fs.is_open(), "Unable to open json file at path \"%s\"", p_path.c_str()))
            return {};

        rapidjson::IStreamWrapper isw(fs);

        rapidjson::Document json;
        json.ParseStream(isw);

        if (!CHECK(!json.HasParseError(), "Unable to parse json from file - Parse error %d (%s)",
                json.GetParseError(), rapidjson::GetParseError_En(json.GetParseError())))
            return {};

        return json;
    }
}
