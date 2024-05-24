---@meta

---@class EClearFlag
EClearFlag = {
    NONE = 0,
    COLOR = 1 << 0,
    DEPTH = 1 << 1,
    STENCIL = 1 << 2
}

---@alias ClearMask number | EClearFlag