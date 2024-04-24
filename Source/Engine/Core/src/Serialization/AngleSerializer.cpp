#include "SurvivantCore/Debug/Assertion.h"
#include "SurvivantCore/Serialization/MathSerializers.h"

using namespace LibMath;

namespace SvCore::Serialization
{
    template <>
    bool ToJson(const Radian& angle, JsonWriter& writer)
    {
        return CHECK(writer.Double(angle.raw()), "Failed to write radian");
    }

    template <>
    bool FromJson(Radian& out, const JsonValue& json)
    {
        if (!CHECK(json.IsNumber(), "Unable to read radian - Json value should be a number"))
            return false;

        out = Radian(json.GetFloat());
        return true;
    }

    template <>
    bool ToJson(const Degree& angle, JsonWriter& writer)
    {
        return CHECK(writer.Double(angle.raw()), "Failed to write degrees");
    }

    template <>
    bool FromJson(Degree& out, const JsonValue& json)
    {
        if (!CHECK(json.IsNumber(), "Unable to read degrees - Json value should be a number"))
            return false;

        out = Degree(json.GetFloat());
        return true;
    }
}
