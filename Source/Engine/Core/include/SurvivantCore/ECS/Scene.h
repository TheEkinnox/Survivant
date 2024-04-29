#pragma once
#include "SurvivantCore/ECS/EntityStorage.h"
#include "SurvivantCore/Resources/IResource.h"
#include "SurvivantCore/Serialization/Serializer.h"

namespace SvCore::ECS
{
    template <class T>
    class ComponentStorage;
    class IComponentStorage;
    class EntityHandle;

    class Scene final : public Resources::IResource
    {
        REGISTERED_RESOURCE_BODY()

    public:
        template <typename T>
        using Storage = std::conditional_t<std::is_same_v<Entity, std::remove_const_t<T>>, EntityStorage, ComponentStorage<T>>;

        /**
         * \brief Creates an empty scene
         */
        Scene();

        /**
         * \brief Creates a copy of the given scene
         * \param p_other The scene to copy
         */
        Scene(const Scene& p_other) = default;

        /**
         * \brief Creates a move copy of the given scene
         * \param p_other The scene to move
         */
        Scene(Scene&& p_other) noexcept = default;

        /**
         * \brief Destroys the scene
         */
        ~Scene() override = default;

        /**
         * \brief Assigns a copy of the given scene to this one
         * \param p_other The scene to copy
         * \return A reference to the modified scene
         */
        Scene& operator=(const Scene& p_other) = default;

        /**
         * \brief Moves the given scene into this one
         * \param p_other The scene to move
         * \return A reference to the modified scene
         */
        Scene& operator=(Scene&& p_other) noexcept = default;

        /**
         * \brief Tries to load the scene from the given file
         * \param p_fileName The resource file's path
         * \return True if the resource was successfully loaded. False otherwise.
         */
        bool Load(const std::string& p_fileName) override;

        /**
         * \brief Initializes the scene
         * \return True on success. False otherwise.
         */
        bool Init() override
        {
            return true;
        }

        /**
         * \brief Saves the scene to the given path
         * \param p_fileName The scene's path
         * \return True on success. False otherwise
         */
        bool Save(const std::string& p_fileName) override;

        /**
         * \brief Serializes the scene to json
         * \param p_writer The output json writer
         * \return True on success. False otherwise.
         */
        bool ToJson(Serialization::JsonWriter& p_writer) const;

        /**
         * \brief Deserializes the scene from json
         * \param p_json The input json data
         * \return True on success. False otherwise.
         */
        bool FromJson(const Serialization::JsonValue& p_json);

        /**
         * \brief Creates a new entity
         * \return A handle to the created entity
         */
        EntityHandle Create();

        /**
         * \brief Creates a copy of the given source entity
         * \param p_source The source entity
         * \return A handle to the created entity
         */
        EntityHandle Create(Entity p_source);

        /**
         * \brief Finds the entity with the given index
         * \param p_index The entity's index
         * \return A handle to the entity with the given index on success. An empty handle otherwise
         */
        EntityHandle Find(Entity::Index p_index);

        /**
         * \brief Destroys the given entity
         * \param p_entity The entity to destroy
         */
        void Destroy(Entity p_entity);

        /**
         * \brief Checks whether the given entity is valid
         * \param p_entity The entity to check
         * \return True if te entity is valid. False otherwise.
         */
        bool IsValid(Entity p_entity) const;

        /**
         * \brief Removes everything from the scene
         */
        void Clear();

        /**
         * \brief Checks if the given entity is in this scene
         * \param p_owner The searched entity
         * \return True if the entity is in this scene. False otherwise
         */
        bool Contains(Entity p_owner) const;

        /**
         * \brief Checks if the given entity owns a component of the given type
         * \tparam T The component's type
         * \param p_owner The searched component's owner
         * \return True if the entity owns a component of the given type. False otherwise
         */
        template <typename T>
        bool Has(Entity p_owner) const;

        /**
         * \brief Finds the component owned by the given entity
         * \tparam T The component's type
         * \param p_owner The searched component's owner
         * \return A pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        T* Get(Entity p_owner);

        /**
         * \brief Finds the component owned by the given entity
         * \tparam T The component's type
         * \param p_owner The searched component's owner
         * \return A constant pointer to the found component on success. Nullptr otherwise
         */
        template <typename T>
        const T* Get(Entity p_owner) const;

        /**
         * \brief Assigns the given component instance to the given entity
         * \tparam T The component's type
         * \param p_owner The component's owner
         * \param p_instance The added component instance
         * \return A reference to the created or modified component
         */
        template <typename T>
        T& Set(Entity p_owner, const T& p_instance);

        /**
         * \brief Creates or modifies the given entity's component of the given type
         * \tparam T The component's type
         * \tparam Args The component's construction parameters
         * \param p_owner The component's owner
         * \param p_args The component's construction parameters
         * \return A reference to the created or modified component
         */
        template <typename T, typename... Args>
        T& Make(Entity p_owner, Args&&... p_args);

        /**
         * \brief Removes the component of the given type owned by the given entity from the scene
         * \tparam T The component's type
         * \param p_owner The component's owner
         */
        template <typename T>
        void Remove(Entity p_owner);

        /**
         * \brief Removes the given component instance from the scene
         * \tparam T The component's type
         * \param p_instance The component instance to remove
         */
        template <typename T>
        void Remove(const T& p_instance);

        /**
         * \brief Gets the storage for the given type
         * \tparam T The storage content type
         * \return A reference to the storage
         */
        template <typename T>
        Storage<T>& GetStorage();

        /**
         * \brief Gets the storage for the given type
         * \tparam T The storage's content type
         * \return A constant reference to the storage
         */
        template <typename T>
        const Storage<T>& GetStorage() const;

        /**
         * \brief Gets the component storage for the given type
         * \param p_id The component's type's id
         * \return A reference to the storage
         */
        IComponentStorage& GetStorage(Utility::TypeId p_id);

        /**
         * \brief Gets the component storage for the given type
         * \param p_id The component's type's id
         * \return A constant reference to the storage
         */
        const IComponentStorage& GetStorage(Utility::TypeId p_id) const;

        /**
         * \brief Gets the ids of all the component types
         * \return The ids of all the component types
         */
        std::vector<Utility::TypeId> GetComponentIds() const;

        /**
         * \brief Gets the number of components owned by the given entity
         * \param p_entity The target entity
         * \return The number of components owned by the entity
         */
        Entity::Id GetComponentCount(Entity p_entity) const;

        /**
         * \brief Gets the ids of all the component types owned by the given entity
         * \param p_owner The components' owner
         * \return The ids of all the component types owned by the given entity
         */
        std::vector<Utility::TypeId> GetComponentIds(Entity p_owner) const;

        /**
         * \brief Gets all the components owned by the given entity
         * \param p_owner The components' owner
         * \return The components owned by the given entity
         */
        std::vector<std::pair<Utility::TypeId, void*>> GetComponents(Entity p_owner) const;

    private:
        EntityStorage                                                                   m_entities;
        mutable std::unordered_map<Utility::TypeId, std::shared_ptr<IComponentStorage>> m_components;

        /**
         * \brief Deserializes a component storage from json
         * \param p_json The input json data
         * \return True on success. False otherwise
         */
        bool DeserializeStorage(const Serialization::JsonValue& p_json);
    };
}

#include "SurvivantCore/ECS/Scene.inl"
