#pragma once

#include "resource_loader.hpp"

/**
 * @brief Resource loader that handles plain text resources.
 */
class TextLoader : public ResourceLoader {
  public:
    TextLoader();
    ~TextLoader();

    Result<Resource*, RuntimeError> load(const String name);
    void                            unload(Resource* resource);

  private:
};