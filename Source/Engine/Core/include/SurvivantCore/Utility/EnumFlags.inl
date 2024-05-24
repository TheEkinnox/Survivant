#pragma once
#include "SurvivantCore/Utility/EnumFlags.h"

namespace SvCore::Utility
{
    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::EnumFlags(EnumT p_value)
        : m_flags(static_cast<DataT>(p_value))
    {
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::EnumFlags(DataT p_value)
        : m_flags(p_value)
    {
    }

    template <typename EnumT, typename DataT>
    constexpr bool EnumFlags<EnumT, DataT>::operator==(EnumT p_value) const
    {
        return m_flags == static_cast<DataT>(p_value);
    }

    template <typename EnumT, typename DataT>
    constexpr bool EnumFlags<EnumT, DataT>::operator==(const EnumFlags& p_other) const
    {
        return m_flags == p_other.m_flags;
    }

    template <typename EnumT, typename DataT>
    constexpr bool EnumFlags<EnumT, DataT>::operator!=(EnumT p_value) const
    {
        return m_flags != static_cast<DataT>(p_value);
    }

    template <typename EnumT, typename DataT>
    constexpr bool EnumFlags<EnumT, DataT>::operator!=(const EnumFlags& p_other) const
    {
        return m_flags != p_other.m_flags;
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::operator&=(EnumT p_value)
    {
        m_flags &= static_cast<DataT>(p_value);
        return *this;
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::operator&=(const EnumFlags& p_other)
    {
        m_flags &= p_other.m_flags;
        return *this;
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator&(EnumT p_value) const
    {
        return EnumFlags(m_flags & static_cast<DataT>(p_value));
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator&(EnumFlags p_other) const
    {
        return p_other &= m_flags;
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::operator|=(EnumT p_value)
    {
        m_flags |= static_cast<DataT>(p_value);
        return *this;
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::operator|=(const EnumFlags& p_other)
    {
        m_flags |= p_other.m_flags;
        return *this;
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator|(EnumT p_value) const
    {
        return EnumFlags(m_flags | static_cast<DataT>(p_value));
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator|(EnumFlags p_other) const
    {
        return p_other |= m_flags;
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::operator^=(EnumT p_value)
    {
        m_flags ^= static_cast<DataT>(p_value);
        return *this;
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::operator^=(const EnumFlags& p_other)
    {
        m_flags ^= p_other.m_flags;
        return *this;
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator^(EnumT p_value) const
    {
        return EnumFlags(m_flags ^ static_cast<DataT>(p_value));
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator^(EnumFlags p_other) const
    {
        return p_other ^= m_flags;
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT> EnumFlags<EnumT, DataT>::operator~() const
    {
        return EnumFlags(~m_flags);
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::operator bool() const
    {
        return m_flags;
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::operator uint8_t() const
    {
        return static_cast<uint8_t>(m_flags);
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::operator uint16_t() const
    {
        return static_cast<uint16_t>(m_flags);
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::operator uint32_t() const
    {
        return static_cast<uint32_t>(m_flags);
    }

    template <typename EnumT, typename DataT>
    constexpr EnumFlags<EnumT, DataT>::operator uint64_t() const
    {
        return static_cast<uint64_t>(m_flags);
    }

    template <typename EnumT, typename DataT>
    EnumFlags<EnumT, DataT>& EnumFlags<EnumT, DataT>::Set(EnumT p_value, bool p_state)
    {
        return p_state ? ((*this) |= p_value) : ((*this) &= ~p_value);
    }

    template <typename EnumT, typename DataT>
    constexpr bool EnumFlags<EnumT, DataT>::IsSet(EnumT p_value) const
    {
        return (m_flags & static_cast<DataT>(p_value)) == static_cast<DataT>(p_value);
    }
}
