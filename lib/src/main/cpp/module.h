#ifndef MY_MODULE_H
#define MY_MODULE_H

#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <cstring>
#include <thread>
#include <string>
#include <fstream>
#include "zygisk.hpp"
 
#define TARGET_PROCESS "com.kakaogames.gdts"

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override;

    void preAppSpecialize(AppSpecializeArgs *args) override;

    void postAppSpecialize(const AppSpecializeArgs *) override;

private:
    Api *api;
    JNIEnv *env;
    bool enable_module = false;
    std::string process_data_dir;

    void preSpecialize(const char *process, const char *app_data_dir);

};

REGISTER_ZYGISK_MODULE(MyModule)

void MainThread(const std::string &process_data_dir);

#endif