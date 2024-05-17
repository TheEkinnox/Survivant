#include "SurvivantTest/SceneTest.h"

#include "SurvivantTest/ComponentRegistrations.h"

#include <SurvivantCore/ECS/SceneView.h>

using namespace LibMath;

using namespace SvCore::ECS;
using namespace SvCore::Resources;
using namespace SvCore::Serialization;

using namespace SvRendering::Components;
using namespace SvRendering::Resources;

namespace SvTest
{
    void TestScene()
    {
        const auto onAddEntity = [](const Entity p_entity)
        {
            SV_LOG("Created entity %d:%d", p_entity.GetIndex(), p_entity.GetVersion());
        };

        const auto onRemoveEntity = [](const Entity p_entity)
        {
            SV_LOG("Removed entity %d:%d", p_entity.GetIndex(), p_entity.GetVersion());
        };

        const auto onAddInt = [](const Entity p_owner, const int& p_val)
        {
            SV_LOG("Added int %d to entity %d:%d", p_val, p_owner.GetIndex(), p_owner.GetVersion());
        };

        const auto onBeforeChangeInt = [](const Entity p_owner, const int& p_currentVal, const int& p_newVal)
        {
            SV_LOG("Changing int %d of entity %d:%d to %d", p_owner.GetIndex(), p_owner.GetVersion(), p_currentVal, p_newVal);
        };

        const auto onChangeInt = [](const Entity p_owner, const int& p_val)
        {
            SV_LOG("Changed int of entity %d:%d to %d", p_owner.GetIndex(), p_owner.GetVersion(), p_val);
        };

        const auto onRemoveInt = [](const Entity p_owner, const int& p_val)
        {
            SV_LOG("Removed int %d from entity %d:%d", p_val, p_owner.GetIndex(), p_owner.GetVersion());
        };

        Scene scene;

        scene.GetStorage<Entity>().m_onAdd.AddListener(onAddEntity);
        scene.GetStorage<Entity>().m_onRemove.AddListener(onRemoveEntity);

        scene.GetStorage<int>().m_onAdd.AddListener(onAddInt);
        scene.GetStorage<int>().m_onBeforeChange.AddListener(onBeforeChangeInt);
        scene.GetStorage<int>().m_onChange.AddListener(onChangeInt);
        scene.GetStorage<int>().m_onRemove.AddListener(onRemoveInt);

        scene.Make<int>(scene.Create(), 0);
        scene.Make<int>(scene.Create(), 1);
        scene.Make<int>(scene.Create(), 2);
        scene.Make<int>(scene.Create(), 3);

        EntityHandle lastEntity = scene.Create();

        {
            const int& i = scene.Make<int>(lastEntity, 999);
            CHECK(i == 999, "Constructing a component should return a reference to said component");

            scene.Set<int>(lastEntity, 4);
            CHECK(i == 4, "Setting a component for an existing entity should have updated it's value");
        }

        CHECK(scene.GetStorage<Entity>().size() == 5);
        CHECK(scene.GetStorage<int>().size() == 5);

        scene.Remove<const int>(Entity(1));
        CHECK(scene.GetStorage<int>().size() == 4);

        scene.Remove<int>(*scene.Get<int>(lastEntity));
        CHECK(scene.GetStorage<int>().size() == 3);

        scene.Remove<int>(lastEntity);
        CHECK(scene.GetStorage<int>().size() == 3);

        SceneView<int, char, float> view(scene);

        lastEntity.Set<int>(8);
        lastEntity.Set<char>('j');
        lastEntity.Set<float>(2.5f);

        CHECK(scene.GetStorage<int>().size() == 4);
        CHECK(scene.GetStorage<float>().size() == 1);
        CHECK(scene.GetStorage<char>().size() == 1);

        const char c1       = *view.Get<char>(lastEntity);
        auto       [i1, f1] = view.Get<int, float>(lastEntity);

        CHECK(c1 == 'j');
        CHECK(i1 && *i1 == 8);
        CHECK(f1 && floatEquals(*f1, 2.5f));

        auto [i, c, f] = view.Get(lastEntity);

        CHECK(i && *i == 8);
        CHECK(c && *c == 'j');
        CHECK(f && floatEquals(*f, 2.5f));

        int count = 0;
        for (const auto entity : view)
        {
            CHECK(++count == 1, "Only one relevant entity should have been found");

            auto [iComp, fComp, cComp] = view.Get<int, const float, char>(entity);

            CHECK(iComp && *iComp == 8);
            CHECK(cComp && *cComp == 'j');
            CHECK(fComp && floatEquals(*fComp, 2.5f));

            *iComp = 123;
            *cComp = 'z';
        }

        auto [i2, c2, f2] = view.Get(lastEntity);

        CHECK(i2 && *i2 == 123);
        CHECK(c2 && *c2 == 'z');
        CHECK(f2 && floatEquals(*f2, 2.5f));

        lastEntity.Destroy();

        CHECK(scene.GetStorage<Entity>().size() == 4);
        CHECK(scene.GetStorage<int>().size() == 3);
        CHECK(scene.GetStorage<float>().size() == 0);
        CHECK(scene.GetStorage<char>().size() == 0);

        auto [iDestroyed, cDestroyed, fDestroyed] = view.Get(lastEntity);

        CHECK(iDestroyed == nullptr);
        CHECK(cDestroyed == nullptr);
        CHECK(fDestroyed == nullptr);

        for ([[maybe_unused]] const auto _ : view)
            CHECK(false, "The only releveant entity should have been destroyed");

        lastEntity = scene.Create();

        CHECK(scene.GetStorage<Entity>().size() == 5);
        CHECK(scene.GetStorage<int>().size() == 3);
        CHECK(scene.GetStorage<float>().size() == 0);
        CHECK(scene.GetStorage<char>().size() == 0);

        for ([[maybe_unused]] const auto _ : view)
            CHECK(false, "The only releveant entity should have been destroyed");
    }

