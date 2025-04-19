import java.io.File

plugins {
    alias(libs.plugins.android.lib)
}

val libName = "mymodule"

android {
    namespace = "com.dpzdev.module"
    compileSdk = 35

    defaultConfig {
        minSdk = 26
        targetSdk = 35
    
        externalNativeBuild {
            cmake {
                arguments += "-DPROJECT_NAME=$libName"
                cppFlags += "-std=c++17"
            }
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }
    
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

afterEvaluate {
    android.libraryVariants.forEach { variant ->

        var buildVariant      = variant.name.capitalize()
        val buildVariantLowered = variant.name.lowercase()
        
        val abiFilter = listOf("arm64-v8a", "armeabi-v7a")
        val projectDir = "${project.layout.projectDirectory}"
        val buildDir = "${project.layout.buildDirectory.get()}"
        val script = File(projectDir, "/module-installer")
        val moduleOut = File(buildDir, "/my-module")

        tasks.register<Copy>("prepareMagisk$buildVariant") {
            dependsOn("assemble$buildVariant")
            from(script)
            into(moduleOut)
            println("Create module template")
            doLast {

                abiFilter.forEach { type ->

                    val nativeLibs = File("$buildDir/intermediates/stripped_native_libs/${buildVariantLowered}/strip${buildVariant}DebugSymbols/out/lib/$type/lib$libName.so")
                    val zygiskDir = File("$moduleOut/zygisk/")

                    if ( nativeLibs.exists() ) {
                        nativeLibs.copyTo( File(zygiskDir, "$type.so"), overwrite = true )
                        println("Copied native libs")
                    } else {
                        println("Native libs is not found on $nativeLibs")
                    }

                }
            
            }

            finalizedBy("zipModule$buildVariant")

        }

        tasks.register<Zip>("zipModule$buildVariant") {
            println("Creating zip for the module")
            archiveFileName.set("my-module.zip")
            destinationDirectory.set( project.layout.buildDirectory.get() )
            from(moduleOut)
        }
        
        variant.assembleProvider.get().finalizedBy(tasks.named("prepareMagisk$buildVariant"))
    }
}
