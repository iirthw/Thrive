#pragma once

#include "biomes.h"
#include "species.h"

#include <Common/ReferenceCounted.h>

#include <unordered_map>
#include <unordered_set>

namespace thrive {

constexpr auto INITIAL_SPECIES_POPULATION = 100;

//! An object that represents a patch
class Patch : public Leviathan::ReferenceCounted {
public:
    struct SpeciesInPatch {

        Species::pointer species;
        int population = 0;
    };

public:
    Patch(const std::string& name, int32_t id, const Biome& biomeTemplate);
    virtual ~Patch() = default;

    //! \brief Adds a connection to patch with id
    //! \returns True if this was new, false if already added
    bool
        addNeighbour(int32_t id);

    //! \brief Returns all species in this patch
    const auto&
        getSpecies() const
    {
        return speciesInPatch;
    }

    //! \brief Looks for a species with the specified name in this patch
    Species::pointer
        searchSpeciesByName(const std::string& name) const;

    //! \brief Adds a new species to this patch
    //! \returns True when added. False if the species was already in this patch
    bool
        addSpecies(Species::pointer species,
            int population = INITIAL_SPECIES_POPULATION);

    int32_t
        getId() const
    {
        return patchId;
    }

    const std::string&
        getName() const
    {
        return name;
    }

    Biome&
        getBiome()
    {
        return biome;
    }

    const Biome&
        getBiome() const
    {
        return biome;
    }

    bool
        addSpeciesWrapper(Species* species, int32_t population)
    {
        return addSpecies(Species::WrapPtr(species), population);
    }

    REFERENCE_COUNTED_PTR_TYPE(Patch);

private:
    const int32_t patchId;
    std::string name;

    Biome biome;

    //! Species in this patch. The Species objects are shared with other
    //! patches. They are contained in SpeciesInPatch struct to allow per patch
    //! properties
    std::vector<SpeciesInPatch> speciesInPatch;

    //! Links to other patches. These don't use Patch::pointer because that
    //! doesn't support weak references
    std::unordered_set<int32_t> adjacentPatches;
};


//! A mesh of connected Patches
class PatchMap : public Leviathan::ReferenceCounted {
public:
    PatchMap() = default;
    ~PatchMap() = default;

    //! \brief Adds a new patch to the map
    //! \returns True on success. False if the id is duplicate or there is some
    //! other problem
    bool
        addPatch(Patch::pointer patch);

    //! \brief Finds a species in the current patch map with name
    //!
    //! This starts from the current patch and then falls back to checking all
    //! patches. This is done to improve performance as it is likely that
    //! species in the current patch are looked up
    Species::pointer
        findSpeciesByName(const std::string& name);

    Patch::pointer
        getCurrentPatch();

    //! \brief Sets the current patch
    //! \returns True if the id was valid, false otherwise
    bool
        setCurrentPatch(int32_t newId);

    inline int32_t
        getCurrentPatchId() const
    {
        return currentPatchId;
    }

    Patch::pointer
        getPatch(int32_t id);

    bool
        addPatchWrapper(Patch* patch)
    {
        return addPatch(Patch::WrapPtr(patch));
    }

    Patch*
        getCurrentPatchWrapper()
    {
        const auto ptr = getCurrentPatch();
        if(ptr)
            ptr->AddRef();
        return ptr.get();
    }

    Patch*
        getPatchWrapper(int32_t id)
    {
        const auto ptr = getPatch(id);
        if(ptr)
            ptr->AddRef();
        return ptr.get();
    }

    Species*
        findSpeciesByNameWrapper(const std::string& name)
    {
        const auto ptr = findSpeciesByName(name);
        if(ptr)
            ptr->AddRef();
        return ptr.get();
    }

    REFERENCE_COUNTED_PTR_TYPE(PatchMap);

private:
    std::unordered_map<int32_t, Patch::pointer> patches;
    int32_t currentPatchId = 0;
};

} // namespace thrive