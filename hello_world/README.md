# Welcome to DesktopDevX for Azure Sphere on Linux

The DesktopDevX is supported on an Intel/ARM Linux (including [Windows WSL 2](https://docs.microsoft.com/en-us/windows/wsl/install)) computer as the project ultimately targets embedded Linux on Azure Sphere.

---

## Azure Sphere DevX Documentation

The DesktopDevX supports the AzureSphereDevX Bindings (except for hardware bindings).

[AzureSphereDevX Library wiki](https://github.com/Azure-Sphere-DevX/AzureSphereDevX.Examples/wiki)

---

## DesktopDexX goal and limitations

The goal of the DesktopDevX project is to provide a rapid Azure Sphere prototyping experience.

1. DesktopDevX is excellent at application interaction modeling, especially for cloud enabled devices.
1. You need to think like an embedded developer especially when considering memory allocation.
1. The development cycle is fast as you do not need to deploy your application to Azure Sphere to develop and test.
1. The both the source code and application model are identical on the desktop and device when using AzureSphereDevX.
1. Application timing will be consistent for desktop and Azure Sphere versions as timing is driven by an event loop. 
1. Did I mention the developer cycle is fast :)

There are some important considerations when using this project:

1. The available memory resources on an Azure Sphere device is considerably less that your desktop computer.
1. Only a subset of POSIX APIs are available on Azure Sphere. Check the Azure Sphere SDK Sysroots to understand what is available and code accordingly.
1. Azure Sphere services like deferred updates are not available for prototyping with this project
1. The sample uses the full azure-iot-sdk-c SDK. The full SDK supports features not available on Azure Sphere, so don't call any azure-iot-sdk-c APIs directly.
1. No support for intercore apps (for now).
1. No memory tracking usage (for now).
1. Persistent flash memory not implemented (for now).

---

## Install dependencies on your Linux desktop computer

Installs the following:

1. [The UV Event Loop library](http://docs.libuv.org/en/v1.x/index.html)
1. [azure-iot-sdk-c](https://github.com/Azure/azure-iot-sdk-c) dependencies
1. C toolchain

```bash
sudo apt-get install libuv1.dev unzip cmake build-essential gdb curl libcurl4-openssl-dev libssl-dev uuid-dev ca-certificates python3-pip git
```

---

## Install Visual Studio Code extensions

Install the following VS Code extensions.

1. [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)

---

## Clone the DesktopDevX Examples project

Clone the following project. If using Windows WSL, you must clone the project into the WSL file system.

```bash
git clone https://github.com/Azure-Sphere-DevX/DesktopDevX.Example.git
```

---

## Open and build the project

1. Start VS Code
1. If project is in WSL or a remote Linux computer then connect to the project using VS Code Remote WSL or Remote SSL
1. The CMake cache will auto generate.
1. You set the Azure IoT connection string in the .vscode/settings.json file.
    The Azure IoT Hub connection string must be split into Hostname, DeviceId, and SharedAccessKey.

    ```json
    "cmake.debugConfig": {
            "args": [
                "--Hostname", 
                "HostName=your_iot_hub.azure-devices.net",
                "--DeviceId", 
                "DeviceId=your_device_id",
                "--SharedAccessKey",
                "SharedAccessKey=your_device_id_key"
            ]
        }
    ```
1. Press <kbd>Ctrl+F5</kbd> to start CMake build and run the project with the debugger.
1. Press <kbd>Shift+F5</kbd> to start CMake build and run the project without the debugger.


For more information, refer to [Get started with CMake Tools on Linux](https://code.visualstudio.com/docs/cpp/cmake-linux).

---

## References

* [Installing the azure-iot-sdk-c on Linux](https://github.com/Azure/azure-iot-sdk-c/blob/master/doc/devbox_setup.md#linux)
* [Event loop programming](https://linuxjedi.co.uk/2020/04/28/event-loop-programming-a-different-way-of-thinking)
* [How to install UV on Linux](https://stackoverflow.com/questions/42175630/how-to-install-libuv-on-ubuntu)
