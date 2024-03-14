#include "SurvivantCore/Utility/UnusedIdGenerator.h"


namespace SvCore::Utility
{
    size_t UnusedIdGenerator::GetUnusedId()
    {
        using SetIt = std::set<size_t>::iterator;

        SetIt iter = m_used.lower_bound(m_freeCounter);
        while (iter != m_used.end() && *iter == m_freeCounter)
        {
            ++iter;
            ++m_freeCounter;
        }

        m_used.insert(m_freeCounter);
        return m_freeCounter++;
    }

    void UnusedIdGenerator::RemoveId(size_t p_id)
    {
        m_used.erase(p_id);

        if (m_freeCounter > p_id)
            m_freeCounter = p_id;
    }

    size_t UnusedIdGenerator::GetNumUsedIds()
    {
        return m_used.size();
    }
}