    void TestSceneSerialization()
    {
        Scene scene;
        scene.Clear();

        EntityHandle entity1 = scene.Create();
        entity1.Make<TagComponent>("Entity 1");
        entity1.Make<Transform>(Vector3::zero(), Quaternion::identity(), Vector3::one());
        entity1.Make<ModelComponent>(ResourceRef<Model>("models/cube.obj"), ResourceRef<Material>());

        EntityHandle entity2 = scene.Create();
        entity2.Make<TagComponent>("Entity 2");
        entity2.Make<Transform>(Vector3::zero(), Quaternion::identity(), Vector3::one());

        EntityHandle entity3 = scene.Create();
        entity3.Make<TagComponent>("Entity 3");
        entity3.Make<Transform>(Vector3(.75f, .5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle entity4 = scene.Create();
        entity4.Make<TagComponent>("Entity 4 (1st child of Entity 3)");
        entity4.Make<HierarchyComponent>(entity3.GetEntity());
        entity4.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle entity5 = scene.Create();
        entity5.Make<TagComponent>("Entity 5 (1st child of Entity 4)");
        entity5.Make<HierarchyComponent>(entity4.GetEntity());
        entity5.Make<Transform>(Vector3(0.f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle entity6 = scene.Create();
        entity6.Make<TagComponent>("Entity 6 (2nd child of Entity 4)");
        entity6.Make<HierarchyComponent>(entity4.GetEntity());
        entity6.Make<Transform>(Vector3(1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle entity7 = scene.Create();
        entity7.Make<TagComponent>("Entity 7 (2nd child of Entity 3)");
        entity7.Make<HierarchyComponent>(entity3.GetEntity());
        entity7.Make<Transform>(Vector3(-1.5f, 1.5f, 0.f), Quaternion::identity(), Vector3::one());

        EntityHandle entity8 = scene.Create();
        entity8.Make<TagComponent>("Entity 8");
        entity8.Make<Transform>(Vector3::front(), Quaternion::identity(), Vector3(1.5f, .5f, .1f));

        JsonStringBuffer buffer;
        JsonWriter       writer(buffer);
        CHECK(scene.ToJson(writer), "Scene json serialization failed");
        CHECK(writer.IsComplete(), "Scene json serialization failed - Produced json is incomplete");

        const std::string validJsonStr(buffer.GetString(), buffer.GetSize());
        SV_LOG("Scene json:\n%s", validJsonStr.c_str());

        Scene        tmp;
        JsonDocument document;
        document.Parse(validJsonStr.c_str(), validJsonStr.size());
        CHECK(tmp.FromJson(document));

#ifdef SV_EDITOR
        const auto printTag = [](const void* p_payload)
        {
            ASSERT(p_payload);

            const TagComponent& tag = *static_cast<const TagComponent*>(p_payload);
            SV_LOG("Called injected func for tag \"%s\"", tag.m_tag.c_str());

            return true;
        };

        CHECK(ComponentRegistry::GetInstance().GetTypeInfo<TagComponent>().Add("Print", printTag));

        const auto printTransform = [](const void* p_payload)
        {
            ASSERT(p_payload);

            const Transform& transform = *static_cast<const Transform*>(p_payload);
            SV_LOG("Called injected func for transform {%s, %s, %s}",
                transform.getPosition().string().c_str(), transform.getRotation().string().c_str(),
                transform.getScale().string().c_str());

            return true;
        };

        CHECK(ComponentRegistry::GetInstance().GetTypeInfo<Transform>().Add("Print", printTransform));

        const auto printHierarchy = [](const void* p_payload)
        {
            ASSERT(p_payload);

            const HierarchyComponent& hierarchy = *static_cast<const HierarchyComponent*>(p_payload);
            SV_LOG("Called injected func for child of {%u:%u}",
                hierarchy.GetParent().GetIndex(), hierarchy.GetParent().GetVersion());

            return true;
        };

        CHECK(ComponentRegistry::GetInstance().GetTypeInfo<HierarchyComponent>().Add("Print", printHierarchy));

        auto components = scene.GetComponents(entity7);
        for (auto [typeId, component] : components)
        {
            const auto& typeInfo = ComponentRegistry::GetInstance().GetTypeInfo(typeId);

            if (typeInfo.Has("Print"))
                CHECK(typeInfo.Call<bool>("Print", component).value_or(false));
        }
#endif
    }

    void RunSceneTests()
    {
        TestScene();
        TestSceneSerialization();
    }
}
