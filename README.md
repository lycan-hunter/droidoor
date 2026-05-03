# droidoor
Simple backdoor for android devices, installed using ADB

[![C++](https://img.shields.io/badge/Language-C++-blue.svg)](https://en.cppreference.com/)
[![Target](https://img.shields.io/badge/Target-Android%20%7C%20Linux-green.svg)](https://www.android.com/)
[![Compiler](https://img.shields.io/badge/Compiler-Zig-orange.svg)](https://ziglang.org/)

**Droidoor** is a lightweight and flexible Reverse Shell tool, specifically optimized for Android environments and Linux systems. The project combines the power of C++ static compilation with the convenience of an interactive Bash deployment script.

---

## 🛠 Architecture & How It Works

Droidoor operates on the **Reverse TCP** principle. Unlike a classic Bind Shell where the target opens a port, Droidoor forces the target to *initiate* the connection to your server. This allows bypassing:
* Firewalls
* NAT (including mobile networks)
* Local permission restrictions

### Components:
1. **The Controller (LHOST):** Your machine or server waiting for a connection via `nc` or a similar listener.
2. **The Payload:** A compact C++ binary, statically compiled (no external dependencies).
3. **The Deployer:** An interactive Bash script for quick configuration, compilation, and delivery.

---

## 🚀 Features

- [x] **Static Build:** Runs on any Android/Linux without pre-installed libraries.
- [x] **Cross-platform:** Easy adaptation for x86_64, aarch64, and armv7.
- [x] **Multi-delivery:**  Download and execute the binary via any connection (including wireless ADB or other).

---

## 📦 Dependencies

Ensure the following are installed on your system:
* **Zig Compiler:** used for C++ cross-compilation (you also can change compiler in shell script)
* **ADB:** for automated Android deployment
* **Netcat:** to receive connections (you can use any tcp-port listener, like python socket and so on)

```bash
# macOS
brew install zig adb netcat
```
```bash
# Linux (Ubuntu/Debian)
sudo apt install zig adb netcat
```

## 🔍 Persistence & Operational Details

### Reconnection Logic & Power Management
The payload is designed with an **automatic retry mechanism**. If the connection is lost or the controller is offline, the binary will attempt to re-establish a link every **5 seconds**.

> [!IMPORTANT]
> **Android Doze Mode:** To optimize battery life, Android may "freeze" or suspend background processes that lack a foreground service notification. 
> * In some cases, the payload may become dormant when the screen is off for an extended period. 
> * Connectivity usually resumes instantly once the device is woken up or the screen is turned on.
> * All data sends with no encryption.
> * Developer mode and USB debugging on the target device are required (otherwise, android will just kill your process).

### Startup & Root Limitations
Since Droidoor operates within the standard `shell` user context (non-root):
* **No Autostart:** The binary cannot be added to the system boot sequence. 
* **Reboot Persistence:** If the device is restarted, the payload will stop running. To regain access, you must physically reconnect the device via USB and execute the `main.sh` deployer again.
* **Storage:** All files are deployed to `/data/local/tmp`. This directory is used because it is one of the few partitions on Android that allows Read/Write/Execute (`rwx`) permissions for the shell user without requiring root access.

---

### 🛡️ Post-Exploitation Capabilities

Once the reverse shell is active, you can leverage standard Android binaries to gather intelligence:

* **Network Monitoring:** Monitor real-time Wi-Fi connection states, SSID history, and signal strength using `dumpsys connectivity`.
* **Data Exfiltration:** Download photos, documents, and databases from the SD card or storage (`/sdcard/DCIM`, `/sdcard/Download`, etc.) using the established stream.
* **System Logs & Tracking:**
    * **Logcat:** Access real-time system logs to monitor application activity.
    * **GPS & Location:** Intercept system logs (`syslog`) containing GPS coordinates and NMEA sentences. By parsing these logs, it is possible to determine the geographical location of the device with high precision.
* **Hardware Info:** Retrieve deep hardware specs, battery health, and sensor data via `dumpsys`.
* **Screenshots:** Take screenshots of the screen (`screencap -p /data/local/tmp/screen.png`)
---

## ⚡️ Installation and quick use

### Install the dependencies

```bash
# macOS
brew install zig adb netcat
```

```bash
# Linux (Ubuntu/Debian)
sudo apt install zig adb netcat
```

### Install droidoor

```bash
git clone https://github.com/lycan-hunter/droidoor.git
cd droidoor
./main.sh
```

- Use the first one action (1):

<img width="530" height="187" alt="1" src="https://github.com/user-attachments/assets/31fe129e-8ff1-4352-9181-98630a7d6e7f" />

- Enter the IP and PORT of your VPS or another machine

- Plug device via USB (Make sure that developer mode is enabled on the device and USB debugging is enabled):

- Start the listener on the selected port using the machine with the previously entered IP

<img width="407" height="139" alt="2" src="https://github.com/user-attachments/assets/d4a9acb5-9e1c-429e-aa60-4536e7d59bf6" />

```bash
# If you use netcat
nc -nlvp <YOUR_PORT>
```

---

## ⚠️ Disclaimer

This tool is developed for **educational purposes** and **authorized security auditing** only. 

The use of **Droidoor** against targets without prior mutual consent is illegal. It is the end user's responsibility to obey all applicable local, state, and federal laws. The author assumes no liability and is not responsible for any misuse or damage caused by this program.

By using this software, you agree to the terms of this license.

---

**Author:** [lycan-hunter](https://github.com/lycan-hunter)  
**Project Home:** [droidoor](https://github.com/lycan-hunter/droidoor)