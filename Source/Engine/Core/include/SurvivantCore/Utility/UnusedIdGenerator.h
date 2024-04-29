#pragma once

#include <set>

namespace SvCore::Utility
{
    class UnusedIdGenerator
    {
    public:
        UnusedIdGenerator() = default;
        ~UnusedIdGenerator() = default;

        size_t  GetUnusedId();
        void    RemoveId(size_t p_id);
        size_t  GetNumUsedIds();

    private:
        std::set<size_t> m_used;
        size_t m_freeCounter = 0;
    };
}