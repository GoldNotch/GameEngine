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
#include <vector>
#include <array>
#include <memory>
#include <cassert>

#include "Cache.hpp"

/// @brief stores data for context (thread-local global data for objects group)
struct IShaderData
{
    IShaderData() = default;
    virtual ~IShaderData() = default;
    virtual void Bind() const = 0;

private:
    IShaderData(const IShaderData &) = delete;
    IShaderData &operator=(const IShaderData &) = delete;
};

/// @brief pointer o ram buffer. Uses tor caching vbo, ibo, textures
struct CPUBufferPtr
{
    CPUBufferPtr(const void* buffer = nullptr, size_t elem_size = 0, size_t elem_count = 0)
    : buffer(buffer), elem_count(elem_count), elem_size(elem_size) {} 
    ~CPUBufferPtr() = default;

    constexpr size_t GetTotalSize() const noexcept
    {  return elem_count * elem_size;  }
    constexpr const void* GetBuffer() const noexcept
    { return buffer; }
    constexpr size_t GetCount() const noexcept
    { return elem_count; }

    bool operator==(const CPUBufferPtr& other) const noexcept
    { return elem_count == other.elem_count && 
            elem_size == other.elem_size && buffer == other.buffer; }
    
private:
    const void* buffer = nullptr;
    size_t elem_size = 0;
    size_t elem_count = 0;
};

struct IGPUBuffer
{
    IGPUBuffer(const CPUBufferPtr& buffer) = default;
    virtual ~IGPUBuffer() = default;
    virtual void Bind() const = 0;
};

/// identifier of objects group. To find context data in renderer
enum EObjectsGroupID : unsigned char;
struct RenderableScene;

/// own context data. thread-local rendering interface
struct IRenderer
{
    using TCache = LRUCache<IGPUBuffer, CPUBufferPtr>;
    static void InitRenderingSystem();
    static TCache& GetBufferCache();
    
    //-------------- API ---------------------
    IRenderer() = default;
    virtual ~IRenderer() = default;
    /// render frame function
    virtual void RenderPass(double timestamp) = 0;
    /// create new context data for specific objects group
    template <typename T>
    T &NewContextData(EObjectsGroupID id)
    {
        ctx_data[id] = std::make_unique<T>();
        return dynamic_cast<T &>(*ctx_data[id]);
    }
protected:
    /// traverse scene, form vbos and send it to GPU
    virtual void PrepareRenderData(const RenderableScene& scene) = 0;
    void BindGroup(EObjectsGroupID id) const
    {
        assert(ctx_data[id] != nullptr);
        ctx_data[id]->Bind();
    }
    
private:
    using ContextID = unsigned int;
    static ContextID RequestNewContextID();
    ContextID ctx_id = RequestNewContextID();
    std::array<std::unique_ptr<IShaderData>, EObjectsGroupID::TOTAL> ctx_data;
    static TCache BufferCache;
};




// ------------------------- Scene object types -----------------------
enum EObjectsGroupID : unsigned char
{
    STATIC_MESH_OBJECT = 0,
    TERRAIN,
    SKYBOX,
    TOTAL
};


/// @brief object class which can be rendered.
/// UPD: renderable object doesn't have any GL objects. It's just object which will be rendered.
///     It's like decoration object on theatre scene
struct IRenderable 
{
    virtual EObjectsGroupID GetGroupID() const noexcept = 0;
    virtual void Render(const IRenderer& renderer, double timestamp) const = 0;
};


/// @brief data to render one frame. It's like scene description: camera transform, objects, etc
/// renderer wil take it and prepare specific data(VBO, IBO, etc) and render it
struct RenderableScene
{
    void SetCamera();
    template<class T, typename... Args>
    T& PlaceObject(Args&& ... args);
private:
    std::array<std::vector<IRenderable>, EObjectsGroupID::TOTAL> scene_objects;
};


/// group - MeshObject
struct StaticMeshObject : public IRenderable
{
    //------------ Types ---------------
    struct Vertex
    {
        float pos[3];
    };

    struct Geometry
    {
        CPUBufferPtr vertices;
        //CPUBufferPtr<unsigned int> indices;
    };
    //----------- Static API ------------
    static void InitForContext(IRenderer& renderer);
    static constexpr EObjectsGroupID GroupID() noexcept
    { return EObjectsGroupID::STATIC_MESH_OBJECT; }

    // ----------- API ------------------
    StaticMeshObject(const Geometry &geometry);
    virtual EObjectsGroupID GetGroupID() const noexcept override 
    { return GroupID(); }
    virtual void Render(const IRenderer& renderer, double timestamp) const override;
protected:
    //transform
    struct Impl;
    std::unique_ptr<Impl> impl;
    struct ContextData;
};
