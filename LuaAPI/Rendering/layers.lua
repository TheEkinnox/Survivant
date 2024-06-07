---@meta

---@class Layer
---Note: This table is mutable - Custom layers can be added at runtime
Layer = {
    NONE = 0,
    ALL = (1 << 32) - 1
}

---@alias LayerMask number | Layer