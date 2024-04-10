local TestRequired = {
    m_instances = 0
}

local callCount = 0

function TestRequired:SomeFunc()
    self.m_instances = self.m_instances + 1
    callCount = callCount + 1
    return "Working - Call number " .. callCount .. " | Instance number " .. self.m_instances
end

return TestRequired