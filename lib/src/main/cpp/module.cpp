#include "module.h"

void MainThread(const std::string &process_data_dir) {
    std::string path = process_data_dir + "/my-log.txt";
    std::fstream buffers(path, std::ios::app);
    if (!buffers) {
        __android_log_print(ANDROID_LOG_ERROR, "MyModule", "Failed to open %s", path.c_str());
        return;
    }
    buffers << "This is from module" << "\n";
    buffers.close();
}

void MyModule::onLoad(Api *api, JNIEnv *env) {
    this->api = api;
    this->env = env;
}

void MyModule::preAppSpecialize(AppSpecializeArgs *args) {
    // Use JNI to fetch our process name
    const char *process = env->GetStringUTFChars(args->nice_name, nullptr);
    const char *app_data_dir = env->GetStringUTFChars(args->app_data_dir, nullptr);
    preSpecialize(process, app_data_dir);
    env->ReleaseStringUTFChars(args->nice_name, process);
    env->ReleaseStringUTFChars(args->app_data_dir, app_data_dir);
}

void MyModule::postAppSpecialize(const AppSpecializeArgs *) {
    if (enable_module == true) {
        std::thread my_thread(MainThread, process_data_dir);
        my_thread.detach();
    }
}

void MyModule::preSpecialize(const char *process, const char *app_data_dir) {
    if (strcmp(process, TARGET_PROCESS) == 0) {
        enable_module = true;
        process_data_dir = app_data_dir;
    } else {
        enable_module = false;
    }
}
