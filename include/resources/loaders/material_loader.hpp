#pragma once

#include "resource_loader.hpp"

class MaterialLoader : public ResourceLoader {
  public:
    MaterialLoader();
    ~MaterialLoader();

    Resource* load(const String name);
    void      unload(Resource* resource);

  private:
};