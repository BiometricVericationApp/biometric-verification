import subprocess
import sys

def compile_lcd(wifi_name, device_path):
    try:
        subprocess.run(
                f'''
                cd lcd && arduino-cli compile -b arduino:avr:uno --build-property build.extra_flags=-DPIN=2 --build-property "compiler.cpp.extra_flags=\"-DSSID=\"hello world\"\"" {device_path}
                ''',
                # f"cd lcd && arduino-cli compile --build-property \"build.extra_flags=\"-SSID=\"{wifi_name}\"\"\" -u -p {device_path}",
            shell=True,
            check=True
        )
        print("LCD compilation successful.")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling LCD: {e}")
    finally:
        subprocess.run("cd ..", shell=True)

def compile_proximity(wifi_name, device_path):
    try:
        subprocess.run(
            f"cd proximity && arduino-cli compile --build-property WIFI_NAME={wifi_name} -u -p {device_path}",
            shell=True,
            check=True
        )
        print("Proximity compilation successful.")
    except subprocess.CalledProcessError as e:
        print(f"Error compiling Proximity: {e}")
    finally:
        subprocess.run("cd ..", shell=True)

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Please provide a target: lcd or proximity")
        sys.exit(1)

    target = sys.argv[1]

    WIFI_NAME = input("Enter WIFI_NAME: ") or "hi"
    DEVICE_PATH = input("Enter DEVICE_PATH: ") or "/dev/ttyUSB0"

    if target == "lcd":
        compile_lcd(WIFI_NAME, DEVICE_PATH)
    elif target == "proximity":
        compile_proximity(WIFI_NAME, DEVICE_PATH)
    else:
        print("Invalid target. Use 'lcd' or 'proximity'.")
        sys.exit(1)
