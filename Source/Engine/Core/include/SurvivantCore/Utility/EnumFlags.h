#pragma once
#include <cstdint>

namespace SvCore::Utility
{
    template <typename EnumT, typename DataT>
    class EnumFlags
    {
    public:
        using DataType = DataT;

        constexpr EnumFlags() = default;
        constexpr EnumFlags(EnumT p_value);
        constexpr EnumFlags(DataT p_value);

        EnumFlags(const EnumFlags& p_other)     = default;
        EnumFlags(EnumFlags&& p_other) noexcept = default;
        ~EnumFlags()                            = default;

        EnumFlags& operator=(const EnumFlags& p_other)     = default;
        EnumFlags& operator=(EnumFlags&& p_other) noexcept = default;

        constexpr bool operator==(EnumT p_value) const;
        constexpr bool operator==(const EnumFlags& p_other) const;
        constexpr bool operator!=(EnumT p_value) const;
        constexpr bool operator!=(const EnumFlags& p_other) const;

        EnumFlags&          operator&=(EnumT p_value);
        EnumFlags&          operator&=(const EnumFlags& p_other);
        constexpr EnumFlags operator&(EnumT p_value) const;
        constexpr EnumFlags operator&(EnumFlags p_other) const;

        EnumFlags&          operator|=(EnumT p_value);
        EnumFlags&          operator|=(const EnumFlags& p_other);
        constexpr EnumFlags operator|(EnumT p_value) const;
        constexpr EnumFlags operator|(EnumFlags p_other) const;

        EnumFlags&          operator^=(EnumT p_value);
        EnumFlags&          operator^=(const EnumFlags& p_other);
        constexpr EnumFlags operator^(EnumT p_value) const;
        constexpr EnumFlags operator^(EnumFlags p_other) const;

        constexpr EnumFlags operator~() const;

        constexpr operator bool() const;
        constexpr operator uint8_t() const;
        constexpr operator uint16_t() const;
        constexpr operator uint32_t() const;
        constexpr operator uint64_t() const;

        EnumFlags&     Set(EnumT p_value, bool p_state);
        constexpr bool IsSet(EnumT p_value) const;

        constexpr friend EnumFlags operator&(const EnumT p_a, const EnumFlags& p_b)
        {
            return EnumFlags(p_a & p_b.m_flags);
        }

        constexpr friend EnumFlags operator|(const EnumT p_a, const EnumFlags& p_b)
        {
            return EnumFlags(p_a | p_b.m_flags);
        }

        constexpr friend EnumFlags operator^(const EnumT p_a, const EnumFlags& p_b)
        {
            return EnumFlags(p_a ^ p_b.m_flags);
        }

    private:
        DataT m_flags = 0;
    };
}

#define SV_ENUM_FLAGS(EnumClass, StorageType)                                \
using EnumClass##Flags = SvCore::Utility::EnumFlags<EnumClass, StorageType>; \
                                                                             \
constexpr EnumClass##Flags operator&(EnumClass Left, EnumClass Right)        \
{                                                                            \
    return EnumClass##Flags(Left) & Right;                                   \
}                                                                            \
                                                                             \
constexpr EnumClass##Flags operator|(EnumClass Left, EnumClass Right)        \
{                                                                            \
    return EnumClass##Flags(Left) | Right;                                   \
}                                                                            \
                                                                             \
constexpr EnumClass##Flags operator^(EnumClass Left, EnumClass Right)        \
{                                                                            \
    return EnumClass##Flags(Left) ^ Right;                                   \
}                                                                            \
                                                                             \
constexpr EnumClass##Flags operator~(const EnumClass Val)                    \
{                                                                            \
    return ~EnumClass##Flags(Val);                                           \
}

#include "SurvivantCore/Utility/EnumFlags.inl"
