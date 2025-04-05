#include <cstdlib>
#include <unistd.h>
#include <fcntl.h>
#include <android/log.h>
#include <cstring>
#include <thread>
#include <string>
#include "zygisk.hpp"
 
#define TARGET_PROCESS "com.packages.name"

void MainThread(const std::string &process_data_dir);

using zygisk::Api;
using zygisk::AppSpecializeArgs;
using zygisk::ServerSpecializeArgs;

class MyModule : public zygisk::ModuleBase {
public:
    void onLoad(Api *api, JNIEnv *env) override {
        this->api = api;
        this->env = env;
    }

    void preAppSpecialize(AppSpecializeArgs *args) override {
        // Use JNI to fetch our process name
        const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
        const char *app_data_dir = env->GetStringUTFChars(args->app_data_dir, nullptr);
        preSpecialize(process, app_data_dir);
        env->ReleaseStringUTFChars(args->nice_name, process);
        env->ReleaseStringUTFChars(args->app_data_dir, app_data_dir);
    }

    void postAppSpecialize(const AppSpecializeArgs *) override {
        if (enable_module == true) {
            std::thread my_thread(MainThread, process_data_dir);
            my_thread.detach();
        }
    }

private:
    Api *api;
    JNIEnv *env;
    bool enable_module = false;
    std::string process_data_dir;

    void preSpecialize(const char *process, const char *app_data_dir) {
        if (strcmp(process, TARGET_PROCESS) == 0) {
            enable_module = true;
            process_data_dir = app_data_dir;
        } else {
            enable_module = false;
        }
    }

};

REGISTER_ZYGISK_MODULE(MyModule)

void MainThread(const std::string &process_data_dir)
{
    // TODO: Feel free to implement your own logic to process data within the application data directory.
    // You can read files, modify data, or perform any other operations that suit your module's purpose.

    // TODO: Use logging to print information or status as needed.
    // Example: __android_log_print(ANDROID_LOG_INFO, "ZygiskModule", "Your log message here");

    // TODO: Implement error handling for any operations that may fail.
    // Example: Ensure that files are successfully opened or read before proceeding.

    // TODO: Add any additional functionality that you think is necessary for your module.
    // This is your opportunity to customize the behavior of your Zygisk module!
}