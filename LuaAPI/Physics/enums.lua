---@meta

---@class EAxisLock
EAxisLock = {
    NONE = 0,
    X_POSITION = 1 << 0,
    Y_POSITION = 1 << 1,
    Z_POSITION = 1 << 2,
    X_ROTATION = 1 << 3,
    Y_ROTATION = 1 << 4,
    Z_ROTATION = 1 << 5,
}

---@alias EAxisLockFlags EnumFlags | EAxisLock

---@class ECollisionDetectionMode
ECollisionDetectionMode = {
    DISCRETE = 0,
    CONTINUOUS = 1,
    CONTINUOUS_SPECULATIVE = 2
}

---@class EForceMode
EForceMode = {
    FORCE = 0,
    IMPULSE = 1,
    ACCELERATION = 2,
    VELOCITY_CHANGE = 3
}