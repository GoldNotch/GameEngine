set(SDL_STATIC 1)
add_subdirectory(SDL)
add_subdirectory(dylib)
add_subdirectory(Catch2)

set(SDLIMAGE_VENDORED OFF)
add_subdirectory(SDL_Image)
