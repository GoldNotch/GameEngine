// Copyright 2023 JohnCorn
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once
#include <vector>
#include <array>
#include <memory>
//#include <memory_resource>
#include <cassert>
#include <map>

using ShaderProgramID = unsigned char;
using RenderableObjectTypeID = unsigned int;
/// @brief stores data for context (thread-local global data for objects group)
struct IShaderProgram
{
    IShaderProgram(ShaderProgramID id) : id(id){};
    virtual ~IShaderProgram() = default;
    virtual void Bind() const = 0;
    inline ShaderProgramID GetID() const noexcept { return id; }

private:
    ShaderProgramID id;
    IShaderProgram(const IShaderProgram &) = delete;
    IShaderProgram &operator=(const IShaderProgram &) = delete;
};


/// @brief long-life buffer on GPU 
struct IGPUStaticBuffer
{
    explicit IGPUStaticBuffer(size_t elem_size, size_t count, const void *data = nullptr)
        : size(elem_size * count){};
    virtual ~IGPUStaticBuffer() = default;
    /// change size of buffer
    virtual void Realloc(size_t elem_size, size_t count, const void *data = nullptr)
    {
        size = elem_size * count;
    };
    /// upload data to allocated buffer (without reallocation)
    virtual void Upload(const void *data, size_t elem_size, size_t count, size_t offset = 0) = 0;

protected:
    size_t size; ///< allocated size in bytes
private:
    IGPUStaticBuffer(const IGPUStaticBuffer &) = delete;
    IGPUStaticBuffer &operator=(const IGPUStaticBuffer &) = delete;
};



struct IRenderer;

/// @brief object class which can be rendered.
/// UPD: renderable object doesn't have any GL objects. It's just object which will be rendered.
///     It's like decoration object on theatre scene
struct IRenderable
{
    virtual void Render(const IRenderer &renderer, double timestamp) const = 0;
    virtual RenderableObjectTypeID GetTypeID() const = 0;
};

/// container for objects on scene sorted and grouped by ShaderProgramID
struct RenderableScene
{
    using RenderableGroup = std::vector<std::unique_ptr<IRenderable>>;
    friend class IRenderer;
    /// constructor
    explicit RenderableScene(const IRenderer &renderer) : context(renderer) {}
    /// destructor
    ~RenderableScene() = default;
    // void SetCamera();
    /// Place object on scene. Uses to add object to render
    template <class T, typename... Args>
    T &PlaceObject(Args &&...args);

private:
    const IRenderer &context;     ///< rendering context
    std::map<RenderableObjectTypeID, RenderableGroup> scene_objects; ///< objects grouped by ShaderProgramID
    std::map<RenderableObjectTypeID, ShaderProgramID> shaders_by_objects_group;

    /// actions executed when new shader program created
    void AddNewObjectsType(RenderableObjectTypeID type_id, ShaderProgramID program_id) noexcept
    { 
        scene_objects.insert(std::make_pair(type_id, RenderableGroup()));
        shaders_by_objects_group.insert(std::make_pair(type_id, program_id));
    }

    // Find shader program by id
    ShaderProgramID GetGroupShaderProgram(RenderableObjectTypeID type_id)
    { return shaders_by_objects_group[type_id]; }

    /// Get Objects with the same shader program
    const RenderableGroup &GetGroup(RenderableObjectTypeID type_id) const
    { return scene_objects[type_id]; }

    RenderableScene(const RenderableScene &) = delete;
    RenderableScene &operator=(const RenderableScene &) = delete;
};

template <class T, typename... Args>
T & RenderableScene::PlaceObject(Args &&...args)
{
    std::unique_ptr<T> obj = std::make_unique<T>(args);
}

/// @brief interface for object will add renderable objects in renderer
struct IRenderableSceneBuilder
{
    /// scene and renderer initialization
    virtual void InitScene(IRenderer& renderer) const = 0;
    /// build scene for rendering
    virtual void BuildScene(RenderableScene & scene) const = 0;
};

/// own context data and begin rendering. thread-local rendering interface
struct IRenderer
{
    friend struct RenderableScene;

    static void InitRenderingSystem();
    //-------------- API ---------------------
    explicit IRenderer() = default;
    virtual ~IRenderer() = default;
    /// render frame function
    virtual void RenderPass(double timestamp) = 0;
    /// create new shader program (build must be in constructor)
    template <typename T>
    T &NewShaderProgram(RenderableObjectTypeID id)
    {
        ShaderProgramID new_id = programs.size();
        T* program = new T(new_id);
        programs.emplace_back(program);
        return *program;
    }

    /// Get shader program by id
    const IShaderProgram& GetBuiltProgram(ShaderProgramID id) const
    { 
        assert(id < programs.size() && programs[id]);
        return *programs[id]; 
    }
    /// bind scene builder. Call before render pass
    void BindSceneBuilder(const IRenderableSceneBuilder* scene_builder)
    { 
        this->scene_builder = scene_builder; 
        scene_builder->InitScene(*this);
    }

protected:
    RenderableScene cached_scene{*this}; ///< cache of objects in scene
    const IRenderableSceneBuilder* scene_builder = nullptr;   ///< client model to build scene
    void RenderObjectsGroup(RenderableObjectTypeID type_id, double timestamp) const;
private:
    using ContextID = unsigned int;
    ContextID RequestNewContextID() const;
    ContextID ctx_id = RequestNewContextID();

    std::vector<std::unique_ptr<IShaderProgram>> programs;  ///< shader programs
};


/// @brief Function to init shaders. Use specialization for each object type to define how to initialize shader
/// @tparam T - object type
/// @param renderer 
template<typename T>
void InitShaders(IRenderer& renderer)
{assert(false && "not implemented");}

// ------------------------- Scene object types -----------------------

namespace scene3d
{
    // object is geometry + placement

    enum ETypeID : RenderableObjectTypeID
    {
        STATIC_MESH = 0,
        TOTAL
    };

    /// group - MeshObject
    struct StaticMeshObject : public IRenderable
    {
        friend void InitShaders<StaticMeshObject>(IRenderer&);
        //------------ Types ---------------
        struct Vertex
        {
            float pos[3];
        };

        struct Geometry
        {
            const Vertex *vertices;
            size_t vertices_count;
            Geometry(const Vertex *vertices, size_t vertices_count) : vertices(vertices), vertices_count(vertices_count) {}
        };

        // ----------- API ------------------
        StaticMeshObject(const Geometry &geometry);
        virtual void Render(const IRenderer &renderer, double timestamp) const override;
        virtual RenderableObjectTypeID GetTypeID() const override 
        { return ETypeID::STATIC_MESH; }
    protected:
        // transform
        struct Impl;
        std::unique_ptr<Impl> impl;
        struct ShaderProgram;
    };

}
