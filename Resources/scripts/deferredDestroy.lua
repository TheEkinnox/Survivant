---@class DeferredDestroy
---@field public to_remove table<number, Entity>
local DeferredDestroy = {
    to_remove = {}
}

---@param entity Entity
function DeferredDestroy:Destroy(entity)
    self.to_remove[#self.to_remove + 1] = entity
end

function DeferredDestroy:Process()
    for i = 1, #self.to_remove do
        self.to_remove[i]:Destroy()
    end

    self.to_remove = {}
end

return DeferredDestroy