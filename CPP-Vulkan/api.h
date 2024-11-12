//
// Created by marti on 12/11/2024.
//

#ifndef API_H
#define API_H

#ifdef _WIN32
#define API_EXPORT __declspec(dllexport)
#else
#define API_EXPORT __attribute__((visibility("default")))
#endif

extern "C"{
    API_EXPORT void* CreateEngine(const char* name, int width, int height, bool fullScreen);
    API_EXPORT void DestroyEngine(void* engine);
}

#endif //API_H
