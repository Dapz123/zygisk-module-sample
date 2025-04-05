import java.io.File

plugins {
    alias(libs.plugins.android.lib)
}

android {
    namespace = "com.dpzdev.module"
    compileSdk = 35

    defaultConfig {
        minSdk = 26
        targetSdk = 35
    
        externalNativeBuild {
            cmake {
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

    val varType = variant.name.capitalize()
    var varTypeLow = variant.name.lowercase()
    val projectDir = project.layout.projectDirectory.asFile
    val moduleDir = File(projectDir, "module-installer")
    val outDir = File(projectDir, "out")

    tasks.register<Copy>("magisk$varType") {

      dependsOn("assemble$varType")
      from(moduleDir)
      into(outDir)

      doLast {
        println("Running tasks : module$varType")

        val archs = listOf("arm64-v8a", "armeabi-v7a", "x86", "x86_64")
        archs.forEach { arch ->

            val srcDir = File("${project.layout.buildDirectory.get()}/intermediates/stripped_native_libs/$varTypeLow/strip${varType}DebugSymbols/out/lib/$arch/libmodule.so")
            val destDir = File(outDir, "zygisk/")
            
            if (srcDir.exists()) {
                println("Library for $arch found, proceeding with transfer.")
                srcDir.copyTo(File(destDir, "$arch.so"), overwrite = true)
            } else {
                println("Error: Library for $arch not found!\n\t$srcDir")
            }

        }

      }

      finalizedBy("zip$varType")

    } // magisk tasks

    tasks.register<Zip>("zip$varType") {
        doFirst {
            println("Creating a zip archive of the module.")
        }

        archiveFileName.set("module-$varTypeLow.zip")
        destinationDirectory.set(project.layout.buildDirectory.get())
        from(outDir)

    }
    
    variant.assembleProvider.get().finalizedBy(tasks.named("magisk$varType"))
  
  } // variant for each
  
} // after evaluate
